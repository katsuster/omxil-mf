
#include "reader_zero/reader_zero.hpp"

namespace mf {

/*
 * Worker thread
 */

reader_zero::worker_main::worker_main(reader_zero *c)
	: component_worker(c), comp(c)
{
}

reader_zero::worker_main::~worker_main()
{
}

const char *reader_zero::worker_main::get_name() const
{
	return "read_zero::wrk_main";
}

void reader_zero::worker_main::run()
{
	OMX_ERRORTYPE result;
	port_buffer pb_out;
	OMX_U32 len;
	OMX_TICKS stamp = 0;
	int i = 0;

	while (is_running()) {
		if (is_request_flush()) {
			set_request_flush(false);
			return;
		}

		result = comp->out_port_video->pop_buffer(&pb_out);
		if (result != OMX_ErrorNone) {
			errprint("out_port_video.pop_buffer().\n");
			continue;
		}

		if (i % 100 < 50) {
			len = pb_out.header->nAllocLen;
		} else {
			len = pb_out.header->nAllocLen / 2;
		}

		memset(pb_out.header->pBuffer, 0, len);
		pb_out.header->nFilledLen = len;
		pb_out.header->nOffset    = 0;
		pb_out.header->nTimeStamp = stamp;
		pb_out.header->nFlags     = 0;
		comp->out_port_video->fill_buffer_done(&pb_out);

		//next one
		i++;
		//16ms
		stamp += 16000;
	}
}


/*
 * Component
 */

reader_zero::reader_zero(OMX_COMPONENTTYPE *c, const char *cname) 
	: component(c, cname),
	out_port_video(nullptr), wk_main(this)
{
	OMX_VIDEO_PARAM_PORTFORMATTYPE f;

	try {
		out_port_video = new port_video(2, this);
		out_port_video->set_dir(OMX_DirOutput);
		out_port_video->set_buffer_count_actual(4);
		out_port_video->set_buffer_count_min(1);
		out_port_video->set_buffer_size(65536);
		out_port_video->set_buffer_alignment(64);
		out_port_video->set_frame_width(640);
		out_port_video->set_frame_height(480);
		f.eCompressionFormat = OMX_VIDEO_CodingUnused;
		f.eColorFormat       = OMX_COLOR_FormatYUV420Planar;
		f.xFramerate         = 0;
		out_port_video->add_port_format(f);
		out_port_video->set_default_format(0);

		insert_port(*out_port_video);

		register_worker_thread(&wk_main);
	} catch (const std::bad_alloc& e) {
		delete out_port_video;
		out_port_video = nullptr;
	}
}

reader_zero::~reader_zero()
{
	unregister_worker_thread(&wk_main);

	delete out_port_video;
	out_port_video = nullptr;
}

const char *reader_zero::get_name() const
{
	return "read_zero";
}


/*
 * protected functions
 */

OMX_U32 reader_zero::get_video_ports()
{
	return 1;
}

OMX_U32 reader_zero::get_video_start_port()
{
	return 2;
}


/*
 * static public functions
 */

reader_zero *reader_zero::get_instance(OMX_HANDLETYPE hComponent)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	reader_zero *comp = (reader_zero *) omx_comp->pComponentPrivate;

	return comp;
}

} //namespace mf

