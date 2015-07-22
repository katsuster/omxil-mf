
#ifndef OMXIL_COMP_HPP__
#define OMXIL_COMP_HPP__

#include <string>

#include <OMX_Core.h>

class omxil_comp {
public:
	omxil_comp(const char *comp_name);
	virtual ~omxil_comp();

	virtual OMX_HANDLETYPE get_component();

	virtual OMX_ERRORTYPE GetComponentVersion(OMX_STRING pComponentName, OMX_VERSIONTYPE *pComponentVersion, OMX_VERSIONTYPE *pSpecVersion, OMX_UUIDTYPE *pComponentUUID);
	virtual OMX_ERRORTYPE SendCommand(OMX_COMMANDTYPE Cmd, OMX_U32 nParam, OMX_PTR pCmdData);
	virtual OMX_ERRORTYPE GetParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);
	virtual OMX_ERRORTYPE SetParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);
	virtual OMX_ERRORTYPE GetConfig(OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure);
	virtual OMX_ERRORTYPE SetConfig(OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure);
	virtual OMX_ERRORTYPE GetExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE *pIndexType);
	virtual OMX_ERRORTYPE GetState(OMX_STATETYPE *pState);
	virtual OMX_ERRORTYPE UseBuffer(OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8 *pBuffer);
	virtual OMX_ERRORTYPE AllocateBuffer(OMX_BUFFERHEADERTYPE **ppBuffer, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes);
	virtual OMX_ERRORTYPE FreeBuffer(OMX_U32 nPortIndex, OMX_BUFFERHEADERTYPE *pBuffer);
	virtual OMX_ERRORTYPE EmptyThisBuffer(OMX_BUFFERHEADERTYPE *pBuffer);
	virtual OMX_ERRORTYPE FillThisBuffer(OMX_BUFFERHEADERTYPE *pBuffer);

	virtual OMX_ERRORTYPE EventHandler(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
	virtual OMX_ERRORTYPE EmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer);
	virtual OMX_ERRORTYPE FillBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer);


protected:
	static OMX_ERRORTYPE gate_EventHandler(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
	static OMX_ERRORTYPE gate_EmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer);
	static OMX_ERRORTYPE gate_FillBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer);


private:
	OMX_HANDLETYPE comp;
	OMX_CALLBACKTYPE callbacks;
	std::string name;

};

#endif //OMXIL_COMP_HPP__

