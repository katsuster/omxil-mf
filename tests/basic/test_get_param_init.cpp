
#include <cstdio>
#include <cstring>
#include <mutex>
#include <condition_variable>

#include <unistd.h>

#include <OMX_Core.h>
#include <OMX_Component.h>

#include "common/test_omxil.h"
#include "common/omxil_utils.h"
#include "common/omxil_comp.hpp"

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
	OMX_PORT_PARAM_TYPE param_a, param_i, param_v, param_o;
	OMX_AUDIO_PARAM_PORTFORMATTYPE format_a;
	OMX_IMAGE_PARAM_PORTFORMATTYPE format_i;
	OMX_VIDEO_PARAM_PORTFORMATTYPE format_v;
	OMX_OTHER_PARAM_PORTFORMATTYPE format_o;
	OMX_PARAM_PORTDEFINITIONTYPE def;
	OMX_ERRORTYPE result;
	OMX_U32 i, j;

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


	//OMX_PORT_PARAM_TYPE
	result = comp->get_param_audio_init(&param_a);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "get_audio_init() failed.\n");
		goto err_out3;
	}
	printf("IndexParamAudioInit: -----\n");
	dump_port_param_type(&param_a);

	result = comp->get_param_image_init(&param_i);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "get_image_init() failed.\n");
		goto err_out3;
	}
	printf("IndexParamImageInit: -----\n");
	dump_port_param_type(&param_i);

	result = comp->get_param_video_init(&param_v);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "get_video_init() failed.\n");
		goto err_out3;
	}
	printf("IndexParamVideoInit: -----\n");
	dump_port_param_type(&param_v);

	result = comp->get_param_other_init(&param_o);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "get_other_init() failed.\n");
		goto err_out3;
	}
	printf("IndexParamOtherInit: -----\n");
	dump_port_param_type(&param_o);


	//OMX_PARAM_PORTDEFINITIONTYPE
	for (i = param_a.nStartPortNumber; i < param_a.nStartPortNumber + param_a.nPorts; i++) {
		result = comp->get_param_port_definition(i, &def);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "get_port_definition(audio) failed.\n");
			goto err_out3;
		}
		printf("IndexParamPortDefinition: audio %d -----\n", (int)def.nPortIndex);
		dump_param_portdefinitiontype(&def);
	}

	for (i = param_i.nStartPortNumber; i < param_i.nStartPortNumber + param_i.nPorts; i++) {
		result = comp->get_param_port_definition(i, &def);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "get_port_definition(image) failed.\n");
			goto err_out3;
		}
		printf("IndexParamPortDefinition: image %d -----\n", (int)def.nPortIndex);
		dump_param_portdefinitiontype(&def);
	}

	for (i = param_v.nStartPortNumber; i < param_v.nStartPortNumber + param_v.nPorts; i++) {
		result = comp->get_param_port_definition(i, &def);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "get_port_definition(video) failed.\n");
			goto err_out3;
		}
		printf("IndexParamPortDefinition: video %d -----\n", (int)def.nPortIndex);
		dump_param_portdefinitiontype(&def);
	}

	for (i = param_o.nStartPortNumber; i < param_o.nStartPortNumber + param_o.nPorts; i++) {
		result = comp->get_param_port_definition(i, &def);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "get_port_definition(other) failed.\n");
			goto err_out3;
		}
		printf("IndexParamPortDefinition: other %d -----\n", (int)def.nPortIndex);
		dump_param_portdefinitiontype(&def);
	}


	//OMX_xxxxx_PARAM_PORTFORMATTYPE
	for (i = param_a.nStartPortNumber; i < param_a.nStartPortNumber + param_a.nPorts; i++) {
		j = 0;
		do {
			result = comp->get_param_audio_format(i, j, &format_a);
			if (result != OMX_ErrorNone && result != OMX_ErrorNoMore) {
				fprintf(stderr, "get_param_audio_format(port:%d, fmt:%d) failed.\n",
					(int)i, (int)j);
				goto err_out3;
			}
			printf("IndexParamAudioPortFormat: port:%d, fmt:%d -----\n",
				(int)format_a.nPortIndex, (int)format_a.nIndex);
			dump_audio_param_portformattype(&format_a);

			j++;
		} while (result == OMX_ErrorNoMore);
	}

	for (i = param_i.nStartPortNumber; i < param_i.nStartPortNumber + param_i.nPorts; i++) {
		j = 0;
		do {
			result = comp->get_param_image_format(i, j, &format_i);
			if (result != OMX_ErrorNone && result != OMX_ErrorNoMore) {
				fprintf(stderr, "get_param_image_format(port:%d, fmt:%d) failed.\n",
					(int)i, (int)j);
				goto err_out3;
			}
			printf("IndexParamImagePortFormat: port:%d, fmt:%d -----\n",
				(int)format_i.nPortIndex, (int)format_i.nIndex);
			dump_image_param_portformattype(&format_i);

			j++;
		} while (result == OMX_ErrorNoMore);
	}

	for (i = param_v.nStartPortNumber; i < param_v.nStartPortNumber + param_v.nPorts; i++) {
		j = 0;
		do {
			result = comp->get_param_video_format(i, j, &format_v);
			if (result != OMX_ErrorNone && result != OMX_ErrorNoMore) {
				fprintf(stderr, "get_param_video_format(port:%d, fmt:%d) failed.\n",
					(int)i, (int)j);
				goto err_out3;
			}
			printf("IndexParamVideoPortFormat: port:%d, fmt:%d -----\n",
				(int)format_v.nPortIndex, (int)format_v.nIndex);
			dump_video_param_portformattype(&format_v);

			j++;
		} while (result == OMX_ErrorNoMore);
	}

	for (i = param_o.nStartPortNumber; i < param_o.nStartPortNumber + param_o.nPorts; i++) {
		j = 0;
		do {
			result = comp->get_param_other_format(i, j, &format_o);
			if (result != OMX_ErrorNone && result != OMX_ErrorNoMore) {
				fprintf(stderr, "get_param_other_format(port:%d, fmt:%d) failed.\n",
					(int)i, (int)j);
				goto err_out3;
			}
			printf("IndexParamOtherPortFormat: port:%d, fmt:%d -----\n",
				(int)format_o.nPortIndex, (int)format_o.nIndex);
			dump_other_param_portformattype(&format_o);

			j++;
		} while (result == OMX_ErrorNoMore);
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
	fprintf(stderr, "ErrorCode:0x%08x(%s).\n",
		result, get_omx_errortype_name(result));

	return -1;
}
