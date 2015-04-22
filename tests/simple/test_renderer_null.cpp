
#include <stdio.h>
#include <stdlib.h>

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
	char *buffer;
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
	//    x.x.x.x xxyyzz

	comp = nullptr;
	result = OMX_ErrorNone;
	
	result = OMX_Init();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Init failed.\n");
		goto err_out1;
	}
	
	comp = new omxil_comp(arg_comp);

	//if (result != OMX_ErrorNone) {
	//	fprintf(stderr, "OMX_GetHandle failed.\n");
	//	goto err_out2;
	//}

	//
	buffer = (char *)calloc(4096, sizeof(char));
	if (buffer == NULL) {
		fprintf(stderr, "OMX_GetHandle failed.\n");
		goto err_out3;
	}



	//free
	free(buffer);
	delete comp;

	result = OMX_Deinit();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Deinit failed.\n");
		goto err_out1;
	}

	return 0;

//err_out4:
	free(buffer);

err_out3:
	delete comp;

err_out2:
	OMX_Deinit();

err_out1:
	return result;
}
