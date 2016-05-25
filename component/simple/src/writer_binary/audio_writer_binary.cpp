
#include "writer_binary/audio_writer_binary.hpp"

namespace mf {

/*
 * Component
 */

audio_writer_binary::audio_writer_binary(OMX_COMPONENTTYPE *c, const char *cname) 
	: super(c, cname), in_port_audio(nullptr)
{
	super::worker_main *wk = &get_worker();
	OMX_AUDIO_PARAM_PORTFORMATTYPE f;

	try {
		in_port_audio = new port_audio(0, this);
		in_port_audio->set_dir(OMX_DirInput);
		in_port_audio->set_buffer_count_actual(4);
		in_port_audio->set_buffer_count_min(1);
		in_port_audio->set_buffer_size(4096);
		in_port_audio->set_buffer_alignment(64);
		f.eEncoding = OMX_AUDIO_CodingAutoDetect;
		in_port_audio->add_port_format(f);
		in_port_audio->set_default_format(0);

		insert_port(*in_port_audio);

		wk->set_in_port(in_port_audio);
		register_worker_thread(wk);
	} catch (const std::bad_alloc& e) {
		delete in_port_audio;
		in_port_audio = nullptr;
	}
}

audio_writer_binary::~audio_writer_binary()
{
	super::worker_main *wk = &get_worker();

	unregister_worker_thread(wk);

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
