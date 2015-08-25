#include <vector>
#include <mutex>

#include <omxil_mf/component.hpp>
#include <omxil_mf/port_other.hpp>
#include <omxil_mf/scoped_log.hpp>

#include "api/consts.hpp"
#include "util/omx_enum_name.hpp"

namespace mf {

port_other::port_other(int ind, component *c)
	: port(ind, c)
{
	scoped_log_begin;

	set_domain(OMX_PortDomainOther);
}

port_other::~port_other()
{
	scoped_log_begin;
	//do nothing
}

const char *port_other::get_name() const
{
	return "port_other";
}

OMX_OTHER_FORMATTYPE port_other::get_format() const
{
	const OMX_OTHER_PARAM_PORTFORMATTYPE *f = get_default_format_other();

	if (f == nullptr) {
		return OMX_OTHER_FormatBinary;
	} else {
		return f->eFormat;
	}
}

const OMX_PARAM_PORTDEFINITIONTYPE *port_other::get_definition() const
{
	scoped_log_begin;

	super::get_definition();

	definition.format.other.eFormat = get_format();

	return &definition;
}

OMX_ERRORTYPE port_other::set_definition(const OMX_PARAM_PORTDEFINITIONTYPE& v)
{
	scoped_log_begin;

	//FIXME: eEncoding を変えられたらどうする？？
	//set_format(v.format.other.eFormat);

	super::set_definition(v);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port_other::set_definition_from_client(const OMX_PARAM_PORTDEFINITIONTYPE& v)
{
	scoped_log_begin;

	//FIXME: eEncoding を変えられたらどうする？？
	//set_format(v.format.other.eFormat);

	super::set_definition_from_client(v);

	return OMX_ErrorNone;
}

const OMX_OTHER_PARAM_PORTFORMATTYPE *port_other::get_default_format_other() const
{
	const port_format *pf = get_default_format();

	if (pf == nullptr) {
		return nullptr;
	}

	return pf->get_format_other();
}

} //namespace mf
