
#include <cstdio>
#include <cstdlib>
#include <array>

#include <OMX_Core.h>

#include "common/test_omxil.h"
#include "common/omxil_utils.h"

int main(int argc, char *argv[])
{
	const char *arg_comp;
	char name_comp[OMX_MAX_STRINGNAME_SIZE];
	OMX_U32 num_roles, acc_roles;
	std::array<OMX_U8 *, 128> name_roles = {nullptr, };
	OMX_ERRORTYPE result;
	OMX_U32 i;

	//get arguments
	if (argc < 2) {
		arg_comp = "OMX.st.audio_decoder.mp3";
	} else {
		arg_comp = argv[1];
	}

	result = OMX_ErrorNone;

	result = OMX_Init();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Init failed.\n");
		goto err_out1;
	}

	snprintf(name_comp, sizeof(name_comp), arg_comp);
	for (auto& elem : name_roles) {
		elem = new OMX_U8[OMX_MAX_STRINGNAME_SIZE];
	}

	num_roles = name_roles.size();
	acc_roles = name_roles.size();
	result = OMX_GetRolesOfComponent(name_comp, &acc_roles, &name_roles.front());
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_GetRolesOfComponent failed.\n");
		goto err_out3;
	}
	num_roles = std::min(num_roles, acc_roles);

	printf("OMX_GetRolesOfComponent: "
		"component:'%s', roles:%d\n", name_comp, (int)num_roles);
	for (i = 0; i < num_roles; i++) {
		printf("%2d: role:'%s'\n", (int)i, name_roles[i]);
	}


	for (auto& elem : name_roles) {
		delete[] elem;
	}
	name_roles.fill(nullptr);

	result = OMX_Deinit();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Deinit failed.\n");
		goto err_out1;
	}

	return 0;

err_out3:
	for (auto& elem : name_roles) {
		delete[] elem;
	}
	name_roles.fill(nullptr);

//err_out2:
	OMX_Deinit();

err_out1:
	fprintf(stderr, "ErrorCode:0x%08x(%s).\n",
		result, get_omx_errortype_name(result));

	return -1;
}
