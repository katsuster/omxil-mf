
#include <omxil_mf/port_format.hpp>
#include <omxil_mf/scoped_log.hpp>

namespace mf {

port_format::port_format(const OMX_AUDIO_PARAM_PORTFORMATTYPE& pf)
	: pf_audio(pf), pf_video(),
	pf_image(), pf_other(),
	dom_type(OMX_PortDomainAudio)
{
	scoped_log_begin;
	//do nothing
}

port_format::port_format(const OMX_VIDEO_PARAM_PORTFORMATTYPE& pf)
	: pf_audio(), pf_video(pf),
	pf_image(), pf_other(),
	dom_type(OMX_PortDomainVideo)
{
	scoped_log_begin;
	//do nothing
}

port_format::port_format(const OMX_IMAGE_PARAM_PORTFORMATTYPE& pf)
	: pf_audio(), pf_video(),
	pf_image(pf), pf_other(),
	dom_type(OMX_PortDomainImage)
{
	scoped_log_begin;
	//do nothing
}

port_format::port_format(const OMX_OTHER_PARAM_PORTFORMATTYPE& pf)
	: pf_audio(), pf_video(),
	pf_image(), pf_other(pf),
	dom_type(OMX_PortDomainOther)
{
	scoped_log_begin;
	//do nothing
}

port_format::~port_format()
{
	scoped_log_begin;
	//do nothing
}

const OMX_AUDIO_PARAM_PORTFORMATTYPE *port_format::get_format_audio() const
{
	if (dom_type == OMX_PortDomainAudio) {
		return &pf_audio;
	} else {
		return nullptr;
	}
}

const OMX_VIDEO_PARAM_PORTFORMATTYPE *port_format::get_format_video() const
{
	if (dom_type == OMX_PortDomainAudio) {
		return &pf_video;
	} else {
		return nullptr;
	}
}

const OMX_IMAGE_PARAM_PORTFORMATTYPE *port_format::get_format_image() const
{
	if (dom_type == OMX_PortDomainImage) {
		return &pf_image;
	} else {
		return nullptr;
	}
}

const OMX_OTHER_PARAM_PORTFORMATTYPE *port_format::get_format_other() const
{
	if (dom_type == OMX_PortDomainOther) {
		return &pf_other;
	} else {
		return nullptr;
	}
}

OMX_PORTDOMAINTYPE port_format::get_domain_type() const
{
	return dom_type;
}

} //namespace mf
