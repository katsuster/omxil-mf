#include <vector>
#include <mutex>

#include <omxil_mf/component.hpp>
#include <omxil_mf/port_other.hpp>
#include <omxil_mf/scoped_log.hpp>

#include "api/consts.hpp"
#include "util/omx_enum_name.hpp"

namespace mf {

port_other::port_other(int ind, component *c)
	: port(ind, c),
	format(OMX_OTHER_FormatBinary)
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
	return format;
}

void port_other::set_format(OMX_OTHER_FORMATTYPE v)
{
	format = v;
}

const OMX_PARAM_PORTDEFINITIONTYPE *port_other::get_definition() const
{
	scoped_log_begin;

	super::get_definition();

	definition.format.other.eFormat = format;

	return &definition;
}

} //namespace mf
