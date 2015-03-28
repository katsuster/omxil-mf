
#include "renderer_null/renderer_null.hpp"

namespace mf {

renderer_null::renderer_null(OMX_COMPONENTTYPE *c, const char *cname) 
	: component(c, cname) {
	//do nothing
}

renderer_null::~renderer_null() {
	//do nothing
}

} //namespace mf

