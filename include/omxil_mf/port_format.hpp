#ifndef OMX_MF_PORT_FORMAT_HPP__
#define OMX_MF_PORT_FORMAT_HPP__

#include <OMX_Component.h>
#include <OMX_Core.h>
#include <OMX_Audio.h>
#include <OMX_Video.h>
#include <OMX_Image.h>
#include <OMX_Other.h>

namespace mf {

class port_format {
public:
	//親クラス
	//typedef xxxx super;

	//disable default constructor
	port_format() = delete;

	explicit port_format(const OMX_AUDIO_PARAM_PORTFORMATTYPE& pf);
	explicit port_format(const OMX_VIDEO_PARAM_PORTFORMATTYPE& pf);
	explicit port_format(const OMX_IMAGE_PARAM_PORTFORMATTYPE& pf);
	explicit port_format(const OMX_OTHER_PARAM_PORTFORMATTYPE& pf);

	~port_format();

	port_format(const port_format& obj) = default;
	port_format& operator=(const port_format& obj) = default;

	const OMX_AUDIO_PARAM_PORTFORMATTYPE *get_format_audio() const;
	const OMX_VIDEO_PARAM_PORTFORMATTYPE *get_format_video() const;
	const OMX_IMAGE_PARAM_PORTFORMATTYPE *get_format_image() const;
	const OMX_OTHER_PARAM_PORTFORMATTYPE *get_format_other() const;
	OMX_PORTDOMAINTYPE get_domain_type() const;

	/**
	 * ダンプをデバッグ出力します。
	 *
	 * @param msg 一緒に出力する文字列
	 */
	void dump(const char *msg);

private:
	OMX_AUDIO_PARAM_PORTFORMATTYPE pf_audio;
	OMX_VIDEO_PARAM_PORTFORMATTYPE pf_video;
	OMX_IMAGE_PARAM_PORTFORMATTYPE pf_image;
	OMX_OTHER_PARAM_PORTFORMATTYPE pf_other;
	OMX_PORTDOMAINTYPE dom_type;

};

} //namespace mf

#endif //OMX_MF_PORT_FORMAT_HPP__
