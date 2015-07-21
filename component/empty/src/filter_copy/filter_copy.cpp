
#include "filter_copy/filter_copy.hpp"

namespace mf {

filter_copy::filter_copy(OMX_COMPONENTTYPE *c, const char *cname) 
	: component(c, cname),
	in_port_video(nullptr), out_port_video(nullptr)
{
	try {
		in_port_video = new port_video(0, this);
		in_port_video->set_dir(OMX_DirInput);
		in_port_video->set_buffer_count_actual(4);
		in_port_video->set_buffer_count_min(1);
		in_port_video->set_buffer_size(32768);
		in_port_video->set_buffer_alignment(64);
		in_port_video->set_compression_format(OMX_VIDEO_CodingAVC);
		in_port_video->set_color_format(OMX_COLOR_FormatUnused);

		insert_port(*in_port_video);

		out_port_video = new port_video(1, this);
		out_port_video->set_dir(OMX_DirOutput);
		out_port_video->set_buffer_count_actual(4);
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
	//do nothing
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
	return 0;
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

