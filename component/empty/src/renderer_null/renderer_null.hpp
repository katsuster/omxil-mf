
#ifndef OMX_MF_COMPONENT_EMPTY__RENDERER_NULL_HPP__
#define OMX_MF_COMPONENT_EMPTY__RENDERER_NULL_HPP__

#include <omxil_mf/omxil_mf.h>
#include <omxil_mf/component.hpp>
#include <omxil_mf/port_video.hpp>
#include <omxil_mf/scoped_log.hpp>

namespace mf {

class renderer_null : public component {
public:
	//親クラス
	typedef component super;

	renderer_null(OMX_COMPONENTTYPE *c, const char *cname);
	virtual ~renderer_null();

protected:
	virtual void run();

	virtual OMX_U32 get_video_ports();
	virtual OMX_U32 get_video_start_port();

public:
	/**
	 * OpenMAX コンポーネントのハンドルから、
	 * renderer_null クラスのインスタンスへのポインタを取得します。
	 *
	 * @param hComponent OpenMAX コンポーネント
	 * @return component インスタンスへのポインタ
	 */
	static renderer_null *get_instance(OMX_HANDLETYPE hComponent);

private:
	port_video *in_port_video;

};

} //namespace mf

#endif //OMX_MF_COMPONENT_EMPTY__RENDERER_NULL_HPP__

