
#define __OMX_EXPORTS

#include <cstdio>
#include <string>

#include <omxil_mf/scoped_log.hpp>

#include "entry.h"
#include "reader_binary/audio_reader_binary.hpp"
#include "reader_binary/video_reader_binary.hpp"
#include "writer_binary/audio_writer_binary.hpp"
#include "writer_binary/video_writer_binary.hpp"

#define AUDIO_READER_BINARY_NAME     "OMX.MF.audio_reader.binary"
#define AUDIO_READER_BINARY_ROLE     "audio_reader.binary"

#define VIDEO_READER_BINARY_NAME     "OMX.MF.video_reader.binary"
#define VIDEO_READER_BINARY_ROLE     "video_reader.binary"

#define AUDIO_WRITER_BINARY_NAME     "OMX.MF.audio_writer.binary"
#define AUDIO_WRITER_BINARY_ROLE     "audio_writer.binary"

#define VIDEO_WRITER_BINARY_NAME     "OMX.MF.video_writer.binary"
#define VIDEO_WRITER_BINARY_ROLE     "video_writer.binary"

extern "C" {

void *OMX_APIENTRY audio_reader_binary_constructor(OMX_COMPONENTTYPE *cComponent, const char *name)
{
	scoped_log_begin;
	std::string strname = name;

	if (strname.compare(AUDIO_READER_BINARY_NAME) != 0) {
		errprint("Error: Wrong component name '%s'.\n",
			strname.c_str());
		return nullptr;
	}

	return new mf::audio_reader_binary(cComponent, name);
}

void OMX_APIENTRY audio_reader_binary_destructor(OMX_COMPONENTTYPE *cComponent)
{
	scoped_log_begin;

	mf::audio_reader_binary *comp = mf::audio_reader_binary::get_instance(cComponent);

	delete comp;
}

void *OMX_APIENTRY video_reader_binary_constructor(OMX_COMPONENTTYPE *cComponent, const char *name)
{
	scoped_log_begin;
	std::string strname = name;

	if (strname.compare(VIDEO_READER_BINARY_NAME) != 0) {
		errprint("Error: Wrong component name '%s'.\n",
			strname.c_str());
		return nullptr;
	}

	return new mf::video_reader_binary(cComponent, name);
}

void OMX_APIENTRY video_reader_binary_destructor(OMX_COMPONENTTYPE *cComponent)
{
	scoped_log_begin;

	mf::video_reader_binary *comp = mf::video_reader_binary::get_instance(cComponent);

	delete comp;
}

void *OMX_APIENTRY audio_writer_binary_constructor(OMX_COMPONENTTYPE *cComponent, const char *name)
{
	scoped_log_begin;
	std::string strname = name;

	if (strname.compare(AUDIO_WRITER_BINARY_NAME) != 0) {
		errprint("Error: Wrong component name '%s'.\n",
			strname.c_str());
		return nullptr;
	}

	return new mf::audio_writer_binary(cComponent, name);
}

void OMX_APIENTRY audio_writer_binary_destructor(OMX_COMPONENTTYPE *cComponent)
{
	scoped_log_begin;

	mf::audio_writer_binary *comp = mf::audio_writer_binary::get_instance(cComponent);

	delete comp;
}

void *OMX_APIENTRY video_writer_binary_constructor(OMX_COMPONENTTYPE *cComponent, const char *name)
{
	scoped_log_begin;
	std::string strname = name;

	if (strname.compare(VIDEO_WRITER_BINARY_NAME) != 0) {
		errprint("Error: Wrong component name '%s'.\n",
			strname.c_str());
		return nullptr;
	}

	return new mf::video_writer_binary(cComponent, name);
}

void OMX_APIENTRY video_writer_binary_destructor(OMX_COMPONENTTYPE *cComponent)
{
	scoped_log_begin;

	mf::video_writer_binary *comp = mf::video_writer_binary::get_instance(cComponent);

	delete comp;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_MF_LibEntry(void)
{
	scoped_log_begin;
	OMX_MF_COMPONENT_INFO comp_info;
	OMX_ERRORTYPE result, func_result = OMX_ErrorNone;

	//register audio_reader_binary component
	comp_info.constructor = audio_reader_binary_constructor;
	comp_info.destructor  = audio_reader_binary_destructor;
	result = OMX_MF_RegisterComponent(AUDIO_READER_BINARY_NAME, &comp_info);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register '%s'.\n",
			AUDIO_READER_BINARY_NAME);
		func_result = result;
	}

	//role
	result = OMX_MF_RegisterComponentRole(AUDIO_READER_BINARY_NAME, AUDIO_READER_BINARY_ROLE);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register role '%s' of '%s'.\n",
			AUDIO_READER_BINARY_ROLE, AUDIO_READER_BINARY_NAME);
		func_result = result;
	}

	//register video_reader_binary component
	comp_info.constructor = video_reader_binary_constructor;
	comp_info.destructor  = video_reader_binary_destructor;
	result = OMX_MF_RegisterComponent(VIDEO_READER_BINARY_NAME, &comp_info);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register '%s'.\n",
			VIDEO_READER_BINARY_NAME);
		func_result = result;
	}

	//role
	result = OMX_MF_RegisterComponentRole(VIDEO_READER_BINARY_NAME, VIDEO_READER_BINARY_ROLE);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register role '%s' of '%s'.\n",
			VIDEO_READER_BINARY_ROLE, VIDEO_READER_BINARY_NAME);
		func_result = result;
	}


	//register audio_writer_binary component
	comp_info.constructor = audio_writer_binary_constructor;
	comp_info.destructor  = audio_writer_binary_destructor;
	result = OMX_MF_RegisterComponent(AUDIO_WRITER_BINARY_NAME, &comp_info);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register '%s'.\n",
			AUDIO_WRITER_BINARY_NAME);
		func_result = result;
	}

	//role
	result = OMX_MF_RegisterComponentRole(AUDIO_WRITER_BINARY_NAME, AUDIO_WRITER_BINARY_ROLE);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register role '%s' of '%s'.\n",
			AUDIO_WRITER_BINARY_ROLE, AUDIO_WRITER_BINARY_NAME);
		func_result = result;
	}

	//register video_writer_binary component
	comp_info.constructor = video_writer_binary_constructor;
	comp_info.destructor  = video_writer_binary_destructor;
	result = OMX_MF_RegisterComponent(VIDEO_WRITER_BINARY_NAME, &comp_info);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register '%s'.\n",
			VIDEO_WRITER_BINARY_NAME);
		func_result = result;
	}

	//role
	result = OMX_MF_RegisterComponentRole(VIDEO_WRITER_BINARY_NAME, VIDEO_WRITER_BINARY_ROLE);
	if (result != OMX_ErrorNone) {
		errprint("Warning: Failed to register role '%s' of '%s'.\n",
			VIDEO_WRITER_BINARY_ROLE, VIDEO_WRITER_BINARY_NAME);
		func_result = result;
	}

	return func_result;
}

} //extern "C"
