
#include "renderer_null/renderer_null.hpp"

namespace mf {

renderer_null::renderer_null(OMX_COMPONENTTYPE *c, const char *cname) 
	: component(c, cname),
	in_port_video(nullptr)
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
	} catch (const std::bad_alloc& e) {
		delete in_port_video;
		in_port_video = nullptr;
	}
}

renderer_null::~renderer_null()
{
	delete in_port_video;
	in_port_video = nullptr;
}

void renderer_null::run()
{
	//do nothing
}


/*
 * protected functions
 */

OMX_U32 renderer_null::get_video_ports()
{
	return 1;
}

OMX_U32 renderer_null::get_video_start_port()
{
	return 0;
}


/*
 * static public functions
 */

renderer_null *renderer_null::get_instance(OMX_HANDLETYPE hComponent)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	renderer_null *comp = (renderer_null *) omx_comp->pComponentPrivate;

	return comp;
}

} //namespace mf

