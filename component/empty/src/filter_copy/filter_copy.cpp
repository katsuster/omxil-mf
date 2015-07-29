
#include "filter_copy/filter_copy.hpp"

namespace mf {

filter_copy::filter_copy(OMX_COMPONENTTYPE *c, const char *cname) 
	: component(c, cname),
	in_port_video(nullptr), out_port_video(nullptr)
{
	try {
		in_port_video = new port_video(4, this);
		in_port_video->set_dir(OMX_DirInput);
		in_port_video->set_buffer_count_actual(4);
		in_port_video->set_buffer_count_min(1);
		in_port_video->set_buffer_size(32768);
		in_port_video->set_buffer_alignment(64);
		in_port_video->set_compression_format(OMX_VIDEO_CodingAVC);
		in_port_video->set_color_format(OMX_COLOR_FormatUnused);

		insert_port(*in_port_video);

		out_port_video = new port_video(5, this);
		out_port_video->set_dir(OMX_DirOutput);
		out_port_video->set_buffer_count_actual(3);
		out_port_video->set_buffer_count_min(1);
		out_port_video->set_buffer_size(65536);
		out_port_video->set_buffer_alignment(64);
		out_port_video->set_compression_format(OMX_VIDEO_CodingUnused);
		out_port_video->set_color_format(OMX_COLOR_FormatYUV420Planar);
                out_port_video->set_frame_width(640);
                out_port_video->set_frame_height(480);

		insert_port(*out_port_video);
	} catch (const std::bad_alloc& e) {
		delete out_port_video;
		out_port_video = nullptr;
		delete in_port_video;
		in_port_video = nullptr;
	}
}

filter_copy::~filter_copy()
{
	delete out_port_video;
	out_port_video = nullptr;
	delete in_port_video;
	in_port_video = nullptr;
}

void filter_copy::run()
{
	OMX_ERRORTYPE result;
	port_buffer pb_in, pb_out;
	OMX_U32 off_in, off_out, len_in, len_out;
	OMX_TICKS stamp = 0;
	int i = 0;

	while (should_run()) {
		result = in_port_video->pop_buffer(&pb_in);
		if (result != OMX_ErrorNone) {
			errprint("in_port_video.pop_buffer().\n");
			break;
		}

		result = out_port_video->pop_buffer(&pb_out);
		if (result != OMX_ErrorNone) {
			errprint("out_port_video.pop_buffer().\n");
			break;
		}

		//memset(pb_out.header->pBuffer, 0, pb_out.header->nAllocLen);

		off_in = pb_in.header->nOffset;
		len_in = pb_in.header->nFilledLen;
		off_out = pb_out.header->nOffset;
		len_out = pb_out.header->nAllocLen - off_out;

		len_out = std::min(len_in, len_out);
		memmove(&pb_out.header->pBuffer[off_out], 
			&pb_in.header->pBuffer[off_in], len_out);

		//NOTE: gst-openmax は nOffset を戻さないとおかしな挙動をする？？
		pb_in.header->nOffset = 0;
		in_port_video->empty_buffer_done(&pb_in);

		pb_out.header->nFilledLen = len_out;
		pb_out.header->nOffset    = 0;
		pb_out.header->nTimeStamp = stamp;
		pb_out.header->nFlags     = 0;
		out_port_video->fill_buffer_done(&pb_out);

		//next one
		i++;
		//16ms
		stamp += 16000;
	}
}


/*
 * protected functions
 */

OMX_U32 filter_copy::get_video_ports()
{
	return 2;
}

OMX_U32 filter_copy::get_video_start_port()
{
	return 4;
}


/*
 * static public functions
 */

filter_copy *filter_copy::get_instance(OMX_HANDLETYPE hComponent)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	filter_copy *comp = (filter_copy *) omx_comp->pComponentPrivate;

	return comp;
}

} //namespace mf

