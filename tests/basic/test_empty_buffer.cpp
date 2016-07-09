
#include <cstdio>
#include <cstring>
#include <vector>

#include <unistd.h>

#include <OMX_Core.h>
#include <OMX_Component.h>

#include "common/test_omxil.h"
#include "common/omxil_utils.h"
#include "common/omxil_comp.hpp"

class comp_test_empty_buffer : public omxil_comp {
public:
	typedef omxil_comp super;

	comp_test_empty_buffer(const char *comp_name)
		: omxil_comp(comp_name)
	{
		//do nothing
	}

	virtual ~comp_test_empty_buffer()
	{
		//do nothing
	}

	virtual OMX_ERRORTYPE EventHandler(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
	{
		/* if (1) {
		} else*/ {
			//default handler
			return super::EventHandler(hComponent, pAppData, eEvent,
				nData1, nData2, pEventData);
		}

		return OMX_ErrorNone;
	}

};

int main(int argc, char *argv[])
{
	const char *arg_comp;
	comp_test_empty_buffer *comp;
	OMX_PORT_PARAM_TYPE param_v;
	OMX_PARAM_PORTDEFINITIONTYPE def_in;
	std::vector<OMX_BUFFERHEADERTYPE *> buf_in;
	OMX_U32 pnum_in;
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
	//    3.4.2.1 Non-tunneled Data Flow
	//    3.2.2.17 OMX_EmptyThisBuffer

	comp = nullptr;
	result = OMX_ErrorNone;
	pnum_in = 0;

	result = OMX_Init();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Init failed.\n");
		goto err_out1;
	}

	comp = new comp_test_empty_buffer(arg_comp);
	if (comp == nullptr || comp->get_component() == nullptr) {
		fprintf(stderr, "OMX_GetHandle failed.\n");
		result = OMX_ErrorInsufficientResources;
		goto err_out2;
	}
	printf("OMX_GetHandle: name:%s, comp:%p\n",
		arg_comp, comp);

	//Get port definition
	result = comp->get_param_video_init(&param_v);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "get_video_init() failed.\n");
		goto err_out2;
	}
	printf("IndexParamVideoInit: -----\n");
	dump_port_param_type(&param_v);

	pnum_in = param_v.nStartPortNumber;

	result = comp->get_param_port_definition(pnum_in, &def_in);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "get_port_definition(in) failed.\n");
		goto err_out2;
	}
	printf("IndexParamPortDefinition: in %d -----\n", (int)def_in.nPortIndex);
	dump_param_portdefinitiontype(&def_in);

	//Set StateIdle
	result = comp->SendCommand(OMX_CommandStateSet, OMX_StateIdle, 0);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_SendCommand(StateSet, Idle) failed.\n");
		goto err_out2;
	}

	buf_in.clear();
	for (i = 0; i < def_in.nBufferCountActual; i++) {
		OMX_BUFFERHEADERTYPE *buf;
		OMX_U8 *pb = nullptr;
		buffer_attr *pbattr = nullptr;

		pb = new OMX_U8[def_in.nBufferSize];
		pbattr = new buffer_attr{0, };

		result = comp->UseBuffer(&buf,
			pnum_in, pbattr, def_in.nBufferSize, pb);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_UseBuffer(in) failed.\n");
			goto err_out2;
		}
		printf("OMX_UseBuffer: in \n");
		dump_bufferheadertype(buf);

		comp->register_buffer(pnum_in, buf);
		buf_in.push_back(buf);
	}

	//Wait for StatusIdle
	printf("wait for StateIdle...\n");
	comp->wait_state_changed(OMX_StateIdle);
	printf("wait for StateIdle... Done!\n");


	//Set StateExecuting
	result = comp->SendCommand(OMX_CommandStateSet, OMX_StateExecuting, 0);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_SendCommand(StateSet, Executing) failed.\n");
		goto err_out2;
	}

	//Wait for StatusExecuting
	printf("wait for StateExecuting...\n");
	comp->wait_state_changed(OMX_StateExecuting);
	printf("wait for StateExecuting... Done!\n");


	//EmptyThisBuffer
	for (i = 0; i < 100; i++) {
		OMX_BUFFERHEADERTYPE *buf;

		comp->wait_buffer_free(pnum_in);

		buf = comp->get_free_buffer(pnum_in);
		if (buf == nullptr) {
			fprintf(stderr, "get_free_buffer(%d) failed.\n",
				(int)pnum_in);
			goto err_out2;
		}

		result = comp->EmptyThisBuffer(buf);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "EmptyThisBuffer(%d) failed.\n",
				(int)pnum_in);
			goto err_out2;
		}
	}


	//Set StateIdle
	result = comp->SendCommand(OMX_CommandStateSet, OMX_StateIdle, 0);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_SendCommand(StateSet, Idle) failed.\n");
		goto err_out2;
	}

	//Wait for StatusIdle
	printf("wait for StateIdle...\n");
	comp->wait_state_changed(OMX_StateIdle);
	printf("wait for StateIdle... Done!\n");


	//Set StateLoaded
	result = comp->SendCommand(OMX_CommandStateSet, OMX_StateLoaded, 0);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_SendCommand(StateSet, Loaded) failed.\n");
		goto err_out2;
	}

	printf("wait for EmptyDone of all buffers...\n");
	comp->wait_all_buffer_free(pnum_in);
	printf("wait for EmptyDone of all buffers... Done!\n");

	//Free buffer
	for (auto it = buf_in.begin(); it != buf_in.end(); it++) {
		OMX_U8 *pb = (*it)->pBuffer;
		buffer_attr *pbattr = static_cast<buffer_attr *>((*it)->pAppPrivate);

		comp->unregister_buffer(pnum_in, *it);

		result = comp->FreeBuffer(pnum_in, *it);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_FreeBuffer(%d) failed.\n",
				(int)pnum_in);
			goto err_out2;
		}

		delete pbattr;
		delete[] pb;
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
	for (auto it = buf_in.begin(); it != buf_in.end(); it++) {
		OMX_U8 *pb = (*it)->pBuffer;
		buffer_attr *pbattr = static_cast<buffer_attr *>((*it)->pAppPrivate);

		comp->unregister_buffer(pnum_in, *it);

		comp->FreeBuffer(pnum_in, *it);

		delete pbattr;
		delete[] pb;
	}

	delete comp;

	OMX_Deinit();

err_out1:
	fprintf(stderr, "ErrorCode:0x%08x(%s).\n",
		result, get_omx_errortype_name(result));

	return -1;
}
