
#include <stdio.h>

#include <OMX_Core.h>

#include "common/test_omxil.h"

int main(int argc, char *argv[])
{
	OMX_ERRORTYPE result;
	int i;

	result = OMX_ErrorNone;

	for (i = 0; i < 100; i++) {
		result = OMX_Init();
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_Init i:%d failed.\n", i);
			goto err_out1;
		}

		result = OMX_Deinit();
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_Deinit i:%d failed.\n", i);
			goto err_out1;
		}
	}

	return 0;

err_out1:
	return -1;
}
