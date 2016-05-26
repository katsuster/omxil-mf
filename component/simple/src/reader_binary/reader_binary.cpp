
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "reader_binary/reader_binary.hpp"

namespace mf {

/*
 * Worker thread
 */

reader_binary::worker_main::worker_main(reader_binary *c)
	: component_worker(c), comp(c), out_port(nullptr)
{
}

reader_binary::worker_main::~worker_main()
{
}

const char *reader_binary::worker_main::get_name() const
{
	return "rd_bin::wrk_main";
}

port *reader_binary::worker_main::get_out_port()
{
	return out_port;
}

const port *reader_binary::worker_main::get_out_port() const
{
	return out_port;
}

void reader_binary::worker_main::set_out_port(port *p)
{
	out_port = p;
}

void reader_binary::worker_main::send_eos()
{
	port_buffer pb_out;
	OMX_ERRORTYPE result;

	result = out_port->pop_buffer(&pb_out);
	if (result != OMX_ErrorNone) {
		errprint("out_port.pop_buffer().\n");
		return;
	}

	pb_out.header->nOffset    = 0;
	pb_out.header->nFilledLen = 0;
	pb_out.header->nFlags     = OMX_BUFFERFLAG_EOS;
	//FIXME: クロックコンポーネントに対応すべき？
	pb_out.header->nTimeStamp = 0;
	out_port->fill_buffer_done(&pb_out);
}

void reader_binary::worker_main::read_file(int fd)
{
	port_buffer pb_out;
	ssize_t nread;
	OMX_ERRORTYPE result;

	while (is_running()) {
		if (is_request_flush()) {
			return;
		}

		result = out_port->pop_buffer(&pb_out);
		if (result != OMX_ErrorNone) {
			errprint("out_port.pop_buffer().\n");
			continue;
		}

		if (pb_out.remain() == 0) {
			//no space
			out_port->fill_buffer_done(&pb_out);
			continue;
		}

		nread = read(fd, pb_out.get_ptr(), pb_out.remain());
		if (nread == 0) {
			//EOF
			infoprint("read() reached EOF.\n");
			send_eos();
			break;
		} else if (nread == -1) {
			//Error
			errprint("read() failed.\n");
			break;
		}

		pb_out.skip(nread);
		pb_out.header->nFlags     = 0;
		//FIXME: クロックコンポーネントに対応すべき？
		pb_out.header->nTimeStamp = 0;
		out_port->fill_buffer_done(&pb_out);
	}
}

void reader_binary::worker_main::run()
{
	int fd = -1;

	fd = open(comp->uri_target.c_str(), O_RDONLY);
	if (fd == -1) {
		errprint("open(%s) failed.\n", comp->uri_target.c_str());
		return;
	}

	try {
		read_file(fd);
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

reader_binary::reader_binary(OMX_COMPONENTTYPE *c, const char *cname) 
	: component(c, cname), wk_main(this)
{
	//do nothing
}

reader_binary::~reader_binary()
{
	//do nothing
}

const char *reader_binary::get_name() const
{
	return "rd_bin";
}

std::string& reader_binary::get_uri()
{
	return uri_target;
}

const std::string& reader_binary::get_uri() const
{
	return uri_target;
}

void reader_binary::set_uri(char *uri)
{
	uri_target = uri;
}

void reader_binary::set_uri(std::string& uri)
{
	uri_target = uri;
}

OMX_ERRORTYPE reader_binary::GetParameter(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure)
{
	scoped_log_begin;
	void *ptr = nullptr;
	OMX_ERRORTYPE err;

	ptr = pComponentParameterStructure;

	switch (nParamIndex) {
	case OMX_IndexParamContentURI: {
		OMX_PARAM_CONTENTURITYPE *p_uri = static_cast<OMX_PARAM_CONTENTURITYPE *>(ptr);
		size_t len_uri;

		len_uri = p_uri->nSize - sizeof(OMX_PARAM_CONTENTURITYPE);
		if (len_uri < uri_target.size()) {
			errprint("length %d is too short, needs %d.\n",
				(int)len_uri, (int)uri_target.size());
			return OMX_ErrorBadParameter;
		}
		strcpy((char *)p_uri->contentURI, uri_target.c_str());

		err = OMX_ErrorNone;
		break;
	}
	default:
		err = super::GetParameter(hComponent, nParamIndex, pComponentParameterStructure);
		break;
	}

	return err;
}

OMX_ERRORTYPE reader_binary::SetParameter(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure)
{
	scoped_log_begin;
	void *ptr = nullptr;
	OMX_ERRORTYPE err;

	ptr = pComponentParameterStructure;

	switch (nParamIndex) {
	case OMX_IndexParamContentURI: {
		OMX_PARAM_CONTENTURITYPE *p_uri = static_cast<OMX_PARAM_CONTENTURITYPE *>(ptr);

		uri_target = (char *)p_uri->contentURI;

		break;
	}
	default:
		err = super::SetParameter(hComponent, nParamIndex, pComponentParameterStructure);
		break;
	}

	return err;
}


/*
 * protected functions
 */

reader_binary::worker_main& reader_binary::get_worker()
{
	return wk_main;
}

const reader_binary::worker_main& reader_binary::get_worker() const
{
	return wk_main;
}

OMX_U32 reader_binary::get_audio_start_port()
{
	return 0;
}

OMX_U32 reader_binary::get_video_start_port()
{
	return 0;
}


/*
 * static public functions
 */

reader_binary *reader_binary::get_instance(OMX_HANDLETYPE hComponent)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	reader_binary *comp = (reader_binary *) omx_comp->pComponentPrivate;

	return comp;
}

} //namespace mf
