
#include "reader_binary/audio_reader_binary.hpp"

namespace mf {

/*
 * Worker thread
 */

audio_reader_binary::worker_main::worker_main(audio_reader_binary *c)
	: component_worker(c), comp(c)
{
}

audio_reader_binary::worker_main::~worker_main()
{
}

const char *audio_reader_binary::worker_main::get_name() const
{
	return "adrd_bin::wrk_main";
}

void audio_reader_binary::worker_main::run()
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

		result = comp->out_port_audio->pop_buffer(&pb_out);
		if (result != OMX_ErrorNone) {
			errprint("out_port_audio.pop_buffer().\n");
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
		comp->out_port_audio->fill_buffer_done(&pb_out);

		//next one
		i++;
		//16ms
		stamp += 16000;
	}
}


/*
 * Component
 */

audio_reader_binary::audio_reader_binary(OMX_COMPONENTTYPE *c, const char *cname) 
	: component(c, cname),
	out_port_audio(nullptr), wk_main(this)
{
	OMX_AUDIO_PARAM_PORTFORMATTYPE f;

	try {
		out_port_audio = new port_audio(2, this);
		out_port_audio->set_dir(OMX_DirOutput);
		out_port_audio->set_buffer_count_actual(4);
		out_port_audio->set_buffer_count_min(1);
		out_port_audio->set_buffer_size(4096);
		out_port_audio->set_buffer_alignment(64);
		f.eEncoding = OMX_AUDIO_CodingAutoDetect;
		out_port_audio->add_port_format(f);
		out_port_audio->set_default_format(0);

		insert_port(*out_port_audio);

		register_worker_thread(&wk_main);
	} catch (const std::bad_alloc& e) {
		delete out_port_audio;
		out_port_audio = nullptr;
	}
}

audio_reader_binary::~audio_reader_binary()
{
	unregister_worker_thread(&wk_main);

	delete out_port_audio;
	out_port_audio = nullptr;
}

const char *audio_reader_binary::get_name() const
{
	return "adrd_bin";
}


/*
 * protected functions
 */

OMX_U32 audio_reader_binary::get_audio_ports()
{
	return 1;
}

OMX_U32 audio_reader_binary::get_audio_start_port()
{
	return 2;
}


/*
 * static public functions
 */

audio_reader_binary *audio_reader_binary::get_instance(OMX_HANDLETYPE hComponent)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	audio_reader_binary *comp = (audio_reader_binary *) omx_comp->pComponentPrivate;

	return comp;
}

} //namespace mf

