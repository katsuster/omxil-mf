
#define __OMX_EXPORTS
#include <OMX_Core.h>

#include "regist/register_component.hpp"
#include "debug/scoped_log.hpp"

//----------------------------------------
//external APIs
//----------------------------------------
extern "C" {

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_MF_RegisterComponent(const char *name, const OMX_MF_COMPONENT_INFO *info)
{
	scoped_log_begin;
	mf::register_component *rc = mf::register_component::get_instance();

	rc->insert(name, info);

	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_MF_RegisterComponentAlias(const char *name, const char *alias)
{
	scoped_log_begin;
	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_MF_RegisterComponentRole(const char *name, const char *role)
{
	scoped_log_begin;
	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

} //extern "C"

