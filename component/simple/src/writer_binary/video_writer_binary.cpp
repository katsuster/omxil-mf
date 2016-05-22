
#include "writer_binary/video_writer_binary.hpp"

namespace mf {

/*
 * Component
 */

video_writer_binary::video_writer_binary(OMX_COMPONENTTYPE *c, const char *cname) 
	: super(c, cname), in_port_video(nullptr)
{
	super::worker_main *wk = &get_worker();
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

		wk->set_in_port(in_port_video);
		register_worker_thread(wk);
	} catch (const std::bad_alloc& e) {
		delete in_port_video;
		in_port_video = nullptr;
	}
}

video_writer_binary::~video_writer_binary()
{
	super::worker_main *wk = &get_worker();

	unregister_worker_thread(wk);

	delete in_port_video;
	in_port_video = nullptr;
}

const char *video_writer_binary::get_name() const
{
	return "vdwr_bin";
}


/*
 * protected functions
 */

OMX_U32 video_writer_binary::get_video_ports()
{
	return 1;
}


/*
 * static public functions
 */

video_writer_binary *video_writer_binary::get_instance(OMX_HANDLETYPE hComponent)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	video_writer_binary *comp = (video_writer_binary *) omx_comp->pComponentPrivate;

	return comp;
}

} //namespace mf

