
#include "util/omx_enum_name.hpp"

namespace mf {

const char *omx_enum_name::get_OMX_VIDEO_CODINGTYPE_name(OMX_VIDEO_CODINGTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_VIDEO_CodingUnused:
		name = "OMX_VIDEO_CodingUnused";
		break;
	case OMX_VIDEO_CodingAutoDetect:
		name = "OMX_VIDEO_CodingAutoDetect";
		break;
	case OMX_VIDEO_CodingMPEG2:
		name = "OMX_VIDEO_CodingMPEG2";
		break;
	case OMX_VIDEO_CodingH263:
		name = "OMX_VIDEO_CodingH263";
		break;
	case OMX_VIDEO_CodingMPEG4:
		name = "OMX_VIDEO_CodingMPEG4";
		break;
	case OMX_VIDEO_CodingWMV:
		name = "OMX_VIDEO_CodingWMV";
		break;
	case OMX_VIDEO_CodingRV:
		name = "OMX_VIDEO_CodingRV";
		break;
	case OMX_VIDEO_CodingAVC:
		name = "OMX_VIDEO_CodingAVC";
		break;
	case OMX_VIDEO_CodingMJPEG:
		name = "OMX_VIDEO_CodingMJPEG";
		break;
	case OMX_VIDEO_CodingMax:
		name = "OMX_VIDEO_CodingMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_VIDEO_CodingKhronosExtensions <= id && 
		id < OMX_VIDEO_CodingVendorStartUnused) {
		name = "(OMX_VIDEO_CodingKhronosExtensions)";
	}
	if (OMX_VIDEO_CodingVendorStartUnused <= id && 
		id < OMX_VIDEO_CodingMax) {
		name = "(OMX_VIDEO_CodingVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_VIDEO_CONTROLRATETYPE_name(OMX_VIDEO_CONTROLRATETYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_Video_ControlRateDisable:
		name = "OMX_Video_ControlRateDisable";
		break;
	case OMX_Video_ControlRateVariable:
		name = "OMX_Video_ControlRateVariable";
		break;
	case OMX_Video_ControlRateConstant:
		name = "OMX_Video_ControlRateConstant";
		break;
	case OMX_Video_ControlRateVariableSkipFrames:
		name = "OMX_Video_ControlRateVariableSkipFrames";
		break;
	case OMX_Video_ControlRateConstantSkipFrames:
		name = "OMX_Video_ControlRateConstantSkipFrames";
		break;
	case OMX_Video_ControlRateMax:
		name = "OMX_Video_ControlRateMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_Video_ControlRateKhronosExtensions <= id && 
		id < OMX_Video_ControlRateVendorStartUnused) {
		name = "(OMX_Video_ControlRateKhronosExtensions)";
	}
	if (OMX_Video_ControlRateVendorStartUnused <= id && 
		id < OMX_Video_ControlRateMax) {
		name = "(OMX_Video_ControlRateVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_VIDEO_MOTIONVECTORTYPE_name(OMX_VIDEO_MOTIONVECTORTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_Video_MotionVectorPixel:
		name = "OMX_Video_MotionVectorPixel";
		break;
	case OMX_Video_MotionVectorHalfPel:
		name = "OMX_Video_MotionVectorHalfPel";
		break;
	case OMX_Video_MotionVectorQuarterPel:
		name = "OMX_Video_MotionVectorQuarterPel";
		break;
	case OMX_Video_MotionVectorEighthPel:
		name = "OMX_Video_MotionVectorEighthPel";
		break;
	case OMX_Video_MotionVectorMax:
		name = "OMX_Video_MotionVectorMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_Video_MotionVectorKhronosExtensions <= id && 
		id < OMX_Video_MotionVectorVendorStartUnused) {
		name = "(OMX_Video_MotionVectorKhronosExtensions)";
	}
	if (OMX_Video_MotionVectorVendorStartUnused <= id && 
		id < OMX_Video_MotionVectorMax) {
		name = "(OMX_Video_MotionVectorVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_VIDEO_INTRAREFRESHTYPE_name(OMX_VIDEO_INTRAREFRESHTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_VIDEO_IntraRefreshCyclic:
		name = "OMX_VIDEO_IntraRefreshCyclic";
		break;
	case OMX_VIDEO_IntraRefreshAdaptive:
		name = "OMX_VIDEO_IntraRefreshAdaptive";
		break;
	case OMX_VIDEO_IntraRefreshBoth:
		name = "OMX_VIDEO_IntraRefreshBoth";
		break;
	case OMX_VIDEO_IntraRefreshMax:
		name = "OMX_VIDEO_IntraRefreshMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_VIDEO_IntraRefreshKhronosExtensions <= id && 
		id < OMX_VIDEO_IntraRefreshVendorStartUnused) {
		name = "(OMX_VIDEO_IntraRefreshKhronosExtensions)";
	}
	if (OMX_VIDEO_IntraRefreshVendorStartUnused <= id && 
		id < OMX_VIDEO_IntraRefreshMax) {
		name = "(OMX_VIDEO_IntraRefreshVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_VIDEO_H263PROFILETYPE_name(OMX_VIDEO_H263PROFILETYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_VIDEO_H263ProfileBaseline:
		name = "OMX_VIDEO_H263ProfileBaseline";
		break;
	case OMX_VIDEO_H263ProfileH320Coding:
		name = "OMX_VIDEO_H263ProfileH320Coding";
		break;
	case OMX_VIDEO_H263ProfileBackwardCompatible:
		name = "OMX_VIDEO_H263ProfileBackwardCompatible";
		break;
	case OMX_VIDEO_H263ProfileISWV2:
		name = "OMX_VIDEO_H263ProfileISWV2";
		break;
	case OMX_VIDEO_H263ProfileISWV3:
		name = "OMX_VIDEO_H263ProfileISWV3";
		break;
	case OMX_VIDEO_H263ProfileHighCompression:
		name = "OMX_VIDEO_H263ProfileHighCompression";
		break;
	case OMX_VIDEO_H263ProfileInternet:
		name = "OMX_VIDEO_H263ProfileInternet";
		break;
	case OMX_VIDEO_H263ProfileInterlace:
		name = "OMX_VIDEO_H263ProfileInterlace";
		break;
	case OMX_VIDEO_H263ProfileHighLatency:
		name = "OMX_VIDEO_H263ProfileHighLatency";
		break;
	case OMX_VIDEO_H263ProfileMax:
		name = "OMX_VIDEO_H263ProfileMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_VIDEO_H263ProfileKhronosExtensions <= id && 
		id < OMX_VIDEO_H263ProfileVendorStartUnused) {
		name = "(OMX_VIDEO_H263ProfileKhronosExtensions)";
	}
	if (OMX_VIDEO_H263ProfileVendorStartUnused <= id && 
		id < OMX_VIDEO_H263ProfileMax) {
		name = "(OMX_VIDEO_H263ProfileVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_VIDEO_H263LEVELTYPE_name(OMX_VIDEO_H263LEVELTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_VIDEO_H263Level10:
		name = "OMX_VIDEO_H263Level10";
		break;
	case OMX_VIDEO_H263Level20:
		name = "OMX_VIDEO_H263Level20";
		break;
	case OMX_VIDEO_H263Level30:
		name = "OMX_VIDEO_H263Level30";
		break;
	case OMX_VIDEO_H263Level40:
		name = "OMX_VIDEO_H263Level40";
		break;
	case OMX_VIDEO_H263Level45:
		name = "OMX_VIDEO_H263Level45";
		break;
	case OMX_VIDEO_H263Level50:
		name = "OMX_VIDEO_H263Level50";
		break;
	case OMX_VIDEO_H263Level60:
		name = "OMX_VIDEO_H263Level60";
		break;
	case OMX_VIDEO_H263Level70:
		name = "OMX_VIDEO_H263Level70";
		break;
	case OMX_VIDEO_H263LevelMax:
		name = "OMX_VIDEO_H263LevelMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_VIDEO_H263LevelKhronosExtensions <= id && 
		id < OMX_VIDEO_H263LevelVendorStartUnused) {
		name = "(OMX_VIDEO_H263LevelKhronosExtensions)";
	}
	if (OMX_VIDEO_H263LevelVendorStartUnused <= id && 
		id < OMX_VIDEO_H263LevelMax) {
		name = "(OMX_VIDEO_H263LevelVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_VIDEO_PICTURETYPE_name(OMX_VIDEO_PICTURETYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_VIDEO_PictureTypeI:
		name = "OMX_VIDEO_PictureTypeI";
		break;
	case OMX_VIDEO_PictureTypeP:
		name = "OMX_VIDEO_PictureTypeP";
		break;
	case OMX_VIDEO_PictureTypeB:
		name = "OMX_VIDEO_PictureTypeB";
		break;
	case OMX_VIDEO_PictureTypeSI:
		name = "OMX_VIDEO_PictureTypeSI";
		break;
	case OMX_VIDEO_PictureTypeSP:
		name = "OMX_VIDEO_PictureTypeSP";
		break;
	case OMX_VIDEO_PictureTypeEI:
		name = "OMX_VIDEO_PictureTypeEI";
		break;
	case OMX_VIDEO_PictureTypeEP:
		name = "OMX_VIDEO_PictureTypeEP";
		break;
	case OMX_VIDEO_PictureTypeS:
		name = "OMX_VIDEO_PictureTypeS";
		break;
	case OMX_VIDEO_PictureTypeMax:
		name = "OMX_VIDEO_PictureTypeMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	return name;
}

const char *omx_enum_name::get_OMX_VIDEO_MPEG2PROFILETYPE_name(OMX_VIDEO_MPEG2PROFILETYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_VIDEO_MPEG2ProfileSimple:
		name = "OMX_VIDEO_MPEG2ProfileSimple";
		break;
	case OMX_VIDEO_MPEG2ProfileMain:
		name = "OMX_VIDEO_MPEG2ProfileMain";
		break;
	case OMX_VIDEO_MPEG2Profile422:
		name = "OMX_VIDEO_MPEG2Profile422";
		break;
	case OMX_VIDEO_MPEG2ProfileSNR:
		name = "OMX_VIDEO_MPEG2ProfileSNR";
		break;
	case OMX_VIDEO_MPEG2ProfileSpatial:
		name = "OMX_VIDEO_MPEG2ProfileSpatial";
		break;
	case OMX_VIDEO_MPEG2ProfileHigh:
		name = "OMX_VIDEO_MPEG2ProfileHigh";
		break;
	case OMX_VIDEO_MPEG2ProfileMax:
		name = "OMX_VIDEO_MPEG2ProfileMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_VIDEO_MPEG2ProfileKhronosExtensions <= id && 
		id < OMX_VIDEO_MPEG2ProfileVendorStartUnused) {
		name = "(OMX_VIDEO_MPEG2ProfileKhronosExtensions)";
	}
	if (OMX_VIDEO_MPEG2ProfileVendorStartUnused <= id && 
		id < OMX_VIDEO_MPEG2ProfileMax) {
		name = "(OMX_VIDEO_MPEG2ProfileVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_VIDEO_MPEG2LEVELTYPE_name(OMX_VIDEO_MPEG2LEVELTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_VIDEO_MPEG2LevelLL:
		name = "OMX_VIDEO_MPEG2LevelLL";
		break;
	case OMX_VIDEO_MPEG2LevelML:
		name = "OMX_VIDEO_MPEG2LevelML";
		break;
	case OMX_VIDEO_MPEG2LevelH14:
		name = "OMX_VIDEO_MPEG2LevelH14";
		break;
	case OMX_VIDEO_MPEG2LevelHL:
		name = "OMX_VIDEO_MPEG2LevelHL";
		break;
	case OMX_VIDEO_MPEG2LevelMax:
		name = "OMX_VIDEO_MPEG2LevelMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_VIDEO_MPEG2LevelKhronosExtensions <= id && 
		id < OMX_VIDEO_MPEG2LevelVendorStartUnused) {
		name = "(OMX_VIDEO_MPEG2LevelKhronosExtensions)";
	}
	if (OMX_VIDEO_MPEG2LevelVendorStartUnused <= id && 
		id < OMX_VIDEO_MPEG2LevelMax) {
		name = "(OMX_VIDEO_MPEG2LevelVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_VIDEO_MPEG4PROFILETYPE_name(OMX_VIDEO_MPEG4PROFILETYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_VIDEO_MPEG4ProfileSimple:
		name = "OMX_VIDEO_MPEG4ProfileSimple";
		break;
	case OMX_VIDEO_MPEG4ProfileSimpleScalable:
		name = "OMX_VIDEO_MPEG4ProfileSimpleScalable";
		break;
	case OMX_VIDEO_MPEG4ProfileCore:
		name = "OMX_VIDEO_MPEG4ProfileCore";
		break;
	case OMX_VIDEO_MPEG4ProfileMain:
		name = "OMX_VIDEO_MPEG4ProfileMain";
		break;
	case OMX_VIDEO_MPEG4ProfileNbit:
		name = "OMX_VIDEO_MPEG4ProfileNbit";
		break;
	case OMX_VIDEO_MPEG4ProfileScalableTexture:
		name = "OMX_VIDEO_MPEG4ProfileScalableTexture";
		break;
	case OMX_VIDEO_MPEG4ProfileSimpleFace:
		name = "OMX_VIDEO_MPEG4ProfileSimpleFace";
		break;
	case OMX_VIDEO_MPEG4ProfileSimpleFBA:
		name = "OMX_VIDEO_MPEG4ProfileSimpleFBA";
		break;
	case OMX_VIDEO_MPEG4ProfileBasicAnimated:
		name = "OMX_VIDEO_MPEG4ProfileBasicAnimated";
		break;
	case OMX_VIDEO_MPEG4ProfileHybrid:
		name = "OMX_VIDEO_MPEG4ProfileHybrid";
		break;
	case OMX_VIDEO_MPEG4ProfileAdvancedRealTime:
		name = "OMX_VIDEO_MPEG4ProfileAdvancedRealTime";
		break;
	case OMX_VIDEO_MPEG4ProfileCoreScalable:
		name = "OMX_VIDEO_MPEG4ProfileCoreScalable";
		break;
	case OMX_VIDEO_MPEG4ProfileAdvancedCoding:
		name = "OMX_VIDEO_MPEG4ProfileAdvancedCoding";
		break;
	case OMX_VIDEO_MPEG4ProfileAdvancedCore:
		name = "OMX_VIDEO_MPEG4ProfileAdvancedCore";
		break;
	case OMX_VIDEO_MPEG4ProfileAdvancedScalable:
		name = "OMX_VIDEO_MPEG4ProfileAdvancedScalable";
		break;
	case OMX_VIDEO_MPEG4ProfileAdvancedSimple:
		name = "OMX_VIDEO_MPEG4ProfileAdvancedSimple";
		break;
	case OMX_VIDEO_MPEG4ProfileMax:
		name = "OMX_VIDEO_MPEG4ProfileMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_VIDEO_MPEG4ProfileKhronosExtensions <= id && 
		id < OMX_VIDEO_MPEG4ProfileVendorStartUnused) {
		name = "(OMX_VIDEO_MPEG4ProfileKhronosExtensions)";
	}
	if (OMX_VIDEO_MPEG4ProfileVendorStartUnused <= id && 
		id < OMX_VIDEO_MPEG4ProfileMax) {
		name = "(OMX_VIDEO_MPEG4ProfileVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_VIDEO_MPEG4LEVELTYPE_name(OMX_VIDEO_MPEG4LEVELTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_VIDEO_MPEG4Level0:
		name = "OMX_VIDEO_MPEG4Level0";
		break;
	case OMX_VIDEO_MPEG4Level0b:
		name = "OMX_VIDEO_MPEG4Level0b";
		break;
	case OMX_VIDEO_MPEG4Level1:
		name = "OMX_VIDEO_MPEG4Level1";
		break;
	case OMX_VIDEO_MPEG4Level2:
		name = "OMX_VIDEO_MPEG4Level2";
		break;
	case OMX_VIDEO_MPEG4Level3:
		name = "OMX_VIDEO_MPEG4Level3";
		break;
	case OMX_VIDEO_MPEG4Level4:
		name = "OMX_VIDEO_MPEG4Level4";
		break;
	case OMX_VIDEO_MPEG4Level4a:
		name = "OMX_VIDEO_MPEG4Level4a";
		break;
	case OMX_VIDEO_MPEG4Level5:
		name = "OMX_VIDEO_MPEG4Level5";
		break;
	case OMX_VIDEO_MPEG4LevelMax:
		name = "OMX_VIDEO_MPEG4LevelMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_VIDEO_MPEG4LevelKhronosExtensions <= id && 
		id < OMX_VIDEO_MPEG4LevelVendorStartUnused) {
		name = "(OMX_VIDEO_MPEG4LevelKhronosExtensions)";
	}
	if (OMX_VIDEO_MPEG4LevelVendorStartUnused <= id && 
		id < OMX_VIDEO_MPEG4LevelMax) {
		name = "(OMX_VIDEO_MPEG4LevelVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_VIDEO_WMVFORMATTYPE_name(OMX_VIDEO_WMVFORMATTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_VIDEO_WMVFormatUnused:
		name = "OMX_VIDEO_WMVFormatUnused";
		break;
	case OMX_VIDEO_WMVFormat7:
		name = "OMX_VIDEO_WMVFormat7";
		break;
	case OMX_VIDEO_WMVFormat8:
		name = "OMX_VIDEO_WMVFormat8";
		break;
	case OMX_VIDEO_WMVFormat9:
		name = "OMX_VIDEO_WMVFormat9";
		break;
	case OMX_VIDEO_WMVFormatMax:
		name = "OMX_VIDEO_WMVFormatMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_VIDEO_WMFFormatKhronosExtensions <= id && 
		id < OMX_VIDEO_WMFFormatVendorStartUnused) {
		name = "(OMX_VIDEO_WMFFormatKhronosExtensions)";
	}
	if (OMX_VIDEO_WMFFormatVendorStartUnused <= id && 
		id < OMX_VIDEO_WMVFormatMax) {
		name = "(OMX_VIDEO_WMFFormatVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_VIDEO_RVFORMATTYPE_name(OMX_VIDEO_RVFORMATTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_VIDEO_RVFormatUnused:
		name = "OMX_VIDEO_RVFormatUnused";
		break;
	case OMX_VIDEO_RVFormat8:
		name = "OMX_VIDEO_RVFormat8";
		break;
	case OMX_VIDEO_RVFormat9:
		name = "OMX_VIDEO_RVFormat9";
		break;
	case OMX_VIDEO_RVFormatG2:
		name = "OMX_VIDEO_RVFormatG2";
		break;
	case OMX_VIDEO_RVFormatMax:
		name = "OMX_VIDEO_RVFormatMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_VIDEO_RVFormatKhronosExtensions <= id && 
		id < OMX_VIDEO_RVFormatVendorStartUnused) {
		name = "(OMX_VIDEO_RVFormatKhronosExtensions)";
	}
	if (OMX_VIDEO_RVFormatVendorStartUnused <= id && 
		id < OMX_VIDEO_RVFormatMax) {
		name = "(OMX_VIDEO_RVFormatVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_VIDEO_AVCPROFILETYPE_name(OMX_VIDEO_AVCPROFILETYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_VIDEO_AVCProfileBaseline:
		name = "OMX_VIDEO_AVCProfileBaseline";
		break;
	case OMX_VIDEO_AVCProfileMain:
		name = "OMX_VIDEO_AVCProfileMain";
		break;
	case OMX_VIDEO_AVCProfileExtended:
		name = "OMX_VIDEO_AVCProfileExtended";
		break;
	case OMX_VIDEO_AVCProfileHigh:
		name = "OMX_VIDEO_AVCProfileHigh";
		break;
	case OMX_VIDEO_AVCProfileHigh10:
		name = "OMX_VIDEO_AVCProfileHigh10";
		break;
	case OMX_VIDEO_AVCProfileHigh422:
		name = "OMX_VIDEO_AVCProfileHigh422";
		break;
	case OMX_VIDEO_AVCProfileHigh444:
		name = "OMX_VIDEO_AVCProfileHigh444";
		break;
	case OMX_VIDEO_AVCProfileMax:
		name = "OMX_VIDEO_AVCProfileMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_VIDEO_AVCProfileKhronosExtensions <= id && 
		id < OMX_VIDEO_AVCProfileVendorStartUnused) {
		name = "(OMX_VIDEO_AVCProfileKhronosExtensions)";
	}
	if (OMX_VIDEO_AVCProfileVendorStartUnused <= id && 
		id < OMX_VIDEO_AVCProfileMax) {
		name = "(OMX_VIDEO_AVCProfileVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_VIDEO_AVCLEVELTYPE_name(OMX_VIDEO_AVCLEVELTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_VIDEO_AVCLevel1:
		name = "OMX_VIDEO_AVCLevel1";
		break;
	case OMX_VIDEO_AVCLevel1b:
		name = "OMX_VIDEO_AVCLevel1b";
		break;
	case OMX_VIDEO_AVCLevel11:
		name = "OMX_VIDEO_AVCLevel11";
		break;
	case OMX_VIDEO_AVCLevel12:
		name = "OMX_VIDEO_AVCLevel12";
		break;
	case OMX_VIDEO_AVCLevel13:
		name = "OMX_VIDEO_AVCLevel13";
		break;
	case OMX_VIDEO_AVCLevel2:
		name = "OMX_VIDEO_AVCLevel2";
		break;
	case OMX_VIDEO_AVCLevel21:
		name = "OMX_VIDEO_AVCLevel21";
		break;
	case OMX_VIDEO_AVCLevel22:
		name = "OMX_VIDEO_AVCLevel22";
		break;
	case OMX_VIDEO_AVCLevel3:
		name = "OMX_VIDEO_AVCLevel3";
		break;
	case OMX_VIDEO_AVCLevel31:
		name = "OMX_VIDEO_AVCLevel31";
		break;
	case OMX_VIDEO_AVCLevel32:
		name = "OMX_VIDEO_AVCLevel32";
		break;
	case OMX_VIDEO_AVCLevel4:
		name = "OMX_VIDEO_AVCLevel4";
		break;
	case OMX_VIDEO_AVCLevel41:
		name = "OMX_VIDEO_AVCLevel41";
		break;
	case OMX_VIDEO_AVCLevel42:
		name = "OMX_VIDEO_AVCLevel42";
		break;
	case OMX_VIDEO_AVCLevel5:
		name = "OMX_VIDEO_AVCLevel5";
		break;
	case OMX_VIDEO_AVCLevel51:
		name = "OMX_VIDEO_AVCLevel51";
		break;
	case OMX_VIDEO_AVCLevelMax:
		name = "OMX_VIDEO_AVCLevelMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_VIDEO_AVCLevelKhronosExtensions <= id && 
		id < OMX_VIDEO_AVCLevelVendorStartUnused) {
		name = "(OMX_VIDEO_AVCLevelKhronosExtensions)";
	}
	if (OMX_VIDEO_AVCLevelVendorStartUnused <= id && 
		id < OMX_VIDEO_AVCLevelMax) {
		name = "(OMX_VIDEO_AVCLevelVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_VIDEO_AVCLOOPFILTERTYPE_name(OMX_VIDEO_AVCLOOPFILTERTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_VIDEO_AVCLoopFilterEnable:
		name = "OMX_VIDEO_AVCLoopFilterEnable";
		break;
	case OMX_VIDEO_AVCLoopFilterDisable:
		name = "OMX_VIDEO_AVCLoopFilterDisable";
		break;
	case OMX_VIDEO_AVCLoopFilterDisableSliceBoundary:
		name = "OMX_VIDEO_AVCLoopFilterDisableSliceBoundary";
		break;
	case OMX_VIDEO_AVCLoopFilterMax:
		name = "OMX_VIDEO_AVCLoopFilterMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_VIDEO_AVCLoopFilterKhronosExtensions <= id && 
		id < OMX_VIDEO_AVCLoopFilterVendorStartUnused) {
		name = "(OMX_VIDEO_AVCLoopFilterKhronosExtensions)";
	}
	if (OMX_VIDEO_AVCLoopFilterVendorStartUnused <= id && 
		id < OMX_VIDEO_AVCLoopFilterMax) {
		name = "(OMX_VIDEO_AVCLoopFilterVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_VIDEO_AVCSLICEMODETYPE_name(OMX_VIDEO_AVCSLICEMODETYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_VIDEO_SLICEMODE_AVCDefault:
		name = "OMX_VIDEO_SLICEMODE_AVCDefault";
		break;
	case OMX_VIDEO_SLICEMODE_AVCMBSlice:
		name = "OMX_VIDEO_SLICEMODE_AVCMBSlice";
		break;
	case OMX_VIDEO_SLICEMODE_AVCByteSlice:
		name = "OMX_VIDEO_SLICEMODE_AVCByteSlice";
		break;
	case OMX_VIDEO_SLICEMODE_AVCLevelMax:
		name = "OMX_VIDEO_SLICEMODE_AVCLevelMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_VIDEO_SLICEMODE_AVCKhronosExtensions <= id && 
		id < OMX_VIDEO_SLICEMODE_AVCVendorStartUnused) {
		name = "(OMX_VIDEO_SLICEMODE_AVCKhronosExtensions)";
	}
	if (OMX_VIDEO_SLICEMODE_AVCVendorStartUnused <= id && 
		id < OMX_VIDEO_SLICEMODE_AVCLevelMax) {
		name = "(OMX_VIDEO_SLICEMODE_AVCVendorStartUnused)";
	}

	return name;
}

} //namespace mf
