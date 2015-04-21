
#ifndef OMXIL_COMP_HPP__
#define OMXIL_COMP_HPP__

#include <string>

#include <OMX_Core.h>

class omxil_comp {
public:
	omxil_comp(const char *comp_name)
		: comp(nullptr)
	{
		char namebuf[OMX_MAX_STRINGNAME_SIZE] = "";
		OMX_CALLBACKTYPE callbacks;
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

	virtual ~omxil_comp()
	{
		OMX_ERRORTYPE result;

		result = OMX_FreeHandle(comp);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_FreeHandle failed.");
		}
	}

	virtual OMX_ERRORTYPE SendCommand(OMX_COMMANDTYPE Cmd, OMX_U32 nParam, OMX_PTR pCmdData)
	{
		OMX_ERRORTYPE result;

		result = OMX_SendCommand(comp, Cmd, nParam, pCmdData);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_SendCommand failed.\n");
		}

		return result;
	}

	virtual OMX_ERRORTYPE GetParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure)
	{
		OMX_ERRORTYPE result;

		result = OMX_GetParameter(comp, nParamIndex, pComponentParameterStructure);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_GetParameter failed.\n");
		}

		return result;
	}

	virtual OMX_ERRORTYPE SetParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure)
	{
		OMX_ERRORTYPE result;

		result = OMX_SetParameter(comp, nParamIndex, pComponentParameterStructure);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_SetParameter failed.\n");
		}

		return result;
	}

	virtual OMX_ERRORTYPE GetConfig(OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure)
	{
		OMX_ERRORTYPE result;

		result = OMX_GetConfig(comp, nIndex, pComponentConfigStructure);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_GetConfig failed.\n");
		}

		return result;
	}

	virtual OMX_ERRORTYPE SetConfig(OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure)
	{
		OMX_ERRORTYPE result;

		result = OMX_SetConfig(comp, nIndex, pComponentConfigStructure);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_SetConfig failed.\n");
		}

		return result;
	}

	virtual OMX_ERRORTYPE GetExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE *pIndexType)
	{
		OMX_ERRORTYPE result;

		result = OMX_GetExtensionIndex(comp, cParameterName, pIndexType);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_GetExtensionIndex failed.\n");
		}

		return result;
	}

	virtual OMX_ERRORTYPE GetState(OMX_STATETYPE *pState)
	{
		OMX_ERRORTYPE result;

		result = OMX_GetState(comp, pState);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_GetState failed.\n");
		}

		return result;
	}

	virtual OMX_ERRORTYPE UseBuffer(OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8 *pBuffer)
	{
		OMX_ERRORTYPE result;

		result = OMX_UseBuffer(comp, ppBufferHdr, nPortIndex, pAppPrivate, nSizeBytes, pBuffer);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_UseBuffer failed.\n");
		}

		return result;
	}

	virtual OMX_ERRORTYPE AllocateBuffer(OMX_BUFFERHEADERTYPE **ppBuffer, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes)
	{
		OMX_ERRORTYPE result;

		result = OMX_AllocateBuffer(comp, ppBuffer, nPortIndex, pAppPrivate, nSizeBytes);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_AllocateBuffer failed.\n");
		}

		return result;
	}

	virtual OMX_ERRORTYPE FreeBuffer(OMX_U32 nPortIndex, OMX_BUFFERHEADERTYPE *pBuffer)
	{
		OMX_ERRORTYPE result;

		result = OMX_FreeBuffer(comp, nPortIndex, pBuffer);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_FreeBuffer failed.\n");
		}

		return result;
	}

	virtual OMX_ERRORTYPE EmptyThisBuffer(OMX_BUFFERHEADERTYPE *pBuffer)
	{
		OMX_ERRORTYPE result;

		result = OMX_EmptyThisBuffer(comp, pBuffer);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_EmptyThisBuffer failed.\n");
		}

		return result;
	}

	virtual OMX_ERRORTYPE FillThisBuffer(OMX_BUFFERHEADERTYPE *pBuffer)
	{
		OMX_ERRORTYPE result;

		result = OMX_FillThisBuffer(comp, pBuffer);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_FillThisBuffer failed.\n");
		}

		return result;
	}

	virtual OMX_ERRORTYPE EventHandler(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
	{
	}

	virtual OMX_ERRORTYPE EmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
	{
	}

	virtual OMX_ERRORTYPE FillBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
	{
	}


protected:
	static OMX_ERRORTYPE gate_EventHandler(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
	{
		omxil_comp *c = (omxil_comp *) pAppData;

		return c->EventHandler(hComponent, pAppData, eEvent, 
				nData1, nData2, pEventData);
	}

	static OMX_ERRORTYPE gate_EmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
	{
		omxil_comp *c = (omxil_comp *) pAppData;

		return c->EmptyBufferDone(hComponent, pAppData, pBuffer);
	}

	static OMX_ERRORTYPE gate_FillBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
	{
		omxil_comp *c = (omxil_comp *) pAppData;

		return c->FillBufferDone(hComponent, pAppData, pBuffer);
	}


private:
	OMX_HANDLETYPE comp;
	std::string name;

};

#endif //OMXIL_COMP_HPP__

