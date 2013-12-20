
#ifdef __cplusplus
extern "C" {
#endif

#include <OMX_Core.h>

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_Init(void)
{
	return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_Deinit(void)
{
	return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_ComponentNameEnum(OMX_OUT OMX_STRING cComponentName, OMX_IN OMX_U32 nNameLength, OMX_IN OMX_U32 nIndex)
{
	OMX_U32 num_max = 0;
	
	if (nIndex > num_max) {
		return OMX_ErrorNoMore;
	}
	
	return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_GetHandle(OMX_OUT OMX_HANDLETYPE* pHandle, OMX_IN OMX_STRING cComponentName, OMX_IN OMX_PTR pAppData, OMX_IN OMX_CALLBACKTYPE* pCallBacks)
{
	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_FreeHandle(OMX_IN OMX_HANDLETYPE hComponent)
{
	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}


OMX_API OMX_ERRORTYPE OMX_GetComponentsOfRole(OMX_IN OMX_STRING role, OMX_INOUT OMX_U32 *pNumComps, OMX_INOUT OMX_U8 **compNames)
{
	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_GetRolesOfComponent(OMX_IN OMX_STRING compName, OMX_INOUT OMX_U32 *pNumRoles, OMX_OUT OMX_U8 **roles)
{
	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

#ifdef __cplusplus
};
#endif
