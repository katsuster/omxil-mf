
#ifndef OMXIL_UTILS_H__
#define OMXIL_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <OMX_Core.h>
#include <OMX_Types.h>
#include <OMX_Component.h>

const char *get_omx_statetype_name(OMX_STATETYPE v);
const char *get_omx_dirtype_name(OMX_DIRTYPE v);
const char *get_omx_portdomaintype_name(OMX_PORTDOMAINTYPE v);

void dump_bufferheadertype(const OMX_BUFFERHEADERTYPE *p);
void dump_port_param_type(const OMX_PORT_PARAM_TYPE *p);
void dump_param_portdefinitiontype(const OMX_PARAM_PORTDEFINITIONTYPE *p);
void dump_audio_param_portformattype(const OMX_AUDIO_PARAM_PORTFORMATTYPE *p);
void dump_image_param_portformattype(const OMX_IMAGE_PARAM_PORTFORMATTYPE *p);
void dump_video_param_portformattype(const OMX_VIDEO_PARAM_PORTFORMATTYPE *p);
void dump_other_param_portformattype(const OMX_OTHER_PARAM_PORTFORMATTYPE *p);

#ifdef __cplusplus
} //extern "C"
#endif

#endif //OMXIL_UTILS_H__
