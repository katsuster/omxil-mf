extern "C" {
#include <OMX_Core.h>
}; //extern "C"

#include "entry.hpp"
#include "omxil_mf/component.hpp"
#include "debug/scoped_log.hpp"

extern "C" {

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_Init(void)
{
	scoped_log_begin;
	return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_Deinit(void)
{
	scoped_log_begin;
	return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_ComponentNameEnum(OMX_OUT OMX_STRING cComponentName, OMX_IN OMX_U32 nNameLength, OMX_IN OMX_U32 nIndex)
{
	scoped_log_begin;
	OMX_U32 num_max = 0;

	if (nIndex > num_max) {
		return OMX_ErrorNoMore;
	}

	return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_GetHandle(OMX_OUT OMX_HANDLETYPE* pHandle, OMX_IN OMX_STRING cComponentName, OMX_IN OMX_PTR pAppData, OMX_IN OMX_CALLBACKTYPE* pCallBacks)
{
	scoped_log_begin;

	if (pHandle == nullptr) {
		return OMX_ErrorInvalidComponent;
	}
	OMX_COMPONENTTYPE *omx_comp = nullptr;

	omx_comp = new OMX_COMPONENTTYPE;
	new mf::component(omx_comp, (OMX_STRING)"test");

	*pHandle = omx_comp;

	return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_FreeHandle(OMX_IN OMX_HANDLETYPE hComponent)
{
	scoped_log_begin;

	if (hComponent == nullptr) {
		return OMX_ErrorInvalidComponent;
	}
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *)hComponent;

	if (omx_comp->pComponentPrivate != nullptr) {
		mf::component *comp = mf::component::get_instance(hComponent);
		comp->ComponentDeInit(hComponent);
	}
	delete(omx_comp);

	return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_SetupTunnel(OMX_IN OMX_HANDLETYPE hOutput, OMX_IN OMX_U32 nPortOutput, OMX_IN OMX_HANDLETYPE hInput, OMX_IN OMX_U32 nPortInput)
{
	scoped_log_begin;

	if (hOutput == nullptr || hInput == nullptr) {
		return OMX_ErrorInvalidComponent;
	}

	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_GetContentPipe(OMX_OUT OMX_HANDLETYPE *hPipe, OMX_IN OMX_STRING szURI)
{
	scoped_log_begin;

	if (hPipe == nullptr) {
		return OMX_ErrorInvalidComponent;
	}

	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_GetComponentsOfRole(OMX_IN OMX_STRING role, OMX_INOUT OMX_U32 *pNumComps, OMX_INOUT OMX_U8 **compNames)
{
	scoped_log_begin;
	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_GetRolesOfComponent(OMX_IN OMX_STRING compName, OMX_INOUT OMX_U32 *pNumRoles, OMX_OUT OMX_U8 **roles)
{
	scoped_log_begin;
	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

}; //extern "C"
