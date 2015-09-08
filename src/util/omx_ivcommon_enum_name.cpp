
#include "util/omx_enum_name.hpp"

namespace mf {

const char *omx_enum_name::get_OMX_COLOR_FORMATTYPE_name(OMX_COLOR_FORMATTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_COLOR_FormatUnused:
		name = "OMX_COLOR_FormatUnused";
		break;
	case OMX_COLOR_FormatMonochrome:
		name = "OMX_COLOR_FormatMonochrome";
		break;
	case OMX_COLOR_Format8bitRGB332:
		name = "OMX_COLOR_Format8bitRGB332";
		break;
	case OMX_COLOR_Format12bitRGB444:
		name = "OMX_COLOR_Format12bitRGB444";
		break;
	case OMX_COLOR_Format16bitARGB4444:
		name = "OMX_COLOR_Format16bitARGB4444";
		break;
	case OMX_COLOR_Format16bitARGB1555:
		name = "OMX_COLOR_Format16bitARGB1555";
		break;
	case OMX_COLOR_Format16bitRGB565:
		name = "OMX_COLOR_Format16bitRGB565";
		break;
	case OMX_COLOR_Format16bitBGR565:
		name = "OMX_COLOR_Format16bitBGR565";
		break;
	case OMX_COLOR_Format18bitRGB666:
		name = "OMX_COLOR_Format18bitRGB666";
		break;
	case OMX_COLOR_Format18bitARGB1665:
		name = "OMX_COLOR_Format18bitARGB1665";
		break;
	case OMX_COLOR_Format19bitARGB1666:
		name = "OMX_COLOR_Format19bitARGB1666";
		break;
	case OMX_COLOR_Format24bitRGB888:
		name = "OMX_COLOR_Format24bitRGB888";
		break;
	case OMX_COLOR_Format24bitBGR888:
		name = "OMX_COLOR_Format24bitBGR888";
		break;
	case OMX_COLOR_Format24bitARGB1887:
		name = "OMX_COLOR_Format24bitARGB1887";
		break;
	case OMX_COLOR_Format25bitARGB1888:
		name = "OMX_COLOR_Format25bitARGB1888";
		break;
	case OMX_COLOR_Format32bitBGRA8888:
		name = "OMX_COLOR_Format32bitBGRA8888";
		break;
	case OMX_COLOR_Format32bitARGB8888:
		name = "OMX_COLOR_Format32bitARGB8888";
		break;
	case OMX_COLOR_FormatYUV411Planar:
		name = "OMX_COLOR_FormatYUV411Planar";
		break;
	case OMX_COLOR_FormatYUV411PackedPlanar:
		name = "OMX_COLOR_FormatYUV411PackedPlanar";
		break;
	case OMX_COLOR_FormatYUV420Planar:
		name = "OMX_COLOR_FormatYUV420Planar";
		break;
	case OMX_COLOR_FormatYUV420PackedPlanar:
		name = "OMX_COLOR_FormatYUV420PackedPlanar";
		break;
	case OMX_COLOR_FormatYUV420SemiPlanar:
		name = "OMX_COLOR_FormatYUV420SemiPlanar";
		break;
	case OMX_COLOR_FormatYUV422Planar:
		name = "OMX_COLOR_FormatYUV422Planar";
		break;
	case OMX_COLOR_FormatYUV422PackedPlanar:
		name = "OMX_COLOR_FormatYUV422PackedPlanar";
		break;
	case OMX_COLOR_FormatYUV422SemiPlanar:
		name = "OMX_COLOR_FormatYUV422SemiPlanar";
		break;
	case OMX_COLOR_FormatYCbYCr:
		name = "OMX_COLOR_FormatYCbYCr";
		break;
	case OMX_COLOR_FormatYCrYCb:
		name = "OMX_COLOR_FormatYCrYCb";
		break;
	case OMX_COLOR_FormatCbYCrY:
		name = "OMX_COLOR_FormatCbYCrY";
		break;
	case OMX_COLOR_FormatCrYCbY:
		name = "OMX_COLOR_FormatCrYCbY";
		break;
	case OMX_COLOR_FormatYUV444Interleaved:
		name = "OMX_COLOR_FormatYUV444Interleaved";
		break;
	case OMX_COLOR_FormatRawBayer8bit:
		name = "OMX_COLOR_FormatRawBayer8bit";
		break;
	case OMX_COLOR_FormatRawBayer10bit:
		name = "OMX_COLOR_FormatRawBayer10bit";
		break;
	case OMX_COLOR_FormatRawBayer8bitcompressed:
		name = "OMX_COLOR_FormatRawBayer8bitcompressed";
		break;
	case OMX_COLOR_FormatL2:
		name = "OMX_COLOR_FormatL2";
		break;
	case OMX_COLOR_FormatL4:
		name = "OMX_COLOR_FormatL4";
		break;
	case OMX_COLOR_FormatL8:
		name = "OMX_COLOR_FormatL8";
		break;
	case OMX_COLOR_FormatL16:
		name = "OMX_COLOR_FormatL16";
		break;
	case OMX_COLOR_FormatL24:
		name = "OMX_COLOR_FormatL24";
		break;
	case OMX_COLOR_FormatL32:
		name = "OMX_COLOR_FormatL32";
		break;
	case OMX_COLOR_FormatYUV420PackedSemiPlanar:
		name = "OMX_COLOR_FormatYUV420PackedSemiPlanar";
		break;
	case OMX_COLOR_FormatYUV422PackedSemiPlanar:
		name = "OMX_COLOR_FormatYUV422PackedSemiPlanar";
		break;
	case OMX_COLOR_Format18BitBGR666:
		name = "OMX_COLOR_Format18BitBGR666";
		break;
	case OMX_COLOR_Format24BitARGB6666:
		name = "OMX_COLOR_Format24BitARGB6666";
		break;
	case OMX_COLOR_Format24BitABGR6666:
		name = "OMX_COLOR_Format24BitABGR6666";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_COLOR_FormatKhronosExtensions <= id &&
		id < OMX_COLOR_FormatVendorStartUnused) {
		name = "(OMX_COLOR_FormatKhronosExtensions)";
	}
	if (OMX_COLOR_FormatVendorStartUnused <= id &&
		id < OMX_COLOR_FormatMax) {
		name = "(OMX_COLOR_FormatVendorStartUnused)";
	}

	return name;
}

} //namespace mf
