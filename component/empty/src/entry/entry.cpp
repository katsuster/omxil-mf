#define __OMX_EXPORTS

#include <cstdio>
#include <string>

#include <omxil_mf/scoped_log.hpp>

#include "entry.h"
#include "reader_zero/reader_zero.hpp"
#include "renderer_null/renderer_null.hpp"
#include "filter_copy/filter_copy.hpp"

#define READER_ZERO_NAME       "OMX.MF.reader.zero"
#define READER_ZERO_A_ALIAS    "OMX.MF.audio.reader.zero"
#define READER_ZERO_V_ALIAS    "OMX.MF.video.reader.zero"
#define READER_ZERO_A_ROLE     "audio_reader.zero"
#define READER_ZERO_V_ROLE     "video_reader.zero"

#define RENDERER_NULL_NAME       "OMX.MF.renderer.null"
#define RENDERER_NULL_A_ALIAS    "OMX.MF.audio.renderer.null"
#define RENDERER_NULL_V_ALIAS    "OMX.MF.video.renderer.null"
#define RENDERER_NULL_A_ROLE     "audio_renderer.null"
#define RENDERER_NULL_V_ROLE     "video_renderer.null"

#define FILTER_COPY_NAME       "OMX.MF.filter.copy"
#define FILTER_COPY_A_ALIAS    "OMX.MF.audio.filter.copy"
#define FILTER_COPY_V_ALIAS    "OMX.MF.video.filter.copy"
#define FILTER_COPY_A_ROLE     "audio_filter.copy"
#define FILTER_COPY_V_ROLE     "video_filter.copy"

extern "C" {

void *OMX_APIENTRY reader_zero_constructor(OMX_COMPONENTTYPE *cComponent, const char *name)
{
	scoped_log_begin;
	std::string strname = name;

	if (strname.compare(READER_ZERO_NAME) != 0 &&
		strname.compare(READER_ZERO_A_ALIAS) != 0 &&
		strname.compare(READER_ZERO_V_ALIAS) != 0) {
		errprint("Error: Wrong component name '%s'.\n",
			strname.c_str());
		return nullptr;
	}

	return new mf::reader_zero(cComponent, name);
}

void OMX_APIENTRY reader_zero_destructor(OMX_COMPONENTTYPE *cComponent)
{
	scoped_log_begin;

	mf::reader_zero *comp = mf::reader_zero::get_instance(cComponent);

	delete comp;
}

void *OMX_APIENTRY renderer_null_constructor(OMX_COMPONENTTYPE *cComponent, const char *name)
{
	scoped_log_begin;
	std::string strname = name;

	if (strname.compare(RENDERER_NULL_NAME) != 0 &&
		strname.compare(RENDERER_NULL_A_ALIAS) != 0 &&
		strname.compare(RENDERER_NULL_V_ALIAS) != 0) {
		errprint("Error: Wrong component name '%s'.\n",
			strname.c_str());
		return nullptr;
	}

	return new mf::renderer_null(cComponent, name);
}

void OMX_APIENTRY renderer_null_destructor(OMX_COMPONENTTYPE *cComponent)
{
	scoped_log_begin;

	mf::renderer_null *comp = mf::renderer_null::get_instance(cComponent);

	delete comp;
}

void *OMX_APIENTRY filter_copy_constructor(OMX_COMPONENTTYPE *cComponent, const char *name)
{
	scoped_log_begin;
	std::string strname = name;

	if (strname.compare(FILTER_COPY_NAME) != 0 &&
		strname.compare(FILTER_COPY_A_ALIAS) != 0 &&
		strname.compare(FILTER_COPY_V_ALIAS) != 0) {
		errprint("Error: Wrong component name '%s'.\n",
			strname.c_str());
		return nullptr;
	}

	return new mf::filter_copy(cComponent, name);
}

void OMX_APIENTRY filter_copy_destructor(OMX_COMPONENTTYPE *cComponent)
{
	scoped_log_begin;

	mf::filter_copy *comp = mf::filter_copy::get_instance(cComponent);

	delete comp;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_MF_LibEntry(void)
{
	scoped_log_begin;
	OMX_MF_COMPONENT_INFO comp_info;
	OMX_ERRORTYPE result, func_result = OMX_ErrorNone;

	//register reader_zero component
	comp_info.constructor = reader_zero_constructor;
	comp_info.destructor = reader_zero_destructor;
	result = OMX_MF_RegisterComponent(READER_ZERO_NAME, &comp_info);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register '%s'.\n",
			READER_ZERO_NAME);
		func_result = result;
	}

	//role
	result = OMX_MF_RegisterComponentRole(READER_ZERO_NAME, READER_ZERO_A_ROLE);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register role '%s' of '%s'.\n",
			READER_ZERO_A_ROLE, READER_ZERO_NAME);
		func_result = result;
	}

	result = OMX_MF_RegisterComponentRole(READER_ZERO_NAME, READER_ZERO_V_ROLE);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register role '%s' of '%s'.\n",
			READER_ZERO_V_ROLE, READER_ZERO_NAME);
		func_result = result;
	}


	//register render_null component
	comp_info.constructor = renderer_null_constructor;
	comp_info.destructor = renderer_null_destructor;
	result = OMX_MF_RegisterComponent(RENDERER_NULL_NAME, &comp_info);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register '%s'.\n",
			RENDERER_NULL_NAME);
		func_result = result;
	}

	//alias
	result = OMX_MF_RegisterComponentAlias(RENDERER_NULL_NAME, RENDERER_NULL_A_ALIAS);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register alias '%s' of '%s'.\n",
			RENDERER_NULL_A_ALIAS, RENDERER_NULL_NAME);
		func_result = result;
	}

	result = OMX_MF_RegisterComponentAlias(RENDERER_NULL_NAME, RENDERER_NULL_V_ALIAS);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register alias '%s' of '%s'.\n",
			RENDERER_NULL_V_ALIAS, RENDERER_NULL_NAME);
		func_result = result;
	}

	//role
	result = OMX_MF_RegisterComponentRole(RENDERER_NULL_NAME, RENDERER_NULL_A_ROLE);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register role '%s' of '%s'.\n",
			RENDERER_NULL_A_ROLE, RENDERER_NULL_NAME);
		func_result = result;
	}

	result = OMX_MF_RegisterComponentRole(RENDERER_NULL_NAME, RENDERER_NULL_V_ROLE);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register role '%s' of '%s'.\n",
			RENDERER_NULL_V_ROLE, RENDERER_NULL_NAME);
		func_result = result;
	}


	//register filter_copy component
	comp_info.constructor = filter_copy_constructor;
	comp_info.destructor = filter_copy_destructor;
	result = OMX_MF_RegisterComponent(FILTER_COPY_NAME, &comp_info);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register '%s'.\n",
			FILTER_COPY_NAME);
		func_result = result;
	}

	//role
	result = OMX_MF_RegisterComponentRole(FILTER_COPY_NAME, FILTER_COPY_A_ROLE);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register role '%s' of '%s'.\n",
			FILTER_COPY_A_ROLE, FILTER_COPY_NAME);
		func_result = result;
	}

	result = OMX_MF_RegisterComponentRole(FILTER_COPY_NAME, FILTER_COPY_V_ROLE);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register role '%s' of '%s'.\n",
			FILTER_COPY_V_ROLE, FILTER_COPY_NAME);
		func_result = result;
	}

	return func_result;
}

} //extern "C"

