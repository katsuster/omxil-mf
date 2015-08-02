
#include <fstream>

#define __OMX_EXPORTS
#include <OMX_Core.h>

#include <omxil_mf/omxil_mf.h>
#include <omxil_mf/component.hpp>
#include <omxil_mf/scoped_log.hpp>

#include "regist/register_component.hpp"

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
	mf::register_component *rc = mf::register_component::get_instance();
	mf::register_info *rinfo = nullptr;

	if (nIndex >= rc->size()) {
		return OMX_ErrorNoMore;
	}

	rinfo = rc->find_index(nIndex);
	if (rinfo == nullptr) {
		errprint("Invalid index %d.\n", (int)nIndex);
		return OMX_ErrorBadParameter;
	}

	strncpy(cComponentName, rinfo->name.c_str(), nNameLength - 1);
	cComponentName[nNameLength - 1] = '\0';

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
		result = OMX_ErrorBadParameter;
		goto err_out;
	}

	rinfo = rc->find(cComponentName);
	if (rinfo == nullptr) {
		errprint("Not found component '%s'.\n", cComponentName);
		result = OMX_ErrorInvalidComponentName;
		goto err_out;
	}

	//Create base component
	omx_comp = new OMX_COMPONENTTYPE;

	//Create derived component.
	//Call original constructor of extend libraries
	//instead of 'new comp'.
	//It is because we need not know the class type of
	//derived component.
	ptr = rinfo->comp_info->constructor(omx_comp, cComponentName);
	if (ptr == nullptr) {
		errprint("Failed to create component '%s'.\n", cComponentName);
		result = OMX_ErrorInsufficientResources;
		goto err_out;
	}

	//Set callbacks.
	omx_comp->SetCallbacks(omx_comp, pCallBacks, pAppData);

	//Return component to OpenMAX IL Client
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
		return OMX_ErrorBadParameter;
	}
	omx_comp = (OMX_COMPONENTTYPE *)hComponent;

	//Delete derived component
	if (omx_comp->pComponentPrivate != nullptr) {
		mf::component *comp = mf::component::get_instance(hComponent);
		comp->ComponentDeInit(hComponent);

		//Call original destructor of extend libraries
		//instead of 'delete comp'.
		rinfo = rc->find(comp->get_component_name().c_str());
		if (rinfo == nullptr) {
			//not found
			errprint("Not found component '%p'.\n", comp->get_component_name().c_str());
			goto err_out;
		}
		rinfo->comp_info->destructor(omx_comp);
	}

err_out:
	//Delete base component
	delete(omx_comp);

	return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_SetupTunnel(OMX_IN OMX_HANDLETYPE hOutput, OMX_IN OMX_U32 nPortOutput, OMX_IN OMX_HANDLETYPE hInput, OMX_IN OMX_U32 nPortInput)
{
	scoped_log_begin;

	if (hOutput == nullptr || hInput == nullptr) {
		return OMX_ErrorBadParameter;
	}

	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_GetContentPipe(OMX_OUT OMX_HANDLETYPE *hPipe, OMX_IN OMX_STRING szURI)
{
	scoped_log_begin;

	if (hPipe == nullptr) {
		return OMX_ErrorBadParameter;
	}

	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

//OpenMAX 1.2.0
OMX_API OMX_ERRORTYPE OMX_ComponentOfRoleEnum(OMX_OUT OMX_STRING compName, OMX_IN OMX_STRING role, OMX_IN OMX_U32 nIndex)
{
	scoped_log_begin;
	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

//OpenMAX 1.2.0
OMX_API OMX_ERRORTYPE OMX_RoleOfComponentEnum(OMX_OUT OMX_STRING role, OMX_IN OMX_STRING compName, OMX_IN OMX_U32 nIndex)
{
	scoped_log_begin;
	mf::register_component *rc = mf::register_component::get_instance();
	mf::register_info *rinfo = nullptr;

	if (compName == nullptr) {
		return OMX_ErrorBadParameter;
	}

	rinfo = rc->find(compName);
	if (rinfo == nullptr) {
		errprint("Invalid component name '%s'.\n", compName);
		return OMX_ErrorBadParameter;
	}

	if (nIndex >= rinfo->roles.size()) {
		return OMX_ErrorNoMore;
	}

	strncpy(role, rinfo->roles.at(nIndex).c_str(), OMX_MAX_STRINGNAME_SIZE - 1);
	role[OMX_MAX_STRINGNAME_SIZE - 1] = '\0';

	return OMX_ErrorNone;
}

//OpenMAX 1.1.2
OMX_API OMX_ERRORTYPE OMX_GetComponentsOfRole(OMX_IN OMX_STRING role, OMX_INOUT OMX_U32 *pNumComps, OMX_INOUT OMX_U8 **compNames)
{
	scoped_log_begin;
	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

//OpenMAX 1.1.2
OMX_API OMX_ERRORTYPE OMX_GetRolesOfComponent(OMX_IN OMX_STRING compName, OMX_INOUT OMX_U32 *pNumRoles, OMX_OUT OMX_U8 **roles)
{
	scoped_log_begin;
	char role_name[OMX_MAX_STRINGNAME_SIZE];
	OMX_U32 size;
	OMX_ERRORTYPE result;
	OMX_U32 i;

	if (compName == nullptr || pNumRoles == nullptr || roles == nullptr) {
		return OMX_ErrorBadParameter;
	}

	size = *pNumRoles;
	i = 0;
	*pNumRoles = 0;
	for (i = 0; ; i++) {
		result = OMX_RoleOfComponentEnum(role_name, compName, i);
		if (result == OMX_ErrorNoMore) {
			*pNumRoles = i;
			break;
		}

		if (i < size) {
			strncpy((char *)roles[i], role_name, OMX_MAX_STRINGNAME_SIZE - 1);
			roles[i][OMX_MAX_STRINGNAME_SIZE - 1] = '\0';
		}
	}

	return OMX_ErrorNone;
}

} //extern "C"
