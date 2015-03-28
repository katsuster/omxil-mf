
#include <cstdio>

#include "entry.h"

extern "C" {

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_MF_LibEntry(void)
{
	printf("%s:%d\n", __func__, __LINE__);
	return OMX_ErrorNotImplemented;
	//return OMX_ErrorNone;
}

} //extern "C"

