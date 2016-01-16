
#ifndef OMX_MF_COMPONENT_READER_TS__READER_TS_HPP__
#define OMX_MF_COMPONENT_READER_TS__READER_TS_HPP__

#include <omxil_mf/omxil_mf.h>
#include <omxil_mf/component.hpp>
#include <omxil_mf/port_video.hpp>
#include <omxil_mf/scoped_log.hpp>

namespace mf {

class reader_ts : public component {
	class worker_main : public component_worker {
	public:
		//親クラス
		typedef component_worker super;

		worker_main(reader_ts *c);
		virtual ~worker_main();

		virtual const char *get_name() const;
		virtual void run();

	private:
		reader_ts *comp;

	};


public:
	//親クラス
	typedef component super;

	reader_ts(OMX_COMPONENTTYPE *c, const char *cname);
	virtual ~reader_ts();

	virtual const char *get_name() const;

protected:
	virtual OMX_U32 get_video_ports();
	virtual OMX_U32 get_video_start_port();

public:
	/**
	 * OpenMAX コンポーネントのハンドルから、
	 * reader_ts クラスのインスタンスへのポインタを取得します。
	 *
	 * @param hComponent OpenMAX コンポーネント
	 * @return component インスタンスへのポインタ
	 */
	static reader_ts *get_instance(OMX_HANDLETYPE hComponent);

private:
	port_video *out_port_video;
	worker_main wk_main;

};

} //namespace mf

#endif //OMX_MF_COMPONENT_READER_TS__READER_TS_HPP__

