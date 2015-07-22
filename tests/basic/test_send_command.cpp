
#include <cstdio>
#include <cstring>
#include <vector>
#include <mutex>
#include <condition_variable>

#include <unistd.h>

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
		: omxil_comp(comp_name), event_done(false)
	{
		//do nothing
	}

	virtual ~comp_test_send_cmd()
	{
		//do nothing
	}

	virtual OMX_ERRORTYPE EventHandler(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
	{
		if (eEvent == OMX_EventCmdComplete && 
			nData1 == OMX_CommandStateSet && 
			nData2 == OMX_StateIdle) {
			std::unique_lock<std::mutex> lock(mut_command);

			printf("comp_test_send_cmd::EventHandler idle state.\n");
			event_done = true;
			cond_command.notify_all();
		} else {
			printf("comp_test_send_cmd::EventHandler ignored.\n");
		}

		return OMX_ErrorNone;
	}

	void wait_event()
	{
		std::unique_lock<std::mutex> lock(mut_command);

		cond_command.wait(lock, [&] { return event_done; });
	}

private:
	std::mutex mut_command;
	std::condition_variable cond_command;
	bool event_done;

};

int main(int argc, char *argv[])
{
	const char *arg_comp;
	comp_test_send_cmd *comp;
	OMX_PARAM_PORTDEFINITIONTYPE def_in, def_out;
	std::vector<OMX_BUFFERHEADERTYPE *> buf_in;
	std::vector<OMX_BUFFERHEADERTYPE *> buf_out;
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

	//Get port definition(before)
	memset(&def_in, 0, sizeof(def_in));
	def_in.nSize = sizeof(def_in);
	def_in.nVersion.s.nVersionMajor = 1;
	def_in.nVersion.s.nVersionMinor = 1;
	def_in.nVersion.s.nRevision = 0;
	def_in.nVersion.s.nStep = 0;
	def_in.nPortIndex = 0;
	result = comp->GetParameter(OMX_IndexParamPortDefinition, 
		&def_in);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetParameter(IndexParamPortDefinition) "
			"failed.\n");
		goto err_out2;
	}
	printf("OMX_GetParameter: before in %d -----\n", def_in.nPortIndex);
	dump_port_definitiontype(&def_in);

	memset(&def_out, 0, sizeof(def_out));
	def_out.nSize = sizeof(def_out);
	def_out.nVersion.s.nVersionMajor = 1;
	def_out.nVersion.s.nVersionMinor = 1;
	def_out.nVersion.s.nRevision = 0;
	def_out.nVersion.s.nStep = 0;
	def_out.nPortIndex = 1;
	result = comp->GetParameter(OMX_IndexParamPortDefinition, 
		&def_out);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetParameter(IndexParamPortDefinition) "
			"failed.\n");
		goto err_out2;
	}
	printf("OMX_GetParameter: before out %d -----\n", def_out.nPortIndex);
	dump_port_definitiontype(&def_out);

	/*
	OMX_VIDEO_PARAM_AVCTYPE
	result = OMX_SetParameter(comp, OMX_IndexParamVideoAvc, );
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_SetParameter() failed.\n");
		goto err_out2;
	}
	*/

	result = comp->SendCommand(OMX_CommandStateSet, 
		OMX_StateIdle, 0);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_SendCommand(StateSet, Idle) failed.\n");
		goto err_out2;
	}

	buf_in.clear();
	for (i = 0; i < def_in.nBufferCountActual; i++) {
		OMX_BUFFERHEADERTYPE *buf;

		buf = new OMX_BUFFERHEADERTYPE();
		buf_in.push_back(buf);

		result = comp->AllocateBuffer(&buf, 
			0, 0, def_in.nBufferSize);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_AllocateBuffer(in) failed.\n");
			goto err_out2;
		}
		printf("OMX_AllocateBuffer: in \n");
		dump_port_bufferheadertype(buf);
	}

	buf_out.clear();
	for (i = 0; i < def_out.nBufferCountActual; i++) {
		OMX_BUFFERHEADERTYPE *buf;

		buf = new OMX_BUFFERHEADERTYPE();
		buf_out.push_back(buf);

		result = comp->AllocateBuffer(&buf, 
			1, 0, def_out.nBufferSize);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_AllocateBuffer(out) failed.\n");
			goto err_out2;
		}
		printf("OMX_AllocateBuffer: out \n");
		dump_port_bufferheadertype(buf);
	}

	//Wait for StatusIdle
	printf("wait for StateIdle...\n");
	comp->wait_event();
	printf("wait for StateIdle... Done!\n");

	//Get port definition(after)
	memset(&def_in, 0, sizeof(def_in));
	def_in.nSize = sizeof(def_in);
	def_in.nVersion.s.nVersionMajor = 1;
	def_in.nVersion.s.nVersionMinor = 1;
	def_in.nVersion.s.nRevision = 0;
	def_in.nVersion.s.nStep = 0;
	def_in.nPortIndex = 0;
	result = comp->GetParameter(OMX_IndexParamPortDefinition, 
		&def_in);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetParameter(IndexParamPortDefinition) "
			"failed.\n");
		goto err_out2;
	}
	printf("OMX_GetParameter: after in %d -----\n", def_in.nPortIndex);
	dump_port_definitiontype(&def_in);

	memset(&def_out, 0, sizeof(def_out));
	def_out.nSize = sizeof(def_out);
	def_out.nVersion.s.nVersionMajor = 1;
	def_out.nVersion.s.nVersionMinor = 1;
	def_out.nVersion.s.nRevision = 0;
	def_out.nVersion.s.nStep = 0;
	def_out.nPortIndex = 1;
	result = comp->GetParameter(OMX_IndexParamPortDefinition, 
		&def_out);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetParameter(IndexParamPortDefinition) "
			"failed.\n");
		goto err_out2;
	}
	printf("OMX_GetParameter: after out %d -----\n", def_out.nPortIndex);
	dump_port_definitiontype(&def_out);

	//Terminate
	for (auto it = buf_out.begin(); it != buf_out.end(); it++) {
		delete *it;
	}
	for (auto it = buf_in.begin(); it != buf_in.end(); it++) {
		delete *it;
	}

	delete comp;

	result = OMX_Deinit();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Deinit failed.\n");
		goto err_out1;
	}

	return 0;

err_out2:
	for (auto it = buf_out.begin(); it != buf_out.end(); it++) {
		delete *it;
	}
	for (auto it = buf_in.begin(); it != buf_in.end(); it++) {
		delete *it;
	}

	delete comp;

	OMX_Deinit();

err_out1:
	return -1;
}
