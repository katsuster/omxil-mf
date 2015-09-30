#include <omxil_mf/port_format.hpp>
#include <omxil_mf/scoped_log.hpp>

#include "util/omx_enum_name.hpp"

namespace mf {

port_format::port_format(const OMX_AUDIO_PARAM_PORTFORMATTYPE& pf)
	: dom_type(OMX_PortDomainAudio),
	pf_audio(pf), pf_video(),
	pf_image(), pf_other()
{
	//do nothing
}

port_format::port_format(const OMX_VIDEO_PARAM_PORTFORMATTYPE& pf)
	: dom_type(OMX_PortDomainVideo),
	pf_audio(), pf_video(pf),
	pf_image(), pf_other()
{
	//do nothing
}

port_format::port_format(const OMX_IMAGE_PARAM_PORTFORMATTYPE& pf)
	: dom_type(OMX_PortDomainImage),
	pf_audio(), pf_video(),
	pf_image(pf), pf_other()
{
	//do nothing
}

port_format::port_format(const OMX_OTHER_PARAM_PORTFORMATTYPE& pf)
	: dom_type(OMX_PortDomainOther),
	pf_audio(), pf_video(),
	pf_image(), pf_other(pf)
{
	//do nothing
}

port_format::port_format(const OMX_PARAM_PORTDEFINITIONTYPE& def)
	: dom_type(def.eDomain),
	pf_audio(), pf_video(),
	pf_image(), pf_other()
{
	scoped_log_begin;

	switch (dom_type) {
	case OMX_PortDomainAudio:
		pf_audio.eEncoding = def.format.audio.eEncoding;
		break;
	case OMX_PortDomainVideo:
		pf_video.eCompressionFormat = def.format.video.eCompressionFormat;
		pf_video.eColorFormat       = def.format.video.eColorFormat;
		pf_video.xFramerate         = def.format.video.xFramerate;
		break;
	case OMX_PortDomainImage:
		pf_image.eCompressionFormat = def.format.image.eCompressionFormat;
		pf_image.eColorFormat       = def.format.image.eColorFormat;
		break;
	case OMX_PortDomainOther:
		pf_other.eFormat = def.format.other.eFormat;
		break;
	default:
		//do nothing
		break;
	}
}

port_format::~port_format()
{
	//do nothing
}

OMX_PORTDOMAINTYPE port_format::get_domain_type() const
{
	return dom_type;
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
	if (dom_type == OMX_PortDomainVideo) {
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

void port_format::dump(const char *msg) const
{
	switch (dom_type) {
	case OMX_PortDomainAudio:
		dprint("port_format dump: %s: \n"
			"    dom_type :0x%08x(%s), \n"
			"    eEncoding:0x%08x(%s).\n",
			msg,
			(int)dom_type,
			omx_enum_name::get_OMX_PORTDOMAINTYPE_name(dom_type),
			(int)pf_audio.eEncoding,
			omx_enum_name::get_OMX_AUDIO_CODINGTYPE_name(pf_audio.eEncoding));
		break;
	case OMX_PortDomainVideo:
		dprint("port_format dump: %s: \n"
			"    dom_type          :0x%08x(%s), \n"
			"    eCompressionFormat:0x%08x(%s), \n"
			"    eColorFormat      :0x%08x(%s), \n"
			"    xFramerate        :0x%08x.\n",
			msg,
			(int)dom_type,
			omx_enum_name::get_OMX_PORTDOMAINTYPE_name(dom_type),
			(int)pf_video.eCompressionFormat,
			omx_enum_name::get_OMX_VIDEO_CODINGTYPE_name(pf_video.eCompressionFormat),
			(int)pf_video.eColorFormat,
			omx_enum_name::get_OMX_COLOR_FORMATTYPE_name(pf_video.eColorFormat),
			(int)pf_video.xFramerate);
		break;
	case OMX_PortDomainImage:
		dprint("port_format dump: %s: \n"
			"    dom_type          :0x%08x(%s), \n"
			"    eCompressionFormat:0x%08x(%s), \n"
			"    eColorFormat      :0x%08x(%s).\n",
			msg,
			(int)dom_type,
			omx_enum_name::get_OMX_PORTDOMAINTYPE_name(dom_type),
			(int)pf_image.eCompressionFormat,
			omx_enum_name::get_OMX_IMAGE_CODINGTYPE_name(pf_image.eCompressionFormat),
			(int)pf_image.eColorFormat,
			omx_enum_name::get_OMX_COLOR_FORMATTYPE_name(pf_image.eColorFormat));
		break;
	case OMX_PortDomainOther:
		dprint("port_format dump: %s: \n"
			"    dom_type:0x%08x(%s), \n"
			"    eFormat :0x%08x(%s).\n",
			msg,
			(int)dom_type,
			omx_enum_name::get_OMX_PORTDOMAINTYPE_name(dom_type),
			(int)pf_other.eFormat,
			omx_enum_name::get_OMX_OTHER_FORMATTYPE_name(pf_other.eFormat));
		break;
	default:
		dprint("port_format dump: %s: dom_type:0x%08x(%s).\n",
			msg,
			(int)dom_type,
			omx_enum_name::get_OMX_PORTDOMAINTYPE_name(dom_type));
		break;
	}
}

} //namespace mf
