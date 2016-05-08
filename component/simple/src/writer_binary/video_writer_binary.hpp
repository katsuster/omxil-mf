
#ifndef OMX_MF_COMPONENT_SIMPLE__VIDEO_WRITER_BINARY_HPP__
#define OMX_MF_COMPONENT_SIMPLE__VIDEO_WRITER_BINARY_HPP__

#include <omxil_mf/omxil_mf.h>
#include <omxil_mf/component.hpp>
#include <omxil_mf/port_video.hpp>
#include <omxil_mf/scoped_log.hpp>

namespace mf {

class video_writer_binary : public component {
	class worker_main : public component_worker {
	public:
		//親クラス
		typedef component_worker super;

		worker_main(video_writer_binary *c);
		virtual ~worker_main();

		virtual const char *get_name() const;
		virtual void run();

	private:
		video_writer_binary *comp;

	};


public:
	//親クラス
	typedef component super;

	video_writer_binary(OMX_COMPONENTTYPE *c, const char *cname);
	virtual ~video_writer_binary();

	virtual const char *get_name() const;

protected:
	virtual OMX_U32 get_video_ports();
	virtual OMX_U32 get_video_start_port();

public:
	/**
	 * OpenMAX コンポーネントのハンドルから、
	 * video_writer_binary クラスのインスタンスへのポインタを取得します。
	 *
	 * @param hComponent OpenMAX コンポーネント
	 * @return component インスタンスへのポインタ
	 */
	static video_writer_binary *get_instance(OMX_HANDLETYPE hComponent);

private:
	port_video *in_port_video;
	worker_main wk_main;

};

} //namespace mf

#endif //OMX_MF_COMPONENT_SIMPLE__VIDEO_WRITER_BINARY_HPP__

