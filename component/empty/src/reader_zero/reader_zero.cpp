
#include "reader_zero/reader_zero.hpp"

namespace mf {

reader_zero::reader_zero(OMX_COMPONENTTYPE *c, const char *cname) 
	: component(c, cname),
	out_port_video(nullptr)
{
	try {
		out_port_video = new port_video(0, this);
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
	}
}

reader_zero::~reader_zero()
{
	delete out_port_video;
	out_port_video = nullptr;
}

void reader_zero::run()
{
	//do nothing
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
	return 0;
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

