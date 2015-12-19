
#define __OMX_EXPORTS

#include <OMX_Core.h>

#include <omxil_mf/scoped_log.hpp>

#include "regist/register_component.hpp"

//----------------------------------------
//external APIs
//----------------------------------------
extern "C" {

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_MF_RegisterComponent(const char *name, const OMX_MF_COMPONENT_INFO *info)
{
	scoped_log_begin;
	mf::register_component *rc = mf::register_component::get_instance();
	OMX_MF_COMPONENT_INFO *comp_info = new OMX_MF_COMPONENT_INFO(*info);
	bool res;

	res = rc->insert(name, name, comp_info);
	if (!res) {
		//Component has already existed
		delete comp_info;
		return OMX_ErrorInvalidComponentName;
	}

	//dump all registered components
	rc->dump();

	return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_MF_RegisterComponentAlias(const char *name, const char *alias)
{
	scoped_log_begin;
	mf::register_component *rc = mf::register_component::get_instance();
	mf::register_info *rinfo = nullptr;
	const OMX_MF_COMPONENT_INFO *comp_info = nullptr;
	bool res;

	rinfo = rc->find(name);
	if (rinfo == nullptr) {
		//not found
		return OMX_ErrorInvalidComponentName;
	}

	//Copy component info.
	//Reason: OMX_FreeHandle() call 'delete' for all key.
	//        If we use shallow copy of component_info,
	//        OMX_FreeHandle() faces double-free and SEGV.
	comp_info = new OMX_MF_COMPONENT_INFO(*rinfo->comp_info);
	res = rc->insert(alias, rinfo->cano_name.c_str(), comp_info);
	if (!res) {
		//Alias has already existed
		delete comp_info;
		return OMX_ErrorInvalidComponentName;
	}

	//dump all registered components
	rc->dump();

	return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_MF_RegisterComponentRole(const char *name, const char *role)
{
	scoped_log_begin;
	mf::register_component *rc = mf::register_component::get_instance();
	bool res;

	res = rc->insert_role(name, role);
	if (!res) {
		//Not found component
		return OMX_ErrorInvalidComponentName;
	}

	return OMX_ErrorNone;
}

} //extern "C"

