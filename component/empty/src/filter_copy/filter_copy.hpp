
#ifndef OMX_MF_COMPONENT_EMPTY__FILTER_COPY_HPP__
#define OMX_MF_COMPONENT_EMPTY__FILTER_COPY_HPP__

#include <omxil_mf/omxil_mf.h>
#include <omxil_mf/component.hpp>
#include <omxil_mf/port_video.hpp>
#include <omxil_mf/scoped_log.hpp>

namespace mf {

class filter_copy : public component {
public:
	//親クラス
	typedef component super;

	filter_copy(OMX_COMPONENTTYPE *c, const char *cname);
	virtual ~filter_copy();

	virtual const char *get_name() const;

protected:
	virtual OMX_U32 get_video_ports();
	virtual OMX_U32 get_video_start_port();

public:
	/**
	 * OpenMAX コンポーネントのハンドルから、
	 * filter_copy クラスのインスタンスへのポインタを取得します。
	 *
	 * @param hComponent OpenMAX コンポーネント
	 * @return component インスタンスへのポインタ
	 */
	static filter_copy *get_instance(OMX_HANDLETYPE hComponent);

private:
	port_video *in_port_video;
	port_video *out_port_video;


	class worker_main : public component_worker {
	public:
		//親クラス
		typedef component_worker super;

		worker_main(filter_copy *c);
		virtual ~worker_main();

		virtual const char *get_name() const;
		virtual void run();

	private:
		filter_copy *comp;

	};

	worker_main wk_main;

};

} //namespace mf

#endif //OMX_MF_COMPONENT_EMPTY__FILTER_COPY_HPP__

