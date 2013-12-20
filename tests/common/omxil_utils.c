
#include <stdio.h>

#include "omxil_utils.h"

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



void dump_port_bufferheadertype(const OMX_BUFFERHEADERTYPE *p)
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
		p->nSize, 
		p->nVersion.s.nVersionMajor, 
		p->nVersion.s.nVersionMinor, 
		p->nVersion.s.nRevision, 
		p->nVersion.s.nStep, 
		p->pBuffer, 
		p->nAllocLen, 
		p->nFilledLen, 
		p->nOffset, 
		p->pAppPrivate, 
		p->pPlatformPrivate, 
		p->pInputPortPrivate, 
		p->pOutputPortPrivate, 
		p->hMarkTargetComponent, 
		p->pMarkData, 
		p->nTickCount, 
#ifndef OMX_SKIP64BIT
		p->nTimeStamp, 
#else
		(uint64_t)(nTimeStamp.nHighPart << 32) | nTimeStamp.nLowPart, 
#endif
		p->nFlags, 
		p->nOutputPortIndex, 
		p->nInputPortIndex);
}

void dump_port_param_type(const OMX_PORT_PARAM_TYPE *p)
{
	printf("OMX_PORT_PARAM_TYPE %p ----\n"
		"  nSize           : %d\n"
		"  nVersion        : %02d.%02d.%02d.%02d\n"
		"  nPorts          : %d\n"
		"  nStartPortNumber: %d\n", 
		p, 
		p->nSize, 
		p->nVersion.s.nVersionMajor, 
		p->nVersion.s.nVersionMinor, 
		p->nVersion.s.nRevision, 
		p->nVersion.s.nStep, 
		p->nPorts, 
		p->nStartPortNumber);
}

void dump_port_definitiontype(const OMX_PARAM_PORTDEFINITIONTYPE *p)
{
	printf("OMX_PARAM_PORTDEFINITIONTYPE %p ----\n"
		"  nPortIndex        : %d\n"
		"  eDir              : %d(%s)\n"
		"  nBufferCountActual: %d\n"
		"  nBufferCountMin   : %d\n"
		"  nBufferSize       : %d\n"
		"  bEnabled          : %d\n"
		"  bPopulated        : %d\n"
		"  eDomain           : %d\n"
		//"  format            : \n"
		"  bBuffersContiguous: %d\n"
		"  nBufferAlignment  : %d\n", 
		p, 
		p->nPortIndex, 
		p->eDir, get_omx_dirtype_name(p->eDir), 
		p->nBufferCountActual, 
		p->nBufferCountMin, 
		p->nBufferSize, 
		p->bEnabled, 
		p->bPopulated, 
		p->eDomain, get_omx_portdomaintype_name(p->eDomain), 
		//TODO: need to dump 'format' member...
		//union {
		//	OMX_AUDIO_PORTDEFINITIONTYPE audio
		//	OMX_VIDEO_PORTDEFINITIONTYPE video
		//	OMX_IMAGE_PORTDEFINITIONTYPE image
		//	OMX_OTHER_PORTDEFINITIONTYPE other
		//} pp->format, 
		p->bBuffersContiguous, 
		p->nBufferAlignment);
}

