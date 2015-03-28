
#ifndef OMX_MF_REGISTER_COMPONENT_HPP__
#define OMX_MF_REGISTER_COMPONENT_HPP__

#include <OMX_Core.h>

#include <omxil_mf/omxil_mf.h>
#include <omxil_mf/component.hpp>

namespace mf {

class register_component {
public:
	//êeÉNÉâÉX
	//typedef xxxx super;

	virtual void init();

	virtual bool is_init();

	/**
	 * register component to this OMX IL library.
	 */
	virtual bool add(const char *name, const OMX_MF_COMPONENT_INFO *info);

protected:
	static void load_components(void);

private:
	register_component();
	virtual ~register_component();

private:
	bool f_init;


public:
	/**
	 * to get the singleton instance of register_component class.
	 */
	static register_component *get_instance(void);

private:
	static void create_instance_once(void);
};

} //namespace mf

#endif //OMX_MF_REGISTER_COMPONENT_HPP__
