
#include "entry.hpp"

namespace mf {

component_empty::component_empty(OMX_COMPONENTTYPE *c, OMX_STRING cname) 
	: component(c, cname) {
	//do nothing
}

component_empty::~component_empty() {
	//do nothing
}

} //namespace mf

