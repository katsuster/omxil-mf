#include <stdexcept>

#include <OMX_Component.h>
#include <OMX_Core.h>

#include <omxil_mf/omx_reflector.hpp>

#include "api/consts.h"
#include "util/omx_enum_name.hpp"
#include "debug/scoped_log.hpp"

namespace mf {

omx_reflector::omx_reflector(OMX_COMPONENTTYPE *c, const char *cname)
		: omx_comp(c), omx_name(cname) {
	scoped_log_begin;

	if (c == NULL) {
		std::string msg(__func__);
		msg += ": failed to construct component.";
		throw std::runtime_error(msg);
	}

	//set this component pointer
	omx_comp->pComponentPrivate      = this;

	//set OpenMAX member functions
	omx_comp->GetComponentVersion    = comp_GetComponentVersion;
	omx_comp->SendCommand            = comp_SendCommand;
	omx_comp->GetParameter           = comp_GetParameter;
	omx_comp->SetParameter           = comp_SetParameter;
	omx_comp->GetConfig              = comp_GetConfig;
	omx_comp->SetConfig              = comp_SetConfig;
	omx_comp->GetExtensionIndex      = comp_GetExtensionIndex;
	omx_comp->GetState               = comp_GetState;
	omx_comp->ComponentTunnelRequest = comp_ComponentTunnelRequest;
	omx_comp->UseBuffer              = comp_UseBuffer;
	omx_comp->AllocateBuffer         = comp_AllocateBuffer;
	omx_comp->FreeBuffer             = comp_FreeBuffer;
	omx_comp->EmptyThisBuffer        = comp_EmptyThisBuffer;
	omx_comp->FillThisBuffer         = comp_FillThisBuffer;
	omx_comp->SetCallbacks           = comp_SetCallbacks;
	omx_comp->ComponentDeInit        = comp_ComponentDeInit;
	omx_comp->UseEGLImage            = comp_UseEGLImage;
	omx_comp->ComponentRoleEnum      = comp_ComponentRoleEnum;
}

omx_reflector::~omx_reflector() {
	scoped_log_begin;

	omx_comp->pComponentPrivate = nullptr;
}

const char *omx_reflector::get_name() const {
	return "omx_reflector";
}

const OMX_COMPONENTTYPE *omx_reflector::get_omx_component() const {
	return omx_comp;
}

OMX_COMPONENTTYPE *omx_reflector::get_omx_component() {
	return omx_comp;
}

const std::string& omx_reflector::get_component_name() const {
	return omx_name;
}


/* 
 * static protected functions
 */
omx_reflector *omx_reflector::get_instance(OMX_HANDLETYPE hComponent) {
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	omx_reflector *comp = (omx_reflector *) omx_comp->pComponentPrivate;

	return comp;
}


//----------
//OpenMAX member function reflectors
//----------

OMX_ERRORTYPE omx_reflector::comp_GetComponentVersion(OMX_HANDLETYPE hComponent, OMX_STRING pComponentName, OMX_VERSIONTYPE *pComponentVersion, OMX_VERSIONTYPE *pSpecVersion, OMX_UUIDTYPE *pComponentUUID) {
	scoped_log_begin;

	if (hComponent == nullptr) {
		return OMX_ErrorInvalidComponent;
	}
	omx_reflector *comp = get_instance(hComponent);

	return comp->GetComponentVersion(hComponent, pComponentName, pComponentVersion, pSpecVersion, pComponentUUID);
}

OMX_ERRORTYPE omx_reflector::comp_SendCommand(OMX_HANDLETYPE hComponent, OMX_COMMANDTYPE Cmd, OMX_U32 nParam1, OMX_PTR pCmdData) {
	scoped_log_begin;

	if (hComponent == nullptr) {
		return OMX_ErrorInvalidComponent;
	}
	omx_reflector *comp = get_instance(hComponent);

	return comp->SendCommand(hComponent, Cmd, nParam1, pCmdData);
}

OMX_ERRORTYPE omx_reflector::comp_GetParameter(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) {
	scoped_log_begin;

	if (hComponent == nullptr) {
		return OMX_ErrorInvalidComponent;
	}
	omx_reflector *comp = get_instance(hComponent);

	return comp->GetParameter(hComponent, nParamIndex, pComponentParameterStructure);
}

OMX_ERRORTYPE omx_reflector::comp_SetParameter(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentParameterStructure) {
	scoped_log_begin;

	if (hComponent == nullptr) {
		return OMX_ErrorInvalidComponent;
	}
	omx_reflector *comp = get_instance(hComponent);

	return comp->SetParameter(hComponent, nIndex, pComponentParameterStructure);
}

OMX_ERRORTYPE omx_reflector::comp_GetConfig(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure) {
	scoped_log_begin;

	if (hComponent == nullptr) {
		return OMX_ErrorInvalidComponent;
	}
	omx_reflector *comp = get_instance(hComponent);

	return comp->GetConfig(hComponent, nIndex, pComponentConfigStructure);
}

OMX_ERRORTYPE omx_reflector::comp_SetConfig(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure) {
	scoped_log_begin;

	if (hComponent == nullptr) {
		return OMX_ErrorInvalidComponent;
	}
	omx_reflector *comp = get_instance(hComponent);

	return comp->SetConfig(hComponent, nIndex, pComponentConfigStructure);
}

OMX_ERRORTYPE omx_reflector::comp_GetExtensionIndex(OMX_HANDLETYPE hComponent, OMX_STRING cParameterName, OMX_INDEXTYPE *pIndexType) {
	scoped_log_begin;

	if (hComponent == nullptr) {
		return OMX_ErrorInvalidComponent;
	}
	omx_reflector *comp = get_instance(hComponent);

	return comp->GetExtensionIndex(hComponent, cParameterName, pIndexType);
}

OMX_ERRORTYPE omx_reflector::comp_GetState(OMX_HANDLETYPE hComponent, OMX_STATETYPE *pState) {
	scoped_log_begin;

	if (hComponent == nullptr) {
		return OMX_ErrorInvalidComponent;
	}
	omx_reflector *comp = get_instance(hComponent);

	return comp->GetState(hComponent, pState);
}

OMX_ERRORTYPE omx_reflector::comp_ComponentTunnelRequest(OMX_HANDLETYPE hComponent, OMX_U32 nPort, OMX_HANDLETYPE hTunneledComp, OMX_U32 nTunneledPort, OMX_TUNNELSETUPTYPE *pTunnelSetup) {
	scoped_log_begin;

	if (hComponent == nullptr) {
		return OMX_ErrorInvalidComponent;
	}
	omx_reflector *comp = get_instance(hComponent);

	return comp->ComponentTunnelRequest(hComponent, nPort, hTunneledComp, nTunneledPort, pTunnelSetup);
}

OMX_ERRORTYPE omx_reflector::comp_UseBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8 *pBuffer) {
	scoped_log_begin;

	if (hComponent == nullptr) {
		return OMX_ErrorInvalidComponent;
	}
	omx_reflector *comp = get_instance(hComponent);

	return comp->UseBuffer(hComponent, ppBufferHdr, nPortIndex, pAppPrivate, nSizeBytes, pBuffer);
}

OMX_ERRORTYPE omx_reflector::comp_AllocateBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBuffer, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes) {
	scoped_log_begin;

	if (hComponent == nullptr) {
		return OMX_ErrorInvalidComponent;
	}
	omx_reflector *comp = get_instance(hComponent);

	return comp->AllocateBuffer(hComponent, ppBuffer, nPortIndex, pAppPrivate, nSizeBytes);
}

OMX_ERRORTYPE omx_reflector::comp_FreeBuffer(OMX_HANDLETYPE hComponent, OMX_U32 nPortIndex, OMX_BUFFERHEADERTYPE *pBuffer) {
	scoped_log_begin;

	if (hComponent == nullptr) {
		return OMX_ErrorInvalidComponent;
	}
	omx_reflector *comp = get_instance(hComponent);

	return comp->FreeBuffer(hComponent, nPortIndex, pBuffer);
}

OMX_ERRORTYPE omx_reflector::comp_EmptyThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE *pBuffer) {
	scoped_log_begin;

	if (hComponent == nullptr) {
		return OMX_ErrorInvalidComponent;
	}
	omx_reflector *comp = get_instance(hComponent);

	return comp->EmptyThisBuffer(hComponent, pBuffer);
}

OMX_ERRORTYPE omx_reflector::comp_FillThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE *pBuffer) {
	scoped_log_begin;

	if (hComponent == nullptr) {
		return OMX_ErrorInvalidComponent;
	}
	omx_reflector *comp = get_instance(hComponent);

	return comp->FillThisBuffer(hComponent, pBuffer);
}

OMX_ERRORTYPE omx_reflector::comp_SetCallbacks(OMX_HANDLETYPE hComponent, OMX_CALLBACKTYPE *pCallbacks, OMX_PTR pAppData) {
	scoped_log_begin;

	if (hComponent == nullptr) {
		return OMX_ErrorInvalidComponent;
	}
	omx_reflector *comp = get_instance(hComponent);

	return comp->SetCallbacks(hComponent, pCallbacks, pAppData);
}

OMX_ERRORTYPE omx_reflector::comp_ComponentDeInit(OMX_HANDLETYPE hComponent) {
	scoped_log_begin;

	if (hComponent == nullptr) {
		return OMX_ErrorInvalidComponent;
	}
	omx_reflector *comp = get_instance(hComponent);
	OMX_ERRORTYPE err;

	err = comp->ComponentDeInit(hComponent);

	//NOTE: ComponentDeInit メンバ関数内で delete this; すると、
	//this ポインタが無効になり SEGV の危険あり。
	//必ず static 関数内で delete すること
	dprint("delete component:%s, ptr:%p.\n",
		comp->get_component_name().c_str(), comp);
	delete comp;

	return err;
}

OMX_ERRORTYPE omx_reflector::comp_UseEGLImage(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, void *eglImage) {
	scoped_log_begin;

	if (hComponent == nullptr) {
		return OMX_ErrorInvalidComponent;
	}
	omx_reflector *comp = get_instance(hComponent);

	return comp->UseEGLImage(hComponent, ppBufferHdr, nPortIndex, pAppPrivate, eglImage);
}

OMX_ERRORTYPE omx_reflector::comp_ComponentRoleEnum(OMX_HANDLETYPE hComponent, OMX_U8 *cRole, OMX_U32 nIndex) {
	scoped_log_begin;

	if (hComponent == nullptr) {
		return OMX_ErrorInvalidComponent;
	}
	omx_reflector *comp = get_instance(hComponent);

	return comp->ComponentRoleEnum(hComponent, cRole, nIndex);
}

} //namespace mf
