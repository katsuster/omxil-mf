
#ifndef OMX_MF_REGISTER_COMPONENT_HPP__
#define OMX_MF_REGISTER_COMPONENT_HPP__

#include <string>
#include <map>
#include <mutex>

#include <OMX_Core.h>

#include <omxil_mf/omxil_mf.h>
#include <omxil_mf/component.hpp>

namespace mf {

struct register_info {
	const OMX_MF_COMPONENT_INFO *info;
};

class register_component {
public:
	//êeÉNÉâÉX
	//typedef xxxx super;

	typedef std::map<std::string, register_info *> map_component_type;
	typedef std::pair<std::string, register_info *> map_component_pair;

	virtual void init();

	virtual bool is_init();

	/**
	 * register component to this OMX IL library.
	 */
	virtual bool insert(const char *name, const OMX_MF_COMPONENT_INFO *info);
	virtual register_info *find(const char *name);
	virtual bool erase(const char *name);
	virtual void clear(const char *name);

protected:
	static void load_components(void);

private:
	register_component();
	virtual ~register_component();

private:
	bool f_init;
	std::recursive_mutex mut_map;
	map_component_type map_comp_name;


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
