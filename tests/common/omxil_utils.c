
#include <stdio.h>

#include "omxil_utils.h"

const char *get_omx_statetype_name(OMX_STATETYPE v)
{
	const char *name = "????";

	switch (v) {
	case OMX_StateInvalid:
		name = "OMX_StateInvalid";
		break;
	case OMX_StateLoaded:
		name = "OMX_StateLoaded";
		break;
	case OMX_StateIdle:
		name = "OMX_StateIdle";
		break;
	case OMX_StateExecuting:
		name = "OMX_StateExecuting";
		break;
	case OMX_StatePause:
		name = "OMX_StatePause";
		break;
	case OMX_StateWaitForResources:
		name = "OMX_StateWaitForResources";
		break;
	default:
		name = "unknown";
		break;
	}

	if (OMX_StateKhronosExtensions <= v &&
		v < OMX_StateVendorStartUnused) {
		name = "OMX_StateKhronosExtensions";
	}
	if (OMX_StateVendorStartUnused <= v &&
		v < OMX_StateMax) {
		name = "OMX_StateVendorStartUnused";
	}

	return name;
}

const char *get_omx_dirtype_name(OMX_DIRTYPE v)
{
	const char *name = "????";

	switch (v) {
	case OMX_DirInput:
		name = "OMX_DirInput";
		break;
	case OMX_DirOutput:
		name = "OMX_DirOutput";
		break;
	case OMX_DirMax:
		name = "OMX_DirMax";
		break;
	default:
		name = "unknown";
		break;
	}

	return name;
}

const char *get_omx_portdomaintype_name(OMX_PORTDOMAINTYPE v)
{
	const char *name = "????";

	switch (v) {
	case OMX_PortDomainAudio:
		name = "OMX_PortDomainAudio";
		break;
	case OMX_PortDomainVideo:
		name = "OMX_PortDomainVideo";
		break;
	case OMX_PortDomainImage:
		name = "OMX_PortDomainImage";
		break;
	case OMX_PortDomainOther:
		name = "OMX_PortDomainOther";
		break;
	case OMX_PortDomainMax:
		name = "OMX_PortDomainMax";
		break;
	default:
		name = "unknown";
		break;
	}

	if (OMX_PortDomainKhronosExtensions <= v &&
		v < OMX_PortDomainVendorStartUnused) {
		name = "OMX_PortDomainKhronosExtensions";
	}
	if (OMX_PortDomainVendorStartUnused <= v &&
		v < OMX_PortDomainMax) {
		name = "OMX_PortDomainVendorStartUnused";
	}

	return name;
}

const char *get_omx_audio_codingtype_name(OMX_AUDIO_CODINGTYPE v)
{
	const char *name = "????";

	switch (v) {
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
		name = "unknown";
		break;
	}

	if (OMX_AUDIO_CodingKhronosExtensions <= v &&
		v < OMX_AUDIO_CodingVendorStartUnused) {
		name = "OMX_AUDIO_CodingKhronosExtensions";
	}
	if (OMX_AUDIO_CodingVendorStartUnused <= v &&
		v < OMX_AUDIO_CodingMax) {
		name = "OMX_AUDIO_CodingVendorStartUnused";
	}

	return name;
}

const char *get_omx_image_codingtype_name(OMX_IMAGE_CODINGTYPE v)
{
	const char *name = "????";

	switch (v) {
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
		name = "unknown";
		break;
	}

	if (OMX_IMAGE_CodingKhronosExtensions <= v &&
		v < OMX_IMAGE_CodingVendorStartUnused) {
		name = "OMX_IMAGE_CodingKhronosExtensions";
	}
	if (OMX_IMAGE_CodingVendorStartUnused <= v &&
		v < OMX_IMAGE_CodingMax) {
		name = "OMX_IMAGE_CodingVendorStartUnused";
	}

	return name;
}

const char *get_omx_color_formattype_name(OMX_COLOR_FORMATTYPE v)
{
	const char *name = "????";

	switch (v) {
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
		name = "unknown";
		break;
	}

	if (OMX_COLOR_FormatKhronosExtensions <= v &&
		v < OMX_COLOR_FormatVendorStartUnused) {
		name = "OMX_COLOR_FormatKhronosExtensions";
	}
	if (OMX_COLOR_FormatVendorStartUnused <= v &&
		v < OMX_COLOR_FormatMax) {
		name = "OMX_COLOR_FormatVendorStartUnused";
	}

	return name;
}

const char *get_omx_video_codingtype_name(OMX_VIDEO_CODINGTYPE v)
{
	const char *name = "????";

	switch (v) {
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
	default:
		name = "unknown";
		break;
	}

	if (OMX_VIDEO_CodingKhronosExtensions <= v &&
		v < OMX_VIDEO_CodingVendorStartUnused) {
		name = "OMX_VIDEO_CodingKhronosExtensions";
	}
	if (OMX_VIDEO_CodingVendorStartUnused <= v &&
		v < OMX_VIDEO_CodingMax) {
		name = "OMX_VIDEO_CodingVendorStartUnused";
	}

	return name;
}

const char *get_omx_other_formattype_name(OMX_OTHER_FORMATTYPE v)
{
	const char *name = "????";

	switch (v) {
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
		name = "unknown";
		break;
	}

	if (OMX_OTHER_FormatKhronosExtensions <= v &&
		v < OMX_OTHER_FormatVendorStartUnused) {
		name = "OMX_OTHER_FormatKhronosExtensions";
	}
	if (OMX_OTHER_FormatVendorStartUnused <= v &&
		v < OMX_OTHER_FormatMax) {
		name = "OMX_OTHER_FormatVendorStartUnused";
	}

	return name;
}



void dump_bufferheadertype(const OMX_BUFFERHEADERTYPE *p)
{
	printf("OMX_BUFFERHEADERTYPE %p ----\n"
		"  nSize               : %d\n"
		"  nVersion            : %02d.%02d.%02d.%02d\n"
		"  pBuffer             : %p\n"
		"  nAllocLen           : %d\n"
		"  nFilledLen          : %d\n"
		"  nOffset             : %d\n"
		"  pAppPrivate         : %p\n"
		"  pPlatformPrivate    : %p\n"
		"  pInputPortPrivate   : %p\n"
		"  pOutputPortPrivate  : %p\n"
		"  hMarkTargetComponent: %p\n"
		"  pMarkData           : %p\n"
		"  nTickCount          : %d\n"
		"  nTimeStamp          : %lld\n"
		"  nFlags              : %d\n"
		"  nOutputPortIndex    : %d\n"
		"  nInputPortIndex     : %d\n",
		p,
		(int)p->nSize,
		(int)p->nVersion.s.nVersionMajor,
		(int)p->nVersion.s.nVersionMinor,
		(int)p->nVersion.s.nRevision,
		(int)p->nVersion.s.nStep,
		p->pBuffer,
		(int)p->nAllocLen,
		(int)p->nFilledLen,
		(int)p->nOffset,
		p->pAppPrivate,
		p->pPlatformPrivate,
		p->pInputPortPrivate,
		p->pOutputPortPrivate,
		p->hMarkTargetComponent,
		p->pMarkData,
		(int)p->nTickCount,
#ifndef OMX_SKIP64BIT
		p->nTimeStamp,
#else
		(uint64_t)(nTimeStamp.nHighPart << 32) | nTimeStamp.nLowPart,
#endif
		(int)p->nFlags,
		(int)p->nOutputPortIndex,
		(int)p->nInputPortIndex);
}

void dump_port_param_type(const OMX_PORT_PARAM_TYPE *p)
{
	printf("OMX_PORT_PARAM_TYPE %p ----\n"
		"  nSize           : %d\n"
		"  nVersion        : %02d.%02d.%02d.%02d\n"
		"  nPorts          : %d\n"
		"  nStartPortNumber: %d\n",
		p,
		(int)p->nSize,
		(int)p->nVersion.s.nVersionMajor,
		(int)p->nVersion.s.nVersionMinor,
		(int)p->nVersion.s.nRevision,
		(int)p->nVersion.s.nStep,
		(int)p->nPorts,
		(int)p->nStartPortNumber);
}

void dump_param_portdefinitiontype(const OMX_PARAM_PORTDEFINITIONTYPE *p)
{
	printf("OMX_PARAM_PORTDEFINITIONTYPE %p ----\n"
		"  nSize             : %d\n"
		"  nVersion          : %02d.%02d.%02d.%02d\n"
		"  nPortIndex        : %d\n"
		"  eDir              : %d(%s)\n"
		"  nBufferCountActual: %d\n"
		"  nBufferCountMin   : %d\n"
		"  nBufferSize       : %d\n"
		"  bEnabled          : %d\n"
		"  bPopulated        : %d\n"
		"  eDomain           : %d(%s)\n"
		//"  format            : \n"
		"  bBuffersContiguous: %d\n"
		"  nBufferAlignment  : %d\n",
		p,
		(int)p->nSize,
		(int)p->nVersion.s.nVersionMajor,
		(int)p->nVersion.s.nVersionMinor,
		(int)p->nVersion.s.nRevision,
		(int)p->nVersion.s.nStep,
		(int)p->nPortIndex,
		p->eDir, get_omx_dirtype_name(p->eDir),
		(int)p->nBufferCountActual,
		(int)p->nBufferCountMin,
		(int)p->nBufferSize,
		p->bEnabled,
		p->bPopulated,
		(int)p->eDomain, get_omx_portdomaintype_name(p->eDomain),
		//TODO: need to dump 'format' member...
		//union {
		//	OMX_AUDIO_PORTDEFINITIONTYPE audio
		//	OMX_VIDEO_PORTDEFINITIONTYPE video
		//	OMX_IMAGE_PORTDEFINITIONTYPE image
		//	OMX_OTHER_PORTDEFINITIONTYPE other
		//} pp->format,
		p->bBuffersContiguous,
		(int)(int)p->nBufferAlignment);
}

void dump_audio_param_portformattype(const OMX_AUDIO_PARAM_PORTFORMATTYPE *p)
{
	printf("OMX_AUDIO_PARAM_PORTFORMATTYPE %p ----\n"
		"  nSize             : %d\n"
		"  nVersion          : %02d.%02d.%02d.%02d\n"
		"  nPortIndex        : %d\n"
		"  nIndex            : %d\n"
		"  eEncoding         : %d(%s)\n",
		p,
		(int)p->nSize,
		(int)p->nVersion.s.nVersionMajor,
		(int)p->nVersion.s.nVersionMinor,
		(int)p->nVersion.s.nRevision,
		(int)p->nVersion.s.nStep,
		(int)p->nPortIndex,
		(int)p->nIndex,
		p->eEncoding, get_omx_audio_codingtype_name(p->eEncoding));
}

void dump_image_param_portformattype(const OMX_IMAGE_PARAM_PORTFORMATTYPE *p)
{
	printf("OMX_IMAGE_PARAM_PORTFORMATTYPE %p ----\n"
		"  nSize             : %d\n"
		"  nVersion          : %02d.%02d.%02d.%02d\n"
		"  nPortIndex        : %d\n"
		"  nIndex            : %d\n"
		"  eCompressionFormat: %d(%s)\n"
		"  eColorFormat      : %d(%s)\n",
		p,
		(int)p->nSize,
		(int)p->nVersion.s.nVersionMajor,
		(int)p->nVersion.s.nVersionMinor,
		(int)p->nVersion.s.nRevision,
		(int)p->nVersion.s.nStep,
		(int)p->nPortIndex,
		(int)p->nIndex,
		p->eCompressionFormat, get_omx_image_codingtype_name(p->eCompressionFormat),
		p->eColorFormat, get_omx_color_formattype_name(p->eColorFormat));
}

void dump_video_param_portformattype(const OMX_VIDEO_PARAM_PORTFORMATTYPE *p)
{
	printf("OMX_VIDEO_PARAM_PORTFORMATTYPE %p ----\n"
		"  nSize             : %d\n"
		"  nVersion          : %02d.%02d.%02d.%02d\n"
		"  nPortIndex        : %d\n"
		"  nIndex            : %d\n"
		"  eCompressionFormat: %d(%s)\n"
		"  eColorFormat      : %d(%s)\n"
		"  xFramerate        : 0x%08x\n",
		p,
		(int)p->nSize,
		(int)p->nVersion.s.nVersionMajor,
		(int)p->nVersion.s.nVersionMinor,
		(int)p->nVersion.s.nRevision,
		(int)p->nVersion.s.nStep,
		(int)p->nPortIndex,
		(int)p->nIndex,
		p->eCompressionFormat, get_omx_video_codingtype_name(p->eCompressionFormat),
		p->eColorFormat, get_omx_color_formattype_name(p->eColorFormat),
		(int)p->xFramerate);
}

void dump_other_param_portformattype(const OMX_OTHER_PARAM_PORTFORMATTYPE *p)
{
	printf("OMX_OTHER_PARAM_PORTFORMATTYPE %p ----\n"
		"  nSize             : %d\n"
		"  nVersion          : %02d.%02d.%02d.%02d\n"
		"  nPortIndex        : %d\n"
		"  nIndex            : %d\n"
		"  eFormat           : %d(%s)\n",
		p,
		(int)p->nSize,
		(int)p->nVersion.s.nVersionMajor,
		(int)p->nVersion.s.nVersionMinor,
		(int)p->nVersion.s.nRevision,
		(int)p->nVersion.s.nStep,
		(int)p->nPortIndex,
		(int)p->nIndex,
		p->eFormat, get_omx_other_formattype_name(p->eFormat));
}

