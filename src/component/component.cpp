#include <map>
#include <mutex>
#include <condition_variable>
#include <pthread.h>
#include <sys/prctl.h>

#include <OMX_Component.h>
#include <OMX_Core.h>

#include "consts.h"
#include "component/component.hpp"
#include "component/port.hpp"
#include "debug/dbgprint.h"

namespace mf {

//----------
//OpenMAX member functions
//----------

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

} //namespace mf
