
#include "writer_binary/writer_binary.hpp"

namespace mf {

/*
 * Worker thread
 */

writer_binary::worker_main::worker_main(writer_binary *c)
	: component_worker(c), comp(c)
{
}

writer_binary::worker_main::~worker_main()
{
}

const char *writer_binary::worker_main::get_name() const
{
	return "rend_null::wrk_main";
}

void writer_binary::worker_main::run()
{
	OMX_ERRORTYPE result;
	port_buffer pb_in;

	while (is_running()) {
		if (is_request_flush()) {
			return;
		}

		result = comp->in_port_video->pop_buffer(&pb_in);
		if (result != OMX_ErrorNone) {
			errprint("in_port_video.pop_buffer().\n");
			continue;
		}

		//NOTE: gst-openmax は nOffset を戻さないとおかしな挙動をする？？
		pb_in.header->nOffset = 0;
		comp->in_port_video->empty_buffer_done(&pb_in);
	}
}


/*
 * Component
 */

writer_binary::writer_binary(OMX_COMPONENTTYPE *c, const char *cname) 
	: component(c, cname),
	in_port_video(nullptr), wk_main(this)
{
	OMX_VIDEO_PARAM_PORTFORMATTYPE f;

	try {
		in_port_video = new port_video(3, this);
		in_port_video->set_dir(OMX_DirInput);
		in_port_video->set_buffer_count_actual(4);
		in_port_video->set_buffer_count_min(1);
		in_port_video->set_buffer_size(32768);
		in_port_video->set_buffer_alignment(64);
		f.eCompressionFormat = OMX_VIDEO_CodingUnused;
		f.eColorFormat       = OMX_COLOR_FormatYUV420Planar;
		f.xFramerate         = 0;
		in_port_video->add_port_format(f);
		in_port_video->set_default_format(0);

		insert_port(*in_port_video);

		register_worker_thread(&wk_main);
	} catch (const std::bad_alloc& e) {
		delete in_port_video;
		in_port_video = nullptr;
	}
}

writer_binary::~writer_binary()
{
	unregister_worker_thread(&wk_main);

	delete in_port_video;
	in_port_video = nullptr;
}

const char *writer_binary::get_name() const
{
	return "rend_null";
}


/*
 * protected functions
 */

OMX_U32 writer_binary::get_video_ports()
{
	return 1;
}

OMX_U32 writer_binary::get_video_start_port()
{
	return 3;
}


/*
 * static public functions
 */

writer_binary *writer_binary::get_instance(OMX_HANDLETYPE hComponent)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	writer_binary *comp = (writer_binary *) omx_comp->pComponentPrivate;

	return comp;
}

} //namespace mf

