#include <cstdarg>
#include <cstdint>
#include <deque>
#include <map>
#include <mutex>
#include <condition_variable>

#include <OMX_Component.h>
#include <OMX_Core.h>

#include <omxil_mf/component.hpp>
#include <omxil_mf/port_audio.hpp>
#include <omxil_mf/port_video.hpp>
#include <omxil_mf/port_image.hpp>
#include <omxil_mf/port_other.hpp>
#include <omxil_mf/port_format.hpp>
#include <omxil_mf/scoped_log.hpp>

#include "api/consts.hpp"
#include "util/util.hpp"
#include "util/omx_enum_name.hpp"

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
	state(OMX_StateInvalid), omx_cbs(), omx_cbs_priv(nullptr),
	th_accept(nullptr), ring_accept(nullptr), bound_accept(nullptr),
	th_main(nullptr), running_main(false)
{
	scoped_log_begin;

	try {
		//create ring buffer
		ring_accept = new ring_buffer<OMX_MF_CMD>(nullptr, OMX_MF_CMD_DEPTH + 1);
		bound_accept = new bounded_buffer<ring_buffer<OMX_MF_CMD>, OMX_MF_CMD>(*ring_accept);

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

component::~component()
{
	scoped_log_begin;

	shutdown();

	//shutdown main thread
	if (th_main) {
		stop_running();
		th_main->join();
	}
	delete th_main;

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

const char *component::get_name() const
{
	return "component";
}

OMX_STATETYPE component::get_state() const
{
	return state;
}

void component::set_state(OMX_STATETYPE s)
{
	std::lock_guard<std::mutex> lock(mut);

	state = s;
	cond.notify_all();
}

void component::wait_state(OMX_STATETYPE s) const
{
	std::unique_lock<std::mutex> lock(mut);

	cond.wait(lock, [&] { return broken || state == s; });
	error_if_broken(lock);
}

void component::wait_state_multiple(int cnt, ...) const
{
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

void component::shutdown()
{
	std::lock_guard<std::mutex> lock(mut);

	broken = true;
	cond.notify_all();
}

const OMX_CALLBACKTYPE *component::get_callbacks() const
{
	return &omx_cbs;
}

const void *component::get_callbacks_data() const
{
	return omx_cbs_priv;
}

const port *component::find_port(OMX_U32 index) const
{
	auto ret = map_ports.find(index);
	if (ret == map_ports.end()) {
		//not found
		return nullptr;
	}

	return &ret->second;
}

port *component::find_port(OMX_U32 index)
{
	auto ret = map_ports.find(index);
	if (ret == map_ports.end()) {
		//not found
		return nullptr;
	}

	return &ret->second;
}

void component::wait_all_port_buffer_returned() const
{
	scoped_log_begin;

	for (auto it = map_ports.begin(); it != map_ports.end(); it++) {
		if (it->second.get_enabled()) {
			it->second.wait_buffer_returned();
		}
	}
}

bool component::should_run() const
{
	return running_main;
}

void component::stop_running()
{
	std::unique_lock<std::mutex> lock(mut);

	running_main = false;
}


/*
 * OpenMAX member functions
 */
OMX_ERRORTYPE component::GetComponentVersion(OMX_HANDLETYPE hComponent, OMX_STRING pComponentName, OMX_VERSIONTYPE *pComponentVersion, OMX_VERSIONTYPE *pSpecVersion, OMX_UUIDTYPE *pComponentUUID)
{
	scoped_log_begin;
	uint32_t uuid[4];

	if (pComponentName == nullptr || pComponentVersion == nullptr || 
			pSpecVersion == nullptr || pComponentUUID == nullptr) {
		errprint("Invalid arguments\n");
		return OMX_ErrorBadParameter;
	}

	//name
	strncpy(pComponentName, get_component_name().c_str(), OMX_MAX_STRINGNAME_SIZE);

	//component version (default is 1.0.0.0)
	pComponentVersion->s.nVersionMajor = 1;
	pComponentVersion->s.nVersionMinor = 0;
	pComponentVersion->s.nRevision     = 0;
	pComponentVersion->s.nStep         = 0;

	//spec version
	*pSpecVersion = get_omx_component()->nVersion;

	//uuid
	memset(*pComponentUUID, 0, sizeof(*pComponentUUID));
	uuid[0] = (uintptr_t)this;
	uuid[1] = get_process_id();
	uuid[2] = 0;
	uuid[3] = 0;
	memmove(*pComponentUUID, uuid, sizeof(uuid));

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::SendCommand(OMX_HANDLETYPE hComponent, OMX_COMMANDTYPE Cmd, OMX_U32 nParam, OMX_PTR pCmdData)
{
	port *port_found;
	OMX_MF_CMD cmd;
	//OMX_ERRORTYPE err;

	dprint("cmd:%s\n", omx_enum_name::get_OMX_COMMANDTYPE_name(Cmd));

	switch (Cmd) {
	case OMX_CommandStateSet:
		//do nothing
		break;
	case OMX_CommandFlush:
	case OMX_CommandPortDisable:
	case OMX_CommandPortEnable:
		if (nParam == OMX_ALL) {
			//All ports
			break;
		}

		port_found = find_port(nParam);
		if (port_found == nullptr) {
			errprint("Invalid port: %d\n", (int)nParam);
			return OMX_ErrorBadPortIndex;
		}

		break;
	case OMX_CommandMarkBuffer:
		port_found = find_port(nParam);
		if (port_found == nullptr) {
			errprint("Invalid port: %d\n", (int)nParam);
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

OMX_ERRORTYPE component::GetParameter(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure)
{
	scoped_log_begin;
	void *ptr = nullptr;
	port *port_found = nullptr;
	const port_format *pf_found = nullptr;
	OMX_ERRORTYPE err;

	dprint("indextype:%s, name:%s\n", omx_enum_name::get_OMX_INDEXTYPE_name(nParamIndex), get_name());

	ptr = pComponentParameterStructure;

	//OpenMAX IL 1.2.0: 8.2 Mandatory Port Parameters
	switch (nParamIndex) {
	case OMX_IndexParamPortDefinition: {
		OMX_PARAM_PORTDEFINITIONTYPE *def = static_cast<OMX_PARAM_PORTDEFINITIONTYPE *>(ptr);

		err = check_omx_header(def, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
		if (err != OMX_ErrorNone) {
			errprint("Invalid header.\n");
			break;
		}

		port_found = find_port(def->nPortIndex);
		if (port_found == nullptr) {
			errprint("Invalid port:%d\n", (int)def->nPortIndex);
			err = OMX_ErrorBadPortIndex;
			break;
		}

		*def = *(port_found->get_definition());

		break;
	}
	case OMX_IndexParamCompBufferSupplier: {
		OMX_PARAM_BUFFERSUPPLIERTYPE *supply = static_cast<OMX_PARAM_BUFFERSUPPLIERTYPE *>(ptr);

		err = check_omx_header(supply, sizeof(OMX_PARAM_BUFFERSUPPLIERTYPE));
		if (err != OMX_ErrorNone) {
			errprint("Invalid header.\n");
			break;
		}

		port_found = find_port(supply->nPortIndex);
		if (port_found == nullptr) {
			errprint("Invalid port:%d\n", (int)supply->nPortIndex);
			err = OMX_ErrorBadPortIndex;
			break;
		}

		if (!port_found->get_tunneled()) {
			supply->eBufferSupplier = OMX_BufferSupplyUnspecified;
			err = OMX_ErrorNone;
			break;
		}

		switch (port_found->get_dir()) {
		case OMX_DirInput:
			if (port_found->get_tunneled_supplier()) {
				supply->eBufferSupplier = OMX_BufferSupplyInput;
			} else {
				supply->eBufferSupplier = OMX_BufferSupplyOutput;
			}
			break;
		case OMX_DirOutput:
			if (port_found->get_tunneled_supplier()) {
				supply->eBufferSupplier = OMX_BufferSupplyOutput;
			} else {
				supply->eBufferSupplier = OMX_BufferSupplyInput;
			}
			break;
		default:
			errprint("Bug, do not reach here.\n");
			err = OMX_ErrorUndefined;
			break;
		}

		err = OMX_ErrorNone;
		break;
	}
	case OMX_IndexParamAudioInit: {
		OMX_PORT_PARAM_TYPE *param = static_cast<OMX_PORT_PARAM_TYPE *>(ptr);

		err = check_omx_header(param, sizeof(OMX_PORT_PARAM_TYPE));
		if (err != OMX_ErrorNone) {
			errprint("Invalid header.\n");
			break;
		}

		param->nPorts = get_audio_ports();
		param->nStartPortNumber = get_audio_start_port();

		break;
	}
	case OMX_IndexParamVideoInit: {
		OMX_PORT_PARAM_TYPE *param = static_cast<OMX_PORT_PARAM_TYPE *>(ptr);

		err = check_omx_header(param, sizeof(OMX_PORT_PARAM_TYPE));
		if (err != OMX_ErrorNone) {
			errprint("Invalid header.\n");
			break;
		}

		param->nPorts = get_video_ports();
		param->nStartPortNumber = get_video_start_port();

		break;
	}
	case OMX_IndexParamImageInit: {
		OMX_PORT_PARAM_TYPE *param = static_cast<OMX_PORT_PARAM_TYPE *>(ptr);

		err = check_omx_header(param, sizeof(OMX_PORT_PARAM_TYPE));
		if (err != OMX_ErrorNone) {
			errprint("Invalid header.\n");
			break;
		}

		param->nPorts = get_image_ports();
		param->nStartPortNumber = get_image_start_port();

		break;
	}
	case OMX_IndexParamOtherInit: {
		OMX_PORT_PARAM_TYPE *param = static_cast<OMX_PORT_PARAM_TYPE *>(ptr);

		err = check_omx_header(param, sizeof(OMX_PORT_PARAM_TYPE));
		if (err != OMX_ErrorNone) {
			errprint("Invalid header.\n");
			break;
		}

		param->nPorts = get_other_ports();
		param->nStartPortNumber = get_other_start_port();

		break;
	}
	//case OMX_IndexParamStandardComponentRole:
	//	static_cast<OMX_PARAM_COMPONENTROLETYPE *>(ptr);
	//	break;
	case OMX_IndexParamAudioPortFormat: {
		OMX_AUDIO_PARAM_PORTFORMATTYPE *pf_audio = static_cast<OMX_AUDIO_PARAM_PORTFORMATTYPE *>(ptr);

		err = check_omx_header(pf_audio, sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE));
		if (err != OMX_ErrorNone) {
			errprint("Invalid header.\n");
			break;
		}

		port_found = find_port(pf_audio->nPortIndex);
		if (port_found == nullptr) {
			errprint("Invalid port:%d\n", (int)pf_audio->nPortIndex);
			err = OMX_ErrorBadPortIndex;
			break;
		}

		pf_found = port_found->get_port_format(pf_audio->nIndex);
		if (pf_found == nullptr || pf_found->get_format_audio() == nullptr) {
			errprint("port:%d does not have audio format(index:%d).\n",
				(int)pf_audio->nPortIndex, (int)pf_audio->nIndex);
			err = OMX_ErrorNoMore;
			break;
		}

		{
			const OMX_AUDIO_PARAM_PORTFORMATTYPE *pf_sup = pf_found->get_format_audio();
			OMX_AUDIO_PARAM_PORTFORMATTYPE tmp;

			tmp = *pf_sup;
			tmp.nSize      = pf_audio->nSize;
			tmp.nVersion   = pf_audio->nVersion;
			tmp.nPortIndex = pf_audio->nPortIndex;
			tmp.nIndex     = pf_audio->nIndex;

			*pf_audio = tmp;
		}

		err = OMX_ErrorNone;
		break;
	}
	case OMX_IndexParamVideoPortFormat: {
		OMX_VIDEO_PARAM_PORTFORMATTYPE *pf_video = static_cast<OMX_VIDEO_PARAM_PORTFORMATTYPE *>(ptr);

		err = check_omx_header(pf_video, sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE));
		if (err != OMX_ErrorNone) {
			errprint("Invalid header.\n");
			break;
		}

		port_found = find_port(pf_video->nPortIndex);
		if (port_found == nullptr) {
			errprint("Invalid port:%d\n", (int)pf_video->nPortIndex);
			err = OMX_ErrorBadPortIndex;
			break;
		}

		pf_found = port_found->get_port_format(pf_video->nIndex);
		if (pf_found == nullptr || pf_found->get_format_video() == nullptr) {
			errprint("port:%d does not have video format(index:%d).\n",
				(int)pf_video->nPortIndex, (int)pf_video->nIndex);
			err = OMX_ErrorNoMore;
			break;
		}

		{
			const OMX_VIDEO_PARAM_PORTFORMATTYPE *pf_sup = pf_found->get_format_video();
			OMX_VIDEO_PARAM_PORTFORMATTYPE tmp;

			tmp = *pf_sup;
			tmp.nSize      = pf_video->nSize;
			tmp.nVersion   = pf_video->nVersion;
			tmp.nPortIndex = pf_video->nPortIndex;
			tmp.nIndex     = pf_video->nIndex;

			*pf_video = tmp;
		}

		err = OMX_ErrorNone;
		break;
	}
	case OMX_IndexParamImagePortFormat: {
		OMX_IMAGE_PARAM_PORTFORMATTYPE *pf_image = static_cast<OMX_IMAGE_PARAM_PORTFORMATTYPE *>(ptr);

		err = check_omx_header(pf_image, sizeof(OMX_IMAGE_PARAM_PORTFORMATTYPE));
		if (err != OMX_ErrorNone) {
			errprint("Invalid header.\n");
			break;
		}

		port_found = find_port(pf_image->nPortIndex);
		if (port_found == nullptr) {
			errprint("Invalid port:%d\n", (int)pf_image->nPortIndex);
			err = OMX_ErrorBadPortIndex;
			break;
		}

		pf_found = port_found->get_port_format(pf_image->nIndex);
		if (pf_found == nullptr || pf_found->get_format_image() == nullptr) {
			errprint("port:%d does not have image format(index:%d).\n",
				(int)pf_image->nPortIndex, (int)pf_image->nIndex);
			err = OMX_ErrorNoMore;
			break;
		}

		{
			const OMX_IMAGE_PARAM_PORTFORMATTYPE *pf_sup = pf_found->get_format_image();
			OMX_IMAGE_PARAM_PORTFORMATTYPE tmp;

			tmp = *pf_sup;
			tmp.nSize      = pf_image->nSize;
			tmp.nVersion   = pf_image->nVersion;
			tmp.nPortIndex = pf_image->nPortIndex;
			tmp.nIndex     = pf_image->nIndex;

			*pf_image = tmp;
		}

		err = OMX_ErrorNone;
		break;
	}
	case OMX_IndexParamOtherPortFormat: {
		OMX_OTHER_PARAM_PORTFORMATTYPE *pf_other = static_cast<OMX_OTHER_PARAM_PORTFORMATTYPE *>(ptr);

		err = check_omx_header(pf_other, sizeof(OMX_OTHER_PARAM_PORTFORMATTYPE));
		if (err != OMX_ErrorNone) {
			errprint("Invalid header.\n");
			break;
		}

		port_found = find_port(pf_other->nPortIndex);
		if (port_found == nullptr) {
			errprint("Invalid port:%d\n", (int)pf_other->nPortIndex);
			err = OMX_ErrorBadPortIndex;
			break;
		}

		pf_found = port_found->get_port_format(pf_other->nIndex);
		if (pf_found == nullptr || pf_found->get_format_other() == nullptr) {
			errprint("port:%d does not have other format(index:%d).\n",
				(int)pf_other->nPortIndex, (int)pf_other->nIndex);
			err = OMX_ErrorNoMore;
			break;
		}

		{
			const OMX_OTHER_PARAM_PORTFORMATTYPE *pf_sup = pf_found->get_format_other();
			OMX_OTHER_PARAM_PORTFORMATTYPE tmp;

			tmp = *pf_sup;
			tmp.nSize      = pf_other->nSize;
			tmp.nVersion   = pf_other->nVersion;
			tmp.nPortIndex = pf_other->nPortIndex;
			tmp.nIndex     = pf_other->nIndex;

			*pf_other = tmp;
		}

		err = OMX_ErrorNone;
		break;
	}
	default:
		errprint("unsupported index:%d.\n", (int)nParamIndex);
		err = OMX_ErrorUnsupportedIndex;
		break;
	}

	return err;
}

OMX_ERRORTYPE component::SetParameter(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure)
{
	scoped_log_begin;
	void *ptr = nullptr;
	port *port_found = nullptr;
	OMX_ERRORTYPE err;

	dprint("indextype:%s, name:%s\n", omx_enum_name::get_OMX_INDEXTYPE_name(nParamIndex), get_name());

	ptr = pComponentParameterStructure;

	//OpenMAX IL 1.2.0: 8.2 Mandatory Port Parameters
	switch (nParamIndex) {
	case OMX_IndexParamPortDefinition: {
		OMX_PARAM_PORTDEFINITIONTYPE *def = static_cast<OMX_PARAM_PORTDEFINITIONTYPE *>(ptr);

		err = check_omx_header(def, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
		if (err != OMX_ErrorNone) {
			errprint("Invalid header.\n");
			break;
		}

		port_found = find_port(def->nPortIndex);
		if (port_found == nullptr) {
			errprint("Invalid port:%d\n", (int)def->nPortIndex);
			err = OMX_ErrorBadPortIndex;
			break;
		}

		if (port_found->get_buffer_count_min() > def->nBufferCountActual) {
			errprint("too few buffers:%d, min:%d\n",
				(int)port_found->get_buffer_count_min(),
				(int)def->nBufferCountActual);
			err = OMX_ErrorBadPortIndex;
			break;
		}

		err = port_found->set_definition_from_client(*def);

		break;
	}
	case OMX_IndexParamCompBufferSupplier: {
		OMX_PARAM_BUFFERSUPPLIERTYPE *supply = static_cast<OMX_PARAM_BUFFERSUPPLIERTYPE *>(ptr);
		bool changed;

		err = check_omx_header(supply, sizeof(OMX_PARAM_BUFFERSUPPLIERTYPE));
		if (err != OMX_ErrorNone) {
			errprint("Invalid header.\n");
			break;
		}

		port_found = find_port(supply->nPortIndex);
		if (port_found == nullptr) {
			errprint("Invalid port:%d\n", (int)supply->nPortIndex);
			err = OMX_ErrorBadPortIndex;
			break;
		}

		if (!port_found->get_tunneled()) {
			errprint("Not tunneled mode (port:%d)\n", (int)supply->nPortIndex);
			err = OMX_ErrorBadPortIndex;
			break;
		}

		if (supply->eBufferSupplier == OMX_BufferSupplyUnspecified) {
			//Cannot overwrite 'unspecified' on other mode
			err = OMX_ErrorBadParameter;
			break;
		}

		changed = false;
		switch (port_found->get_dir()) {
		case OMX_DirInput:
			if (port_found->get_tunneled_supplier() &&
				supply->eBufferSupplier == OMX_BufferSupplyOutput) {
				//Port is SupplyInput, change to user
				dprint("Input port: input supplier -> output supplier.\n");
				port_found->set_tunneled_supplier(OMX_FALSE);
				changed = true;
			} else if (!port_found->get_tunneled_supplier() &&
				supply->eBufferSupplier == OMX_BufferSupplyInput) {
				//Port is SupplyOutput, change to supplier
				dprint("Input port: output supplier -> input supplier.\n");
				port_found->set_tunneled_supplier(OMX_TRUE);
				changed = true;
			}
			break;
		case OMX_DirOutput:
			if (port_found->get_tunneled_supplier() &&
				supply->eBufferSupplier == OMX_BufferSupplyInput) {
				//Port is SupplyOutput, change to user
				dprint("Output port: output supplier -> input supplier.\n");
				port_found->set_tunneled_supplier(OMX_FALSE);
				changed = true;
			} else if (!port_found->get_tunneled_supplier() &&
				supply->eBufferSupplier == OMX_BufferSupplyOutput) {
				//Port is SupplyInput, change to supplier
				dprint("Output port: input supplier -> output supplier.\n");
				port_found->set_tunneled_supplier(OMX_TRUE);
				changed = true;
			}
			break;
		default:
			errprint("Bug, do not reach here.\n");
			err = OMX_ErrorUndefined;
			break;
		}

		if (changed) {
			OMX_PARAM_BUFFERSUPPLIERTYPE bufsup;

			bufsup.nSize           = sizeof(bufsup);
			bufsup.nVersion        = supply->nVersion;
			bufsup.nPortIndex      = port_found->get_tunneled_port();
			bufsup.eBufferSupplier = supply->eBufferSupplier;

			err = OMX_SetParameter(port_found->get_tunneled_component(),
				OMX_IndexParamCompBufferSupplier, &bufsup);
			if (err != OMX_ErrorNone) {
				errprint("Change buffer supplier failed (comp:%p, port:%d).\n",
					port_found->get_tunneled_component(),
					(int)port_found->get_tunneled_port());
				break;
			}
		}

		break;
	}
	case OMX_IndexParamAudioInit:
	case OMX_IndexParamVideoInit:
	case OMX_IndexParamImageInit:
	case OMX_IndexParamOtherInit: {
		OMX_PORT_PARAM_TYPE *param = static_cast<OMX_PORT_PARAM_TYPE *>(ptr);

		err = check_omx_header(param, sizeof(OMX_PORT_PARAM_TYPE));
		if (err != OMX_ErrorNone) {
			errprint("Invalid header.\n");
			break;
		}

		//FIXME: not supported
		err = OMX_ErrorUnsupportedIndex;

		break;
	}
	//case OMX_IndexParamStandardComponentRole:
	//	static_cast<OMX_PARAM_COMPONENTROLETYPE *>(ptr);
	//	break;
	case OMX_IndexParamAudioPortFormat: {
		OMX_AUDIO_PARAM_PORTFORMATTYPE *pf_audio = static_cast<OMX_AUDIO_PARAM_PORTFORMATTYPE *>(ptr);

		err = check_omx_header(pf_audio, sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE));
		if (err != OMX_ErrorNone) {
			errprint("Invalid header.\n");
			break;
		}

		port_found = find_port(pf_audio->nPortIndex);
		if (port_found == nullptr) {
			errprint("Invalid port:%d\n", (int)pf_audio->nPortIndex);
			err = OMX_ErrorBadPortIndex;
			break;
		}

		err = port_found->set_default_format(port_format(*pf_audio));

		break;
	}
	case OMX_IndexParamVideoPortFormat: {
		OMX_VIDEO_PARAM_PORTFORMATTYPE *pf_video = static_cast<OMX_VIDEO_PARAM_PORTFORMATTYPE *>(ptr);

		err = check_omx_header(pf_video, sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE));
		if (err != OMX_ErrorNone) {
			errprint("Invalid header.\n");
			break;
		}

		port_found = find_port(pf_video->nPortIndex);
		if (port_found == nullptr) {
			errprint("Invalid port:%d\n", (int)pf_video->nPortIndex);
			err = OMX_ErrorBadPortIndex;
			break;
		}

		err = port_found->set_default_format(port_format(*pf_video));

		break;
	}
	case OMX_IndexParamImagePortFormat: {
		OMX_IMAGE_PARAM_PORTFORMATTYPE *pf_image = static_cast<OMX_IMAGE_PARAM_PORTFORMATTYPE *>(ptr);

		err = check_omx_header(pf_image, sizeof(OMX_IMAGE_PARAM_PORTFORMATTYPE));
		if (err != OMX_ErrorNone) {
			errprint("Invalid header.\n");
			break;
		}

		port_found = find_port(pf_image->nPortIndex);
		if (port_found == nullptr) {
			errprint("Invalid port:%d\n", (int)pf_image->nPortIndex);
			err = OMX_ErrorBadPortIndex;
			break;
		}

		err = port_found->set_default_format(port_format(*pf_image));

		break;
	}
	case OMX_IndexParamOtherPortFormat: {
		OMX_OTHER_PARAM_PORTFORMATTYPE *pf_other = static_cast<OMX_OTHER_PARAM_PORTFORMATTYPE *>(ptr);

		err = check_omx_header(pf_other, sizeof(OMX_OTHER_PARAM_PORTFORMATTYPE));
		if (err != OMX_ErrorNone) {
			errprint("Invalid header.\n");
			break;
		}

		port_found = find_port(pf_other->nPortIndex);
		if (port_found == nullptr) {
			errprint("Invalid port:%d\n", (int)pf_other->nPortIndex);
			err = OMX_ErrorBadPortIndex;
			break;
		}

		err = port_found->set_default_format(port_format(*pf_other));

		break;
	}
	default:
		errprint("unsupported index:%d.\n", (int)nParamIndex);
		err = OMX_ErrorUnsupportedIndex;
		break;
	}

	return err;
}

OMX_ERRORTYPE component::GetConfig(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure)
{
	scoped_log_begin;

	//do nothing

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::SetConfig(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure)
{
	scoped_log_begin;

	//do nothing

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::GetExtensionIndex(OMX_HANDLETYPE hComponent, OMX_STRING cParameterName, OMX_INDEXTYPE *pIndexType)
{
	scoped_log_begin;

	//do nothing

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::GetState(OMX_HANDLETYPE hComponent, OMX_STATETYPE *pState)
{
	scoped_log_begin;

	if (pState == nullptr) {
		errprint("pState is nullptr.\n");
		return OMX_ErrorBadParameter;
	}
	*pState = get_state();

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::ComponentTunnelRequest(OMX_HANDLETYPE hComponent, OMX_U32 nPort, OMX_HANDLETYPE hTunneledComp, OMX_U32 nTunneledPort, OMX_TUNNELSETUPTYPE *pTunnelSetup)
{
	scoped_log_begin;
	port *port_found = nullptr;
	OMX_ERRORTYPE err;

	port_found = find_port(nPort);
	if (port_found == nullptr) {
		errprint("Invalid port:%d\n", (int)nPort);
		return OMX_ErrorBadPortIndex;
	}

	err = port_found->component_tunnel_request(hTunneledComp, nTunneledPort,
		pTunnelSetup);

	return err;
}

OMX_ERRORTYPE component::UseBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8 *pBuffer)
{
	scoped_log_begin;
	port *port_found = nullptr;
	OMX_ERRORTYPE err;

	port_found = find_port(nPortIndex);
	if (port_found == nullptr) {
		errprint("Invalid port:%d\n", (int)nPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	err = port_found->use_buffer(ppBufferHdr,
		pAppPrivate, nSizeBytes, pBuffer);

	return err;
}

OMX_ERRORTYPE component::AllocateBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBuffer, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes)
{
	scoped_log_begin;
	port *port_found = nullptr;
	OMX_ERRORTYPE err;

	port_found = find_port(nPortIndex);
	if (port_found == nullptr) {
		errprint("Invalid port:%d\n", (int)nPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	err = port_found->allocate_buffer(ppBuffer,
		pAppPrivate, nSizeBytes);

	return err;
}

OMX_ERRORTYPE component::FreeBuffer(OMX_HANDLETYPE hComponent, OMX_U32 nPortIndex, OMX_BUFFERHEADERTYPE *pBuffer)
{
	scoped_log_begin;
	port *port_found = nullptr;
	OMX_ERRORTYPE err;

	port_found = find_port(nPortIndex);
	if (port_found == nullptr) {
		errprint("Invalid port:%d\n", (int)nPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	err = port_found->free_buffer(pBuffer);

	return err;
}

OMX_ERRORTYPE component::EmptyThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE *pBuffer)
{
	port *port_found = nullptr;
	OMX_ERRORTYPE err;

	switch (get_state()) {
	case OMX_StateIdle:
	case OMX_StateExecuting:
	case OMX_StatePause:
		//OK
		break;
	default:
		//NG
		errprint("Invalid state:%s.\n",
			omx_enum_name::get_OMX_STATETYPE_name(get_state()));
		return OMX_ErrorInvalidState;
	}

	port_found = find_port(pBuffer->nInputPortIndex);
	if (port_found == nullptr) {
		errprint("Invalid input port:%d\n",
			(int)pBuffer->nInputPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	err = port_found->empty_buffer(pBuffer);

	return err;
}

OMX_ERRORTYPE component::FillThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE *pBuffer)
{
	port *port_found = nullptr;
	OMX_ERRORTYPE err;

	switch (get_state()) {
	case OMX_StateIdle:
	case OMX_StateExecuting:
	case OMX_StatePause:
		//OK
		break;
	default:
		//NG
		errprint("Invalid state:%s.\n",
			omx_enum_name::get_OMX_STATETYPE_name(get_state()));
		return OMX_ErrorInvalidState;
	}

	port_found = find_port(pBuffer->nOutputPortIndex);
	if (port_found == nullptr) {
		errprint("Invalid output port:%d\n",
			(int)pBuffer->nOutputPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	err = port_found->fill_buffer(pBuffer);

	return err;
}

OMX_ERRORTYPE component::SetCallbacks(OMX_HANDLETYPE hComponent, OMX_CALLBACKTYPE *pCallbacks, OMX_PTR pAppData)
{
	scoped_log_begin;

	switch (get_state()) {
	case OMX_StateLoaded:
		//OK
		break;
	default:
		//NG
		errprint("Invalid state:%s.\n",
			omx_enum_name::get_OMX_STATETYPE_name(get_state()));
		return OMX_ErrorInvalidState;
	}

	if (pCallbacks == nullptr) {
		return OMX_ErrorBadParameter;
	}

	omx_cbs = *pCallbacks;
	omx_cbs_priv = pAppData;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::ComponentDeInit(OMX_HANDLETYPE hComponent)
{
	scoped_log_begin;

	//do nothing

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::UseEGLImage(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, void *eglImage)
{
	scoped_log_begin;

	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

OMX_ERRORTYPE component::ComponentRoleEnum(OMX_HANDLETYPE hComponent, OMX_U8 *cRole, OMX_U32 nIndex)
{
	scoped_log_begin;

	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}


//----------
//OpenMAX callback function wrappers
//----------

OMX_ERRORTYPE component::EventHandler(OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	scoped_log_begin;
	bool dump_all = false;
	OMX_ERRORTYPE err;

	switch (eEvent) {
	case OMX_EventCmdComplete: {
		OMX_COMMANDTYPE cmd = static_cast<OMX_COMMANDTYPE>(nData1);
		OMX_STATETYPE sta = static_cast<OMX_STATETYPE>(nData2);
		OMX_ERRORTYPE ret = static_cast<OMX_ERRORTYPE>(reinterpret_cast<intptr_t>(pEventData));

		switch (cmd) {
		case OMX_CommandStateSet:
			dprint("eEvent:%s, Command:%d(%s), Reached:%d(%s), Error:%d(%s).\n",
				omx_enum_name::get_OMX_EVENTTYPE_name(eEvent),
				(int)cmd, omx_enum_name::get_OMX_COMMANDTYPE_name(cmd),
				(int)sta, omx_enum_name::get_OMX_STATETYPE_name(sta),
				(int)ret, omx_enum_name::get_OMX_ERRORTYPE_name(ret));
			break;
		case OMX_CommandFlush:
		case OMX_CommandPortDisable:
		case OMX_CommandPortEnable:
		case OMX_CommandMarkBuffer:
			dprint("eEvent:%s, Command:%d(%s), Port:%d, Error:%d(%s).\n",
				omx_enum_name::get_OMX_EVENTTYPE_name(eEvent),
				(int)cmd, omx_enum_name::get_OMX_COMMANDTYPE_name(cmd),
				(int)nData2,
				(int)ret, omx_enum_name::get_OMX_ERRORTYPE_name(ret));
			break;
		default:
			//Unknown
			dump_all = true;
			break;
		}
		break;
	}
	case OMX_EventError:
		dprint("eEvent:%s, Error:%d(%s), Additional:0x%08x\n",
			omx_enum_name::get_OMX_EVENTTYPE_name(eEvent),
			(int)nData1, omx_enum_name::get_OMX_ERRORTYPE_name((OMX_ERRORTYPE)nData1),
			(int)nData2);
		break;
	case OMX_EventMark:
		dprint("eEvent:%s, Data:0x%p\n",
			omx_enum_name::get_OMX_EVENTTYPE_name(eEvent),
			pEventData);
		break;
	case OMX_EventPortSettingsChanged:
		dprint("eEvent:%s, Port:%d, Param:%d(%s).\n",
			omx_enum_name::get_OMX_EVENTTYPE_name(eEvent),
			(int)nData1,
			(int)nData2, omx_enum_name::get_OMX_INDEXTYPE_name((OMX_INDEXTYPE)nData2));
		break;
	case OMX_EventBufferFlag:
		dprint("eEvent:%s, Port:%d, Flags:0x%08x.\n",
			omx_enum_name::get_OMX_EVENTTYPE_name(eEvent),
			(int)nData1,
			(int)nData2);
		break;
	case OMX_EventResourcesAcquired:
	case OMX_EventComponentResumed:
	case OMX_EventDynamicResourcesAvailable:
	//case OMX_EventPortFormatDetected:
		dump_all = true;
		break;
	//case OMX_EventIndexSettingChanged:
	//	dprint("eEvent:%s, Port:%d, Param:%d(%s).\n",
	//		omx_enum_name::get_OMX_EVENTTYPE_name(eEvent),
	//		(int)nData1,
	//		(int)nData2, omx_enum_name::get_OMX_INDEXTYPE_name((OMX_INDEXTYPE)nData2));
	//	break;
	//case OMX_EventPortNeedsDisable:
	//case OMX_EventPortNeedsFlush:
	//	dprint("eEvent:%s, Port:%d, Requested:%d.\n",
	//		omx_enum_name::get_OMX_EVENTTYPE_name(eEvent),
	//		(int)nData1,
	//		(int)nData2);
	//	break;
	default:
		//Unknown
		dump_all = true;
		break;
	}

	if (dump_all) {
		dprint("eEvent:%s, nData1:0x%08x, nData2:0x%08x, pEventData:0x%p\n",
			omx_enum_name::get_OMX_EVENTTYPE_name(eEvent),
			(int)nData1, (int)nData2, pEventData);
	}

	err = omx_cbs.EventHandler(get_omx_component(), omx_cbs_priv,
		eEvent, nData1, nData2, pEventData);

	return err;
}

OMX_ERRORTYPE component::EmptyBufferDone(OMX_BUFFERHEADERTYPE *pBuffer)
{
	OMX_ERRORTYPE err;

	err = omx_cbs.EmptyBufferDone(get_omx_component(),
		omx_cbs_priv, pBuffer);

	return err;
}

OMX_ERRORTYPE component::EmptyBufferDone(port_buffer *pb)
{
	//EOS detected
	if (pb->header->nFlags & OMX_BUFFERFLAG_EOS) {
		EventHandler(OMX_EventBufferFlag,
			pb->p->get_port_index(), pb->header->nFlags, nullptr);
	}

	return EmptyBufferDone(pb->header);
}

OMX_ERRORTYPE component::FillBufferDone(OMX_BUFFERHEADERTYPE *pBuffer)
{
	OMX_ERRORTYPE err;

	err = omx_cbs.FillBufferDone(get_omx_component(),
		omx_cbs_priv, pBuffer);

	return err;
}

OMX_ERRORTYPE component::FillBufferDone(port_buffer *pb)
{
	//EOS detected
	if (pb->header->nFlags & OMX_BUFFERFLAG_EOS) {
		EventHandler(OMX_EventBufferFlag,
			pb->p->get_port_index(), pb->header->nFlags, nullptr);
	}

	return FillBufferDone(pb->header);
}


/*
 * protected functions (Maybe override by derived classes)
 */

OMX_ERRORTYPE component::allocate_static_resouces()
{
	scoped_log_begin;
	//do nothing

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::free_static_resouces()
{
	scoped_log_begin;
	//do nothing

	return OMX_ErrorNone;
}

void component::run()
{
	scoped_log_begin;
	//do nothing
}

OMX_U32 component::get_audio_ports()
{
	return 0;
}

OMX_U32 component::get_audio_start_port()
{
	return 0;
}

OMX_U32 component::get_video_ports()
{
	return 0;
}

OMX_U32 component::get_video_start_port()
{
	return 0;
}

OMX_U32 component::get_image_ports()
{
	return 0;
}

OMX_U32 component::get_image_start_port()
{
	return 0;
}

OMX_U32 component::get_other_ports()
{
	return 0;
}

OMX_U32 component::get_other_start_port()
{
	return 0;
}


/*
 * protected functions (API for derived classes)
 */

void component::error_if_broken(std::unique_lock<std::mutex>& lock) const
{
	if (broken) {
		std::string msg(__func__);
		msg += ": interrupted.";
		throw std::runtime_error(msg);
	}
}

void *component::accept_command()
{
	scoped_log_begin;
	OMX_MF_CMD cmd;
	OMX_STATETYPE new_state;
	OMX_U32 port_index;
	//callback するかしないか
	bool f_callback;
	//EventHandler の引数
	OMX_U32 data1 = 0, data2 = 0;
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

			//command_flush() had already called
			//EventHandler() callback function.
			f_callback = false;

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
			event_data = nullptr;

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

OMX_ERRORTYPE component::command_state_set(OMX_STATETYPE new_state)
{
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

OMX_ERRORTYPE component::command_state_set_to_invalid()
{
	scoped_log_begin;

	set_state(OMX_StateInvalid);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::command_state_set_to_loaded()
{
	scoped_log_begin;
	OMX_ERRORTYPE err;

	switch (get_state()) {
	case OMX_StateLoaded:
		err = OMX_ErrorSameState;
		break;
	case OMX_StateIdle:
		try {
			err = command_state_set_to_loaded_from_idle();
		} catch (const std::runtime_error& e) {
			errprint("runtime_error: %s\n", e.what());
			err = OMX_ErrorInsufficientResources;
		}
		break;
	default:
		errprint("Invalid state:%s.\n",
			omx_enum_name::get_OMX_STATETYPE_name(get_state()));
		err = OMX_ErrorInvalidState;
		break;
	}

	if (err == OMX_ErrorNone) {
		set_state(OMX_StateLoaded);
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::command_state_set_to_loaded_from_idle()
{
	scoped_log_begin;
	OMX_ERRORTYPE err, errtmp;

	err = OMX_ErrorNone;

	//Prepare to flush all ports
	for (auto it = map_ports.begin(); it != map_ports.end(); it++) {
		if (!it->second.get_enabled()) {
			continue;
		}
		it->second.begin_flush();
	}

	//Stop & wait main thread
	stop_running();
	th_main->join();
	delete th_main;
	th_main = nullptr;

	//Free static resources of component
	err = free_static_resouces();
	if (err != OMX_ErrorNone) {
		errprint("Failed to free_static_resouces().\n");
	}

	for (auto it = map_ports.begin(); it != map_ports.end(); it++) {
		if (!it->second.get_enabled()) {
			continue;
		}
		it->second.flush_buffers();
	}

	//Wait for all buffer returned to supplier
	wait_all_port_buffer_returned();

	for (auto it = map_ports.begin(); it != map_ports.end(); it++) {
		if (!it->second.get_enabled()) {
			continue;
		}
		it->second.end_flush();
	}

	for (auto it = map_ports.begin(); it != map_ports.end(); it++) {
		if (!it->second.get_enabled()) {
			//Disabled port, skip
			continue;
		}

		if (it->second.get_tunneled() && it->second.get_tunneled_supplier()) {
			//Tunneled and Supplier port
			//Free buffers for tunneled port
			errtmp = it->second.free_tunnel_buffers();
			if (errtmp != OMX_ErrorNone) {
				errprint("Failed to free_tunnel_buffers().\n");
				err = errtmp;
			}
		} else {
			//Other
			//Wait for all enabled port to be "no buffer"
			it->second.wait_no_buffer(OMX_TRUE);
		}
	}

	return err;
}

OMX_ERRORTYPE component::command_state_set_to_idle()
{
	scoped_log_begin;
	OMX_ERRORTYPE err;

	switch (get_state()) {
	case OMX_StateLoaded:
		try {
			err = command_state_set_to_idle_from_loaded();
		} catch (const std::runtime_error& e) {
			errprint("runtime_error: %s\n", e.what());
			err = OMX_ErrorInsufficientResources;
		}
		break;
	case OMX_StateIdle:
		err = OMX_ErrorSameState;
		break;
	case OMX_StateExecuting:
		try {
			err = command_state_set_to_idle_from_executing();
		} catch (const std::runtime_error& e) {
			errprint("runtime_error: %s\n", e.what());
			err = OMX_ErrorInsufficientResources;
		}
		break;
	default:
		errprint("Invalid state:%s.\n",
			omx_enum_name::get_OMX_STATETYPE_name(get_state()));
		err = OMX_ErrorInvalidState;
		break;
	}

	if (err == OMX_ErrorNone) {
		set_state(OMX_StateIdle);
	}

	return err;
}

OMX_ERRORTYPE component::command_state_set_to_idle_from_loaded()
{
	scoped_log_begin;
	OMX_ERRORTYPE err, errtmp;

	err = OMX_ErrorNone;
	for (auto it = map_ports.begin(); it != map_ports.end(); it++) {
		if (!it->second.get_enabled()) {
			//Disabled port, skip
			continue;
		}

		if (it->second.get_tunneled() && it->second.get_tunneled_supplier()) {
			//Tunneled and Supplier port
			//Allocate buffers for tunneled port
			errtmp = it->second.allocate_tunnel_buffers();
			if (errtmp != OMX_ErrorNone) {
				errprint("Failed to allocate_tunnel_buffer().\n");
				err = errtmp;
			}
		} else if (it->second.get_tunneled() && !it->second.get_tunneled_supplier()) {
			//Tunneled and User port
			//Notify to accept OMX_UseBuffer() calls
			//FIXME: not implemented
			err = OMX_ErrorNone;//OMX_ErrorNotImplemented;
		} else {
			//Other
			//Wait for all enabled port to be populated
			it->second.wait_populated(OMX_TRUE);
		}
	}
	if (err != OMX_ErrorNone) {
		return err;
	}

	//Allocate static resources of component
	err = allocate_static_resouces();
	if (err != OMX_ErrorNone) {
		errprint("Failed to allocate_static_resouces().\n");
		return err;
	}

	try {
		//Start main thread
		th_main = new std::thread(component_thread_main, get_omx_component());
	} catch (const std::bad_alloc& e) {
		errprint("failed to create main thread '%s'.\n", e.what());
		return OMX_ErrorInsufficientResources;
	}

	return err;
}

OMX_ERRORTYPE component::command_state_set_to_idle_from_executing()
{
	scoped_log_begin;

	//Flushing all ports
	for (auto it = map_ports.begin(); it != map_ports.end(); it++) {
		if (!it->second.get_enabled()) {
			continue;
		}
		it->second.begin_flush();
		it->second.flush_buffers();
	}

	//Wait for all buffer returned to supplier
	wait_all_port_buffer_returned();

	for (auto it = map_ports.begin(); it != map_ports.end(); it++) {
		if (!it->second.get_enabled()) {
			continue;
		}
		it->second.end_flush();
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::command_state_set_to_executing()
{
	scoped_log_begin;
	OMX_ERRORTYPE err;

	switch (get_state()) {
	case OMX_StateIdle:
		err = command_state_set_to_executing_from_idle();
		break;
	case OMX_StateExecuting:
		err = OMX_ErrorSameState;
		break;
	default:
		errprint("Invalid state:%s.\n",
			omx_enum_name::get_OMX_STATETYPE_name(get_state()));

		err = OMX_ErrorInvalidState;
		break;
	}

	if (err == OMX_ErrorNone) {
		set_state(OMX_StateExecuting);
	}

	return err;
}

OMX_ERRORTYPE component::command_state_set_to_executing_from_idle()
{
	//メイン処理が始まってから状態を切り替える
	{
		std::unique_lock<std::mutex> lock(mut);
		cond.wait(lock, [&] { return broken || should_run(); });
		error_if_broken(lock);
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::command_state_set_to_pause()
{
	scoped_log_begin;

	set_state(OMX_StatePause);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::command_state_set_to_wait_for_resources()
{
	scoped_log_begin;

	set_state(OMX_StateWaitForResources);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::command_flush(OMX_U32 port_index)
{
	scoped_log_begin;
	std::deque<OMX_U32> ev_results;
	port *port_found = nullptr;
	bool success = true;
	OMX_ERRORTYPE err = OMX_ErrorNone, err_handler = OMX_ErrorNone;

	if (port_index == OMX_ALL) {
		try {
			//Flush all ports
			for (auto it = map_ports.begin(); it != map_ports.end(); it++) {
				if (!it->second.get_enabled()) {
					continue;
				}
				it->second.begin_flush();
				it->second.flush_buffers();
			}

			//Wait for all buffer returned to supplier
			wait_all_port_buffer_returned();

			for (auto it = map_ports.begin(); it != map_ports.end(); it++) {
				if (!it->second.get_enabled()) {
					continue;
				}
				it->second.end_flush();
				ev_results.push_back(it->first);
			}
		} catch (const std::runtime_error& e) {
			errprint("runtime_error: %s\n", e.what());
			err = OMX_ErrorInsufficientResources;
			success = false;
		}
	} else {
		//Flushing specify ports
		try {
			port_found = find_port(port_index);
			if (port_found == nullptr || !port_found->get_enabled()) {
				errprint("Invalid or disabled port:%d\n",
					(int)port_index);
				err = OMX_ErrorBadPortIndex;
				success = false;
			} else {
				port_found->begin_flush();
				err = port_found->flush_buffers();

				//Wait for all buffer returned to supplier
				port_found->wait_buffer_returned();

				ev_results.push_back(port_index);
				port_found->end_flush();
			}
		} catch (const std::runtime_error& e) {
			errprint("runtime_error: %s\n", e.what());
			err = OMX_ErrorInsufficientResources;
			success = false;
		}
	}

	//Callback EventHandler(EventError)
	if (!success) {
		err_handler = EventHandler(OMX_EventError,
			err, 0, nullptr);
	}
	if (err_handler != OMX_ErrorNone) {
		errprint("event handler(error) returns error: %s\n",
			omx_enum_name::get_OMX_ERRORTYPE_name(err_handler));
	}

	//Callback EventHandler(CmdComplete)
	for (auto it = ev_results.begin(); it != ev_results.end(); it++) {
		err_handler = EventHandler(OMX_EventCmdComplete,
			OMX_CommandFlush, *it, nullptr);
		if (err_handler != OMX_ErrorNone) {
			errprint("event handler(complete) returns error: %s\n",
				omx_enum_name::get_OMX_ERRORTYPE_name(err_handler));
		}
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::command_port_disable(OMX_U32 port_index)
{
	scoped_log_begin;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::command_port_enable(OMX_U32 port_index)
{
	scoped_log_begin;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE component::command_mark_buffer(OMX_U32 port_index)
{
	scoped_log_begin;

	return OMX_ErrorNone;
}

bool component::insert_port(port& p)
{
	std::pair<component::portmap_t::iterator, bool> ret;

	ret = map_ports.insert(component::portmap_t::value_type(p.get_port_index(), p));

	return ret.second;
}

bool component::erase_port(OMX_U32 index)
{
	size_t ret;

	ret = map_ports.erase(index);

	return (ret != 0);
}

const component::portmap_t& component::get_map_ports() const
{
	return map_ports;
}

component::portmap_t& component::get_map_ports()
{
	return map_ports;
}

OMX_ERRORTYPE component::check_omx_header(const void *p, size_t size) const
{
	const OMX_MF_HEADERTYPE *h = reinterpret_cast<const OMX_MF_HEADERTYPE *>(p);

	if (h == nullptr) {
		errprint("Header is null.\n");
		return OMX_ErrorBadParameter;
	}
	if (h->nSize != size) {
		errprint("Invalid nSize(%d) != size:%d.\n",
			(int)h->nSize, (int)size);
		return OMX_ErrorBadParameter;
	}
	if (h->nVersion.s.nVersionMajor != OMX_MF_IL_MAJOR ||
		h->nVersion.s.nVersionMinor > OMX_MF_IL_MINOR) {
		errprint("Invalid version %d.%d required.\n",
			h->nVersion.s.nVersionMajor,
			h->nVersion.s.nVersionMinor);
		return OMX_ErrorVersionMismatch;
	}

	return OMX_ErrorNone;
}


/*
 * static public functions
 */

component *component::get_instance(OMX_HANDLETYPE hComponent)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	component *comp = (component *) omx_comp->pComponentPrivate;

	return comp;
}


/*
 * static protected functions
 */

void *component::accept_command_thread_main(OMX_COMPONENTTYPE *arg)
{
	scoped_log_begin;
	std::string thname;
	component *comp = get_instance(arg);

	try {
		//スレッド名をつける
		thname = "omx:cmd:";
		thname += comp->get_name();
		set_thread_name(thname.c_str());

		comp->accept_command();
	} catch (const std::runtime_error& e) {
		errprint("runtime_error: %s\n", e.what());
	}

	return nullptr;
}

void *component::component_thread_main(OMX_COMPONENTTYPE *arg)
{
	scoped_log_begin;
	std::string thname;
	component *comp = get_instance(arg);

	try {
		//スレッド名をつける
		thname = "omx:run:";
		thname += comp->get_name();
		set_thread_name(thname.c_str());

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
