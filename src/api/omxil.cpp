
#include <fstream>

#define __OMX_EXPORTS
#include <OMX_Core.h>

#include <omxil_mf/omxil_mf.h>
#include <omxil_mf/component.hpp>

#include "regist/register_component.hpp"
#include "debug/scoped_log.hpp"

//----------------------------------------
//external APIs
//----------------------------------------
extern "C" {

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_Init(void)
{
	scoped_log_begin;
	mf::register_component *rc = mf::register_component::get_instance();

	if (rc->is_init()) {
		errprint("Already inited.\n");
		return OMX_ErrorNone;
	}
	rc->init();

	return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_Deinit(void)
{
	scoped_log_begin;
	mf::register_component *rc = mf::register_component::get_instance();

	if (!rc->is_init()) {
		errprint("Already deinited.\n");
		return OMX_ErrorNone;
	}
	rc->deinit();

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
	mf::register_component *rc = mf::register_component::get_instance();
	mf::register_info *rinfo = nullptr;
	void *ptr = nullptr;
	OMX_COMPONENTTYPE *omx_comp = nullptr;
	OMX_ERRORTYPE result = OMX_ErrorUndefined;

	if (pHandle == nullptr) {
		errprint("Invalid handle %p.\n", pHandle);
		result = OMX_ErrorInvalidComponent;
		goto err_out;
	}

	rinfo = rc->find(cComponentName);
	if (rinfo == nullptr) {
		errprint("Not found component '%s'.\n", cComponentName);
		result = OMX_ErrorInvalidComponentName;
		goto err_out;
	}

	//create component
	omx_comp = new OMX_COMPONENTTYPE;
	ptr = rinfo->comp_info->constructor(omx_comp, cComponentName);
	if (ptr == nullptr) {
		errprint("Failed to create component '%s'.\n", cComponentName);
		result = OMX_ErrorInvalidComponent;
		goto err_out;
	}

	*pHandle = omx_comp;

	return OMX_ErrorNone;

err_out:
	*pHandle = nullptr;
	delete omx_comp;

	return result;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_FreeHandle(OMX_IN OMX_HANDLETYPE hComponent)
{
	scoped_log_begin;
	mf::register_component *rc = mf::register_component::get_instance();
	mf::register_info *rinfo = nullptr;
	OMX_COMPONENTTYPE *omx_comp = nullptr;

	if (hComponent == nullptr) {
		errprint("Invalid component %p.\n", hComponent);
		return OMX_ErrorInvalidComponent;
	}
	omx_comp = (OMX_COMPONENTTYPE *)hComponent;

	//delete component
	if (omx_comp->pComponentPrivate != nullptr) {
		mf::component *comp = mf::component::get_instance(hComponent);
		comp->ComponentDeInit(hComponent);

		rinfo = rc->find(comp->get_component_name().c_str());
		if (rinfo == nullptr) {
			//not found
			errprint("Not found component '%p'.\n", comp->get_component_name().c_str());
			goto err_out;
		}
		rinfo->comp_info->destructor(omx_comp);
	}

err_out:
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

} //extern "C"
