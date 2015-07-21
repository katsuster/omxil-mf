
#include <stdio.h>

#include <OMX_Core.h>

#include "common/test_omxil.h"

int main(int argc, char *argv[])
{
	char name_comp[OMX_MAX_STRINGNAME_SIZE];
	OMX_ERRORTYPE result;

	result = OMX_ErrorNone;

	result = OMX_Init();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Init failed.\n");
		goto err_out1;
	}

	result = OMX_Deinit();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Deinit failed.\n");
		goto err_out1;
	}

	return 0;

err_out1:
	return -1;
}
