
#include <cstdio>
#include <cstring>
#include <vector>

#include <unistd.h>

#include <OMX_Core.h>
#include <OMX_Component.h>

#include "common/test_omxil.h"
#include "common/omxil_utils.h"
#include "common/omxil_comp.hpp"

class comp_test_send_cmd : public omxil_comp {
public:
	typedef omxil_comp super;

	comp_test_send_cmd(const char *comp_name)
		: omxil_comp(comp_name)
	{
		//do nothing
	}

	virtual ~comp_test_send_cmd()
	{
		//do nothing
	}

private:

};

int main(int argc, char *argv[])
{
	const char *arg_comp;
	comp_test_send_cmd *comp;
	OMX_PORT_PARAM_TYPE param_v;
	OMX_PARAM_PORTDEFINITIONTYPE def_in, def_out;
	std::vector<OMX_BUFFERHEADERTYPE *> buf_in;
	std::vector<OMX_BUFFERHEADERTYPE *> buf_out;
	OMX_U32 pnum_in, pnum_out;
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
	//    3.2.2.8 OMX_GetParameter
	//    8.2 Mandatory Port Parameters

	comp = nullptr;
	result = OMX_ErrorNone;
	pnum_in = 0;
	pnum_out = 0;

	result = OMX_Init();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Init failed.\n");
		goto err_out1;
	}

	comp = new comp_test_send_cmd(arg_comp);
	if (comp == nullptr || comp->get_component() == nullptr) {
		fprintf(stderr, "OMX_GetHandle failed.\n");
		result = OMX_ErrorInsufficientResources;
		goto err_out2;
	}
	printf("OMX_GetHandle: name:%s, comp:%p\n",
		arg_comp, comp);

	//Get port definition(port number)
	result = comp->get_param_video_init(&param_v);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "get_video_init() failed.\n");
		goto err_out2;
	}
	printf("IndexParamVideoInit: -----\n");
	dump_port_param_type(&param_v);

	pnum_in = param_v.nStartPortNumber;
	pnum_out = param_v.nStartPortNumber + 1;

	//Get port definition(before)
	result = comp->get_param_port_definition(pnum_in, &def_in);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "get_port_definition(before, in:%d) failed.\n",
			(int)pnum_in);
		goto err_out2;
	}
	printf("IndexParamPortDefinition: before in %d -----\n", (int)def_in.nPortIndex);
	dump_param_portdefinitiontype(&def_in);

	result = comp->get_param_port_definition(pnum_out, &def_out);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "get_port_definition(before, out:%d) failed.\n",
			(int)pnum_out);
		goto err_out2;
	}
	printf("IndexParamPortDefinition: before out %d -----\n", (int)def_out.nPortIndex);
	dump_param_portdefinitiontype(&def_out);

	/*
	OMX_VIDEO_PARAM_AVCTYPE
	result = OMX_SetParameter(comp, OMX_IndexParamVideoAvc, );
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_SetParameter() failed.\n");
		goto err_out2;
	}
	*/

	//Set StateIdle
	result = comp->SendCommand(OMX_CommandStateSet, OMX_StateIdle, 0);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_SendCommand(StateSet, Idle) failed.\n");
		goto err_out2;
	}

	//Allocate buffer
	buf_in.clear();
	for (i = 0; i < def_in.nBufferCountActual; i++) {
		OMX_BUFFERHEADERTYPE *buf;

		result = comp->AllocateBuffer(&buf,
			pnum_in, 0, def_in.nBufferSize);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_AllocateBuffer(in:%d) failed.\n",
				(int)pnum_in);
			goto err_out2;
		}
		printf("OMX_AllocateBuffer: in \n");
		dump_bufferheadertype(buf);

		buf_in.push_back(buf);
	}

	buf_out.clear();
	for (i = 0; i < def_out.nBufferCountActual; i++) {
		OMX_BUFFERHEADERTYPE *buf;

		result = comp->AllocateBuffer(&buf,
			pnum_out, 0, def_out.nBufferSize);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_AllocateBuffer(out:%d) failed.\n",
				(int)pnum_out);
			goto err_out2;
		}
		printf("OMX_AllocateBuffer: out \n");
		dump_bufferheadertype(buf);

		buf_out.push_back(buf);
	}

	//Wait for StatusIdle
	printf("wait for StateIdle...\n");
	comp->wait_state_changed(OMX_StateIdle);
	printf("wait for StateIdle... Done!\n");


	//Get port definition(after)
	result = comp->get_param_port_definition(pnum_in, &def_in);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "get_port_definition(after, in:%d) failed.\n",
			(int)pnum_in);
		goto err_out2;
	}
	printf("IndexParamPortDefinition: after in %d -----\n", (int)def_in.nPortIndex);
	dump_param_portdefinitiontype(&def_in);

	result = comp->get_param_port_definition(pnum_out, &def_out);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "get_port_definition(after, out:%d) failed.\n",
			(int)pnum_out);
		goto err_out2;
	}
	printf("IndexParamPortDefinition: after out %d -----\n", (int)def_out.nPortIndex);
	dump_param_portdefinitiontype(&def_out);


	//Set StateLoaded
	result = comp->SendCommand(OMX_CommandStateSet, OMX_StateLoaded, 0);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_SendCommand(StateSet, Loaded) failed.\n");
		goto err_out2;
	}

	//Free buffer
	for (auto it = buf_out.begin(); it != buf_out.end(); it++) {
		result = comp->FreeBuffer(pnum_out, *it);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_FreeBuffer(out:%d) failed.\n",
				(int)pnum_out);
			goto err_out2;
		}
	}
	buf_out.clear();

	for (auto it = buf_in.begin(); it != buf_in.end(); it++) {
		result = comp->FreeBuffer(pnum_in, *it);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_FreeBuffer(in:%d) failed.\n",
				(int)pnum_in);
			goto err_out2;
		}
	}
	buf_in.clear();

	//Wait for StatusLoaded
	printf("wait for StateLoaded...\n");
	comp->wait_state_changed(OMX_StateLoaded);
	printf("wait for StateLoaded... Done!\n");


	//Terminate
	delete comp;

	result = OMX_Deinit();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Deinit failed.\n");
		goto err_out1;
	}

	return 0;

err_out2:
	for (auto it = buf_out.begin(); it != buf_out.end(); it++) {
		comp->FreeBuffer(1, *it);
	}
	for (auto it = buf_in.begin(); it != buf_in.end(); it++) {
		comp->FreeBuffer(0, *it);
	}

	delete comp;

	OMX_Deinit();

err_out1:
	fprintf(stderr, "ErrorCode:0x%08x(%s).\n",
		result, get_omx_errortype_name(result));

	return -1;
}
