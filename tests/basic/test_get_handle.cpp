
#include <cstdio>

#include <OMX_Core.h>
#include <OMX_Component.h>

#include "common/omxil_comp.hpp"

extern "C" {
#include "common/test_omxil.h"
}

int main(int argc, char *argv[])
{
	const char *arg_comp;
	omxil_comp *comp;
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

	comp = nullptr;
	result = OMX_ErrorNone;

	result = OMX_Init();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Init failed.\n");
		goto err_out1;
	}

	comp = new omxil_comp(arg_comp);
	if (comp == nullptr || comp->get_component() == nullptr) {
		fprintf(stderr, "OMX_GetHandle failed.\n");
		goto err_out2;
	}
	printf("OMX_GetHandle: name:%s, comp:%p\n", 
		arg_comp, comp);

	result = comp->GetComponentVersion((OMX_STRING)arg_comp, 
		&ver_comp, &ver_spec, &uuid_comp);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetComponentVersion failed.\n");
		goto err_out3;
	}
	printf("OMX_GetComponentVersion: name:%s\n"
		"comp ver:0x%08x(v%d.%d.%d.%d)\n"
		"spec ver:0x%08x(OpenMAX IL %d.%d.%d.%d)\n", 
		arg_comp, 
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
