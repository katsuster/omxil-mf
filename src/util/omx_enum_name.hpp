
#ifndef OMX_ENUM_NAME_HPP__
#define OMX_ENUM_NAME_HPP__

extern "C" {
#include <OMX_Core.h>
}

namespace mf {

class omx_enum_name {
public:
	omx_enum_name() = delete;
	~omx_enum_name() = delete;
	omx_enum_name& operator=(const omx_enum_name& obj) = delete;
	
	static const char *get_OMX_COMMANDTYPE_name(OMX_COMMANDTYPE id);
	static const char *get_OMX_STATETYPE_name(OMX_STATETYPE id);
	static const char *get_OMX_ERRORTYPE_name(OMX_ERRORTYPE id);
};

} //namespace mf

#endif
