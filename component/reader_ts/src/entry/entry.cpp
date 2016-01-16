
#define __OMX_EXPORTS

#include <cstdio>
#include <string>

#include <omxil_mf/scoped_log.hpp>

#include "entry.h"
#include "reader_ts/reader_ts.hpp"

#define READER_TS_NAME       "OMX.MF.reader.ts"
#define READER_TS_A_ALIAS    "OMX.MF.audio.reader.ts"
#define READER_TS_V_ALIAS    "OMX.MF.video.reader.ts"
#define READER_TS_A_ROLE     "audio_reader.ts"
#define READER_TS_V_ROLE     "video_reader.ts"

extern "C" {

void *OMX_APIENTRY reader_ts_constructor(OMX_COMPONENTTYPE *cComponent, const char *name)
{
	scoped_log_begin;
	std::string strname = name;

	if (strname.compare(READER_TS_NAME) != 0 &&
		strname.compare(READER_TS_A_ALIAS) != 0 &&
		strname.compare(READER_TS_V_ALIAS) != 0) {
		errprint("Error: Wrong component name '%s'.\n",
			strname.c_str());
		return nullptr;
	}

	return new mf::reader_ts(cComponent, name);
}

void OMX_APIENTRY reader_ts_destructor(OMX_COMPONENTTYPE *cComponent)
{
	scoped_log_begin;

	mf::reader_ts *comp = mf::reader_ts::get_instance(cComponent);

	delete comp;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_MF_LibEntry(void)
{
	scoped_log_begin;
	OMX_MF_COMPONENT_INFO comp_info;
	OMX_ERRORTYPE result, func_result = OMX_ErrorNone;

	//register reader_ts component
	comp_info.constructor = reader_ts_constructor;
	comp_info.destructor = reader_ts_destructor;
	result = OMX_MF_RegisterComponent(READER_TS_NAME, &comp_info);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register '%s'.\n",
			READER_TS_NAME);
		func_result = result;
	}

	//role
	result = OMX_MF_RegisterComponentRole(READER_TS_NAME, READER_TS_A_ROLE);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register role '%s' of '%s'.\n",
			READER_TS_A_ROLE, READER_TS_NAME);
		func_result = result;
	}

	result = OMX_MF_RegisterComponentRole(READER_TS_NAME, READER_TS_V_ROLE);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register role '%s' of '%s'.\n",
			READER_TS_V_ROLE, READER_TS_NAME);
		func_result = result;
	}

	//alias
	result = OMX_MF_RegisterComponentAlias(READER_TS_NAME, READER_TS_A_ALIAS);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register alias '%s' of '%s'.\n",
			READER_TS_A_ALIAS, READER_TS_NAME);
		func_result = result;
	}

	result = OMX_MF_RegisterComponentAlias(READER_TS_NAME, READER_TS_V_ALIAS);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register alias '%s' of '%s'.\n",
			READER_TS_V_ALIAS, READER_TS_NAME);
		func_result = result;
	}

	return func_result;
}

} //extern "C"

