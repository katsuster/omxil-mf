
#ifndef MF_COMPONENT_EMPTY_ENTRY_HPP__
#define MF_COMPONENT_EMPTY_ENTRY_HPP__

#include <omxil_mf/component.hpp>

namespace mf {

class component_empty : public component {
public:
	//親クラス
	typedef component super;

	component_empty(OMX_COMPONENTTYPE *c, OMX_STRING cname) 
		: component(c, cname) {
		//do nothing
	}

	virtual ~component_empty() {
		//do nothing
	}
};

} //namespace mf

#endif //MF_COMPONENT_EMPTY_ENTRY_HPP__
