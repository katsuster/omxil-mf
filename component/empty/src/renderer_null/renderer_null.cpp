
#include "renderer_null/renderer_null.hpp"

namespace mf {

renderer_null::renderer_null(OMX_COMPONENTTYPE *c, OMX_STRING cname) 
	: component(c, cname) {
	//do nothing
}

renderer_null::~renderer_null() {
	//do nothing
}

} //namespace mf

