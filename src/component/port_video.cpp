#include <vector>
#include <mutex>

#include <omxil_mf/component.hpp>
#include <omxil_mf/port_video.hpp>
#include <omxil_mf/scoped_log.hpp>

#include "api/consts.hpp"
#include "util/omx_enum_name.hpp"

namespace mf {

port_video::port_video(int ind, component *c)
	: port(ind, c),
	mime_type(nullptr), native_render(nullptr),
	frame_width(0), frame_height(0), stride(0), slice_height(0),
	bitrate(0), flag_error_concealment(OMX_FALSE),
	native_window(nullptr),
	default_format(-1)
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
	const OMX_VIDEO_PARAM_PORTFORMATTYPE *f = get_default_format();

	if (f == nullptr) {
		return 0;
	} else {
		return f->xFramerate;
	}
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
	const OMX_VIDEO_PARAM_PORTFORMATTYPE *f = get_default_format();

	if (f == nullptr) {
		return OMX_VIDEO_CodingUnused;
	} else {
		return f->eCompressionFormat;
	}
}

OMX_COLOR_FORMATTYPE port_video::get_color_format() const
{
	const OMX_VIDEO_PARAM_PORTFORMATTYPE *f = get_default_format();

	if (f == nullptr) {
		return OMX_COLOR_FormatUnused;
	} else {
		return f->eColorFormat;
	}
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
	definition.format.video.xFramerate    = get_framerate();
	definition.format.video.bFlagErrorConcealment = flag_error_concealment;
	definition.format.video.eCompressionFormat    = get_compression_format();
	definition.format.video.eColorFormat  = get_color_format();
	definition.format.video.pNativeWindow = native_window;

        return &definition;
}

OMX_ERRORTYPE port_video::set_definition(const OMX_PARAM_PORTDEFINITIONTYPE& v)
{
	scoped_log_begin;

	mime_type                = v.format.video.cMIMEType;
	native_render            = v.format.video.pNativeRender;
	frame_width              = v.format.video.nFrameWidth;
	frame_height             = v.format.video.nFrameHeight;
	stride                   = v.format.video.nStride;
	slice_height             = v.format.video.nSliceHeight;
	bitrate                  = v.format.video.nBitrate;
	//FIXME: xFramerate, eCompressionFormat, eColorFormat を変えられたらどうする？？
	//set_framerate(v.format.video.xFramerate);
	flag_error_concealment   = v.format.video.bFlagErrorConcealment;
	//set_compression_format(v.format.video.eCompressionFormat);
	//set_color_format(v.format.video.eColorFormat);
	native_window            = v.format.video.pNativeWindow;

	super::set_definition(v);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port_video::set_definition_from_client(const OMX_PARAM_PORTDEFINITIONTYPE& v)
{
	scoped_log_begin;

	mime_type                = v.format.video.cMIMEType;
	native_render            = v.format.video.pNativeRender;
	frame_width              = v.format.video.nFrameWidth;
	frame_height             = v.format.video.nFrameHeight;
	stride                   = v.format.video.nStride;
	slice_height             = v.format.video.nSliceHeight;
	bitrate                  = v.format.video.nBitrate;
	//FIXME: xFramerate, eCompressionFormat, eColorFormat を変えられたらどうする？？
	//set_framerate(v.format.video.xFramerate);
	flag_error_concealment   = v.format.video.bFlagErrorConcealment;
	//set_compression_format(v.format.video.eCompressionFormat);
	//set_color_format(v.format.video.eColorFormat);
	native_window            = v.format.video.pNativeWindow;

	super::set_definition_from_client(v);

	return OMX_ErrorNone;
}

const OMX_VIDEO_PARAM_PORTFORMATTYPE *port_video::get_supported_format(size_t index) const
{
	if (index < 0 || formats.size() <= index) {
		return nullptr;
	}

	return &formats.at(index);
}

OMX_ERRORTYPE port_video::add_supported_format(const OMX_VIDEO_PARAM_PORTFORMATTYPE *f)
{
	OMX_VIDEO_PARAM_PORTFORMATTYPE fmt;

	if (f == nullptr) {
		return OMX_ErrorBadParameter;
	}

	fmt = *f;
	fmt.nPortIndex = 0;
	fmt.nIndex = 0;
	formats.push_back(fmt);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port_video::remove_supported_format(size_t index)
{
	if (index < 0 || formats.size() <= index) {
		return OMX_ErrorBadParameter;
	}

	formats.erase(formats.begin() + index);

	return OMX_ErrorNone;
}

const OMX_VIDEO_PARAM_PORTFORMATTYPE *port_video::get_default_format() const
{
	return get_supported_format(default_format);
}

OMX_ERRORTYPE port_video::set_default_format(size_t index)
{
	if (index < 0 || formats.size() <= index) {
		return OMX_ErrorBadParameter;
	}

	default_format = index;

	return OMX_ErrorNone;
}

} //namespace mf
