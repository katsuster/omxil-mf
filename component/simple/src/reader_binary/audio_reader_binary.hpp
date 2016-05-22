
#ifndef OMX_MF_COMPONENT_SIMPLE__AUDIO_READER_BINARY_HPP__
#define OMX_MF_COMPONENT_SIMPLE__AUDIO_READER_BINARY_HPP__

#include <omxil_mf/omxil_mf.h>
#include <omxil_mf/component.hpp>
#include <omxil_mf/port_audio.hpp>
#include <omxil_mf/scoped_log.hpp>

#include "reader_binary/reader_binary.hpp"

namespace mf {

class audio_reader_binary : public reader_binary {
public:
	//親クラス
	typedef reader_binary super;

	audio_reader_binary(OMX_COMPONENTTYPE *c, const char *cname);
	virtual ~audio_reader_binary();

	virtual const char *get_name() const;

protected:
	virtual OMX_U32 get_audio_ports();

public:
	/**
	 * OpenMAX コンポーネントのハンドルから、
	 * audio_reader_binary クラスのインスタンスへのポインタを取得します。
	 *
	 * @param hComponent OpenMAX コンポーネント
	 * @return component インスタンスへのポインタ
	 */
	static audio_reader_binary *get_instance(OMX_HANDLETYPE hComponent);

private:
	port_audio *out_port_audio;

};

} //namespace mf

#endif //OMX_MF_COMPONENT_SIMPLE__AUDIO_READER_BINARY_HPP__

