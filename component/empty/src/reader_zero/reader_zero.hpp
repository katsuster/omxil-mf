
#ifndef OMX_MF_COMPONENT_EMPTY__READER_ZERO_HPP__
#define OMX_MF_COMPONENT_EMPTY__READER_ZERO_HPP__

#include <omxil_mf/omxil_mf.h>
#include <omxil_mf/component.hpp>
#include <omxil_mf/port_video.hpp>

namespace mf {

class reader_zero : public component {
public:
	//親クラス
	typedef component super;

	reader_zero(OMX_COMPONENTTYPE *c, const char *cname);
	virtual ~reader_zero();

	virtual void run();

protected:
	virtual OMX_U32 get_video_ports();
	virtual OMX_U32 get_video_start_port();

public:
	/**
	 * OpenMAX コンポーネントのハンドルから、
	 * reader_zero クラスのインスタンスへのポインタを取得します。
	 *
	 * @param hComponent OpenMAX コンポーネント
	 * @return component インスタンスへのポインタ
	 */
	static reader_zero *get_instance(OMX_HANDLETYPE hComponent);

private:
	port_video *out_port_video;

};

} //namespace mf

#endif //OMX_MF_COMPONENT_EMPTY__READER_ZERO_HPP__

