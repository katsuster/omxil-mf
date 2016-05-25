
#include "reader_binary/video_reader_binary.hpp"

namespace mf {

/*
 * Component
 */

video_reader_binary::video_reader_binary(OMX_COMPONENTTYPE *c, const char *cname) 
	: super(c, cname), out_port_video(nullptr)
{
	super::worker_main *wk = &get_worker();
	OMX_VIDEO_PARAM_PORTFORMATTYPE f;

	try {
		out_port_video = new port_video(0, this);
		out_port_video->set_dir(OMX_DirOutput);
		out_port_video->set_buffer_count_actual(4);
		out_port_video->set_buffer_count_min(1);
		out_port_video->set_buffer_size(4096);
		out_port_video->set_buffer_alignment(64);
		f.eCompressionFormat = OMX_VIDEO_CodingAutoDetect;
		f.eColorFormat       = OMX_COLOR_FormatUnused;
		f.xFramerate         = 0;
		out_port_video->add_port_format(f);
		out_port_video->set_default_format(0);

		insert_port(*out_port_video);

		wk->set_out_port(out_port_video);
		register_worker_thread(wk);
	} catch (const std::bad_alloc& e) {
		delete out_port_video;
		out_port_video = nullptr;
	}
}

video_reader_binary::~video_reader_binary()
{
	super::worker_main *wk = &get_worker();

	unregister_worker_thread(wk);

	delete out_port_video;
	out_port_video = nullptr;
}

const char *video_reader_binary::get_name() const
{
	return "vdrd_bin";
}


/*
 * protected functions
 */

OMX_U32 video_reader_binary::get_video_ports()
{
	return 1;
}


/*
 * static public functions
 */

video_reader_binary *video_reader_binary::get_instance(OMX_HANDLETYPE hComponent)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	video_reader_binary *comp = (video_reader_binary *) omx_comp->pComponentPrivate;

	return comp;
}

} //namespace mf
