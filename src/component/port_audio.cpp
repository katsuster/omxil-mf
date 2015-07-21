#include <vector>
#include <mutex>

#include <omxil_mf/component.hpp>
#include <omxil_mf/port_audio.hpp>

#include "api/consts.h"
#include "util/omx_enum_name.hpp"
#include "debug/scoped_log.hpp"

namespace mf {

port_audio::port_audio(int ind, component *c)
	: port(ind, c),
	mime_type(nullptr), native_render(nullptr),
	flag_error_concealment(OMX_FALSE),
	encoding(OMX_AUDIO_CodingUnused)
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
	return encoding;
}

void port_audio::set_encoding(OMX_AUDIO_CODINGTYPE v)
{
	encoding = v;
}

const OMX_PARAM_PORTDEFINITIONTYPE *port_audio::get_definition() const
{
	scoped_log_begin;

	super::get_definition();

	definition.format.audio.cMIMEType     = mime_type;
	definition.format.audio.pNativeRender = native_render;
	definition.format.audio.bFlagErrorConcealment = flag_error_concealment;
	definition.format.audio.eEncoding     = encoding;

	return &definition;
}

} //namespace mf
