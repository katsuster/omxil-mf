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
	native_window(nullptr)
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
	const OMX_VIDEO_PARAM_PORTFORMATTYPE *f = get_default_format_video();

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
	const OMX_VIDEO_PARAM_PORTFORMATTYPE *f = get_default_format_video();

	if (f == nullptr) {
		return OMX_VIDEO_CodingUnused;
	} else {
		return f->eCompressionFormat;
	}
}

OMX_COLOR_FORMATTYPE port_video::get_color_format() const
{
	const OMX_VIDEO_PARAM_PORTFORMATTYPE *f = get_default_format_video();

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
	OMX_VIDEO_PARAM_PORTFORMATTYPE t = {0, };
	OMX_ERRORTYPE err;

	//xFramerate, eCompressionFormat, eColorFormat を変えられたら、
	//デフォルトフォーマットを切り替える
	t.eCompressionFormat = v.format.video.eCompressionFormat;
	t.eColorFormat       = v.format.video.eColorFormat;
	t.xFramerate         = v.format.video.xFramerate;
	err = set_default_format(port_format(t));
	if (err != OMX_ErrorNone) {
		errprint("unsupported video format in port definition.\n");
		return err;
	}

	mime_type                = v.format.video.cMIMEType;
	native_render            = v.format.video.pNativeRender;
	frame_width              = v.format.video.nFrameWidth;
	frame_height             = v.format.video.nFrameHeight;
	stride                   = v.format.video.nStride;
	slice_height             = v.format.video.nSliceHeight;
	bitrate                  = v.format.video.nBitrate;
	flag_error_concealment   = v.format.video.bFlagErrorConcealment;
	native_window            = v.format.video.pNativeWindow;
	//下記はデフォルトフォーマットの切り替えによって変更を反映すること
	//v.format.video.xFramerate
	//v.format.video.eCompressionFormat
	//v.format.video.eColorFormat

	super::set_definition(v);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port_video::set_definition_from_client(const OMX_PARAM_PORTDEFINITIONTYPE& v)
{
	scoped_log_begin;
	OMX_VIDEO_PARAM_PORTFORMATTYPE t = {0, };
	OMX_ERRORTYPE err;

	//xFramerate, eCompressionFormat, eColorFormat を変えられたら、
	//デフォルトフォーマットを切り替える
	t.eCompressionFormat = v.format.video.eCompressionFormat;
	t.eColorFormat       = v.format.video.eColorFormat;
	t.xFramerate         = v.format.video.xFramerate;
	err = set_default_format(port_format(t));
	if (err != OMX_ErrorNone) {
		errprint("unsupported video format in port definition.\n");
		return err;
	}

	mime_type                = v.format.video.cMIMEType;
	native_render            = v.format.video.pNativeRender;
	frame_width              = v.format.video.nFrameWidth;
	frame_height             = v.format.video.nFrameHeight;
	stride                   = v.format.video.nStride;
	slice_height             = v.format.video.nSliceHeight;
	bitrate                  = v.format.video.nBitrate;
	flag_error_concealment   = v.format.video.bFlagErrorConcealment;
	native_window            = v.format.video.pNativeWindow;
	//下記はデフォルトフォーマットの切り替えによって変更を反映すること
	//v.format.video.xFramerate
	//v.format.video.eCompressionFormat
	//v.format.video.eColorFormat

	super::set_definition_from_client(v);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port_video::get_port_format_index(const port_format& f, size_t *ind) const
{
	scoped_log_begin;
	const OMX_VIDEO_PARAM_PORTFORMATTYPE *t = f.get_format_video();
	size_t i = 0;

	if (ind != nullptr) {
		*ind = (size_t)~0;
	}

	if (t == nullptr) {
		//not video
		errprint("Format is not video.\n");
		return OMX_ErrorBadParameter;
	}

	//全て不定の場合はエラーとする
	if (t->eCompressionFormat == OMX_VIDEO_CodingUnused &&
		t->eColorFormat == OMX_COLOR_FormatUnused &&
		t->xFramerate == 0) {
		errprint("Invalid video condition.\n");
		return OMX_ErrorBadParameter;
	}

	for (auto elem : get_port_format_list()) {
		const OMX_VIDEO_PARAM_PORTFORMATTYPE *e = elem.get_format_video();
		if (e == nullptr) {
			//not video
			i++;
			continue;
		}

		if ((t->eCompressionFormat != OMX_VIDEO_CodingUnused &&
				e->eCompressionFormat != OMX_VIDEO_CodingUnused &&
				t->eCompressionFormat != e->eCompressionFormat) ||
			(t->eColorFormat != OMX_COLOR_FormatUnused &&
				e->eColorFormat != OMX_COLOR_FormatUnused &&
				t->eColorFormat != e->eColorFormat) ||
			(t->xFramerate != 0 &&
				e->xFramerate != 0 &&
				t->xFramerate != e->xFramerate)) {
			i++;
			continue;
		}

		//found
		if (ind != nullptr) {
			*ind = i;
		}
		return OMX_ErrorNone;
	}

	errprint("Not found specified video format.\n"
		"    eCompressionFormat:0x%08x(%s), \n"
		"    eColorFormat      :0x%08x(%s), \n"
		"    xFramerate        :0x%08x.\n",
		(int)t->eCompressionFormat,
		omx_enum_name::get_OMX_VIDEO_CODINGTYPE_name(t->eCompressionFormat),
		(int)t->eColorFormat,
		omx_enum_name::get_OMX_COLOR_FORMATTYPE_name(t->eColorFormat),
		(int)t->xFramerate);

	//not found
	return OMX_ErrorUnsupportedSetting;
}

const OMX_VIDEO_PARAM_PORTFORMATTYPE *port_video::get_default_format_video() const
{
	scoped_log_begin;
	const port_format *pf = get_default_format();

	if (pf == nullptr) {
		return nullptr;
	}

	return pf->get_format_video();
}

} //namespace mf
