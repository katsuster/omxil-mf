
#include <fstream>

#include <dlfcn.h>

#define __OMX_EXPORTS
#include <OMX_Core.h>

#include <omxil_mf/omxil_mf.h>
#include <omxil_mf/component.hpp>

#include "debug/scoped_log.hpp"

extern "C" {

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_Init(void)
{
	scoped_log_begin;
	const char *homedir = getenv("HOME");
	std::string rcfilename = "/.omxilmfrc";

	if (homedir != nullptr) {
		rcfilename.insert(0, homedir);
	}
	dprint("rcfile:%s\n", rcfilename.c_str());

	std::ifstream ifs(rcfilename, std::ifstream::in);

	while (ifs.good()) {
		std::string libname;
		void *libhandle = nullptr;
		OMX_MF_ENTRY_FUNC entry_func = nullptr;
		OMX_ERRORTYPE libresult;
		int result;

		std::getline(ifs, libname);
		if (libname.compare("") == 0) {
			//ignore empty line
			continue;
		}

		//load component
		libhandle = dlopen(libname.c_str(), RTLD_LAZY);
		if (libhandle == nullptr) {
			//not found or error
			errprint("Library '%s' is not found. "
					"Skipped.\n", 
				libname.c_str());
			continue;
		}

		entry_func = (OMX_MF_ENTRY_FUNC)
			dlsym(libhandle, OMX_MF_ENTRY_FUNCNAME);
		if (entry_func == nullptr) {
			//not have entry func
			errprint("Library '%s' does not have entry '%s'. "
					"Skipped.\n", 
				libname.c_str(), OMX_MF_ENTRY_FUNCNAME);
			goto err_load;
		}

		//register component
		libresult = entry_func();
		if (libresult != OMX_ErrorNone) {
			//failed to regist
			errprint("Library '%s' entry '%s' was failed. "
					"Skipped.\n", 
				libname.c_str(), OMX_MF_ENTRY_FUNCNAME);
			goto err_load;
		}

err_load:
		//lazy close
		result = dlclose(libhandle);
		if (result != 0) {
			errprint("Library '%s' cannot close. "
					"Ignored.\n", 
				libname.c_str());
		}
	}

	return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_Deinit(void)
{
	scoped_log_begin;
	return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_ComponentNameEnum(OMX_OUT OMX_STRING cComponentName, OMX_IN OMX_U32 nNameLength, OMX_IN OMX_U32 nIndex)
{
	scoped_log_begin;
	OMX_U32 num_max = 0;

	if (nIndex > num_max) {
		return OMX_ErrorNoMore;
	}

	return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_GetHandle(OMX_OUT OMX_HANDLETYPE* pHandle, OMX_IN OMX_STRING cComponentName, OMX_IN OMX_PTR pAppData, OMX_IN OMX_CALLBACKTYPE* pCallBacks)
{
	scoped_log_begin;

	if (pHandle == nullptr) {
		return OMX_ErrorInvalidComponent;
	}
	OMX_COMPONENTTYPE *omx_comp = nullptr;

	omx_comp = new OMX_COMPONENTTYPE;
	new mf::component(omx_comp, (OMX_STRING)"test");

	*pHandle = omx_comp;

	return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_FreeHandle(OMX_IN OMX_HANDLETYPE hComponent)
{
	scoped_log_begin;

	if (hComponent == nullptr) {
		return OMX_ErrorInvalidComponent;
	}
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *)hComponent;

	if (omx_comp->pComponentPrivate != nullptr) {
		mf::component *comp = mf::component::get_instance(hComponent);
		comp->ComponentDeInit(hComponent);
	}
	delete(omx_comp);

	return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_SetupTunnel(OMX_IN OMX_HANDLETYPE hOutput, OMX_IN OMX_U32 nPortOutput, OMX_IN OMX_HANDLETYPE hInput, OMX_IN OMX_U32 nPortInput)
{
	scoped_log_begin;

	if (hOutput == nullptr || hInput == nullptr) {
		return OMX_ErrorInvalidComponent;
	}

	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_GetContentPipe(OMX_OUT OMX_HANDLETYPE *hPipe, OMX_IN OMX_STRING szURI)
{
	scoped_log_begin;

	if (hPipe == nullptr) {
		return OMX_ErrorInvalidComponent;
	}

	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_GetComponentsOfRole(OMX_IN OMX_STRING role, OMX_INOUT OMX_U32 *pNumComps, OMX_INOUT OMX_U8 **compNames)
{
	scoped_log_begin;
	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_GetRolesOfComponent(OMX_IN OMX_STRING compName, OMX_INOUT OMX_U32 *pNumRoles, OMX_OUT OMX_U8 **roles)
{
	scoped_log_begin;
	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

} //extern "C"
