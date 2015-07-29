
#include <cstdio>
#include <cstring>
#include <vector>
#include <future>
#include <chrono>

#include <unistd.h>

#include <OMX_Core.h>
#include <OMX_Component.h>

#include "common/test_omxil.h"
#include "common/omxil_utils.h"
#include "common/omxil_comp.hpp"

class comp_test_empty_fill : public omxil_comp {
public:
	typedef omxil_comp super;

	comp_test_empty_fill(const char *comp_name)
		: omxil_comp(comp_name)
	{
		//do nothing
	}

	virtual ~comp_test_empty_fill()
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

	virtual OMX_ERRORTYPE EmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
	{
		printf("empty %3d: ", cnt_in);
		for (int i = 0; i < 16; i++) {
			printf("%02x ", pBuffer->pBuffer[i]);
		}
		printf("\n");

		cnt_in++;

		return super::EmptyBufferDone(hComponent, pAppData, pBuffer);
	}

	virtual OMX_ERRORTYPE FillBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
	{
		printf("fill  %3d: ", cnt_out);
		for (int i = 0; i < 16; i++) {
			printf("%02x ", pBuffer->pBuffer[i]);
		}
		printf("\n");

		cnt_out++;

		return super::FillBufferDone(hComponent, pAppData, pBuffer);
	}

private:
	int cnt_in, cnt_out;

};

int main(int argc, char *argv[])
{
	const char *arg_comp;
	comp_test_empty_fill *comp;
	OMX_PORT_PARAM_TYPE param_v;
	OMX_PARAM_PORTDEFINITIONTYPE def_in, def_out;
	std::vector<OMX_BUFFERHEADERTYPE *> buf_in;
	std::vector<OMX_BUFFERHEADERTYPE *> buf_out;
	OMX_U32 pnum_in, pnum_out;
	std::future<int> fut_in;
	std::future<int> fut_out;
	std::chrono::milliseconds span(10);
	int ret_in, ret_out;
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
	//    3.2.2.18 OMX_FillThisBuffer

	comp = nullptr;
	result = OMX_ErrorNone;
	pnum_in = 0;
	pnum_out = 0;

	result = OMX_Init();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Init failed.\n");
		goto err_out1;
	}

	comp = new comp_test_empty_fill(arg_comp);
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
	pnum_out = param_v.nStartPortNumber + 1;

	result = comp->get_param_port_definition(pnum_in, &def_in);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "get_port_definition(in) failed.\n");
		goto err_out2;
	}
	printf("IndexParamPortDefinition: in %d -----\n", (int)def_in.nPortIndex);
	dump_param_portdefinitiontype(&def_in);

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

	buf_in.clear();
	for (i = 0; i < def_in.nBufferCountActual; i++) {
		OMX_BUFFERHEADERTYPE *buf;
		OMX_U8 *pb = nullptr;
		buffer_attr *pbattr = nullptr;

		pb = new OMX_U8[def_in.nBufferSize];
		pbattr = new buffer_attr;
		memset(pbattr, 0, sizeof(buffer_attr));

		result = comp->UseBuffer(&buf,
			pnum_in, pbattr, def_in.nBufferSize, pb);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_UseBuffer(in) failed.\n");
			goto err_out2;
		}
		printf("OMX_UseBuffer: in \n");
		dump_bufferheadertype(buf);

		comp->push_back_buffer(pnum_in, buf);
		buf_in.push_back(buf);
	}

	buf_out.clear();
	for (i = 0; i < def_out.nBufferCountActual; i++) {
		OMX_BUFFERHEADERTYPE *buf;
		OMX_U8 *pb = nullptr;
		buffer_attr *pbattr = nullptr;

		pb = new OMX_U8[def_out.nBufferSize];
		pbattr = new buffer_attr;
		memset(pbattr, 0, sizeof(buffer_attr));

		result = comp->UseBuffer(&buf,
			pnum_out, pbattr, def_out.nBufferSize, pb);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_UseBuffer(out) failed.\n");
			goto err_out2;
		}
		printf("OMX_UseBuffer: out \n");
		dump_bufferheadertype(buf);

		comp->push_back_buffer(pnum_out, buf);
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

	//EmptyThisBuffer
	fut_in = std::async(std::launch::async,
		[&] (int maxcnt) -> int {
		for (int i = 0; i < maxcnt; i++) {
			OMX_BUFFERHEADERTYPE *buf;
			OMX_ERRORTYPE result;

			comp->wait_buffer_free(pnum_in);

			buf = comp->use_free_buffer(pnum_in);
			if (buf == nullptr) {
				fprintf(stderr, "find_free_buffer(%d) failed.\n",
					(int)pnum_in);
				return -1;
			}

			buf->pBuffer[0] = (OMX_U8)i;
			buf->pBuffer[9] = (OMX_U8)i;
			buf->nFilledLen = 8;
			result = comp->EmptyThisBuffer(buf);
			if (result != OMX_ErrorNone) {
				fprintf(stderr, "EmptyThisBuffer(%d) failed.\n",
					(int)pnum_in);
				return -1;
			}
		}

		return 0;
	}, 100);

	//FillThisBuffer
	fut_out = std::async(std::launch::async,
		[&] (int maxcnt) -> int {
		for (int i = 0; i < maxcnt; i++) {
			OMX_BUFFERHEADERTYPE *buf;
			OMX_ERRORTYPE result;

			comp->wait_buffer_free(pnum_out);

			buf = comp->use_free_buffer(pnum_out);
			if (buf == nullptr) {
				fprintf(stderr, "find_free_buffer(%d) failed.\n",
					(int)pnum_out);
				return -1;
			}

			buf->pBuffer[0] = (OMX_U8)i + 1;
			buf->pBuffer[1] = (OMX_U8)i + 1;
			buf->pBuffer[9] = (OMX_U8)i + 1;
			buf->nFilledLen = 8;
			result = comp->FillThisBuffer(buf);
			if (result != OMX_ErrorNone) {
				fprintf(stderr, "FillThisBuffer(%d) failed.\n",
					(int)pnum_out);
				return -1;
			}
		}

		return 0;
	}, 100);

	//Get Empty/Fill result
	ret_in = fut_in.get();
	if (ret_in != 0) {
		fprintf(stderr, "EmptyThisBuffer(%d) failed.\n",
			(int)pnum_in);
	}

	ret_out = fut_out.get();
	if (ret_out != 0) {
		fprintf(stderr, "FillThisBuffer(%d) failed.\n",
			(int)pnum_out);
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

	printf("wait for FillDone of all buffers...\n");
	comp->wait_all_buffer_free(pnum_out);
	printf("wait for FillDone of all buffers... Done!\n");

	//Free buffer
	for (auto it = buf_in.begin(); it != buf_in.end(); it++) {
		OMX_U8 *pb = (*it)->pBuffer;
		buffer_attr *pbattr = static_cast<buffer_attr *>((*it)->pAppPrivate);

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
	for (auto it = buf_in.begin(); it != buf_in.end(); it++) {
		OMX_U8 *pb = (*it)->pBuffer;
		buffer_attr *pbattr = static_cast<buffer_attr *>((*it)->pAppPrivate);

		comp->FreeBuffer(pnum_in, *it);

		delete pbattr;
		delete[] pb;
	}

	delete comp;

	OMX_Deinit();

err_out1:
	return -1;
}
