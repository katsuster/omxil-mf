
#include <fstream>

#define __OMX_EXPORTS
#include <OMX_Core.h>

#include <omxil_mf/omxil_mf.h>
#include <omxil_mf/component.hpp>
#include <omxil_mf/scoped_log.hpp>

#include "regist/register_component.hpp"
#include "util/omx_enum_name.hpp"

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
	OMX_STATETYPE st;
	OMX_ERRORTYPE err;

	if (hComponent == nullptr) {
		errprint("Invalid component %p.\n", hComponent);
		return OMX_ErrorBadParameter;
	}
	omx_comp = (OMX_COMPONENTTYPE *)hComponent;

	//OpenMAX IL 1.2.0: Table 3-17: Valid Component Calls
	err = OMX_GetState(omx_comp, &st);
	if (err != OMX_ErrorNone) {
		errprint("Cannot get state of component %p.\n", hComponent);
		return err;
	}

	switch (st) {
	case OMX_StateLoaded:
		//OK
		break;
	default:
		//NG
		errprint("Invalid state:%s.\n",
			mf::omx_enum_name::get_OMX_STATETYPE_name(st));
		return OMX_ErrorInvalidState;
	}

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
	OMX_COMPONENTTYPE *comp_out, *comp_in;
	OMX_TUNNELSETUPTYPE tunnel;
	OMX_ERRORTYPE result_out = OMX_ErrorNone, result_in = OMX_ErrorNone;

	//OpenMAX IL 1.2.0: Figure 3-11 Tunnel Setup

	// +----+----------+---------+----------+---------+-------------------------+
	// | No | output   | err_out | input    | err_in  | result                  |
	// +----+----------+---------+----------+---------+-------------------------+
	// | 1  | null     | ---     | null     | ---     | error                   |
	// +----+----------+---------+----------+---------+-------------------------+
	// | 2  | not null | error   | (skip)   | none    | error                   |
	// | 3  | not null | none    | not null | error   | tunnel&rel out, error   |
	// | 4  | null     | none    | not null | error   | error                   |
	// +----+----------+---------+----------+---------+-------------------------+
	// | 5  | not null | none    | null     | none    | release output, success |
	// | 6  | null     | none    | not null | none    | release input, success  |
	// | 7  | not null | none    | not null | none    | tunnel out->in, success |
	// +----+----------+---------+----------+---------+-------------------------+
	if (hOutput == nullptr && hInput == nullptr) {
		//Failed, case No.1
		return OMX_ErrorBadParameter;
	}
	comp_out = static_cast<OMX_COMPONENTTYPE *>(hOutput);
	comp_in = static_cast<OMX_COMPONENTTYPE *>(hInput);

	tunnel.nTunnelFlags = 0;
	tunnel.eSupplier    = OMX_BufferSupplyUnspecified;

	//Request to output port
	if (comp_out != nullptr) {
		result_out = comp_out->ComponentTunnelRequest(hOutput, nPortOutput,
			hInput, nPortInput, &tunnel);
		if (result_out != OMX_ErrorNone) {
			//Failed, case No.2
			errprint("Failed to ComponentTunnelRequest(out)(case 2), "
				"comp '%p' out-port '%d' -> comp '%p' in-port '%d'.\n",
				comp_out, (int)nPortOutput,
				comp_in, (int)nPortInput);
			return result_out;
		}
	}

	//Request to input port
	if (comp_in != nullptr) {
		result_in = comp_in->ComponentTunnelRequest(hInput, nPortInput,
			hOutput, nPortOutput, &tunnel);
		if (result_in != OMX_ErrorNone) {
			errprint("Failed to ComponentTunnelRequest(in), "
				"comp '%p' out-port '%d' -> comp '%p' in-port '%d'.\n",
				comp_out, (int)nPortOutput,
				comp_in, (int)nPortInput);
		}
	}

	if (comp_out != nullptr && comp_in != nullptr && result_in != OMX_ErrorNone) {
		//Failed, case No.3
		errprint("Failed to ComponentTunnelRequest(case 3), "
			"reset comp '%p' out-port '%d' mode.\n",
			comp_out, (int)nPortOutput);

		//Release output port, if failed to tunnel
		result_out = comp_out->ComponentTunnelRequest(hOutput, nPortOutput,
			nullptr, 0, &tunnel);
		if (result_out != OMX_ErrorNone) {
			//Fatal error
			errprint("Failed to ComponentTunnelRequest(out, null), "
				"comp '%p' out-port '%d'.\n",
				comp_out, (int)nPortOutput);
			return result_out;
		}
		return result_in;
	} else if (comp_out == nullptr && comp_in != nullptr && result_in != OMX_ErrorNone) {
		//Failed, case No.4
		errprint("Failed to ComponentTunnelRequest(case 4), "
			"comp '%p' out-port '%d' -> comp '%p' in-port '%d'.\n",
			comp_out, (int)nPortOutput,
			comp_in, (int)nPortInput);

		return result_in;
	}

	return OMX_ErrorNone;
}

//OpenMAX 1.2.0
OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_TeardownTunnel(OMX_IN OMX_HANDLETYPE hOutput, OMX_IN OMX_U32 nPortOutput, OMX_IN OMX_HANDLETYPE hInput, OMX_IN  OMX_U32 nPortInput)
{
	scoped_log_begin;

	//FIXME: not implemented

	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

//OpenMAX 1.1.2
OMX_API OMX_ERRORTYPE OMX_GetContentPipe(OMX_OUT OMX_HANDLETYPE *hPipe, OMX_IN OMX_STRING szURI)
{
	scoped_log_begin;

	if (hPipe == nullptr) {
		return OMX_ErrorBadParameter;
	}

	//FIXME: not implemented

	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

//OpenMAX 1.2.0
OMX_API OMX_ERRORTYPE OMX_ComponentOfRoleEnum(OMX_OUT OMX_STRING compName, OMX_IN OMX_STRING role, OMX_IN OMX_U32 nIndex)
{
	scoped_log_begin;
	mf::register_component *rc = mf::register_component::get_instance();
	const std::vector<std::string> *comps = nullptr;

	if (role == nullptr) {
		return OMX_ErrorBadParameter;
	}

	comps = rc->find_by_role(role);
	if (comps == nullptr) {
		errprint("Invalid role name '%s'.\n", role);
		return OMX_ErrorBadParameter;
	}

	if (nIndex >= comps->size()) {
		return OMX_ErrorNoMore;
	}

	strncpy(compName, comps->at(nIndex).c_str(), OMX_MAX_STRINGNAME_SIZE - 1);
	role[OMX_MAX_STRINGNAME_SIZE - 1] = '\0';

	return OMX_ErrorNone;
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
	char comp_name[OMX_MAX_STRINGNAME_SIZE];
	OMX_U32 size;
	OMX_ERRORTYPE result = OMX_ErrorBadParameter;
	OMX_U32 i;

	if (role == nullptr || pNumComps == nullptr || compNames == nullptr) {
		return OMX_ErrorBadParameter;
	}

	size = *pNumComps;
	i = 0;
	*pNumComps = 0;
	for (i = 0; ; i++) {
		result = OMX_ComponentOfRoleEnum(comp_name, role, i);
		if (result == OMX_ErrorNoMore) {
			result = OMX_ErrorNone;
			*pNumComps = i;
			break;
		} else if (result != OMX_ErrorNone) {
			break;
		}

		if (i < size) {
			strncpy((char *)compNames[i], comp_name, OMX_MAX_STRINGNAME_SIZE - 1);
			compNames[i][OMX_MAX_STRINGNAME_SIZE - 1] = '\0';
		}
	}

	return result;
}

//OpenMAX 1.1.2
OMX_API OMX_ERRORTYPE OMX_GetRolesOfComponent(OMX_IN OMX_STRING compName, OMX_INOUT OMX_U32 *pNumRoles, OMX_OUT OMX_U8 **roles)
{
	scoped_log_begin;
	char role_name[OMX_MAX_STRINGNAME_SIZE];
	OMX_U32 size;
	OMX_ERRORTYPE result = OMX_ErrorBadParameter;
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
			result = OMX_ErrorNone;
			*pNumRoles = i;
			break;
		} else if (result != OMX_ErrorNone) {
			break;
		}

		if (i < size) {
			strncpy((char *)roles[i], role_name, OMX_MAX_STRINGNAME_SIZE - 1);
			roles[i][OMX_MAX_STRINGNAME_SIZE - 1] = '\0';
		}
	}

	return result;
}

//OpenMAX 1.2.0
OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_GetCoreInterface(OMX_OUT void **ppItf, OMX_IN OMX_STRING cExtensionName)
{
	scoped_log_begin;

	//FIXME: not implemented

	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

//OpenMAX 1.2.0
OMX_API void OMX_APIENTRY OMX_FreeCoreInterface(OMX_IN void *pItf)
{
	scoped_log_begin;

	//FIXME: not implemented

	return;
	//return OMX_ErrorNone;
}

} //extern "C"
