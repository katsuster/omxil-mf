#include <map>
#include <mutex>
#include <condition_variable>
#include <pthread.h>
#include <sys/prctl.h>

#include <OMX_Component.h>
#include <OMX_Core.h>

#include <omxil_mf/component.hpp>
#include <omxil_mf/port.hpp>

#include "api/consts.h"
#include "util/omx_enum_name.hpp"
#include "debug/scoped_log.hpp"

namespace mf {

/*
 * スレッド一覧
 * omx:cmd:xxxx: OMX_SendCommand を処理するためのスレッド。
 *    OpenMAX の規格上 OMX_SendCommand はブロックしてはいけないので、
 *    コマンドを受け付けたらすぐ呼び出し元に制御を返す。
 *    実際の処理は cmd_xxxx スレッドで処理し、処理が終わったら
 *    イベントコールバックで呼び出し元に通知する。
 *
 * omx:run:xxxx: コンポーネントの処理を行うスレッド。
 */

/*
 * public functions
 */

component::component(OMX_COMPONENTTYPE *c, const char *cname)
		: omx_reflector(c, cname), broken(false),
		state(OMX_StateInvalid), omx_cbs(nullptr), omx_cbs_priv(nullptr),
		th_accept(nullptr), ring_accept(nullptr), bound_accept(nullptr),
		th_main(nullptr), running_main(false) {
	scoped_log_begin;

	try {
		//create ring buffer
		ring_accept = new ring_buffer<OMX_SEND_CMD>(nullptr, OMX_SEND_CMD_DEPTH + 1);
		bound_accept = new bounded_buffer<ring_buffer<OMX_SEND_CMD>, OMX_SEND_CMD>(*ring_accept);

		//start command accept thread
		th_accept = new std::thread(accept_command_thread_main, get_omx_component());
	} catch (const std::bad_alloc& e) {
		errprint("failed to construct '%s'.\n", e.what());

		delete th_accept;
		th_accept = nullptr;

		delete ring_accept;
		ring_accept = nullptr;
		delete bound_accept;
		bound_accept = nullptr;

		throw;
	}

	//component is ready to work
	set_state(OMX_StateLoaded);
}

component::~component() {
	scoped_log_begin;

	//shutdown accept thread
	if (bound_accept) {
		bound_accept->shutdown();
	}
	if (th_accept) {
		th_accept->join();
	}
	delete th_accept;
	delete bound_accept;
	delete ring_accept;
}

const char *component::get_name() const {
	return "component";
}

OMX_STATETYPE component::get_state() const {
	return state;
}

void component::set_state(OMX_STATETYPE s) {
	std::unique_lock<std::mutex> lock(mut);

	state = s;
	cond.notify_all();
}

void component::wait_state(OMX_STATETYPE s) {
	std::unique_lock<std::mutex> lock(mut);

	cond.wait(lock, [&] { return broken || state == s; });
	error_if_broken(lock);
}

void component::wait_state_multiple(int cnt, ...) {
	std::unique_lock<std::mutex> lock(mut);
	va_list ap;
	OMX_STATETYPE states[16];

	if (cnt <= 0) {
		return;
	}
	if (cnt > 16) {
		errprint("count:%d is too much.\n", cnt);
		return;
	}

	va_start(ap, cnt);
	for (int i = 0; i < cnt; i++) {
		//enum は暗黙の型変換により int にキャストされ、
		//関数に渡される。
		//int から enum への暗黙のキャストは出来ないので、
		//static_cast が必要。
		states[i] = static_cast<OMX_STATETYPE>(va_arg(ap, int));
	}
	va_end(ap);

	cond.wait(lock, [&] {
		for (int i = 0; i < cnt; i++) {
			if (broken || state == states[i]) {
				return true;
		}
		}
		return false; });
	error_if_broken(lock);
}

void component::shutdown() {
	std::unique_lock<std::mutex> lock(mut);

	broken = true;
	cond.notify_all();
}

const OMX_CALLBACKTYPE *component::get_callbacks() const {
	return omx_cbs;
}

const void *component::get_callbacks_data() const {
	return omx_cbs_priv;
}

port *component::find_port(OMX_U32 index) {
	component::portmap_t::iterator ret;

	ret = map_ports.find(index);
	if (ret == map_ports.end()) {
		//not found
		return nullptr;
	}

	return &ret->second;
}

bool component::should_run() {
	return running_main;
}

void component::stop_running() {
	running_main = false;
}


/*
 * OpenMAX member functions
 */
OMX_ERRORTYPE component::GetComponentVersion(OMX_HANDLETYPE hComponent, OMX_STRING pComponentName, OMX_VERSIONTYPE *pComponentVersion, OMX_VERSIONTYPE *pSpecVersion, OMX_UUIDTYPE *pComponentUUID) {
	scoped_log_begin;

	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

OMX_ERRORTYPE component::SendCommand(OMX_HANDLETYPE hComponent, OMX_COMMANDTYPE Cmd, OMX_U32 nParam, OMX_PTR pCmdData) {
	scoped_log_begin;
	port *port_found;
	OMX_SEND_CMD cmd;
	//OMX_ERRORTYPE err;

	dprint("cmd:%s\n", omx_enum_name::get_OMX_COMMANDTYPE_name(Cmd));

	switch (Cmd) {
	case OMX_CommandStateSet:
		//do nothing
		break;
	case OMX_CommandFlush:
	case OMX_CommandPortDisable:
	case OMX_CommandPortEnable:
	case OMX_CommandMarkBuffer:
		port_found = find_port(nParam);
		if (port_found == nullptr) {
			errprint("invalid port: %d\n", (int)nParam);
			return OMX_ErrorBadPortIndex;
		}

		break;
	default:
		return OMX_ErrorUnsupportedIndex;
	}

	try {
		cmd.cmd = Cmd;
		cmd.param = nParam;
		cmd.data = pCmdData;

		bound_accept->write_fully(&cmd, 1);
	} catch (const std::runtime_error& e) {
		errprint("runtime_error: %s\n", e.what());
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::GetParameter(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) {
	scoped_log_begin;
	port *port_found = nullptr;
	OMX_PARAM_PORTDEFINITIONTYPE *def = nullptr;
	OMX_PARAM_BUFFERSUPPLIERTYPE *supply = nullptr;
	OMX_PORT_PARAM_TYPE *param = nullptr;
	//OMX_PRIORITYMGMTTYPE *mgm;
	void *ptr = nullptr;
	OMX_ERRORTYPE err;

	dprint("indextype:%s, name:%s\n", omx_enum_name::get_OMX_INDEXTYPE_name(nParamIndex), get_name());

	ptr = pComponentParameterStructure;

	//OpenMAX IL 1.1.2: 8.3 Mandatory Port Parameters
	switch (nParamIndex) {
	case OMX_IndexParamPortDefinition:
		def = (OMX_PARAM_PORTDEFINITIONTYPE *) ptr;

		err = check_omx_header(def, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
		if (err != OMX_ErrorNone) {
			errprint("invalid header.\n");
			break;
		}

		port_found = find_port(def->nPortIndex);
		if (port_found == nullptr) {
			errprint("invalid port:%d\n", (int)def->nPortIndex);
			err = OMX_ErrorBadPortIndex;
			break;
		}

		*def = *(port_found->get_definition());

		break;
	case OMX_IndexParamCompBufferSupplier:
		supply = (OMX_PARAM_BUFFERSUPPLIERTYPE *) ptr;

		err = check_omx_header(supply, sizeof(OMX_PARAM_BUFFERSUPPLIERTYPE));
		if (err != OMX_ErrorNone) {
			errprint("invalid header.\n");
			break;
		}

		port_found = find_port(supply->nPortIndex);
		if (port_found == nullptr) {
			errprint("invalid port:%d\n", (int)supply->nPortIndex);
			err = OMX_ErrorBadPortIndex;
			break;
		}

		//FIXME: not implemented
		throw std::runtime_error("not implemented.");

		break;
	case OMX_IndexParamAudioInit:
	case OMX_IndexParamVideoInit:
	case OMX_IndexParamImageInit:
	case OMX_IndexParamOtherInit:
		param = (OMX_PORT_PARAM_TYPE *) ptr;

		err = check_omx_header(param, sizeof(OMX_PARAM_HEADERTYPE));
		if (err != OMX_ErrorNone) {
			errprint("invalid header.\n");
			break;
		}

		param->nPorts = 0;
		param->nStartPortNumber = 0;

		break;
	//case OMX_IndexParamPriorityMgmt:
	//	mgm = (OMX_PRIORITYMGMTTYPE *) ptr;
	//	break;
	default:
		errprint("unsupported index:%d.\n", (int)nParamIndex);
		err = OMX_ErrorUnsupportedIndex;
		break;
	}

	return err;
}

OMX_ERRORTYPE component::SetParameter(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) {
	scoped_log_begin;
	port *port_found = nullptr;
	OMX_PARAM_PORTDEFINITIONTYPE *def = nullptr;
	OMX_PARAM_BUFFERSUPPLIERTYPE *supply = nullptr;
	OMX_PORT_PARAM_TYPE *param = nullptr;
	//OMX_PRIORITYMGMTTYPE *mgm;
	void *ptr = nullptr;
	OMX_ERRORTYPE err;

	dprint("indextype:%s, name:%s\n", omx_enum_name::get_OMX_INDEXTYPE_name(nParamIndex), get_name());

	ptr = pComponentParameterStructure;

	//OpenMAX IL 1.1.2: 8.3 Mandatory Port Parameters
	switch (nParamIndex) {
	case OMX_IndexParamPortDefinition:
		def = (OMX_PARAM_PORTDEFINITIONTYPE *) ptr;

		err = check_omx_header(def, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
		if (err != OMX_ErrorNone) {
			errprint("invalid header.\n");
			break;
		}

		port_found = find_port(def->nPortIndex);
		if (port_found == nullptr) {
			errprint("invalid port:%d\n", (int)def->nPortIndex);
			err = OMX_ErrorBadPortIndex;
			break;
		}

		//nBufferCountActual 以外は全て read-only
		port_found->set_buffer_count_actual(def->nBufferCountActual);

		break;
	case OMX_IndexParamCompBufferSupplier:
		supply = (OMX_PARAM_BUFFERSUPPLIERTYPE *) ptr;

		err = check_omx_header(supply, sizeof(OMX_PARAM_BUFFERSUPPLIERTYPE));
		if (err != OMX_ErrorNone) {
			errprint("invalid header.\n");
			break;
		}

		port_found = find_port(supply->nPortIndex);
		if (port_found == nullptr) {
			errprint("invalid port:%d\n", (int)supply->nPortIndex);
			err = OMX_ErrorBadPortIndex;
			break;
		}

		//FIXME: not implemented
		throw std::runtime_error("not implemented.");

		break;
	case OMX_IndexParamAudioInit:
	case OMX_IndexParamVideoInit:
	case OMX_IndexParamImageInit:
	case OMX_IndexParamOtherInit:
		param = (OMX_PORT_PARAM_TYPE *) ptr;

		err = check_omx_header(param, sizeof(OMX_PARAM_HEADERTYPE));
		if (err != OMX_ErrorNone) {
			errprint("invalid header.\n");
			break;
		}

		//FIXME: not supported
		err = OMX_ErrorUnsupportedIndex;

		break;
	//case OMX_IndexParamPriorityMgmt:
	//	mgm = (OMX_PRIORITYMGMTTYPE *) ptr;
	//	break;
	default:
		errprint("unsupported index:%d.\n", (int)nParamIndex);
		err = OMX_ErrorUnsupportedIndex;
		break;
	}

	return err;
}

OMX_ERRORTYPE component::GetConfig(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure) {
	scoped_log_begin;

	//do nothing

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::SetConfig(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure) {
	scoped_log_begin;

	//do nothing

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::GetExtensionIndex(OMX_HANDLETYPE hComponent, OMX_STRING cParameterName, OMX_INDEXTYPE *pIndexType) {
	scoped_log_begin;

	//do nothing

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::GetState(OMX_HANDLETYPE hComponent, OMX_STATETYPE *pState) {
	scoped_log_begin;

	if (pState == nullptr) {
		errprint("pState is nullptr.\n");
		return OMX_ErrorBadParameter;
	}
	*pState = get_state();

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::ComponentTunnelRequest(OMX_HANDLETYPE hComponent, OMX_U32 nPort, OMX_HANDLETYPE hTunneledComp, OMX_U32 nTunneledPort, OMX_TUNNELSETUPTYPE *pTunnelSetup) {
	scoped_log_begin;

	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

OMX_ERRORTYPE component::UseBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8 *pBuffer) {
	scoped_log_begin;
	port *port_found = nullptr;
	OMX_ERRORTYPE err;

	port_found = find_port(nPortIndex);
	if (port_found == nullptr) {
		errprint("invalid port:%d\n", (int)nPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	if (nSizeBytes < port_found->get_buffer_size()) {
		errprint("buffer size(%d) < minimum size(%d)\n",
			(int)nSizeBytes, (int)port_found->get_buffer_size());
		return OMX_ErrorBadPortIndex;
	}

	err = port_found->use_buffer(ppBufferHdr,
		pAppPrivate, nSizeBytes, pBuffer);

	return err;
}

OMX_ERRORTYPE component::AllocateBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBuffer, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes) {
	scoped_log_begin;
	port *port_found = nullptr;
	OMX_ERRORTYPE err;

	port_found = find_port(nPortIndex);
	if (port_found == nullptr) {
		errprint("invalid port:%d\n", (int)nPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	if (nSizeBytes < port_found->get_buffer_size()) {
		errprint("allocate size(%d) < minimum size(%d)\n",
			(int)nSizeBytes, (int)port_found->get_buffer_size());
		return OMX_ErrorBadPortIndex;
	}

	err = port_found->allocate_buffer(ppBuffer,
		pAppPrivate, nSizeBytes);

	return err;
}

OMX_ERRORTYPE component::FreeBuffer(OMX_HANDLETYPE hComponent, OMX_U32 nPortIndex, OMX_BUFFERHEADERTYPE *pBuffer) {
	scoped_log_begin;
	port *port_found = nullptr;
	OMX_ERRORTYPE err;

	port_found = find_port(nPortIndex);
	if (port_found == nullptr) {
		errprint("invalid port:%d\n", (int)nPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	err = port_found->free_buffer(pBuffer);

	return err;
}

OMX_ERRORTYPE component::EmptyThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE *pBuffer) {
	//scoped_log_begin;
	port *port_found = nullptr;
	OMX_ERRORTYPE err;

	port_found = find_port(pBuffer->nInputPortIndex);
	if (port_found == nullptr) {
		errprint("invalid input port:%d\n",
			(int)pBuffer->nInputPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	err = port_found->empty_buffer(pBuffer);

	return err;
}

OMX_ERRORTYPE component::FillThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE *pBuffer) {
	//scoped_log_begin;
	port *port_found = nullptr;
	OMX_ERRORTYPE err;

	port_found = find_port(pBuffer->nOutputPortIndex);
	if (port_found == nullptr) {
		errprint("invalid output port:%d\n",
			(int)pBuffer->nOutputPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	err = port_found->fill_buffer(pBuffer);

	return err;
}

OMX_ERRORTYPE component::SetCallbacks(OMX_HANDLETYPE hComponent, OMX_CALLBACKTYPE *pCallbacks, OMX_PTR pAppData) {
	scoped_log_begin;

	omx_cbs = pCallbacks;
	omx_cbs_priv = pAppData;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::ComponentDeInit(OMX_HANDLETYPE hComponent) {
	scoped_log_begin;

	//do nothing

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::UseEGLImage(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, void *eglImage) {
	scoped_log_begin;

	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

OMX_ERRORTYPE component::ComponentRoleEnum(OMX_HANDLETYPE hComponent, OMX_U8 *cRole, OMX_U32 nIndex) {
	scoped_log_begin;

	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}


//----------
//OpenMAX callback function wrappers
//----------

OMX_ERRORTYPE component::EventHandler(OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData) {
	scoped_log_begin;
	OMX_ERRORTYPE err;

	dprint("eEvent:%s, nData1:%s\n",
		omx_enum_name::get_OMX_EVENTTYPE_name(eEvent),
		omx_enum_name::get_OMX_COMMANDTYPE_name((OMX_COMMANDTYPE)nData1));

	err = omx_cbs->EventHandler(get_omx_component(), omx_cbs_priv,
		eEvent, nData1, nData2, pEventData);

	return err;
}

OMX_ERRORTYPE component::EmptyBufferDone(OMX_BUFFERHEADERTYPE *pBuffer) {
	//scoped_log_begin;
	OMX_ERRORTYPE err;

	err = omx_cbs->EmptyBufferDone(get_omx_component(),
		omx_cbs_priv, pBuffer);

	return err;
}

OMX_ERRORTYPE component::EmptyBufferDone(port_buffer *pb) {
	//scoped_log_begin;
	//EOS detected
	if (pb->header->nFlags & OMX_BUFFERFLAG_EOS) {
		EventHandler(OMX_EventBufferFlag,
			pb->p->get_index(), pb->header->nFlags, nullptr);
	}

	return EmptyBufferDone(pb->header);
}

OMX_ERRORTYPE component::FillBufferDone(OMX_BUFFERHEADERTYPE *pBuffer) {
	//scoped_log_begin;
	OMX_ERRORTYPE err;

	err = omx_cbs->FillBufferDone(get_omx_component(),
		omx_cbs_priv, pBuffer);

	return err;
}

OMX_ERRORTYPE component::FillBufferDone(port_buffer *pb) {
	//scoped_log_begin;
	//EOS detected
	if (pb->header->nFlags & OMX_BUFFERFLAG_EOS) {
		EventHandler(OMX_EventBufferFlag,
			pb->p->get_index(), pb->header->nFlags, nullptr);
	}

	return FillBufferDone(pb->header);
}


/*
 * protected functions
 */

void component::error_if_broken(std::unique_lock<std::mutex>& lock) {
	if (broken) {
		std::string msg(__func__);
		msg += ": interrupted.";
		throw std::runtime_error(msg);
	}
}

void *component::accept_command() {
	scoped_log_begin;
	OMX_SEND_CMD cmd;
	OMX_STATETYPE new_state;
	OMX_U32 port_index;
	//callback するかしないか
	bool f_callback;
	//EventHandler の引数
	OMX_U32 data1, data2;
	OMX_PTR event_data;
	OMX_ERRORTYPE err, err_handler;

	while (1) {
		//blocked read
		bound_accept->read_fully(&cmd, 1);

		dprint("cmd:%s, param1:0x%08x, data:%p\n",
			omx_enum_name::get_OMX_COMMANDTYPE_name(cmd.cmd),
			(int)cmd.param, cmd.data);

		//process command
		err = OMX_ErrorNone;
		err_handler = OMX_ErrorNone;
		f_callback = true;

		switch (cmd.cmd) {
		case OMX_CommandStateSet:
			new_state = (OMX_STATETYPE) cmd.param;
			err = command_state_set(new_state);

			data1 = OMX_CommandStateSet;
			data2 = new_state;
			event_data = nullptr;

			break;
		case OMX_CommandFlush:
			port_index = cmd.param;
			err = command_flush(port_index);

			data1 = OMX_CommandFlush;
			data2 = port_index;
			event_data = nullptr;

			break;
		case OMX_CommandPortDisable:
			port_index = cmd.param;
			err = command_port_disable(port_index);

			data1 = OMX_CommandPortDisable;
			data2 = port_index;
			event_data = nullptr;

			break;
		case OMX_CommandPortEnable:
			port_index = cmd.param;
			err = command_port_enable(port_index);

			data1 = OMX_CommandPortEnable;
			data2 = port_index;
			event_data = nullptr;

			break;
		case OMX_CommandMarkBuffer:
			port_index = cmd.param;
			err = command_mark_buffer(port_index);

			data1 = OMX_CommandMarkBuffer;
			data2 = port_index;
			event_data = nullptr;

			break;
		default:
			errprint("unsupported index:%d.\n",
				(int)cmd.cmd);
			f_callback = false;
			break;
		}

		//done/error event callback
		if (f_callback && err == OMX_ErrorNone) {
			//done
			err_handler = EventHandler(OMX_EventCmdComplete,
				data1, data2, event_data);
		} else if (f_callback) {
			//error
			err_handler = EventHandler(OMX_EventError,
				err_handler, 0, nullptr);
		}
		if (err_handler != OMX_ErrorNone) {
			errprint("event handler returns error: %s\n",
				omx_enum_name::get_OMX_ERRORTYPE_name(err_handler));
		}
	}

	return nullptr;
}

OMX_ERRORTYPE component::command_state_set(OMX_STATETYPE new_state) {
	scoped_log_begin;
	OMX_ERRORTYPE err;

	dprint("state:%s -> %s\n",
		omx_enum_name::get_OMX_STATETYPE_name(get_state()),
		omx_enum_name::get_OMX_STATETYPE_name(new_state));

	switch (new_state) {
	case OMX_StateInvalid:
		err = command_state_set_to_invalid();
		break;
	case OMX_StateLoaded:
		err = command_state_set_to_loaded();
		break;
	case OMX_StateIdle:
		err = command_state_set_to_idle();
		break;
	case OMX_StateExecuting:
		err = command_state_set_to_executing();
		break;
	case OMX_StatePause:
		err = command_state_set_to_pause();
		break;
	case OMX_StateWaitForResources:
		err = command_state_set_to_wait_for_resources();
		break;
	default:
		errprint("unsupported state:%d.\n", (int)new_state);
		err = OMX_ErrorIncorrectStateTransition;
		break;
	}

	return err;
}

OMX_ERRORTYPE component::command_state_set_to_invalid() {
	scoped_log_begin;

	set_state(OMX_StateInvalid);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::command_state_set_to_loaded() {
	scoped_log_begin;
	OMX_ERRORTYPE err;

	switch (get_state()) {
	case OMX_StateLoaded:
		err = OMX_ErrorSameState;
		break;
	case OMX_StateIdle:
		//FIXME: port flush not implemented yet

		//stop & wait main thread
		stop_running();
		th_main->join();
		delete th_main;
		th_main = nullptr;

		err = OMX_ErrorNone;

		break;
	default:
		errprint("invalid state:%s.\n",
			omx_enum_name::get_OMX_STATETYPE_name(get_state()));
		err = OMX_ErrorInvalidState;
		break;
	}

	if (err == OMX_ErrorNone) {
		set_state(OMX_StateLoaded);
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::command_state_set_to_idle() {
	scoped_log_begin;
	OMX_ERRORTYPE err;

	switch (get_state()) {
	case OMX_StateLoaded:
		try {
			//start main thread
			th_main = new std::thread(component_thread_main, get_omx_component());
		} catch (const std::bad_alloc& e) {
			errprint("failed to create main thread '%s'.\n", e.what());
			err = OMX_ErrorInsufficientResources;
			break;
		}

		err = OMX_ErrorNone;

		break;
	case OMX_StateIdle:
		err = OMX_ErrorSameState;
		break;
	case OMX_StateExecuting:
		//FIXME: flush not implemented yet

		err = OMX_ErrorNone;

		break;
	default:
		errprint("invalid state:%s.\n",
			omx_enum_name::get_OMX_STATETYPE_name(get_state()));
		err = OMX_ErrorInvalidState;
		break;
	}

	if (err == OMX_ErrorNone) {
		set_state(OMX_StateIdle);
	}

	return err;
}

OMX_ERRORTYPE component::command_state_set_to_executing() {
	scoped_log_begin;
	OMX_ERRORTYPE err;

	switch (get_state()) {
	case OMX_StateIdle:
		//メイン処理が始まってから状態を切り替える
		{
			std::unique_lock<std::mutex> lock(mut);
			cond.wait(lock, [&] { return broken || should_run(); });
			error_if_broken(lock);
		}

		err = OMX_ErrorNone;
		break;
	case OMX_StateExecuting:
		err = OMX_ErrorSameState;
		break;
	default:
		errprint("invalid state:%s.\n",
			omx_enum_name::get_OMX_STATETYPE_name(get_state()));
		err = OMX_ErrorInvalidState;
		break;
	}

	set_state(OMX_StateExecuting);

	return err;
}

OMX_ERRORTYPE component::command_state_set_to_pause() {
	scoped_log_begin;

	set_state(OMX_StatePause);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::command_state_set_to_wait_for_resources() {
	scoped_log_begin;

	set_state(OMX_StateWaitForResources);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::command_flush(OMX_U32 port_index) {
	scoped_log_begin;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::command_port_disable(OMX_U32 port_index) {
	scoped_log_begin;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::command_port_enable(OMX_U32 port_index) {
	scoped_log_begin;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::command_mark_buffer(OMX_U32 port_index) {
	scoped_log_begin;

	return OMX_ErrorNone;
}

bool component::insert_port(port& p) {
	std::pair<component::portmap_t::iterator, bool> ret;

	ret = map_ports.insert(component::portmap_t::value_type(p.get_index(), p));

	return ret.second;
}

bool component::erase_port(OMX_U32 index) {
	size_t ret;

	ret = map_ports.erase(index);

	return (ret != 0);
}

const component::portmap_t& component::get_map_ports() const {
	return map_ports;
}

component::portmap_t& component::get_map_ports() {
	return map_ports;
}

OMX_ERRORTYPE component::check_omx_header(const void *p, size_t size) const {
	const OMX_PARAM_HEADERTYPE *h = (const OMX_PARAM_HEADERTYPE *)p;

	if (h == nullptr || h->nSize != size) {
		return OMX_ErrorBadParameter;
	}
	if (h->nVersion.s.nVersionMajor != OMX_MF_IL_MAJOR ||
		h->nVersion.s.nVersionMinor != OMX_MF_IL_MINOR) {
		return OMX_ErrorVersionMismatch;
	}

	return OMX_ErrorNone;
}


/*
 * static public functions
 */
component *component::get_instance(OMX_HANDLETYPE hComponent) {
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	component *comp = (component *) omx_comp->pComponentPrivate;

	return comp;
}


/*
 * static protected functions
 */

void *component::accept_command_thread_main(OMX_COMPONENTTYPE *arg) {
	scoped_log_begin;
	std::string thname;
	component *comp = get_instance(arg);

	try {
		//スレッド名をつける
		thname = "omx:cmd:";
		thname += comp->get_name();
		prctl(PR_SET_NAME, thname.c_str());

		comp->accept_command();
	} catch (const std::runtime_error& e) {
		errprint("runtime_error: %s\n", e.what());
	}

	return nullptr;
}

void *component::component_thread_main(OMX_COMPONENTTYPE *arg) {
	scoped_log_begin;
	std::string thname;
	component *comp = get_instance(arg);

	try {
		//スレッド名をつける
		thname = "omx:run:";
		thname += comp->get_name();
		prctl(PR_SET_NAME, thname.c_str());

		//Idle 状態になってから開始する
		comp->wait_state(OMX_StateIdle);

		//メイン処理が始まったことを通知する
		{
			std::unique_lock<std::mutex> lock(comp->mut);
			comp->running_main = true;
			comp->cond.notify_all();
		}
		comp->run();
	} catch (const std::runtime_error& e) {
		errprint("runtime_error: %s\n", e.what());
	}

	return nullptr;
}

} //namespace mf
