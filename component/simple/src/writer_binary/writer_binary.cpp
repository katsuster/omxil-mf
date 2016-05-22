
#include "writer_binary/writer_binary.hpp"

namespace mf {

/*
 * Worker thread
 */

writer_binary::worker_main::worker_main(writer_binary *c)
	: component_worker(c), comp(c), in_port(nullptr)
{
}

writer_binary::worker_main::~worker_main()
{
}

const char *writer_binary::worker_main::get_name() const
{
	return "rend_null::wrk_main";
}

port *writer_binary::worker_main::get_in_port()
{
	return in_port;
}

const port *writer_binary::worker_main::get_in_port() const
{
	return in_port;
}

void writer_binary::worker_main::set_in_port(port *p)
{
	in_port = p;
}

void writer_binary::worker_main::run()
{
	OMX_ERRORTYPE result;
	port_buffer pb_in;

	while (is_running()) {
		if (is_request_flush()) {
			return;
		}

		result = in_port->pop_buffer(&pb_in);
		if (result != OMX_ErrorNone) {
			errprint("in_port_video.pop_buffer().\n");
			continue;
		}

		//NOTE: gst-openmax は nOffset を戻さないとおかしな挙動をする？？
		pb_in.header->nOffset = 0;
		in_port->empty_buffer_done(&pb_in);
	}
}


/*
 * Component
 */

writer_binary::writer_binary(OMX_COMPONENTTYPE *c, const char *cname) 
	: component(c, cname), wk_main(this)
{
	//do nothing
}

writer_binary::~writer_binary()
{
	//do nothing
}

const char *writer_binary::get_name() const
{
	return "rend_null";
}


/*
 * protected functions
 */

writer_binary::worker_main& writer_binary::get_worker()
{
	return wk_main;
}

const writer_binary::worker_main& writer_binary::get_worker() const
{
	return wk_main;
}

OMX_U32 writer_binary::get_audio_start_port()
{
	return 0;
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

