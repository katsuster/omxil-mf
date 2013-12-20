
#ifndef OMXIL_UTILS_H__
#define OMXIL_UTILS_H__

#include <OMX_Core.h>
#include <OMX_Types.h>
#include <OMX_Component.h>

const char *get_omx_dirtype_name(OMX_DIRTYPE v);
const char *get_omx_portdomaintype_name(OMX_PORTDOMAINTYPE v);


void dump_port_param_type(const OMX_PORT_PARAM_TYPE *p);
void dump_port_definitiontype(const OMX_PARAM_PORTDEFINITIONTYPE *p);

#endif //OMXIL_UTILS_H__
