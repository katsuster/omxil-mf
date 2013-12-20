
#include <stdio.h>
#include <string.h>

#include <OMX_Core.h>
#include <OMX_Component.h>

#include "common/test_omxil.h"
#include "common/omxil_utils.h"

OMX_ERRORTYPE test_event_handler(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_EVENTTYPE eEvent, OMX_IN OMX_U32 nData1, OMX_IN OMX_U32 nData2, OMX_IN OMX_PTR pEventData)
{
	printf("%s\n", __func__);
}

OMX_ERRORTYPE test_empty_buffer_done(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
	printf("%s\n", __func__);
}

OMX_ERRORTYPE test_fill_buffer_done(OMX_OUT OMX_HANDLETYPE hComponent, OMX_OUT OMX_PTR pAppData, OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer)
{
	printf("%s\n", __func__);
}

int main(int argc, char *argv[])
{
	const char *arg_comp;
	OMX_HANDLETYPE comp;
	char name_comp[OMX_MAX_STRINGNAME_SIZE];
	OMX_PTR app_data;
	OMX_CALLBACKTYPE callbacks;
	OMX_PORT_PARAM_TYPE parm_v, parm_a;
	
	OMX_BUFFERHEADERTYPE *buf_in, *buf_out;
	OMX_ERRORTYPE result;
	OMX_U32 i;
	
	//get arguments
	if (argc < 2) {
		arg_comp = "OMX.st.video_decoder.avc";
	} else {
		arg_comp = argv[1];
	}
	
	//Reference:
	//    OpenMAX IL specification version 1.1.2
	//    3.2.3.4.3 Sample Code Showing Calling Sequence
	
	comp = NULL;
	result = OMX_ErrorNone;
	
	result = OMX_Init();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Init failed.\n");
		goto err_out1;
	}
	
	snprintf(name_comp, sizeof(name_comp), arg_comp);
	app_data = NULL;
	callbacks.EventHandler = test_event_handler;
	callbacks.EmptyBufferDone = test_empty_buffer_done;
	callbacks.FillBufferDone = test_fill_buffer_done;
	result = OMX_GetHandle(&comp, name_comp, 
		app_data, &callbacks);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetHandle failed.\n");
		goto err_out2;
	}
	printf("name:%s, comp:%p\n", 
		name_comp, comp);
	
	memset(&parm_v, 0, sizeof(parm_v));
	parm_v.nSize = sizeof(parm_v);
	parm_v.nVersion.s.nVersionMajor = 1;
	parm_v.nVersion.s.nVersionMinor = 1;
	parm_v.nVersion.s.nRevision = 0;
	parm_v.nVersion.s.nStep = 0;
	result = OMX_GetParameter(comp, OMX_IndexParamVideoInit, 
		&parm_v);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetParameter(IndexParamVideoInit) "
			"failed.\n");
		goto err_out3;
	}
	printf("video -----\n");
	dump_port_param_type(&parm_v);
	
	memset(&parm_a, 0, sizeof(parm_a));
	parm_a.nSize = sizeof(parm_a);
	parm_a.nVersion.s.nVersionMajor = 1;
	parm_a.nVersion.s.nVersionMinor = 1;
	parm_a.nVersion.s.nRevision = 0;
	parm_a.nVersion.s.nStep = 0;
	result = OMX_GetParameter(comp, OMX_IndexParamAudioInit, 
		&parm_a);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetParameter(IndexParamAudioInit) "
			"failed.\n");
		goto err_out3;
	}
	printf("audio -----\n");
	dump_port_param_type(&parm_a);
	
	
	/*
	OMX_VIDEO_PARAM_AVCTYPE
	result = OMX_SetParameter(comp, OMX_IndexParamVideoAvc, );
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_SetParameter() failed.\n");
		goto err_out3;
	}*/
	
	
	result = OMX_SendCommand(comp, OMX_CommandStateSet, 
		OMX_StateIdle, 0);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_SendCommand(SteteSet, Idle) failed.\n");
		goto err_out3;
	}
	
	result = OMX_AllocateBuffer(comp, &buf_in, 
		0, 0, 1048576);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_AllocateBuffer(in) failed.\n");
		goto err_out3;
	}
	dump_port_bufferheadertype(buf_in);
	
	result = OMX_AllocateBuffer(comp, &buf_out, 
		1, 0, 1048576);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_AllocateBuffer(out) failed.\n");
		goto err_out3;
	}
	dump_port_bufferheadertype(buf_out);
	
	result = OMX_FreeHandle(comp);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_FreeHandle failed.\n");
		goto err_out2;
	}
	
	result = OMX_Deinit();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Deinit failed.\n");
		goto err_out1;
	}
	
	return 0;
	
err_out3:
	OMX_FreeHandle(comp);
	
err_out2:
	OMX_Deinit();
	
err_out1:
	return result;
}
