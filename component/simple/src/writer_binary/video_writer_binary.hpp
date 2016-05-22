
#ifndef OMX_MF_COMPONENT_SIMPLE__VIDEO_WRITER_BINARY_HPP__
#define OMX_MF_COMPONENT_SIMPLE__VIDEO_WRITER_BINARY_HPP__

#include <omxil_mf/omxil_mf.h>
#include <omxil_mf/component.hpp>
#include <omxil_mf/port_video.hpp>
#include <omxil_mf/scoped_log.hpp>

#include "writer_binary/writer_binary.hpp"

namespace mf {

class video_writer_binary : public writer_binary {
public:
	//親クラス
	typedef writer_binary super;

	video_writer_binary(OMX_COMPONENTTYPE *c, const char *cname);
	virtual ~video_writer_binary();

	virtual const char *get_name() const;

protected:
	virtual OMX_U32 get_video_ports();

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

};

} //namespace mf

#endif //OMX_MF_COMPONENT_SIMPLE__VIDEO_WRITER_BINARY_HPP__
