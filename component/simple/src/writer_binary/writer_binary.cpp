
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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
	return "wr_bin::wrk_main";
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

void writer_binary::worker_main::write_file(int fd)
{
	port_buffer pb_in;
	ssize_t nwritten;
	OMX_ERRORTYPE result;

	while (is_running()) {
		if (is_request_flush()) {
			return;
		}

		result = in_port->pop_buffer(&pb_in);
		if (result != OMX_ErrorNone) {
			errprint("in_port_video.pop_buffer().\n");
			continue;
		}

		if (pb_in.remain() == 0) {
			//no data
			in_port->fill_buffer_done(&pb_in);
			continue;
		}

		nwritten = write(fd, pb_in.get_ptr(), pb_in.remain());
		if (nwritten == -1) {
			//Error
			errprint("write() failed.\n");
			break;
		}

		pb_in.skip(nwritten);
		in_port->empty_buffer_done(&pb_in);
	}
}

void writer_binary::worker_main::run()
{
	int fd = -1;

	fd = open(comp->uri_target.c_str(), O_RDWR);
	if (fd == -1) {
		errprint("open(%s) failed.\n", comp->uri_target.c_str());
		return;
	}

	try {
		write_file(fd);
	} catch (const std::exception& ex) {
		errprint("catch exception '%s'.\n", ex.what());
	} catch (...) {
		errprint("catch unknown exception.\n");
	}

	close(fd);
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
	return "wr_bin";
}

std::string& writer_binary::get_uri()
{
	return uri_target;
}

const std::string& writer_binary::get_uri() const
{
	return uri_target;
}

void writer_binary::set_uri(char *uri)
{
	uri_target = uri;
}

void writer_binary::set_uri(std::string& uri)
{
	uri_target = uri;
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
	return 0;
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

