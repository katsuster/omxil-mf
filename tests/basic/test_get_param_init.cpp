
#include <cstdio>
#include <cstring>
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

class comp_test_get_param_init : public omxil_comp {
public:
	comp_test_get_param_init(const char *comp_name)
		: omxil_comp(comp_name)
	{
		//do nothing
	}

	virtual ~comp_test_get_param_init()
	{
		//do nothing
	}

private:

};

int main(int argc, char *argv[])
{
	const char *arg_comp;
	comp_test_get_param_init *comp;
	OMX_PORT_PARAM_TYPE parm_a, parm_i, parm_v, parm_o;
	OMX_ERRORTYPE result;

	//get arguments
	if (argc < 2) {
		arg_comp = "OMX.st.video_decoder.avc";
	} else {
		arg_comp = argv[1];
	}

	//Reference:
	//    OpenMAX IL specification version 1.2.0
	//    3.2.2.8 OMX_GetParameter
	//    8.2 Mandatory Port Parameters

	comp = nullptr;
	result = OMX_ErrorNone;

	result = OMX_Init();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Init failed.\n");
		goto err_out1;
	}

	comp = new comp_test_get_param_init(arg_comp);
	if (comp == nullptr || comp->get_component() == nullptr) {
		fprintf(stderr, "OMX_GetHandle failed.\n");
		result = OMX_ErrorInsufficientResources;
		goto err_out2;
	}
	printf("OMX_GetHandle: name:%s, comp:%p\n", 
		arg_comp, comp);

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

	memset(&parm_i, 0, sizeof(parm_i));
	parm_i.nSize = sizeof(parm_i);
	parm_i.nVersion.s.nVersionMajor = 1;
	parm_i.nVersion.s.nVersionMinor = 1;
	parm_i.nVersion.s.nRevision = 0;
	parm_i.nVersion.s.nStep = 0;
	result = comp->GetParameter(OMX_IndexParamImageInit, 
		&parm_i);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetParameter(IndexParamImageInit) "
			"failed.\n");
		goto err_out3;
	}
	printf("OMX_GetParameter: image -----\n");
	dump_port_param_type(&parm_i);

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

	memset(&parm_o, 0, sizeof(parm_o));
	parm_o.nSize = sizeof(parm_o);
	parm_o.nVersion.s.nVersionMajor = 1;
	parm_o.nVersion.s.nVersionMinor = 1;
	parm_o.nVersion.s.nRevision = 0;
	parm_o.nVersion.s.nStep = 0;
	result = comp->GetParameter(OMX_IndexParamOtherInit, 
		&parm_o);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetParameter(IndexParamOtherInit) "
			"failed.\n");
		goto err_out3;
	}
	printf("OMX_GetParameter: other -----\n");
	dump_port_param_type(&parm_o);

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
	return -1;
}
