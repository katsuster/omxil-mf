
#include "util/omx_enum_name.hpp"

namespace mf {

const char *omx_enum_name::get_OMX_INDEXTYPE_name(OMX_INDEXTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_IndexComponentStartUnused:
		name = "OMX_IndexComponentStartUnused";
		break;
	case OMX_IndexParamPriorityMgmt:
		name = "OMX_IndexParamPriorityMgmt";
		break;
	case OMX_IndexParamAudioInit:
		name = "OMX_IndexParamAudioInit";
		break;
	case OMX_IndexParamImageInit:
		name = "OMX_IndexParamImageInit";
		break;
	case OMX_IndexParamVideoInit:
		name = "OMX_IndexParamVideoInit";
		break;
	case OMX_IndexParamOtherInit:
		name = "OMX_IndexParamOtherInit";
		break;
	case OMX_IndexParamNumAvailableStreams:
		name = "OMX_IndexParamNumAvailableStreams";
		break;
	case OMX_IndexParamActiveStream:
		name = "OMX_IndexParamActiveStream";
		break;
	case OMX_IndexParamSuspensionPolicy:
		name = "OMX_IndexParamSuspensionPolicy";
		break;
	case OMX_IndexParamComponentSuspended:
		name = "OMX_IndexParamComponentSuspended";
		break;
	case OMX_IndexConfigCapturing:
		name = "OMX_IndexConfigCapturing";
		break;
	case OMX_IndexConfigCaptureMode:
		name = "OMX_IndexConfigCaptureMode";
		break;
	case OMX_IndexAutoPauseAfterCapture:
		name = "OMX_IndexAutoPauseAfterCapture";
		break;
	case OMX_IndexParamContentURI:
		name = "OMX_IndexParamContentURI";
		break;
	case OMX_IndexParamCustomContentPipe:
		name = "OMX_IndexParamCustomContentPipe";
		break;
	case OMX_IndexParamDisableResourceConcealment:
		name = "OMX_IndexParamDisableResourceConcealment";
		break;
	case OMX_IndexConfigMetadataItemCount:
		name = "OMX_IndexConfigMetadataItemCount";
		break;
	case OMX_IndexConfigContainerNodeCount:
		name = "OMX_IndexConfigContainerNodeCount";
		break;
	case OMX_IndexConfigMetadataItem:
		name = "OMX_IndexConfigMetadataItem";
		break;
	case OMX_IndexConfigCounterNodeID:
		name = "OMX_IndexConfigCounterNodeID";
		break;
	case OMX_IndexParamMetadataFilterType:
		name = "OMX_IndexParamMetadataFilterType";
		break;
	case OMX_IndexParamMetadataKeyFilter:
		name = "OMX_IndexParamMetadataKeyFilter";
		break;
	case OMX_IndexConfigPriorityMgmt:
		name = "OMX_IndexConfigPriorityMgmt";
		break;
	case OMX_IndexParamStandardComponentRole:
		name = "OMX_IndexParamStandardComponentRole";
		break;

	case OMX_IndexPortStartUnused:
		name = "OMX_IndexPortStartUnused";
		break;
	case OMX_IndexParamPortDefinition:
		name = "OMX_IndexParamPortDefinition";
		break;
	case OMX_IndexParamCompBufferSupplier:
		name = "OMX_IndexParamCompBufferSupplier";
		break;
	case OMX_IndexReservedStartUnused:
		name = "OMX_IndexReservedStartUnused";
		break;

	//Audio parameters and configurations
	case OMX_IndexAudioStartUnused:
		name = "OMX_IndexAudioStartUnused";
		break;
	case OMX_IndexParamAudioPortFormat:
		name = "OMX_IndexParamAudioPortFormat";
		break;
	case OMX_IndexParamAudioPcm:
		name = "OMX_IndexParamAudioPcm";
		break;
	case OMX_IndexParamAudioAac:
		name = "OMX_IndexParamAudioAac";
		break;
	case OMX_IndexParamAudioRa:
		name = "OMX_IndexParamAudioRa";
		break;
	case OMX_IndexParamAudioMp3:
		name = "OMX_IndexParamAudioMp3";
		break;
	case OMX_IndexParamAudioAdpcm:
		name = "OMX_IndexParamAudioAdpcm";
		break;
	case OMX_IndexParamAudioG723:
		name = "OMX_IndexParamAudioG723";
		break;
	case OMX_IndexParamAudioG729:
		name = "OMX_IndexParamAudioG729";
		break;
	case OMX_IndexParamAudioAmr:
		name = "OMX_IndexParamAudioAmr";
		break;
	case OMX_IndexParamAudioWma:
		name = "OMX_IndexParamAudioWma";
		break;
	case OMX_IndexParamAudioSbc:
		name = "OMX_IndexParamAudioSbc";
		break;
	case OMX_IndexParamAudioMidi:
		name = "OMX_IndexParamAudioMidi";
		break;
	case OMX_IndexParamAudioGsm_FR:
		name = "OMX_IndexParamAudioGsm_FR";
		break;
	case OMX_IndexParamAudioMidiLoadUserSound:
		name = "OMX_IndexParamAudioMidiLoadUserSound";
		break;
	case OMX_IndexParamAudioG726:
		name = "OMX_IndexParamAudioG726";
		break;
	case OMX_IndexParamAudioGsm_EFR:
		name = "OMX_IndexParamAudioGsm_EFR";
		break;
	case OMX_IndexParamAudioGsm_HR:
		name = "OMX_IndexParamAudioGsm_HR";
		break;
	case OMX_IndexParamAudioPdc_FR:
		name = "OMX_IndexParamAudioPdc_FR";
		break;
	case OMX_IndexParamAudioPdc_EFR:
		name = "OMX_IndexParamAudioPdc_EFR";
		break;
	case OMX_IndexParamAudioPdc_HR:
		name = "OMX_IndexParamAudioPdc_HR";
		break;
	case OMX_IndexParamAudioTdma_FR:
		name = "OMX_IndexParamAudioTdma_FR";
		break;
	case OMX_IndexParamAudioTdma_EFR:
		name = "OMX_IndexParamAudioTdma_EFR";
		break;
	case OMX_IndexParamAudioQcelp8:
		name = "OMX_IndexParamAudioQcelp8";
		break;
	case OMX_IndexParamAudioQcelp13:
		name = "OMX_IndexParamAudioQcelp13";
		break;
	case OMX_IndexParamAudioEvrc:
		name = "OMX_IndexParamAudioEvrc";
		break;
	case OMX_IndexParamAudioSmv:
		name = "OMX_IndexParamAudioSmv";
		break;
	case OMX_IndexParamAudioVorbis:
		name = "OMX_IndexParamAudioVorbis";
		break;

	case OMX_IndexConfigAudioMidiImmediateEvent:
		name = "OMX_IndexConfigAudioMidiImmediateEvent";
		break;
	case OMX_IndexConfigAudioMidiControl:
		name = "OMX_IndexConfigAudioMidiControl";
		break;
	case OMX_IndexConfigAudioMidiSoundBankProgram:
		name = "OMX_IndexConfigAudioMidiSoundBankProgram";
		break;
	case OMX_IndexConfigAudioMidiStatus:
		name = "OMX_IndexConfigAudioMidiStatus";
		break;
	case OMX_IndexConfigAudioMidiMetaEvent:
		name = "OMX_IndexConfigAudioMidiMetaEvent";
		break;
	case OMX_IndexConfigAudioMidiMetaEventData:
		name = "OMX_IndexConfigAudioMidiMetaEventData";
		break;
	case OMX_IndexConfigAudioVolume:
		name = "OMX_IndexConfigAudioVolume";
		break;
	case OMX_IndexConfigAudioBalance:
		name = "OMX_IndexConfigAudioBalance";
		break;
	case OMX_IndexConfigAudioChannelMute:
		name = "OMX_IndexConfigAudioChannelMute";
		break;
	case OMX_IndexConfigAudioMute:
		name = "OMX_IndexConfigAudioMute";
		break;
	case OMX_IndexConfigAudioLoudness:
		name = "OMX_IndexConfigAudioLoudness";
		break;
	case OMX_IndexConfigAudioEchoCancelation:
		name = "OMX_IndexConfigAudioEchoCancelation";
		break;
	case OMX_IndexConfigAudioNoiseReduction:
		name = "OMX_IndexConfigAudioNoiseReduction";
		break;
	case OMX_IndexConfigAudioBass:
		name = "OMX_IndexConfigAudioBass";
		break;
	case OMX_IndexConfigAudioTreble:
		name = "OMX_IndexConfigAudioTreble";
		break;
	case OMX_IndexConfigAudioStereoWidening:
		name = "OMX_IndexConfigAudioStereoWidening";
		break;
	case OMX_IndexConfigAudioChorus:
		name = "OMX_IndexConfigAudioChorus";
		break;
	case OMX_IndexConfigAudioEqualizer:
		name = "OMX_IndexConfigAudioEqualizer";
		break;
	case OMX_IndexConfigAudioReverberation:
		name = "OMX_IndexConfigAudioReverberation";
		break;
	case OMX_IndexConfigAudioChannelVolume:
		name = "OMX_IndexConfigAudioChannelVolume";
		break;

	//Image specific parameters and configurations
	case OMX_IndexImageStartUnused:
		name = "OMX_IndexImageStartUnused";
		break;
	case OMX_IndexParamImagePortFormat:
		name = "OMX_IndexParamImagePortFormat";
		break;
	case OMX_IndexParamFlashControl:
		name = "OMX_IndexParamFlashControl";
		break;
	case OMX_IndexConfigFocusControl:
		name = "OMX_IndexConfigFocusControl";
		break;
	case OMX_IndexParamQFactor:
		name = "OMX_IndexParamQFactor";
		break;
	case OMX_IndexParamQuantizationTable:
		name = "OMX_IndexParamQuantizationTable";
		break;
	case OMX_IndexParamHuffmanTable:
		name = "OMX_IndexParamHuffmanTable";
		break;
	case OMX_IndexConfigFlashControl:
		name = "OMX_IndexConfigFlashControl";
		break;

	//Video specific parameters and configurations
	case OMX_IndexVideoStartUnused:
		name = "OMX_IndexVideoStartUnused";
		break;
	case OMX_IndexParamVideoPortFormat:
		name = "OMX_IndexParamVideoPortFormat";
		break;
	case OMX_IndexParamVideoQuantization:
		name = "OMX_IndexParamVideoQuantization";
		break;
	case OMX_IndexParamVideoFastUpdate:
		name = "OMX_IndexParamVideoFastUpdate";
		break;
	case OMX_IndexParamVideoBitrate:
		name = "OMX_IndexParamVideoBitrate";
		break;
	case OMX_IndexParamVideoMotionVector:
		name = "OMX_IndexParamVideoMotionVector";
		break;
	case OMX_IndexParamVideoIntraRefresh:
		name = "OMX_IndexParamVideoIntraRefresh";
		break;
	case OMX_IndexParamVideoErrorCorrection:
		name = "OMX_IndexParamVideoErrorCorrection";
		break;
	case OMX_IndexParamVideoVBSMC:
		name = "OMX_IndexParamVideoVBSMC";
		break;
	case OMX_IndexParamVideoMpeg2:
		name = "OMX_IndexParamVideoMpeg2";
		break;
	case OMX_IndexParamVideoMpeg4:
		name = "OMX_IndexParamVideoMpeg4";
		break;
	case OMX_IndexParamVideoWmv:
		name = "OMX_IndexParamVideoWmv";
		break;
	case OMX_IndexParamVideoRv:
		name = "OMX_IndexParamVideoRv";
		break;
	case OMX_IndexParamVideoAvc:
		name = "OMX_IndexParamVideoAvc";
		break;
	case OMX_IndexParamVideoH263:
		name = "OMX_IndexParamVideoH263";
		break;
	case OMX_IndexParamVideoProfileLevelQuerySupported:
		name = "OMX_IndexParamVideoProfileLevelQuerySupported";
		break;
	case OMX_IndexParamVideoProfileLevelCurrent:
		name = "OMX_IndexParamVideoProfileLevelCurrent";
		break;
	case OMX_IndexConfigVideoBitrate:
		name = "OMX_IndexConfigVideoBitrate";
		break;
	case OMX_IndexConfigVideoFramerate:
		name = "OMX_IndexConfigVideoFramerate";
		break;
	case OMX_IndexConfigVideoIntraVOPRefresh:
		name = "OMX_IndexConfigVideoIntraVOPRefresh";
		break;
	case OMX_IndexConfigVideoIntraMBRefresh:
		name = "OMX_IndexConfigVideoIntraMBRefresh";
		break;
	case OMX_IndexConfigVideoMBErrorReporting:
		name = "OMX_IndexConfigVideoMBErrorReporting";
		break;
	case OMX_IndexParamVideoMacroblocksPerFrame:
		name = "OMX_IndexParamVideoMacroblocksPerFrame";
		break;
	case OMX_IndexConfigVideoMacroBlockErrorMap:
		name = "OMX_IndexConfigVideoMacroBlockErrorMap";
		break;
	case OMX_IndexParamVideoSliceFMO:
		name = "OMX_IndexParamVideoSliceFMO";
		break;
	case OMX_IndexConfigVideoAVCIntraPeriod:
		name = "OMX_IndexConfigVideoAVCIntraPeriod";
		break;
	case OMX_IndexConfigVideoNalSize:
		name = "OMX_IndexConfigVideoNalSize";
		break;

	//Image & Video common Configurations
	case OMX_IndexCommonStartUnused:
		name = "OMX_IndexCommonStartUnused";
		break;
	case OMX_IndexParamCommonDeblocking:
		name = "OMX_IndexParamCommonDeblocking";
		break;
	case OMX_IndexParamCommonSensorMode:
		name = "OMX_IndexParamCommonSensorMode";
		break;
	case OMX_IndexParamCommonInterleave:
		name = "OMX_IndexParamCommonInterleave";
		break;
	case OMX_IndexConfigCommonColorFormatConversion:
		name = "OMX_IndexConfigCommonColorFormatConversion";
		break;
	case OMX_IndexConfigCommonScale:
		name = "OMX_IndexConfigCommonScale";
		break;
	case OMX_IndexConfigCommonImageFilter:
		name = "OMX_IndexConfigCommonImageFilter";
		break;
	case OMX_IndexConfigCommonColorEnhancement:
		name = "OMX_IndexConfigCommonColorEnhancement";
		break;
	case OMX_IndexConfigCommonColorKey:
		name = "OMX_IndexConfigCommonColorKey";
		break;
	case OMX_IndexConfigCommonColorBlend:
		name = "OMX_IndexConfigCommonColorBlend";
		break;
	case OMX_IndexConfigCommonFrameStabilisation:
		name = "OMX_IndexConfigCommonFrameStabilisation";
		break;
	case OMX_IndexConfigCommonRotate:
		name = "OMX_IndexConfigCommonRotate";
		break;
	case OMX_IndexConfigCommonMirror:
		name = "OMX_IndexConfigCommonMirror";
		break;
	case OMX_IndexConfigCommonOutputPosition:
		name = "OMX_IndexConfigCommonOutputPosition";
		break;
	case OMX_IndexConfigCommonInputCrop:
		name = "OMX_IndexConfigCommonInputCrop";
		break;
	case OMX_IndexConfigCommonOutputCrop:
		name = "OMX_IndexConfigCommonOutputCrop";
		break;
	case OMX_IndexConfigCommonDigitalZoom:
		name = "OMX_IndexConfigCommonDigitalZoom";
		break;
	case OMX_IndexConfigCommonOpticalZoom:
		name = "OMX_IndexConfigCommonOpticalZoom";
		break;
	case OMX_IndexConfigCommonWhiteBalance:
		name = "OMX_IndexConfigCommonWhiteBalance";
		break;
	case OMX_IndexConfigCommonExposure:
		name = "OMX_IndexConfigCommonExposure";
		break;
	case OMX_IndexConfigCommonContrast:
		name = "OMX_IndexConfigCommonContrast";
		break;
	case OMX_IndexConfigCommonBrightness:
		name = "OMX_IndexConfigCommonBrightness";
		break;
	case OMX_IndexConfigCommonBacklight:
		name = "OMX_IndexConfigCommonBacklight";
		break;
	case OMX_IndexConfigCommonGamma:
		name = "OMX_IndexConfigCommonGamma";
		break;
	case OMX_IndexConfigCommonSaturation:
		name = "OMX_IndexConfigCommonSaturation";
		break;
	case OMX_IndexConfigCommonLightness:
		name = "OMX_IndexConfigCommonLightness";
		break;
	case OMX_IndexConfigCommonExclusionRect:
		name = "OMX_IndexConfigCommonExclusionRect";
		break;
	case OMX_IndexConfigCommonDithering:
		name = "OMX_IndexConfigCommonDithering";
		break;
	case OMX_IndexConfigCommonPlaneBlend:
		name = "OMX_IndexConfigCommonPlaneBlend";
		break;
	case OMX_IndexConfigCommonExposureValue:
		name = "OMX_IndexConfigCommonExposureValue";
		break;
	case OMX_IndexConfigCommonOutputSize:
		name = "OMX_IndexConfigCommonOutputSize";
		break;
	case OMX_IndexParamCommonExtraQuantData:
		name = "OMX_IndexParamCommonExtraQuantData";
		break;
	case OMX_IndexConfigCommonFocusRegion:
		name = "OMX_IndexConfigCommonFocusRegion";
		break;
	case OMX_IndexConfigCommonFocusStatus:
		name = "OMX_IndexConfigCommonFocusStatus";
		break;
	case OMX_IndexConfigCommonTransitionEffect:
		name = "OMX_IndexConfigCommonTransitionEffect";
		break;

	//Reserved Configuration range
	case OMX_IndexOtherStartUnused:
		name = "OMX_IndexOtherStartUnused";
		break;
	case OMX_IndexParamOtherPortFormat:
		name = "OMX_IndexParamOtherPortFormat";
		break;
	case OMX_IndexConfigOtherPower:
		name = "OMX_IndexConfigOtherPower";
		break;
	case OMX_IndexConfigOtherStats:
		name = "OMX_IndexConfigOtherStats";
		break;

	//Reserved Time range
	case OMX_IndexTimeStartUnused:
		name = "OMX_IndexTimeStartUnused";
		break;
	case OMX_IndexConfigTimeScale:
		name = "OMX_IndexConfigTimeScale";
		break;
	case OMX_IndexConfigTimeClockState:
		name = "OMX_IndexConfigTimeClockState";
		break;
	case OMX_IndexConfigTimeActiveRefClock:
		name = "OMX_IndexConfigTimeActiveRefClock";
		break;
	case OMX_IndexConfigTimeCurrentMediaTime:
		name = "OMX_IndexConfigTimeCurrentMediaTime";
		break;
	case OMX_IndexConfigTimeCurrentWallTime:
		name = "OMX_IndexConfigTimeCurrentWallTime";
		break;
	case OMX_IndexConfigTimeCurrentAudioReference:
		name = "OMX_IndexConfigTimeCurrentAudioReference";
		break;
	case OMX_IndexConfigTimeCurrentVideoReference:
		name = "OMX_IndexConfigTimeCurrentVideoReference";
		break;
	case OMX_IndexConfigTimeMediaTimeRequest:
		name = "OMX_IndexConfigTimeMediaTimeRequest";
		break;
	case OMX_IndexConfigTimeClientStartTime:
		name = "OMX_IndexConfigTimeClientStartTime";
		break;
	case OMX_IndexConfigTimePosition:
		name = "OMX_IndexConfigTimePosition";
		break;
	case OMX_IndexConfigTimeSeekMode:
		name = "OMX_IndexConfigTimeSeekMode";
		break;

	case OMX_IndexMax:
		name = "OMX_IndexMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_IndexKhronosExtensions <= id &&
		id < OMX_IndexVendorStartUnused) {
		name = "(OMX_IndexKhronosExtensions)";
	}
	if (OMX_IndexVendorStartUnused <= id &&
		id < OMX_IndexMax) {
		name = "(OMX_IndexVendorStartUnused)";
	}

	return name;
}

} //namespace mf
