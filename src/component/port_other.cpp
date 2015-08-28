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
	OMX_OTHER_PARAM_PORTFORMATTYPE t = {0, };
	OMX_ERRORTYPE err;

	//xFramerate, eCompressionFormat, eColorFormat を変えられたら、
	//デフォルトフォーマットを切り替える
	t.eFormat = v.format.other.eFormat;
	err = set_default_format(port_format(t));
	if (err != OMX_ErrorNone) {
		errprint("unsupported other format in port definition.\n");
		return err;
	}

	//下記はデフォルトフォーマットの切り替えによって変更を反映すること
	//v.format.other.eFormat

	super::set_definition_from_client(v);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port_other::get_port_format_index(const port_format& f, size_t *ind) const
{
	scoped_log_begin;
	const OMX_OTHER_PARAM_PORTFORMATTYPE *t = f.get_format_other();
	size_t i = 0;

	if (ind != nullptr) {
		*ind = (size_t)~0;
	}

	if (t == nullptr) {
		//not other
		errprint("argument has not other.\n");
		return OMX_ErrorBadParameter;
	}

	//全て不定の場合はエラーとするが、other の場合は不定値がない

	for (auto elem : get_port_format_list()) {
		const OMX_OTHER_PARAM_PORTFORMATTYPE *e = elem.get_format_other();
		if (e == nullptr) {
			//not other
			continue;
		}

		if (t->eFormat == e->eFormat) {
			//found
			if (ind != nullptr) {
				*ind = i;
			}
			return OMX_ErrorNone;
		}
		i++;
	}

	//not found
	return OMX_ErrorUnsupportedSetting;
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
