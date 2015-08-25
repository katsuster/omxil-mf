
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

class comp_test_tunnel_setup : public omxil_comp {
public:
	typedef omxil_comp super;

	typedef std::map<OMX_U32, OMX_PARAM_PORTDEFINITIONTYPE *> map_portdef;

	comp_test_tunnel_setup(const char *comp_name)
		: omxil_comp(comp_name)
	{
		OMX_PARAM_PORTDEFINITIONTYPE *def;
		OMX_ERRORTYPE result;

		result = get_param_audio_init(&param_a);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "get_param_audio_init() failed.\n");
		}
		result = get_param_video_init(&param_v);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "get_param_video_init() failed.\n");
		}
		result = get_param_image_init(&param_i);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "get_param_image_init() failed.\n");
		}
		result = get_param_other_init(&param_o);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "get_param_other_init() failed.\n");
		}

		for (OMX_U32 i = param_a.nStartPortNumber; i < param_a.nStartPortNumber + param_a.nPorts; i++) {
			def = new OMX_PARAM_PORTDEFINITIONTYPE{0, };

			result = get_param_port_definition(i, def);
			if (result != OMX_ErrorNone) {
				fprintf(stderr, "get_param_port_definition(audio) failed.\n");
			}
			def_all.insert(map_portdef::value_type(i, def));
		}
		for (OMX_U32 i = param_v.nStartPortNumber; i < param_v.nStartPortNumber + param_v.nPorts; i++) {
			def = new OMX_PARAM_PORTDEFINITIONTYPE{0, };

			result = get_param_port_definition(i, def);
			if (result != OMX_ErrorNone) {
				fprintf(stderr, "get_param_port_definition(video) failed.\n");
			}
			def_all.insert(map_portdef::value_type(i, def));
		}
		for (OMX_U32 i = param_i.nStartPortNumber; i < param_i.nStartPortNumber + param_i.nPorts; i++) {
			def = new OMX_PARAM_PORTDEFINITIONTYPE{0, };

			result = get_param_port_definition(i, def);
			if (result != OMX_ErrorNone) {
				fprintf(stderr, "get_param_port_definition(image) failed.\n");
			}
			def_all.insert(map_portdef::value_type(i, def));
		}
		for (OMX_U32 i = param_o.nStartPortNumber; i < param_o.nStartPortNumber + param_o.nPorts; i++) {
			def = new OMX_PARAM_PORTDEFINITIONTYPE{0, };

			result = get_param_port_definition(i, def);
			if (result != OMX_ErrorNone) {
				fprintf(stderr, "get_param_port_definition(other) failed.\n");
			}
			def_all.insert(map_portdef::value_type(i, def));
		}
	}

	virtual ~comp_test_tunnel_setup()
	{
		for (auto& elem : def_all) {
			delete elem.second;
		}
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

	OMX_PORT_PARAM_TYPE param_a;
	OMX_PORT_PARAM_TYPE param_i;
	OMX_PORT_PARAM_TYPE param_v;
	OMX_PORT_PARAM_TYPE param_o;
	map_portdef def_all;
};

int main(int argc, char *argv[])
{
	OMX_U32 n_comps;
	const char *arg_comp[2];
	comp_test_tunnel_setup *comp[2] = {};
	OMX_ERRORTYPE result;
	OMX_U32 i;

	//get arguments
	if (argc < 3) {
		arg_comp[0] = "OMX.st.video_decoder.avc";
		arg_comp[1] = "OMX.st.video.xvideo_sink";
	} else {
		arg_comp[0] = argv[1];
		arg_comp[1] = argv[2];
	}

	//Reference:
	//    OpenMAX IL specification version 1.2.0
	//    3.4.1.2 Tunneled Initialization

	n_comps = 2;
	result = OMX_ErrorNone;

	result = OMX_Init();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Init failed.\n");
		goto err_out1;
	}

	for (i = 0; i < n_comps; i++) {
		comp[i] = new comp_test_tunnel_setup(arg_comp[i]);
		if (comp[i] == nullptr || comp[i]->get_component() == nullptr) {
			fprintf(stderr, "OMX_GetHandle(%d) failed.\n",
				(int)i);
			result = OMX_ErrorInsufficientResources;
			goto err_out2;
		}
		printf("OMX_GetHandle: i:%d, name:%s, comp:%p\n",
			(int)i, arg_comp[i], comp[i]);
	}

	{
		result = OMX_SetupTunnel(comp[0]->get_component(), 0, comp[1]->get_component(), 0);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_SetupTunnel(comp%d:%d, comp%d:%d) failed.\n",
				(int)0, 0, 1, 0);
			goto err_out2;
		}
	}

	//
	//TODO: Setup tunnel
	//
	result = OMX_ErrorNotImplemented;
	goto err_out2;

	//Set StateIdle
	for (i = 0; i < n_comps; i++) {
		result = comp[i]->SendCommand(OMX_CommandStateSet, OMX_StateIdle, 0);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_SendCommand(i:%d, StateSet, Idle) failed.\n",
				(int)i);
			goto err_out2;
		}
	}

	//Wait for StatusIdle
	for (i = 0; i < n_comps; i++) {
		printf("wait for StateIdle i:%d...\n", (int)i);
		comp[i]->wait_state_changed(OMX_StateIdle);
		printf("wait for StateIdle i:%d... Done!\n", (int)i);
	}


	//Set StateExecuting
	for (i = 0; i < n_comps; i++) {
		result = comp[i]->SendCommand(OMX_CommandStateSet, OMX_StateExecuting, 0);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_SendCommand(i:%d, StateSet, Executing) failed.\n",
				(int)i);
			goto err_out2;
		}
	}

	//Wait for StatusExecuting
	for (i = 0; i < n_comps; i++) {
		printf("wait for StateExecuting i:%d...\n", (int)i);
		comp[i]->wait_state_changed(OMX_StateExecuting);
		printf("wait for StateExecuting i:%d... Done!\n", (int)i);
	}


	//
	//TODO: Process some buffers...
	//


	//Set StateIdle
	for (i = 0; i < n_comps; i++) {
		result = comp[i]->SendCommand(OMX_CommandStateSet, OMX_StateIdle, 0);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_SendCommand(i:%d, StateSet, Idle) failed.\n",
				(int)i);
			goto err_out2;
		}
	}

	//Wait for StatusIdle
	for (i = 0; i < n_comps; i++) {
		printf("wait for StateIdle i:%d...\n", (int)i);
		comp[i]->wait_state_changed(OMX_StateIdle);
		printf("wait for StateIdle i:%d... Done!\n", (int)i);
	}


	//Set StateLoaded
	for (i = 0; i < n_comps; i++) {
		result = comp[i]->SendCommand(OMX_CommandStateSet, OMX_StateLoaded, 0);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_SendCommand(i:%d, StateSet, Loaded) failed.\n",
				(int)i);
			goto err_out2;
		}
	}

	//Wait for StatusLoaded
	for (i = 0; i < n_comps; i++) {
		printf("wait for StateLoaded i:%d...\n", (int)i);
		comp[i]->wait_state_changed(OMX_StateLoaded);
		printf("wait for StateLoaded i:%d... Done!\n", (int)i);
	}


	//Terminate
	for (i = 0; i < n_comps; i++) {
		delete comp[i];
	}

	result = OMX_Deinit();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Deinit failed.\n");
		goto err_out1;
	}

	return 0;

err_out2:
	for (i = 0; i < n_comps; i++) {
		delete comp[i];
	}

	OMX_Deinit();

err_out1:
	fprintf(stderr, "ErrorCode:0x%08x(%s).\n",
		result, get_omx_errortype_name(result));

	return -1;
}
