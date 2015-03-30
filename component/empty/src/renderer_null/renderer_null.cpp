
#include "renderer_null/renderer_null.hpp"

namespace mf {

renderer_null::renderer_null(OMX_COMPONENTTYPE *c, const char *cname) 
	: component(c, cname) {
	//do nothing
}

renderer_null::~renderer_null() {
	//do nothing
}

renderer_null *renderer_null::get_instance(OMX_HANDLETYPE hComponent) {
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	renderer_null *comp = (renderer_null *) omx_comp->pComponentPrivate;

	return comp;
}

} //namespace mf

