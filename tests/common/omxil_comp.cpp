
#include <cstring>
#include <string>

#include <OMX_Component.h>

#include <common/omxil_comp.hpp>

#include "common/omxil_utils.h"

omxil_comp::omxil_comp(const char *comp_name)
	: comp(nullptr)
{
	char namebuf[OMX_MAX_STRINGNAME_SIZE] = "";
	OMX_ERRORTYPE result;

	//save arguments
	name = comp_name;

	//get handle
	snprintf(namebuf, sizeof(namebuf) - 1, "%s", name.c_str());
	namebuf[sizeof(namebuf) - 1] = '\0';
	callbacks.EventHandler    = gate_EventHandler;
	callbacks.EmptyBufferDone = gate_EmptyBufferDone;
	callbacks.FillBufferDone  = gate_FillBufferDone;
	result = OMX_GetHandle(&comp, namebuf, this, &callbacks);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetHandle failed.");
	}
}

omxil_comp::~omxil_comp()
{
	OMX_ERRORTYPE result;

	result = OMX_FreeHandle(comp);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_FreeHandle failed.");
	}
}

OMX_HANDLETYPE omxil_comp::get_component()
{
	return comp;
}

OMX_ERRORTYPE omxil_comp::GetComponentVersion(OMX_STRING pComponentName, OMX_VERSIONTYPE *pComponentVersion, OMX_VERSIONTYPE *pSpecVersion, OMX_UUIDTYPE *pComponentUUID) const
{
	OMX_ERRORTYPE result;

	result = OMX_GetComponentVersion(comp, pComponentName, pComponentVersion, pSpecVersion, pComponentUUID);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetComponentVersion failed.\n");
	}

	return result;
}

OMX_ERRORTYPE omxil_comp::SendCommand(OMX_COMMANDTYPE Cmd, OMX_U32 nParam, OMX_PTR pCmdData)
{
	OMX_ERRORTYPE result;

	result = OMX_SendCommand(comp, Cmd, nParam, pCmdData);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_SendCommand failed.\n");
	}

	return result;
}

OMX_ERRORTYPE omxil_comp::GetParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) const
{
	OMX_ERRORTYPE result;

	result = OMX_GetParameter(comp, nParamIndex, pComponentParameterStructure);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetParameter failed.\n");
	}

	return result;
}

OMX_ERRORTYPE omxil_comp::SetParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure)
{
	OMX_ERRORTYPE result;

	result = OMX_SetParameter(comp, nParamIndex, pComponentParameterStructure);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_SetParameter failed.\n");
	}

	return result;
}

OMX_ERRORTYPE omxil_comp::GetConfig(OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure) const
{
	OMX_ERRORTYPE result;

	result = OMX_GetConfig(comp, nIndex, pComponentConfigStructure);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetConfig failed.\n");
	}

	return result;
}

OMX_ERRORTYPE omxil_comp::SetConfig(OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure)
{
	OMX_ERRORTYPE result;

	result = OMX_SetConfig(comp, nIndex, pComponentConfigStructure);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_SetConfig failed.\n");
	}

	return result;
}

OMX_ERRORTYPE omxil_comp::GetExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE *pIndexType) const
{
	OMX_ERRORTYPE result;

	result = OMX_GetExtensionIndex(comp, cParameterName, pIndexType);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetExtensionIndex failed.\n");
	}

	return result;
}

OMX_ERRORTYPE omxil_comp::GetState(OMX_STATETYPE *pState) const
{
	OMX_ERRORTYPE result;

	result = OMX_GetState(comp, pState);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetState failed.\n");
	}

	return result;
}

OMX_ERRORTYPE omxil_comp::UseBuffer(OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8 *pBuffer)
{
	OMX_ERRORTYPE result;

	result = OMX_UseBuffer(comp, ppBufferHdr, nPortIndex, pAppPrivate, nSizeBytes, pBuffer);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_UseBuffer failed.\n");
	}

	return result;
}

OMX_ERRORTYPE omxil_comp::AllocateBuffer(OMX_BUFFERHEADERTYPE **ppBuffer, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes)
{
	OMX_ERRORTYPE result;

	result = OMX_AllocateBuffer(comp, ppBuffer, nPortIndex, pAppPrivate, nSizeBytes);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_AllocateBuffer failed.\n");
	}

	return result;
}

OMX_ERRORTYPE omxil_comp::FreeBuffer(OMX_U32 nPortIndex, OMX_BUFFERHEADERTYPE *pBuffer)
{
	OMX_ERRORTYPE result;

	result = OMX_FreeBuffer(comp, nPortIndex, pBuffer);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_FreeBuffer failed.\n");
	}

	return result;
}

OMX_ERRORTYPE omxil_comp::EmptyThisBuffer(OMX_BUFFERHEADERTYPE *pBuffer)
{
	OMX_ERRORTYPE result;

	result = OMX_EmptyThisBuffer(comp, pBuffer);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_EmptyThisBuffer failed.\n");
	}

	return result;
}

OMX_ERRORTYPE omxil_comp::FillThisBuffer(OMX_BUFFERHEADERTYPE *pBuffer)
{
	OMX_ERRORTYPE result;

	result = OMX_FillThisBuffer(comp, pBuffer);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_FillThisBuffer failed.\n");
	}

	return result;
}

OMX_ERRORTYPE omxil_comp::EventHandler(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	return OMX_ErrorNone;
}

OMX_ERRORTYPE omxil_comp::EmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
{
	return OMX_ErrorNone;
}

OMX_ERRORTYPE omxil_comp::FillBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
{
	return OMX_ErrorNone;
}


OMX_ERRORTYPE omxil_comp::gate_EventHandler(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	omxil_comp *c = static_cast<omxil_comp *>(pAppData);

	return c->EventHandler(hComponent, pAppData, eEvent,
			nData1, nData2, pEventData);
}

OMX_ERRORTYPE omxil_comp::gate_EmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
{
	omxil_comp *c = static_cast<omxil_comp *>(pAppData);

	return c->EmptyBufferDone(hComponent, pAppData, pBuffer);
}

OMX_ERRORTYPE omxil_comp::gate_FillBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
{
	omxil_comp *c = static_cast<omxil_comp *>(pAppData);

	return c->FillBufferDone(hComponent, pAppData, pBuffer);
}

OMX_ERRORTYPE omxil_comp::get_param_audio_init(OMX_PORT_PARAM_TYPE *param) const
{
	OMX_ERRORTYPE result;

	memset(param, 0, sizeof(OMX_PORT_PARAM_TYPE));
	param->nSize = sizeof(OMX_PORT_PARAM_TYPE);
	fill_version(&param->nVersion);
	result = GetParameter(OMX_IndexParamAudioInit, param);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetParameter(IndexParamAudioInit) "
			"failed.\n");
		return result;
	}

	return result;
}

OMX_ERRORTYPE omxil_comp::get_param_image_init(OMX_PORT_PARAM_TYPE *param) const
{
	OMX_ERRORTYPE result;

	memset(param, 0, sizeof(OMX_PORT_PARAM_TYPE));
	param->nSize = sizeof(OMX_PORT_PARAM_TYPE);
	fill_version(&param->nVersion);
	result = GetParameter(OMX_IndexParamImageInit, param);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetParameter(IndexParamImageInit) "
			"failed.\n");
		return result;
	}

	return result;
}

OMX_ERRORTYPE omxil_comp::get_param_video_init(OMX_PORT_PARAM_TYPE *param) const
{
	OMX_ERRORTYPE result;

	memset(param, 0, sizeof(OMX_PORT_PARAM_TYPE));
	param->nSize = sizeof(OMX_PORT_PARAM_TYPE);
	fill_version(&param->nVersion);
	result = GetParameter(OMX_IndexParamVideoInit, param);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetParameter(IndexParamVideoInit) "
			"failed.\n");
		return result;
	}

	return result;
}

OMX_ERRORTYPE omxil_comp::get_param_other_init(OMX_PORT_PARAM_TYPE *param) const
{
	OMX_ERRORTYPE result;

	memset(param, 0, sizeof(OMX_PORT_PARAM_TYPE));
	param->nSize = sizeof(OMX_PORT_PARAM_TYPE);
	fill_version(&param->nVersion);
	result = GetParameter(OMX_IndexParamOtherInit, param);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetParameter(IndexParamOtherInit) "
			"failed.\n");
		return result;
	}

	return result;
}

OMX_ERRORTYPE omxil_comp::get_param_port_definition(OMX_U32 port_index, OMX_PARAM_PORTDEFINITIONTYPE *def) const
{
	OMX_ERRORTYPE result;

	memset(def, 0, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
	def->nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
	fill_version(&def->nVersion);
	def->nPortIndex = port_index;
	result = GetParameter(OMX_IndexParamPortDefinition, def);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetParameter(IndexParamPortDefinition) "
			"failed.\n");
		return result;
	}

	return result;
}

/*
 * public static function
 */

OMX_ERRORTYPE omxil_comp::fill_version(OMX_VERSIONTYPE *v)
{
	memset(v, 0, sizeof(OMX_VERSIONTYPE));
	v->s.nVersionMajor = 1;
	v->s.nVersionMinor = 1;
	v->s.nRevision = 0;
	v->s.nStep = 0;

	return OMX_ErrorNone;
}
