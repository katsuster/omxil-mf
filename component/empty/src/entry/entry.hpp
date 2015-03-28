
#ifndef MF_COMPONENT_EMPTY__ENTRY_HPP__
#define MF_COMPONENT_EMPTY__ENTRY_HPP__

#include <omxil_mf/omxil_mf.h>
#include <omxil_mf/component.hpp>

namespace mf {

class component_empty : public component {
public:
	//親クラス
	typedef component super;

	component_empty(OMX_COMPONENTTYPE *c, OMX_STRING cname);
	virtual ~component_empty();
};

} //namespace mf

#endif //MF_COMPONENT_EMPTY__ENTRY_HPP__
