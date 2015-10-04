
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

class comp_test_set_port_format : public omxil_comp {
public:
	comp_test_set_port_format(const char *comp_name)
		: omxil_comp(comp_name)
	{
		//do nothing
	}

	virtual ~comp_test_set_port_format()
	{
		//do nothing
	}

private:

};

int main(int argc, char *argv[])
{
	const char *arg_comp;
	comp_test_set_port_format *comp;
	OMX_PORT_PARAM_TYPE param_a, param_i, param_v, param_o;
	OMX_PARAM_PORTDEFINITIONTYPE def;
	OMX_ERRORTYPE result;
	OMX_U32 i;

	//get arguments
	if (argc < 2) {
		arg_comp = "OMX.st.video_decoder.avc";
	} else {
		arg_comp = argv[1];
	}

	//Reference:
	//    OpenMAX IL specification version 1.2.0
	//    3.2.2.9 OMX_SetParameter
	//    4.3.5 OMX_VIDEO_PARAM_PORTFORMATTYPE

	comp = nullptr;
	result = OMX_ErrorNone;

	result = OMX_Init();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Init failed.\n");
		goto err_out1;
	}

	comp = new comp_test_set_port_format(arg_comp);
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
		fprintf(stderr, "get_param_audio_init() failed.\n");
		goto err_out3;
	}
	printf("IndexParamAudioInit: -----\n");
	dump_port_param_type(&param_a);

	result = comp->get_param_video_init(&param_v);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "get_param_video_init() failed.\n");
		goto err_out3;
	}
	printf("IndexParamVideoInit: -----\n");
	dump_port_param_type(&param_v);

	result = comp->get_param_image_init(&param_i);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "get_param_image_init() failed.\n");
		goto err_out3;
	}
	printf("IndexParamImageInit: -----\n");
	dump_port_param_type(&param_i);

	result = comp->get_param_other_init(&param_o);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "get_param_other_init() failed.\n");
		goto err_out3;
	}
	printf("IndexParamOtherInit: -----\n");
	dump_port_param_type(&param_o);


	//OMX_PARAM_PORTDEFINITIONTYPE
	for (i = param_a.nStartPortNumber; i < param_a.nStartPortNumber + param_a.nPorts; i++) {
		result = comp->get_param_port_definition(i, &def);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "get_param_port_definition(audio) failed.\n");
			goto err_out3;
		}
		printf("IndexParamPortDefinition: audio %d -----\n", (int)def.nPortIndex);
		dump_param_portdefinitiontype(&def);
	}

	for (i = param_v.nStartPortNumber; i < param_v.nStartPortNumber + param_v.nPorts; i++) {
		result = comp->get_param_port_definition(i, &def);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "get_param_port_definition(video) failed.\n");
			goto err_out3;
		}
		printf("IndexParamPortDefinition: video %d -----\n", (int)def.nPortIndex);
		dump_param_portdefinitiontype(&def);
	}

	for (i = param_i.nStartPortNumber; i < param_i.nStartPortNumber + param_i.nPorts; i++) {
		result = comp->get_param_port_definition(i, &def);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "get_param_port_definition(image) failed.\n");
			goto err_out3;
		}
		printf("IndexParamPortDefinition: image %d -----\n", (int)def.nPortIndex);
		dump_param_portdefinitiontype(&def);
	}

	for (i = param_o.nStartPortNumber; i < param_o.nStartPortNumber + param_o.nPorts; i++) {
		result = comp->get_param_port_definition(i, &def);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "get_param_port_definition(other) failed.\n");
			goto err_out3;
		}
		printf("IndexParamPortDefinition: other %d -----\n", (int)def.nPortIndex);
		dump_param_portdefinitiontype(&def);
	}


	//GetParam(OMX_xxxxx_PARAM_PORTFORMATTYPE)
	for (i = param_a.nStartPortNumber; i < param_a.nStartPortNumber + param_a.nPorts; i++) {
		std::vector<OMX_AUDIO_PARAM_PORTFORMATTYPE> formats_a;

		result = comp->get_param_audio_format_all(i, &formats_a);
		if (result != OMX_ErrorNone && result != OMX_ErrorNoMore) {
			fprintf(stderr, "get_param_audio_format_all(port:%d) failed.\n",
				(int)i);
			goto err_out3;
		}

		for (auto& elem : formats_a) {
			printf("IndexParamAudioPortFormat: port:%d, fmt:%d -----\n",
				(int)elem.nPortIndex, (int)elem.nIndex);
			dump_audio_param_portformattype(&elem);
		}
	}

	for (i = param_v.nStartPortNumber; i < param_v.nStartPortNumber + param_v.nPorts; i++) {
		std::vector<OMX_VIDEO_PARAM_PORTFORMATTYPE> formats_v;

		result = comp->get_param_video_format_all(i, &formats_v);
		if (result != OMX_ErrorNone && result != OMX_ErrorNoMore) {
			fprintf(stderr, "get_param_video_format_all(port:%d) failed.\n",
				(int)i);
			goto err_out3;
		}

		for (auto& elem : formats_v) {
			printf("IndexParamVideoPortFormat: port:%d, fmt:%d -----\n",
				(int)elem.nPortIndex, (int)elem.nIndex);
			dump_video_param_portformattype(&elem);
		}
	}

	for (i = param_i.nStartPortNumber; i < param_i.nStartPortNumber + param_i.nPorts; i++) {
		std::vector<OMX_IMAGE_PARAM_PORTFORMATTYPE> formats_i;

		result = comp->get_param_image_format_all(i, &formats_i);
		if (result != OMX_ErrorNone && result != OMX_ErrorNoMore) {
			fprintf(stderr, "get_param_image_format_all(port:%d) failed.\n",
				(int)i);
			goto err_out3;
		}

		for (auto& elem : formats_i) {
			printf("IndexParamImagePortFormat: port:%d, fmt:%d -----\n",
				(int)elem.nPortIndex, (int)elem.nIndex);
			dump_image_param_portformattype(&elem);
		}
	}

	for (i = param_o.nStartPortNumber; i < param_o.nStartPortNumber + param_o.nPorts; i++) {
		std::vector<OMX_OTHER_PARAM_PORTFORMATTYPE> formats_o;

		result = comp->get_param_other_format_all(i, &formats_o);
		if (result != OMX_ErrorNone && result != OMX_ErrorNoMore) {
			fprintf(stderr, "get_param_other_format_all(port:%d) failed.\n",
				(int)i);
			goto err_out3;
		}

		for (auto& elem : formats_o) {
			printf("IndexParamOtherPortFormat: port:%d, fmt:%d -----\n",
				(int)elem.nPortIndex, (int)elem.nIndex);
			dump_other_param_portformattype(&elem);
		}
	}


	//SetParam()
	{
		OMX_VIDEO_PARAM_PORTFORMATTYPE fmt_v;
		int i = param_v.nStartPortNumber + 1; //output

		//Normal parameter
		fmt_v.nSize      = sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE);
		omxil_comp::fill_version(&fmt_v.nVersion);
		fmt_v.nPortIndex = i;
		fmt_v.nIndex     = 0;
		fmt_v.eCompressionFormat = OMX_VIDEO_CodingUnused;
		fmt_v.eColorFormat       = OMX_COLOR_Format32bitBGRA8888;
		fmt_v.xFramerate         = 0;
		result = comp->SetParameter(OMX_IndexParamVideoPortFormat, &fmt_v);
		if (result != OMX_ErrorNone && result != OMX_ErrorNoMore) {
			fprintf(stderr, "SetParameter(VideoPortFormat, port:%d) failed.\n",
				(int)i);
			goto err_out3;
		}

		//Invalid parameter
		fmt_v.eCompressionFormat = OMX_VIDEO_CodingUnused;
		fmt_v.eColorFormat       = OMX_COLOR_FormatUnused;
		fmt_v.xFramerate         = 0;
		result = comp->SetParameter(OMX_IndexParamVideoPortFormat, &fmt_v);
		if (result != OMX_ErrorBadParameter) {
			fprintf(stderr, "SetParameter(VideoPortFormat, port:%d) cannot detect invalid parameter.\n",
				(int)i);
			goto err_out3;
		}

		//Unsupported parameter
		fmt_v.eCompressionFormat = OMX_VIDEO_CodingUnused;
		fmt_v.eColorFormat       = OMX_COLOR_FormatMonochrome;
		fmt_v.xFramerate         = 0;
		result = comp->SetParameter(OMX_IndexParamVideoPortFormat, &fmt_v);
		if (result != OMX_ErrorUnsupportedSetting) {
			fprintf(stderr, "SetParameter(VideoPortFormat, port:%d) cannot detect unsupported parameter.\n",
				(int)i);
			goto err_out3;
		}
	}


	//OMX_PARAM_PORTDEFINITIONTYPE(2nd)
	for (i = param_a.nStartPortNumber; i < param_a.nStartPortNumber + param_a.nPorts; i++) {
		result = comp->get_param_port_definition(i, &def);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "get_param_port_definition(audio) failed.\n");
			goto err_out3;
		}
		printf("IndexParamPortDefinition: audio %d -----\n", (int)def.nPortIndex);
		dump_param_portdefinitiontype(&def);
	}

	for (i = param_v.nStartPortNumber; i < param_v.nStartPortNumber + param_v.nPorts; i++) {
		result = comp->get_param_port_definition(i, &def);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "get_param_port_definition(video) failed.\n");
			goto err_out3;
		}
		printf("IndexParamPortDefinition: video %d -----\n", (int)def.nPortIndex);
		dump_param_portdefinitiontype(&def);
	}

	for (i = param_i.nStartPortNumber; i < param_i.nStartPortNumber + param_i.nPorts; i++) {
		result = comp->get_param_port_definition(i, &def);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "get_param_port_definition(image) failed.\n");
			goto err_out3;
		}
		printf("IndexParamPortDefinition: image %d -----\n", (int)def.nPortIndex);
		dump_param_portdefinitiontype(&def);
	}

	for (i = param_o.nStartPortNumber; i < param_o.nStartPortNumber + param_o.nPorts; i++) {
		result = comp->get_param_port_definition(i, &def);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "get_param_port_definition(other) failed.\n");
			goto err_out3;
		}
		printf("IndexParamPortDefinition: other %d -----\n", (int)def.nPortIndex);
		dump_param_portdefinitiontype(&def);
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
