
#include "filter_copy/filter_copy.hpp"

namespace mf {

/*
 * Worker thread
 */

filter_copy::worker_main::worker_main(filter_copy *c)
	: component_worker(c), comp(c)
{
}

filter_copy::worker_main::~worker_main()
{
}

const char *filter_copy::worker_main::get_name() const
{
	return "filt_copy::wrk_main";
}

void filter_copy::worker_main::run()
{
	OMX_ERRORTYPE result;
	port_buffer pb_in, pb_out;
	OMX_U32 off_in, off_out, len_in, len_out;
	OMX_TICKS stamp = 0;

	while (is_running()) {
		if (is_request_flush()) {
			set_request_flush(false);
			return;
		}

		result = comp->in_port_video->pop_buffer(&pb_in);
		if (result != OMX_ErrorNone) {
			errprint("in_port_video.pop_buffer().\n");
			continue;
		}

		result = comp->out_port_video->pop_buffer(&pb_out);
		if (result != OMX_ErrorNone) {
			errprint("out_port_video.pop_buffer().\n");
			continue;
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
		comp->in_port_video->empty_buffer_done(&pb_in);

		pb_out.header->nFilledLen = len_out;
		pb_out.header->nOffset    = 0;
		pb_out.header->nTimeStamp = stamp;
		pb_out.header->nFlags     = 0;
		comp->out_port_video->fill_buffer_done(&pb_out);

		//16ms
		stamp += 16000;
	}
}


/*
 * Component
 */

filter_copy::filter_copy(OMX_COMPONENTTYPE *c, const char *cname) 
	: component(c, cname),
	in_port_video(nullptr), out_port_video(nullptr),
	wk_main(this)
{
	OMX_VIDEO_PARAM_PORTFORMATTYPE f;

	try {
		in_port_video = new port_video(4, this);
		in_port_video->set_dir(OMX_DirInput);
		in_port_video->set_buffer_count_actual(4);
		in_port_video->set_buffer_count_min(1);
		in_port_video->set_buffer_size(32768);
		in_port_video->set_buffer_alignment(64);
		f.eCompressionFormat = OMX_VIDEO_CodingAVC;
		f.eColorFormat       = OMX_COLOR_FormatUnused;
		f.xFramerate         = 0;
		in_port_video->add_port_format(f);
		in_port_video->set_default_format(0);

		insert_port(*in_port_video);

		out_port_video = new port_video(5, this);
		out_port_video->set_dir(OMX_DirOutput);
		out_port_video->set_buffer_count_actual(3);
		out_port_video->set_buffer_count_min(1);
		out_port_video->set_buffer_size(65536);
		out_port_video->set_buffer_alignment(64);
		out_port_video->set_frame_width(640);
		out_port_video->set_frame_height(480);
		f.eCompressionFormat = OMX_VIDEO_CodingUnused;
		f.eColorFormat       = OMX_COLOR_FormatYUV420Planar;
		f.xFramerate         = 0;
		out_port_video->add_port_format(f);
		f.eCompressionFormat = OMX_VIDEO_CodingUnused;
		f.eColorFormat       = OMX_COLOR_Format32bitBGRA8888;
		f.xFramerate         = 0;
		out_port_video->add_port_format(f);
		out_port_video->set_default_format(0);

		insert_port(*out_port_video);

		register_worker_thread(&wk_main);
	} catch (const std::bad_alloc& e) {
		delete out_port_video;
		out_port_video = nullptr;
		delete in_port_video;
		in_port_video = nullptr;
	}
}

filter_copy::~filter_copy()
{
	unregister_worker_thread(&wk_main);

	delete out_port_video;
	out_port_video = nullptr;
	delete in_port_video;
	in_port_video = nullptr;
}

const char *filter_copy::get_name() const
{
	return "filt_copy";
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

