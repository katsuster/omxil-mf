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
	native_window(nullptr)
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
	const OMX_IMAGE_PARAM_PORTFORMATTYPE *f = get_default_format_image();

	if (f == nullptr) {
		return OMX_IMAGE_CodingUnused;
	} else {
		return f->eCompressionFormat;
	}
}

OMX_COLOR_FORMATTYPE port_image::get_color_format() const
{
	const OMX_IMAGE_PARAM_PORTFORMATTYPE *f = get_default_format_image();

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

OMX_ERRORTYPE port_image::set_definition(const OMX_PARAM_PORTDEFINITIONTYPE& v)
{
	scoped_log_begin;
	OMX_IMAGE_PARAM_PORTFORMATTYPE t = {0, };
	OMX_ERRORTYPE err;

	//eCompressionFormat, eColorFormat を変えられたら、
	//デフォルトフォーマットを切り替える
	t.eCompressionFormat = v.format.image.eCompressionFormat;
	t.eColorFormat       = v.format.image.eColorFormat;
	err = set_default_format(port_format(t));
	if (err != OMX_ErrorNone) {
		errprint("unsupported image format in port definition.\n");
		return err;
	}

	mime_type                = v.format.image.cMIMEType;
	native_render            = v.format.image.pNativeRender;
	frame_width              = v.format.image.nFrameWidth;
	frame_height             = v.format.image.nFrameHeight;
	stride                   = v.format.image.nStride;
	slice_height             = v.format.image.nSliceHeight;
	flag_error_concealment   = v.format.image.bFlagErrorConcealment;
	native_window            = v.format.image.pNativeWindow;
	//下記はデフォルトフォーマットの切り替えによって変更を反映すること
	//set_compression_format(v.format.image.eCompressionFormat);
	//set_color_format(v.format.image.eColorFormat);

	super::set_definition(v);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port_image::set_definition_from_client(const OMX_PARAM_PORTDEFINITIONTYPE& v)
{
	scoped_log_begin;
	OMX_IMAGE_PARAM_PORTFORMATTYPE t = {0, };
	OMX_ERRORTYPE err;

	//eCompressionFormat, eColorFormat を変えられたら、
	//デフォルトフォーマットを切り替える
	t.eCompressionFormat = v.format.image.eCompressionFormat;
	t.eColorFormat       = v.format.image.eColorFormat;
	err = set_default_format(port_format(t));
	if (err != OMX_ErrorNone) {
		errprint("unsupported image format in port definition.\n");
		return err;
	}

	mime_type                = v.format.image.cMIMEType;
	native_render            = v.format.image.pNativeRender;
	frame_width              = v.format.image.nFrameWidth;
	frame_height             = v.format.image.nFrameHeight;
	stride                   = v.format.image.nStride;
	slice_height             = v.format.image.nSliceHeight;
	flag_error_concealment   = v.format.image.bFlagErrorConcealment;
	native_window            = v.format.image.pNativeWindow;
	//下記はデフォルトフォーマットの切り替えによって変更を反映すること
	//set_compression_format(v.format.image.eCompressionFormat);
	//set_color_format(v.format.image.eColorFormat);

	super::set_definition_from_client(v);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port_image::get_port_format_index(const port_format& f, size_t *ind) const
{
	scoped_log_begin;
	const OMX_IMAGE_PARAM_PORTFORMATTYPE *t = f.get_format_image();
	size_t i = 0;

	if (ind != nullptr) {
		*ind = (size_t)~0;
	}

	if (t == nullptr) {
		//not image
		errprint("argument has not image.\n");
		return OMX_ErrorBadParameter;
	}

	//全て不定の場合はエラーとする
	if (t->eCompressionFormat == OMX_IMAGE_CodingUnused &&
		t->eColorFormat == OMX_COLOR_FormatUnused) {
		errprint("argument has invalid image condition.\n");
		return OMX_ErrorBadParameter;
	}

	for (auto elem : get_port_format_list()) {
		const OMX_IMAGE_PARAM_PORTFORMATTYPE *e = elem.get_format_image();
		if (e == nullptr) {
			//not image
			continue;
		}

		if ((t->eCompressionFormat == OMX_IMAGE_CodingUnused || t->eCompressionFormat == e->eCompressionFormat) &&
			(t->eColorFormat == OMX_COLOR_FormatUnused || t->eColorFormat == e->eColorFormat)) {
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

const OMX_IMAGE_PARAM_PORTFORMATTYPE *port_image::get_default_format_image() const
{
	const port_format *pf = get_default_format();

	if (pf == nullptr) {
		return nullptr;
	}

	return pf->get_format_image();
}

} //namespace mf
