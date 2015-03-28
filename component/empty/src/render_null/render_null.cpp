
#include "render_null.hpp"

namespace mf {

reader_null::reader_null(OMX_COMPONENTTYPE *c, OMX_STRING cname) 
	: component(c, cname) {
	//do nothing
}

reader_null::~reader_null() {
	//do nothing
}

} //namespace mf

