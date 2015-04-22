
#include <cstdio>
#include <cstring>

#include <OMX_Core.h>
#include <OMX_Component.h>

#include "common/omxil_comp.hpp"

extern "C" {
#include "common/test_omxil.h"
#include "common/omxil_utils.h"
}

class comp_test_send_cmd : public omxil_comp {
public:
	comp_test_send_cmd(const char *comp_name)
		: omxil_comp(comp_name)
	{
		//do nothing
	}

	virtual ~comp_test_send_cmd()
	{
		//do nothing
	}

	virtual OMX_ERRORTYPE EventHandler(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
	{
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	}

};

int main(int argc, char *argv[])
{
	const char *arg_comp;
	omxil_comp *comp;
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
	//    3.4.1.1 Non-tunneled Initialization

	comp = nullptr;
	result = OMX_ErrorNone;

	result = OMX_Init();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Init failed.\n");
		goto err_out1;
	}

	comp = new comp_test_send_cmd(arg_comp);
	if (comp == nullptr || comp->get_component() == nullptr) {
		fprintf(stderr, "OMX_GetHandle failed.\n");
		goto err_out2;
	}
	printf("OMX_GetHandle: name:%s, comp:%p\n", 
		arg_comp, comp);

	memset(&parm_v, 0, sizeof(parm_v));
	parm_v.nSize = sizeof(parm_v);
	parm_v.nVersion.s.nVersionMajor = 1;
	parm_v.nVersion.s.nVersionMinor = 1;
	parm_v.nVersion.s.nRevision = 0;
	parm_v.nVersion.s.nStep = 0;
	result = comp->GetParameter(OMX_IndexParamVideoInit, 
		&parm_v);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetParameter(IndexParamVideoInit) "
			"failed.\n");
		goto err_out3;
	}
	printf("OMX_GetParameter: video -----\n");
	dump_port_param_type(&parm_v);

	memset(&parm_a, 0, sizeof(parm_a));
	parm_a.nSize = sizeof(parm_a);
	parm_a.nVersion.s.nVersionMajor = 1;
	parm_a.nVersion.s.nVersionMinor = 1;
	parm_a.nVersion.s.nRevision = 0;
	parm_a.nVersion.s.nStep = 0;
	result = comp->GetParameter(OMX_IndexParamAudioInit, 
		&parm_a);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetParameter(IndexParamAudioInit) "
			"failed.\n");
		goto err_out3;
	}
	printf("OMX_GetParameter: audio -----\n");
	dump_port_param_type(&parm_a);


	/*
	OMX_VIDEO_PARAM_AVCTYPE
	result = OMX_SetParameter(comp, OMX_IndexParamVideoAvc, );
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_SetParameter() failed.\n");
		goto err_out3;
	}*/


	result = comp->SendCommand(OMX_CommandStateSet, 
		OMX_StateIdle, 0);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_SendCommand(SteteSet, Idle) failed.\n");
		goto err_out3;
	}

	result = comp->AllocateBuffer(&buf_in, 
		0, 0, 1048576);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_AllocateBuffer(in) failed.\n");
		goto err_out3;
	}
	printf("OMX_AllocateBuffer: \n");
	dump_port_bufferheadertype(buf_in);

	result = comp->AllocateBuffer(&buf_out, 
		1, 0, 1048576);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_AllocateBuffer(out) failed.\n");
		goto err_out3;
	}
	printf("OMX_AllocateBuffer: \n");
	dump_port_bufferheadertype(buf_out);

	delete comp;

	result = OMX_Deinit();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Deinit failed.\n");
		goto err_out1;
	}

	return 0;

err_out3:
	delete comp;

err_out2:
	OMX_Deinit();

err_out1:
	return result;
}
