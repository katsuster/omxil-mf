#include <vector>
#include <mutex>

#include <omxil_mf/component.hpp>
#include <omxil_mf/port_image.hpp>
#include <omxil_mf/scoped_log.hpp>

#include "api/consts.hpp"
#include "util/omx_enum_name.hpp"

namespace mf {

port_image::port_image(int ind, component *c)
	: port(ind, c),
	mime_type(nullptr), native_render(nullptr),
	frame_width(0), frame_height(0), stride(0), slice_height(0),
	flag_error_concealment(OMX_FALSE),
	native_window(nullptr),
	default_format(-1)
{
	scoped_log_begin;

	set_domain(OMX_PortDomainImage);
}

port_image::~port_image()
{
	scoped_log_begin;
	//do nothing
}

const char *port_image::get_name() const
{
	return "port_image";
}

OMX_STRING port_image::get_mime_type() const
{
	return mime_type;
}

void port_image::set_mime_type(OMX_STRING v)
{
	mime_type = v;
}

OMX_NATIVE_DEVICETYPE port_image::get_native_render() const
{
	return native_render;
}

void port_image::set_native_render(OMX_NATIVE_DEVICETYPE v)
{
	native_render = v;
}

OMX_U32 port_image::get_frame_width() const
{
	return frame_width;
}

void port_image::set_frame_width(OMX_U32 v)
{
	frame_width = v;
}

OMX_U32 port_image::get_frame_height() const
{
	return frame_height;
}

void port_image::set_frame_height(OMX_U32 v)
{
	frame_height = v;
}

OMX_S32 port_image::get_stride() const
{
	return stride;
}

void port_image::set_stride(OMX_S32 v)
{
	stride = v;
}

OMX_U32 port_image::get_slice_height() const
{
	return slice_height;
}

void port_image::set_slice_height(OMX_U32 v)
{
	slice_height = v;
}

OMX_BOOL port_image::get_flag_error_concealment() const
{
	return flag_error_concealment;
}

void port_image::set_flag_error_concealment(OMX_BOOL v)
{
	flag_error_concealment = v;
}

OMX_IMAGE_CODINGTYPE port_image::get_compression_format() const
{
	const OMX_IMAGE_PARAM_PORTFORMATTYPE *f = get_default_format();

	if (f == nullptr) {
		return OMX_IMAGE_CodingUnused;
	} else {
		return f->eCompressionFormat;
	}
}

OMX_COLOR_FORMATTYPE port_image::get_color_format() const
{
	const OMX_IMAGE_PARAM_PORTFORMATTYPE *f = get_default_format();

	if (f == nullptr) {
		return OMX_COLOR_FormatUnused;
	} else {
		return f->eColorFormat;
	}
}

OMX_NATIVE_WINDOWTYPE port_image::get_native_window() const
{
	return native_window;
}

void port_image::set_native_window(OMX_NATIVE_WINDOWTYPE v)
{
	native_window = v;
}

const OMX_PARAM_PORTDEFINITIONTYPE *port_image::get_definition() const
{
	scoped_log_begin;

	super::get_definition();

	definition.format.image.cMIMEType     = mime_type;
	definition.format.image.pNativeRender = native_render;
	definition.format.image.nFrameWidth   = frame_width;
	definition.format.image.nFrameHeight  = frame_height;
	definition.format.image.nStride       = stride;
	definition.format.image.nSliceHeight  = slice_height;
	definition.format.image.bFlagErrorConcealment = flag_error_concealment;
	definition.format.image.eCompressionFormat = get_compression_format();
	definition.format.image.eColorFormat  = get_color_format();
	definition.format.image.pNativeWindow = native_window;

	return &definition;
}

const OMX_IMAGE_PARAM_PORTFORMATTYPE *port_image::get_supported_format(size_t index) const
{
	if (index < 0 || formats.size() <= index) {
		return nullptr;
	}

	return &formats.at(index);
}

OMX_ERRORTYPE port_image::add_supported_format(const OMX_IMAGE_PARAM_PORTFORMATTYPE *f)
{
	OMX_IMAGE_PARAM_PORTFORMATTYPE fmt;

	if (f == nullptr) {
		return OMX_ErrorBadParameter;
	}

	fmt = *f;
	fmt.nPortIndex = 0;
	fmt.nIndex = 0;
	formats.push_back(fmt);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port_image::remove_supported_format(size_t index)
{
	if (index < 0 || formats.size() <= index) {
		return OMX_ErrorBadParameter;
	}

	formats.erase(formats.begin() + index);

	return OMX_ErrorNone;
}

const OMX_IMAGE_PARAM_PORTFORMATTYPE *port_image::get_default_format() const
{
	return get_supported_format(default_format);
}

OMX_ERRORTYPE port_image::set_default_format(size_t index)
{
	if (index < 0 || formats.size() <= index) {
		return OMX_ErrorBadParameter;
	}

	default_format = index;

	return OMX_ErrorNone;
}

} //namespace mf
