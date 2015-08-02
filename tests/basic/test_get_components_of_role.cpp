
#include <cstdio>
#include <cstdlib>
#include <array>

#include <OMX_Core.h>

#include "common/test_omxil.h"
#include "common/omxil_utils.h"

int main(int argc, char *argv[])
{
	const char *arg_role;
	char name_role[OMX_MAX_STRINGNAME_SIZE];
	OMX_U32 num_comps, acc_comps;
	std::array<OMX_U8 *, 128> name_comps = {nullptr, };
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
	for (auto& it : name_comps) {
		it = new OMX_U8[OMX_MAX_STRINGNAME_SIZE];
	}

	num_comps = name_comps.size();
	acc_comps = name_comps.size();
	result = OMX_GetComponentsOfRole(name_role, &acc_comps, &name_comps.front());
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetComponentsOfRole failed.\n");
		goto err_out3;
	}
	num_comps = std::min(num_comps, acc_comps);

	printf("OMX_GetComponentsOfRole: "
		"role:'%s', comps:%d\n", name_role, (int)num_comps);
	for (i = 0; i < num_comps; i++) {
		printf("%2d: comp:'%s'\n", (int)i, name_comps[i]);
	}


	for (auto& it : name_comps) {
		free(it);
	}
	name_comps.fill(nullptr);

	result = OMX_Deinit();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Deinit failed.\n");
		goto err_out1;
	}

	return 0;

err_out3:
	for (auto& it : name_comps) {
		free(it);
	}
	name_comps.fill(nullptr);

//err_out2:
	OMX_Deinit();

err_out1:
	fprintf(stderr, "ErrorCode:0x%08x(%s).\n",
		result, get_omx_errortype_name(result));

	return -1;
}
