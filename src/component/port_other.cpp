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
	default_format(-1)
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
	const OMX_OTHER_PARAM_PORTFORMATTYPE *f = get_default_format();

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

const OMX_OTHER_PARAM_PORTFORMATTYPE *port_other::get_supported_format(size_t index) const
{
	if (index < 0 || formats.size() <= index) {
		return nullptr;
	}

	return &formats.at(index);
}

OMX_ERRORTYPE port_other::add_supported_format(const OMX_OTHER_PARAM_PORTFORMATTYPE *f)
{
	OMX_OTHER_PARAM_PORTFORMATTYPE fmt;

	if (f == nullptr) {
		return OMX_ErrorBadParameter;
	}

	fmt = *f;
	fmt.nPortIndex = 0;
	fmt.nIndex = 0;
	formats.push_back(fmt);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port_other::remove_supported_format(size_t index)
{
	if (index < 0 || formats.size() <= index) {
		return OMX_ErrorBadParameter;
	}

	formats.erase(formats.begin() + index);

	return OMX_ErrorNone;
}

const OMX_OTHER_PARAM_PORTFORMATTYPE *port_other::get_default_format() const
{
	return get_supported_format(default_format);
}

OMX_ERRORTYPE port_other::set_default_format(size_t index)
{
	if (index < 0 || formats.size() <= index) {
		return OMX_ErrorBadParameter;
	}

	default_format = index;

	return OMX_ErrorNone;
}

} //namespace mf
