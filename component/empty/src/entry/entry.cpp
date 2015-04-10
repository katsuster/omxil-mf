
#include <cstdio>
#include <string>

#include "entry.h"
#include "renderer_null/renderer_null.hpp"

#define READER_NULL_NAME       "OMX.MF.reader.null"
#define READER_NULL_A_ALIAS    "OMX.MF.audio.reader.zero"
#define READER_NULL_V_ALIAS    "OMX.MF.video.reader.zero"
#define READER_NULL_A_ROLE     "audio_reader.zero"
#define READER_NULL_V_ROLE     "video_reader.zero"

#define RENDERER_NULL_NAME       "OMX.MF.renderer.null"
#define RENDERER_NULL_A_ALIAS    "OMX.MF.audio.renderer.null"
#define RENDERER_NULL_V_ALIAS    "OMX.MF.video.renderer.null"
#define RENDERER_NULL_A_ROLE     "audio_renderer.null"
#define RENDERER_NULL_V_ROLE     "video_renderer.null"

extern "C" {

void *OMX_APIENTRY renderer_null_constructor(OMX_COMPONENTTYPE *cComponent, const char *name)
{
	printf("%s:%d\n", __func__, __LINE__);

	std::string strname = name;

	if (strname.compare(RENDERER_NULL_NAME) != 0) {
		fprintf(stderr, "Error: Wrong component name '%s'.\n", 
			strname.c_str());
		return nullptr;
	}

	return new mf::renderer_null(cComponent, name);
}

void OMX_APIENTRY renderer_null_destructor(OMX_COMPONENTTYPE *cComponent)
{
	printf("%s:%d\n", __func__, __LINE__);

	mf::renderer_null *comp = mf::renderer_null::get_instance(cComponent);

	delete comp;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_MF_LibEntry(void)
{
	OMX_MF_COMPONENT_INFO comp_info;
	OMX_ERRORTYPE result, func_result = OMX_ErrorNone;

	printf("%s:%d\n", __func__, __LINE__);

	//register reader_zero component


	//register render_null component
	comp_info.constructor = renderer_null_constructor;
	comp_info.destructor = renderer_null_destructor;
	result = OMX_MF_RegisterComponent(RENDERER_NULL_NAME, &comp_info);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "Warning: Failed to register '%s'.\n", 
			RENDERER_NULL_NAME);
		func_result = result; 
	}

	//alias
	result = OMX_MF_RegisterComponentAlias(RENDERER_NULL_NAME, RENDERER_NULL_A_ALIAS);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "Warning: Failed to register alias '%s' of '%s'.\n", 
			RENDERER_NULL_A_ALIAS, RENDERER_NULL_NAME);
		func_result = result; 
	}

	result = OMX_MF_RegisterComponentAlias(RENDERER_NULL_NAME, RENDERER_NULL_V_ALIAS);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "Warning: Failed to register alias '%s' of '%s'.\n", 
			RENDERER_NULL_V_ALIAS, RENDERER_NULL_NAME);
		func_result = result; 
	}

	//role
	result = OMX_MF_RegisterComponentRole(RENDERER_NULL_NAME, RENDERER_NULL_A_ROLE);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "Warning: Failed to register role '%s' of '%s'.\n", 
			RENDERER_NULL_A_ROLE, RENDERER_NULL_NAME);
		func_result = result; 
	}

	result = OMX_MF_RegisterComponentRole(RENDERER_NULL_NAME, RENDERER_NULL_V_ROLE);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "Warning: Failed to register role '%s' of '%s'.\n", 
			RENDERER_NULL_V_ROLE, RENDERER_NULL_NAME);
		func_result = result; 
	}

	return func_result;
}

} //extern "C"

