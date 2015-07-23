#include <vector>
#include <mutex>

#include <omxil_mf/component.hpp>
#include <omxil_mf/port_video.hpp>

#include "api/consts.hpp"
#include "util/omx_enum_name.hpp"
#include "debug/scoped_log.hpp"

namespace mf {

port_video::port_video(int ind, component *c)
	: port(ind, c),
	mime_type(nullptr), native_render(nullptr),
	frame_width(0), frame_height(0), stride(0), slice_height(0),
	bitrate(0), framerate(0), flag_error_concealment(OMX_FALSE),
	compression_format(OMX_VIDEO_CodingUnused),
	color_format(OMX_COLOR_FormatUnused), native_window(nullptr)
{
	scoped_log_begin;

	set_domain(OMX_PortDomainVideo);
}

port_video::~port_video()
{
	scoped_log_begin;
	//do nothing
}

const char *port_video::get_name() const
{
	return "port_video";
}

OMX_STRING port_video::get_mime_type() const
{
	return mime_type;
}

void port_video::set_mime_type(OMX_STRING v)
{
	mime_type = v;
}

OMX_NATIVE_DEVICETYPE port_video::get_native_render() const
{
	return native_render;
}

void port_video::set_native_render(OMX_NATIVE_DEVICETYPE v)
{
	native_render = v;
}

OMX_U32 port_video::get_frame_width() const
{
	return frame_width;
}

void port_video::set_frame_width(OMX_U32 v)
{
	frame_width = v;
}

OMX_U32 port_video::get_frame_height() const
{
	return frame_height;
}

void port_video::set_frame_height(OMX_U32 v)
{
	frame_height = v;
}

OMX_S32 port_video::get_stride() const
{
	return stride;
}

void port_video::set_stride(OMX_S32 v)
{
	stride = v;
}

OMX_U32 port_video::get_slice_height() const
{
	return slice_height;
}

void port_video::set_slice_height(OMX_U32 v)
{
	slice_height = v;
}

OMX_U32 port_video::get_bitrate() const
{
	return bitrate;
}

void port_video::set_bitrate(OMX_U32 v)
{
	bitrate = v;
}

OMX_U32 port_video::get_framerate() const
{
	return framerate;
}

void port_video::set_framerate(OMX_U32 v)
{
	framerate = v;
}

OMX_BOOL port_video::get_flag_error_concealment() const
{
	return flag_error_concealment;
}

void port_video::set_flag_error_concealment(OMX_BOOL v)
{
	flag_error_concealment = v;
}

OMX_VIDEO_CODINGTYPE port_video::get_compression_format() const
{
	return compression_format;
}

void port_video::set_compression_format(OMX_VIDEO_CODINGTYPE v)
{
	compression_format = v;
}

OMX_COLOR_FORMATTYPE port_video::get_color_format() const
{
	return color_format;
}

void port_video::set_color_format(OMX_COLOR_FORMATTYPE v)
{
	color_format = v;
}

OMX_NATIVE_WINDOWTYPE port_video::get_native_window() const
{
	return native_window;
}

void port_video::set_native_window(OMX_NATIVE_WINDOWTYPE v)
{
	native_window = v;
}

const OMX_PARAM_PORTDEFINITIONTYPE *port_video::get_definition() const
{
	scoped_log_begin;

	super::get_definition();

	definition.format.video.cMIMEType     = mime_type;
	definition.format.video.pNativeRender = native_render;
	definition.format.video.nFrameWidth   = frame_width;
	definition.format.video.nFrameHeight  = frame_height;
	definition.format.video.nStride       = stride;
	definition.format.video.nSliceHeight  = slice_height;
	definition.format.video.nBitrate      = bitrate;
	definition.format.video.xFramerate    = framerate;
	definition.format.video.bFlagErrorConcealment = flag_error_concealment;
	definition.format.video.eCompressionFormat    = compression_format;
	definition.format.video.eColorFormat  = color_format;
	definition.format.video.pNativeWindow = native_window;

        return &definition;
}

} //namespace mf
