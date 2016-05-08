
#include "writer_binary/audio_writer_binary.hpp"

namespace mf {

/*
 * Worker thread
 */

audio_writer_binary::worker_main::worker_main(audio_writer_binary *c)
	: component_worker(c), comp(c)
{
}

audio_writer_binary::worker_main::~worker_main()
{
}

const char *audio_writer_binary::worker_main::get_name() const
{
	return "adwr_bin::wrk_main";
}

void audio_writer_binary::worker_main::run()
{
	OMX_ERRORTYPE result;
	port_buffer pb_in;

	while (is_running()) {
		if (is_request_flush()) {
			return;
		}

		result = comp->in_port_audio->pop_buffer(&pb_in);
		if (result != OMX_ErrorNone) {
			errprint("in_port_audio.pop_buffer().\n");
			continue;
		}

		//NOTE: gst-openmax は nOffset を戻さないとおかしな挙動をする？？
		pb_in.header->nOffset = 0;
		comp->in_port_audio->empty_buffer_done(&pb_in);
	}
}


/*
 * Component
 */

audio_writer_binary::audio_writer_binary(OMX_COMPONENTTYPE *c, const char *cname) 
	: component(c, cname),
	in_port_audio(nullptr), wk_main(this)
{
	OMX_AUDIO_PARAM_PORTFORMATTYPE f;

	try {
		in_port_audio = new port_audio(3, this);
		in_port_audio->set_dir(OMX_DirInput);
		in_port_audio->set_buffer_count_actual(4);
		in_port_audio->set_buffer_count_min(1);
		in_port_audio->set_buffer_size(4096);
		in_port_audio->set_buffer_alignment(64);
		f.eEncoding = OMX_AUDIO_CodingAutoDetect;
		in_port_audio->add_port_format(f);
		in_port_audio->set_default_format(0);

		insert_port(*in_port_audio);

		register_worker_thread(&wk_main);
	} catch (const std::bad_alloc& e) {
		delete in_port_audio;
		in_port_audio = nullptr;
	}
}

audio_writer_binary::~audio_writer_binary()
{
	unregister_worker_thread(&wk_main);

	delete in_port_audio;
	in_port_audio = nullptr;
}

const char *audio_writer_binary::get_name() const
{
	return "adwr_bin";
}


/*
 * protected functions
 */

OMX_U32 audio_writer_binary::get_audio_ports()
{
	return 1;
}

OMX_U32 audio_writer_binary::get_audio_start_port()
{
	return 3;
}


/*
 * static public functions
 */

audio_writer_binary *audio_writer_binary::get_instance(OMX_HANDLETYPE hComponent)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	audio_writer_binary *comp = (audio_writer_binary *) omx_comp->pComponentPrivate;

	return comp;
}

} //namespace mf

