
#include "reader_binary/audio_reader_binary.hpp"

namespace mf {

/*
 * Component
 */

audio_reader_binary::audio_reader_binary(OMX_COMPONENTTYPE *c, const char *cname) 
	: super(c, cname), out_port_audio(nullptr)
{
	super::worker_main *wk = &get_worker();
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

		wk->set_out_port(out_port_audio);
		register_worker_thread(wk);
	} catch (const std::bad_alloc& e) {
		delete out_port_audio;
		out_port_audio = nullptr;
	}
}

audio_reader_binary::~audio_reader_binary()
{
	super::worker_main *wk = &get_worker();

	unregister_worker_thread(wk);

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
