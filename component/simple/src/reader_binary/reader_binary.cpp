
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

void reader_binary::worker_main::run()
{
	OMX_ERRORTYPE result;
	port_buffer pb_out;
	OMX_U32 len;
	OMX_TICKS stamp = 0;
	int i = 0;

	while (is_running()) {
		if (is_request_flush()) {
			return;
		}

		result = out_port->pop_buffer(&pb_out);
		if (result != OMX_ErrorNone) {
			errprint("out_port.pop_buffer().\n");
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
		out_port->fill_buffer_done(&pb_out);

		//next one
		i++;
		//16ms
		stamp += 16000;
	}
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
