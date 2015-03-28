
#ifndef MF_OMXIL_MF_H__
#define MF_OMXIL_MF_H__

#include <OMX_Core.h>
#include <OMX_Component.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The entry function of addtional components library.
 *
 * All components must implements this function.
 */
OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_MF_LibEntry(void);

#define OMX_MF_ENTRY_FUNCNAME    "OMX_MF_LibEntry"

typedef OMX_ERRORTYPE OMX_APIENTRY (*OMX_MF_ENTRY_FUNC)(void);
typedef void *OMX_APIENTRY (*OMX_MF_CONSTRUCTOR_FUNC)(OMX_COMPONENTTYPE *comp, const char *name);
typedef void OMX_APIENTRY (*OMX_MF_DESTRUCTOR_FUNC)(OMX_COMPONENTTYPE *comp);


/**
 * API for additional components library.
 */

typedef struct OMX_MF_COMPONENT_INFO_tag {
	OMX_VERSIONTYPE version;
	OMX_MF_CONSTRUCTOR_FUNC constructor;
	OMX_MF_DESTRUCTOR_FUNC destructor;
} OMX_MF_COMPONENT_INFO;

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_MF_RegisterComponent(const char *name, const OMX_MF_COMPONENT_INFO *info);
OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_MF_RegisterComponentAlias(const char *name, const char *alias);
OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_MF_RegisterComponentRole(const char *name, const char *role);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MF_OMXIL_MF_H__ */

