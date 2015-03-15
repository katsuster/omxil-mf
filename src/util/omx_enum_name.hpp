
#ifndef OMX_ENUM_NAME_HPP__
#define OMX_ENUM_NAME_HPP__

extern "C" {
#include <OMX_Types.h>
#include <OMX_Core.h>
#include <OMX_Component.h>
}

namespace mf {

class omx_enum_name {
public:
	omx_enum_name() = delete;
	~omx_enum_name() = delete;
	omx_enum_name& operator=(const omx_enum_name& obj) = delete;

public:
	//OMX_Types.h
	static const char *get_OMX_BOOL_name(OMX_BOOL id);
	static const char *get_OMX_DIRTYPE_name(OMX_DIRTYPE id);
	static const char *get_OMX_ENDIANTYPE_name(OMX_ENDIANTYPE id);
	static const char *get_OMX_NUMERICALDATATYPE_name(OMX_NUMERICALDATATYPE id);

	//OMX_Core.h
	static const char *get_OMX_COMMANDTYPE_name(OMX_COMMANDTYPE id);
	static const char *get_OMX_STATETYPE_name(OMX_STATETYPE id);
	static const char *get_OMX_ERRORTYPE_name(OMX_ERRORTYPE id);
	static const char *get_OMX_EXTRADATATYPE_name(OMX_EXTRADATATYPE id);
	static const char *get_OMX_EVENTTYPE_name(OMX_EVENTTYPE id);
	static const char *get_OMX_BUFFERSUPPLIERTYPE_name(OMX_BUFFERSUPPLIERTYPE id);
	
	//OMX_Component.h
	static const char *get_OMX_PORTDOMAINTYPE_name(OMX_PORTDOMAINTYPE id);
	static const char *get_OMX_SUSPENSIONPOLICYTYPE_name(OMX_SUSPENSIONPOLICYTYPE id);
	static const char *get_OMX_SUSPENSIONTYPE_name(OMX_SUSPENSIONTYPE id);
	static const char *get_OMX_METADATACHARSETTYPE_name(OMX_METADATACHARSETTYPE id);
	static const char *get_OMX_METADATASCOPETYPE_name(OMX_METADATASCOPETYPE id);
	static const char *get_OMX_METADATASEARCHMODETYPE_name(OMX_METADATASEARCHMODETYPE id);

	//OMX_Video.h
	static const char *get_OMX_VIDEO_CODINGTYPE_name(OMX_VIDEO_CODINGTYPE id);
	static const char *get_OMX_VIDEO_CONTROLRATETYPE_name(OMX_VIDEO_CONTROLRATETYPE id);
	static const char *get_OMX_VIDEO_MOTIONVECTORTYPE_name(OMX_VIDEO_MOTIONVECTORTYPE id);
	static const char *get_OMX_VIDEO_INTRAREFRESHTYPE_name(OMX_VIDEO_INTRAREFRESHTYPE id);
	static const char *get_OMX_VIDEO_H263PROFILETYPE_name(OMX_VIDEO_H263PROFILETYPE id);
	static const char *get_OMX_VIDEO_H263LEVELTYPE_name(OMX_VIDEO_H263LEVELTYPE id);
	static const char *get_OMX_VIDEO_PICTURETYPE_name(OMX_VIDEO_PICTURETYPE id);
	static const char *get_OMX_VIDEO_MPEG2PROFILETYPE_name(OMX_VIDEO_MPEG2PROFILETYPE id);
	static const char *get_OMX_VIDEO_MPEG2LEVELTYPE_name(OMX_VIDEO_MPEG2LEVELTYPE id);
	static const char *get_OMX_VIDEO_MPEG4PROFILETYPE_name(OMX_VIDEO_MPEG4PROFILETYPE id);
	static const char *get_OMX_VIDEO_MPEG4LEVELTYPE_name(OMX_VIDEO_MPEG4LEVELTYPE id);
	static const char *get_OMX_VIDEO_WMVFORMATTYPE_name(OMX_VIDEO_WMVFORMATTYPE id);
	static const char *get_OMX_VIDEO_RVFORMATTYPE_name(OMX_VIDEO_RVFORMATTYPE id);
	static const char *get_OMX_VIDEO_AVCPROFILETYPE_name(OMX_VIDEO_AVCPROFILETYPE id);
	static const char *get_OMX_VIDEO_AVCLEVELTYPE_name(OMX_VIDEO_AVCLEVELTYPE id);
	static const char *get_OMX_VIDEO_AVCLOOPFILTERTYPE_name(OMX_VIDEO_AVCLOOPFILTERTYPE id);
	static const char *get_OMX_VIDEO_AVCSLICEMODETYPE_name(OMX_VIDEO_AVCSLICEMODETYPE id);

};

} //namespace mf

#endif
