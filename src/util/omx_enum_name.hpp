
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

};

} //namespace mf

#endif
