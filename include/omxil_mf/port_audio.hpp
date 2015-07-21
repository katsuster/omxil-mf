#ifndef OMX_MF_PORT_AUDIO_HPP__
#define OMX_MF_PORT_AUDIO_HPP__

#include <OMX_Component.h>
#include <OMX_Core.h>
#include <OMX_Audio.h>

#include <omxil_mf/port.hpp>

namespace mf {

class port_audio : public port {
public:
	//親クラス
	typedef port super;

	//disable default constructor
	port_audio() = delete;

	port_audio(int ind, component *c);

	virtual ~port_audio();

	//disable copy constructor
	port_audio(const port_audio& obj) = delete;

	//disable operator=
	port_audio& operator=(const port_audio& obj) = delete;

	virtual const char *get_name() const;

	virtual OMX_STRING get_mime_type() const;
	virtual void set_mime_type(OMX_STRING v);

	virtual OMX_NATIVE_DEVICETYPE get_native_render() const;
	virtual void set_native_render(OMX_NATIVE_DEVICETYPE v);

	virtual OMX_BOOL get_flag_error_concealment() const;
	virtual void set_flag_error_concealment(OMX_BOOL v);

	virtual OMX_AUDIO_CODINGTYPE get_encoding() const;
	virtual void set_encoding(OMX_AUDIO_CODINGTYPE v);

	/**
	 * Get OpenMAX IL definition data of this port.
	 *
	 * OMX_PARAM_PORTDEFINITIONTYPE の各メンバに加え、
	 * format.audio メンバにも設定値がセットされます。
	 *
	 * struct OMX_AUDIO_PORTDEFINITIONTYPE {
	 *     OMX_STRING cMIMEType;
	 *     OMX_NATIVE_DEVICETYPE pNativeRender; 
	 *     OMX_BOOL bFlagErrorConcealment;
	 *     OMX_AUDIO_CODINGTYPE eEncoding;
	 * }
	 *
	 * @return port definition data of OpenMAX IL
	 */
	virtual const OMX_PARAM_PORTDEFINITIONTYPE *get_definition() const;

private:
	OMX_STRING mime_type;
	OMX_NATIVE_DEVICETYPE native_render; 
	OMX_BOOL flag_error_concealment;
	OMX_AUDIO_CODINGTYPE encoding;

};

} //namespace mf

#endif //OMX_MF_PORT_AUDIO_HPP__
