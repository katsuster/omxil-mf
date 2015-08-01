
#include <cstdio>
#include <cstring>
#include <vector>

#include <unistd.h>

#include <OMX_Core.h>
#include <OMX_Component.h>

#include "common/test_omxil.h"
#include "common/omxil_utils.h"
#include "common/omxil_comp.hpp"

class comp_test_fill_buffer : public omxil_comp {
public:
	typedef omxil_comp super;

	comp_test_fill_buffer(const char *comp_name)
		: omxil_comp(comp_name)
	{
		//do nothing
	}

	virtual ~comp_test_fill_buffer()
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

	virtual OMX_ERRORTYPE FillBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
	{
		//printf("comp_test_fill_buffer::FillBufferDone\n");
		//dump_bufferheadertype(pBuffer);
		
		return super::FillBufferDone(hComponent, pAppData, pBuffer);
	}

};

int main(int argc, char *argv[])
{
	const char *arg_comp;
	comp_test_fill_buffer *comp;
	OMX_PORT_PARAM_TYPE param_v;
	OMX_PARAM_PORTDEFINITIONTYPE def_out;
	std::vector<OMX_BUFFERHEADERTYPE *> buf_out;
	OMX_U32 pnum_out;
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
	//    3.2.2.18 OMX_FillThisBuffer

	comp = nullptr;
	result = OMX_ErrorNone;
	pnum_out = 0;

	result = OMX_Init();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Init failed.\n");
		goto err_out1;
	}

	comp = new comp_test_fill_buffer(arg_comp);
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

	pnum_out = param_v.nStartPortNumber;

	result = comp->get_param_port_definition(pnum_out, &def_out);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "get_port_definition(out) failed.\n");
		goto err_out2;
	}
	printf("IndexParamPortDefinition: out %d -----\n", (int)def_out.nPortIndex);
	dump_param_portdefinitiontype(&def_out);

	//Set StateIdle
	result = comp->SendCommand(OMX_CommandStateSet, OMX_StateIdle, 0);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_SendCommand(StateSet, Idle) failed.\n");
		goto err_out2;
	}

	buf_out.clear();
	for (i = 0; i < def_out.nBufferCountActual; i++) {
		OMX_BUFFERHEADERTYPE *buf;
		OMX_U8 *pb = nullptr;
		buffer_attr *pbattr = nullptr;

		pb = new OMX_U8[def_out.nBufferSize];
		pbattr = new buffer_attr{0, };

		result = comp->UseBuffer(&buf,
			pnum_out, pbattr, def_out.nBufferSize, pb);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_UseBuffer(out) failed.\n");
			goto err_out2;
		}
		printf("OMX_UseBuffer: out \n");
		dump_bufferheadertype(buf);

		comp->register_buffer(pnum_out, buf);
		buf_out.push_back(buf);
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


	//FillThisBuffer
	for (i = 0; i < 100; i++) {
		OMX_BUFFERHEADERTYPE *buf;

		comp->wait_buffer_free(pnum_out);

		buf = comp->get_free_buffer(pnum_out);
		if (buf == nullptr) {
			fprintf(stderr, "get_free_buffer(%d) failed.\n",
				(int)pnum_out);
			goto err_out2;
		}

		result = comp->FillThisBuffer(buf);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "FillThisBuffer(%d) failed.\n",
				(int)pnum_out);
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

	printf("wait for FillDone of all buffers...\n");
	comp->wait_all_buffer_free(pnum_out);
	printf("wait for FillDone of all buffers... Done!\n");

	//Free buffer
	for (auto it = buf_out.begin(); it != buf_out.end(); it++) {
		OMX_U8 *pb = (*it)->pBuffer;
		buffer_attr *pbattr = static_cast<buffer_attr *>((*it)->pAppPrivate);

		result = comp->FreeBuffer(pnum_out, *it);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_FreeBuffer(%d) failed.\n",
				(int)pnum_out);
			goto err_out2;
		}

		delete pbattr;
		delete[] pb;
	}
	buf_out.clear();

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
		OMX_U8 *pb = (*it)->pBuffer;
		buffer_attr *pbattr = static_cast<buffer_attr *>((*it)->pAppPrivate);

		comp->FreeBuffer(pnum_out, *it);

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
