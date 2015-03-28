
#ifndef MF_COMPONENT_EMPTY__RENDER_NULL_HPP__
#define MF_COMPONENT_EMPTY__RENDER_NULL_HPP__

#include <omxil_mf/omxil_mf.h>
#include <omxil_mf/component.hpp>

namespace mf {

class reader_null : public component {
public:
	//親クラス
	typedef component super;

	reader_null(OMX_COMPONENTTYPE *c, OMX_STRING cname);
	virtual ~reader_null();
};

} //namespace mf

#endif //MF_COMPONENT_EMPTY__RENDER_NULL_HPP__
