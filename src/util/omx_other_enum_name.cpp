
#include "util/omx_enum_name.hpp"

namespace mf {

const char *omx_enum_name::get_OMX_OTHER_FORMATTYPE_name(OMX_OTHER_FORMATTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_OTHER_FormatTime:
		name = "OMX_OTHER_FormatTime";
		break;
	case OMX_OTHER_FormatPower:
		name = "OMX_OTHER_FormatPower";
		break;
	case OMX_OTHER_FormatStats:
		name = "OMX_OTHER_FormatStats";
		break;
	case OMX_OTHER_FormatBinary:
		name = "OMX_OTHER_FormatBinary";
		break;
	case OMX_OTHER_FormatVendorReserved:
		name = "OMX_OTHER_FormatVendorReserved";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_OTHER_FormatKhronosExtensions <= id && 
		id < OMX_OTHER_FormatVendorStartUnused) {
		name = "(OMX_OTHER_FormatKhronosExtensions)";
	}
	if (OMX_OTHER_FormatVendorStartUnused <= id && 
		id < OMX_OTHER_FormatMax) {
		name = "(OMX_OTHER_FormatVendorStartUnused)";
	}

	return name;
}

} //namespace mf
