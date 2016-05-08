
#ifndef OMX_MF_COMPONENT_SIMPLE__AUDIO_WRITER_BINARY_HPP__
#define OMX_MF_COMPONENT_SIMPLE__AUDIO_WRITER_BINARY_HPP__

#include <omxil_mf/omxil_mf.h>
#include <omxil_mf/component.hpp>
#include <omxil_mf/port_audio.hpp>
#include <omxil_mf/scoped_log.hpp>

namespace mf {

class audio_writer_binary : public component {
	class worker_main : public component_worker {
	public:
		//親クラス
		typedef component_worker super;

		worker_main(audio_writer_binary *c);
		virtual ~worker_main();

		virtual const char *get_name() const;
		virtual void run();

	private:
		audio_writer_binary *comp;

	};


public:
	//親クラス
	typedef component super;

	audio_writer_binary(OMX_COMPONENTTYPE *c, const char *cname);
	virtual ~audio_writer_binary();

	virtual const char *get_name() const;

protected:
	virtual OMX_U32 get_audio_ports();
	virtual OMX_U32 get_audio_start_port();

public:
	/**
	 * OpenMAX コンポーネントのハンドルから、
	 * audio_writer_binary クラスのインスタンスへのポインタを取得します。
	 *
	 * @param hComponent OpenMAX コンポーネント
	 * @return component インスタンスへのポインタ
	 */
	static audio_writer_binary *get_instance(OMX_HANDLETYPE hComponent);

private:
	port_audio *in_port_audio;
	worker_main wk_main;

};

} //namespace mf

#endif //OMX_MF_COMPONENT_SIMPLE__AUDIO_WRITER_BINARY_HPP__

