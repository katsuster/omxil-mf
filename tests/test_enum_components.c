
#include <stdio.h>

#include <OMX_Core.h>

#include "common/test_omxil.h"

int main(int argc, char *argv[])
{
	char name_comp[OMX_MAX_STRINGNAME_SIZE];
	OMX_ERRORTYPE result;
	OMX_U32 i;
	
	//Reference:
	//    OpenMAX IL 3.2.3.2.3 Sample Code Showing Calling Sequence
	
	result = OMX_ErrorNone;
	
	result = OMX_Init();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Init failed.\n");
		goto err_out1;
	}
	
	for (i = 0; ; i++) {
		result = OMX_ComponentNameEnum(name_comp, 
			sizeof(name_comp) - 1, i);
		if (result == OMX_ErrorNone) {
			//end of list
			break;
		} else {
			fprintf(stderr, "OMX_ComponentNameEnum failed.\n");
			goto err_out2;
		}
		
		printf("component %d: %s\n", (int)i, name_comp);
	}
	
	result = OMX_Deinit();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Deinit failed.\n");
		goto err_out1;
	}
	
	return 0;
	
err_out2:
	OMX_Deinit();
	
err_out1:
	return result;
}
