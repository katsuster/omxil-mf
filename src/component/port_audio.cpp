#include <omxil_mf/component.hpp>
#include <omxil_mf/port_audio.hpp>
#include <omxil_mf/scoped_log.hpp>

#include "api/consts.hpp"
#include "util/omx_enum_name.hpp"

namespace mf {

port_audio::port_audio(int ind, component *c)
	: port(ind, c),
	mime_type(nullptr), native_render(nullptr),
	flag_error_concealment(OMX_FALSE)
{
	scoped_log_begin;

	set_domain(OMX_PortDomainAudio);
}

port_audio::~port_audio()
{
	scoped_log_begin;
	//do nothing
}

const char *port_audio::get_name() const
{
	return "port_audio";
}

OMX_STRING port_audio::get_mime_type() const
{
	return mime_type;
}

void port_audio::set_mime_type(OMX_STRING v)
{
	mime_type = v;
}

OMX_NATIVE_DEVICETYPE port_audio::get_native_render() const
{
	return native_render;
}

void port_audio::set_native_render(OMX_NATIVE_DEVICETYPE v)
{
	native_render = v;
}

OMX_BOOL port_audio::get_flag_error_concealment() const
{
	return flag_error_concealment;
}

void port_audio::set_flag_error_concealment(OMX_BOOL v)
{
	flag_error_concealment = v;
}

OMX_AUDIO_CODINGTYPE port_audio::get_encoding() const
{
	const OMX_AUDIO_PARAM_PORTFORMATTYPE *f = get_default_format_audio();

	if (f == nullptr) {
		return OMX_AUDIO_CodingUnused;
	} else {
		return f->eEncoding;
	}
}

const OMX_PARAM_PORTDEFINITIONTYPE *port_audio::get_definition() const
{
	scoped_log_begin;

	super::get_definition();

	definition.format.audio.cMIMEType     = mime_type;
	definition.format.audio.pNativeRender = native_render;
	definition.format.audio.bFlagErrorConcealment = flag_error_concealment;
	definition.format.audio.eEncoding     = get_encoding();

	return &definition;
}

OMX_ERRORTYPE port_audio::set_definition(const OMX_PARAM_PORTDEFINITIONTYPE& v)
{
	scoped_log_begin;
	OMX_AUDIO_PARAM_PORTFORMATTYPE t = {0, };
	OMX_ERRORTYPE err;

	//eEncoding を変えられたら、
	//デフォルトフォーマットを切り替える
	t.eEncoding = v.format.audio.eEncoding;
	err = set_default_format(port_format(t));
	if (err != OMX_ErrorNone) {
		errprint("unsupported audio format in port definition.\n");
		return err;
	}

	mime_type              = v.format.audio.cMIMEType;
	native_render          = v.format.audio.pNativeRender;
	flag_error_concealment = v.format.audio.bFlagErrorConcealment;
	//下記はデフォルトフォーマットの切り替えによって変更を反映すること
	//v.format.audio.eEncoding

	super::set_definition(v);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port_audio::set_definition_from_client(const OMX_PARAM_PORTDEFINITIONTYPE& v)
{
	scoped_log_begin;
	OMX_AUDIO_PARAM_PORTFORMATTYPE t = {0, };
	OMX_ERRORTYPE err;

	//eEncoding を変えられたら、
	//デフォルトフォーマットを切り替える
	t.eEncoding = v.format.audio.eEncoding;
	err = set_default_format(port_format(t));
	if (err != OMX_ErrorNone) {
		errprint("unsupported audio format in port definition.\n");
		return err;
	}

	mime_type              = v.format.audio.cMIMEType;
	native_render          = v.format.audio.pNativeRender;
	flag_error_concealment = v.format.audio.bFlagErrorConcealment;
	//下記はデフォルトフォーマットの切り替えによって変更を反映すること
	//v.format.audio.eEncoding

	super::set_definition_from_client(v);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port_audio::get_port_format_index(const port_format& f, size_t *ind) const
{
	scoped_log_begin;
	const OMX_AUDIO_PARAM_PORTFORMATTYPE *t = f.get_format_audio();
	size_t i = 0;

	if (ind != nullptr) {
		*ind = (size_t)~0;
	}

	if (t == nullptr) {
		//not audio
		errprint("argument has not audio.\n");
		return OMX_ErrorBadParameter;
	}

	//全て不定の場合はエラーとする
	if (t->eEncoding == OMX_AUDIO_CodingUnused) {
		errprint("argument has invalid audio condition.\n");
		return OMX_ErrorBadParameter;
	}

	for (auto elem : get_port_format_list()) {
		const OMX_AUDIO_PARAM_PORTFORMATTYPE *e = elem.get_format_audio();
		if (e == nullptr) {
			//not audio
			continue;
		}

		if (t->eEncoding == e->eEncoding) {
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

const OMX_AUDIO_PARAM_PORTFORMATTYPE *port_audio::get_default_format_audio() const
{
	const port_format *pf = get_default_format();

	if (pf == nullptr) {
		return nullptr;
	}

	return pf->get_format_audio();
}

} //namespace mf
