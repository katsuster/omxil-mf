
#include <stdio.h>

#include <OMX_Core.h>
#include <OMX_Component.h>

#include "common/test_omxil.h"

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
	OMX_VERSIONTYPE ver_comp, ver_spec;
	OMX_UUIDTYPE uuid_comp;
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
	printf("OMX_GetHandle: name:%s, comp:%p\n", 
		name_comp, comp);
	
	result = OMX_GetComponentVersion(comp, name_comp, 
		&ver_comp, &ver_spec, &uuid_comp);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "GetComponentVersion failed.\n");
		goto err_out3;
	}
	printf("OMX_GetComponentVersion: name:%s\n"
		"comp ver:0x%08x(v%d.%d.%d.%d)\n"
		"spec ver:0x%08x(OpenMAX IL %d.%d.%d.%d)\n", 
		name_comp, 
		ver_comp, 
		ver_comp.s.nVersionMajor, ver_comp.s.nVersionMinor, 
		ver_comp.s.nRevision, ver_comp.s.nStep, 
		ver_spec, 
		ver_spec.s.nVersionMajor, ver_spec.s.nVersionMinor, 
		ver_spec.s.nRevision, ver_spec.s.nStep);
	printf("uuid:\n  ");
	for (i = 0; i < sizeof(uuid_comp); i++) {
		printf("%02x ", uuid_comp[i]);
		if (i % 8 == 7) {
			printf("\n  ");
		}
	}

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
