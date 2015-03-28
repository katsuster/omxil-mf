
#ifndef MF_OMXIL_MF_H__
#define MF_OMXIL_MF_H__

#define __OMX_EXPORTS
#include <OMX_Core.h>

#ifdef __cplusplus
extern "C" {
#endif

struct OMX_MF_COMPONENT_INFO {
	OMX_VERSIONTYPE version;
};

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_MF_RegisterComponent(OMX_STRING name, struct OMX_MF_COMPONENT_INFO *info);
OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_MF_RegisterComponentAlias(OMX_STRING name, OMX_STRING alias);
OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_MF_RegisterComponentRole(OMX_STRING name, OMX_STRING role);

/**
 * The entry function of addtional components library.
 *
 * All components must implements this function.
 */
OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_MF_LibEntry(void);

#define OMX_MF_ENTRY_FUNCNAME    "OMX_MF_LibEntry"

typedef OMX_ERRORTYPE OMX_APIENTRY (*OMX_MF_ENTRY_FUNC)(void);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MF_OMXIL_MF_H__ */

