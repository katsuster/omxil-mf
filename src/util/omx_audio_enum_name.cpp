
#include "util/omx_enum_name.hpp"

namespace mf {

const char *omx_enum_name::get_OMX_AUDIO_CODINGTYPE_name(OMX_AUDIO_CODINGTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_AUDIO_CodingUnused:
		name = "OMX_AUDIO_CodingUnused";
		break;
	case OMX_AUDIO_CodingAutoDetect:
		name = "OMX_AUDIO_CodingAutoDetect";
		break;
	case OMX_AUDIO_CodingPCM:
		name = "OMX_AUDIO_CodingPCM";
		break;
	case OMX_AUDIO_CodingADPCM:
		name = "OMX_AUDIO_CodingADPCM";
		break;
	case OMX_AUDIO_CodingAMR:
		name = "OMX_AUDIO_CodingAMR";
		break;
	case OMX_AUDIO_CodingGSMFR:
		name = "OMX_AUDIO_CodingGSMFR";
		break;
	case OMX_AUDIO_CodingGSMEFR:
		name = "OMX_AUDIO_CodingGSMEFR";
		break;
	case OMX_AUDIO_CodingGSMHR:
		name = "OMX_AUDIO_CodingGSMHR";
		break;
	case OMX_AUDIO_CodingPDCFR:
		name = "OMX_AUDIO_CodingPDCFR";
		break;
	case OMX_AUDIO_CodingPDCEFR:
		name = "OMX_AUDIO_CodingPDCEFR";
		break;
	case OMX_AUDIO_CodingPDCHR:
		name = "OMX_AUDIO_CodingPDCHR";
		break;
	case OMX_AUDIO_CodingTDMAFR:
		name = "OMX_AUDIO_CodingTDMAFR";
		break;
	case OMX_AUDIO_CodingTDMAEFR:
		name = "OMX_AUDIO_CodingTDMAEFR";
		break;
	case OMX_AUDIO_CodingQCELP8:
		name = "OMX_AUDIO_CodingQCELP8";
		break;
	case OMX_AUDIO_CodingQCELP13:
		name = "OMX_AUDIO_CodingQCELP13";
		break;
	case OMX_AUDIO_CodingEVRC:
		name = "OMX_AUDIO_CodingEVRC";
		break;
	case OMX_AUDIO_CodingSMV:
		name = "OMX_AUDIO_CodingSMV";
		break;
	case OMX_AUDIO_CodingG711:
		name = "OMX_AUDIO_CodingG711";
		break;
	case OMX_AUDIO_CodingG723:
		name = "OMX_AUDIO_CodingG723";
		break;
	case OMX_AUDIO_CodingG726:
		name = "OMX_AUDIO_CodingG726";
		break;
	case OMX_AUDIO_CodingG729:
		name = "OMX_AUDIO_CodingG729";
		break;
	case OMX_AUDIO_CodingAAC:
		name = "OMX_AUDIO_CodingAAC";
		break;
	case OMX_AUDIO_CodingMP3:
		name = "OMX_AUDIO_CodingMP3";
		break;
	case OMX_AUDIO_CodingSBC:
		name = "OMX_AUDIO_CodingSBC";
		break;
	case OMX_AUDIO_CodingVORBIS:
		name = "OMX_AUDIO_CodingVORBIS";
		break;
	case OMX_AUDIO_CodingWMA:
		name = "OMX_AUDIO_CodingWMA";
		break;
	case OMX_AUDIO_CodingRA:
		name = "OMX_AUDIO_CodingRA";
		break;
	case OMX_AUDIO_CodingMIDI:
		name = "OMX_AUDIO_CodingMIDI";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_AUDIO_CodingKhronosExtensions <= id &&
		id < OMX_AUDIO_CodingVendorStartUnused) {
		name = "(OMX_AUDIO_CodingKhronosExtensions)";
	}
	if (OMX_AUDIO_CodingVendorStartUnused <= id &&
		id < OMX_AUDIO_CodingMax) {
		name = "(OMX_AUDIO_CodingVendorStartUnused)";
	}

	return name;
}

} //namespace mf
