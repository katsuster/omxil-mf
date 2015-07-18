
#include <stdio.h>
#include <stdlib.h>

#include <OMX_Core.h>

#include "common/test_omxil.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))


int main(int argc, char *argv[])
{
	const char *arg_role;
	char name_role[OMX_MAX_STRINGNAME_SIZE];
	OMX_U32 num_comps;
	OMX_U8 *name_comps[128] = {NULL, };
	OMX_ERRORTYPE result;
	OMX_U32 i;

	//get arguments
	if (argc < 2) {
		arg_role = "video_decoder.avc";
	} else {
		arg_role = argv[1];
	}

	result = OMX_ErrorNone;

	result = OMX_Init();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Init failed.\n");
		goto err_out1;
	}

	snprintf(name_role, sizeof(name_role), arg_role);
	num_comps = ARRAY_SIZE(name_comps);
	for (i = 0; i < ARRAY_SIZE(name_comps); i++) {
		name_comps[i] = (OMX_U8 *)malloc(OMX_MAX_STRINGNAME_SIZE);
		if (name_comps[i] == NULL) {
			goto err_out3;
		}
	}
	result = OMX_GetComponentsOfRole(name_role, &num_comps, name_comps);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetComponentsOfRole failed.\n");
		goto err_out3;
	}

	printf("OMX_GetComponentsOfRole: "
		"role:'%s', comps:%d\n", name_role, num_comps);
	for (i = 0; i < num_comps; i++) {
		printf("%2d: comp:'%s'\n", i, name_comps[i]);
	}


	for (i = 0; i < ARRAY_SIZE(name_comps); i++) {
		free(name_comps[i]);
		name_comps[i] = NULL;
	}

	result = OMX_Deinit();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Deinit failed.\n");
		goto err_out1;
	}

	return 0;

err_out3:
	for (i = 0; i < ARRAY_SIZE(name_comps); i++) {
		free(name_comps[i]);
		name_comps[i] = NULL;
	}

//err_out2:
	OMX_Deinit();

err_out1:
	return result;
}
