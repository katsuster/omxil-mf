
#include "util/omx_enum_name.hpp"

namespace mf {

const char *omx_enum_name::get_OMX_IMAGE_CODINGTYPE_name(OMX_IMAGE_CODINGTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_IMAGE_CodingUnused:
		name = "OMX_IMAGE_CodingUnused";
		break;
	case OMX_IMAGE_CodingAutoDetect:
		name = "OMX_IMAGE_CodingAutoDetect";
		break;
	case OMX_IMAGE_CodingJPEG:
		name = "OMX_IMAGE_CodingJPEG";
		break;
	case OMX_IMAGE_CodingJPEG2K:
		name = "OMX_IMAGE_CodingJPEG2K";
		break;
	case OMX_IMAGE_CodingEXIF:
		name = "OMX_IMAGE_CodingEXIF";
		break;
	case OMX_IMAGE_CodingTIFF:
		name = "OMX_IMAGE_CodingTIFF";
		break;
	case OMX_IMAGE_CodingGIF:
		name = "OMX_IMAGE_CodingGIF";
		break;
	case OMX_IMAGE_CodingPNG:
		name = "OMX_IMAGE_CodingPNG";
		break;
	case OMX_IMAGE_CodingLZW:
		name = "OMX_IMAGE_CodingLZW";
		break;
	case OMX_IMAGE_CodingBMP:
		name = "OMX_IMAGE_CodingBMP";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_IMAGE_CodingKhronosExtensions <= id &&
		id < OMX_IMAGE_CodingVendorStartUnused) {
		name = "(OMX_IMAGE_CodingKhronosExtensions)";
	}
	if (OMX_IMAGE_CodingVendorStartUnused <= id &&
		id < OMX_IMAGE_CodingMax) {
		name = "(OMX_IMAGE_CodingVendorStartUnused)";
	}

	return name;
}

} //namespace mf
