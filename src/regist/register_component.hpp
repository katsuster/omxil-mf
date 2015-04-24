
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
	const OMX_MF_COMPONENT_INFO *comp_info;
};

class register_component {
public:
	//親クラス
	//typedef xxxx super;

	typedef std::map<std::string, void *> map_library_type;
	typedef std::pair<std::string, void *> map_library_pair;
	
	typedef std::map<std::string, register_info *> map_component_type;
	typedef std::pair<std::string, register_info *> map_component_pair;

	/**
	 * Initialize this instance.
	 *
	 * We should call this method before register the components
	 * into this OMX IL library.
	 */
	virtual void init();

	/**
	 * De-initialize this instance.
	 *
	 * Release the all resources used by this instance.
	 */
	virtual void deinit();

	/**
	 * Get is this instance initialized.
	 *
	 * @return true if already initialized, false if not initialized.
	 */
	virtual bool is_init();

	/**
	 * Get components in this OMX IL library.
	 *
	 * @return counts of components.
	 */
	virtual std::size_t size() const;

	/**
	 * Register new component to this OMX IL library.
	 *
	 * @param name  Name of new component.
	 * @param info  Settings and parameters of new component.
	 * @return true if successful, false if failed.
	 */
	virtual bool insert(const char *name, const OMX_MF_COMPONENT_INFO *info);

	/**
	 * Find the registered component by name.
	 *
	 * @param name  Name of component.
	 * @return Registration info of component.
	 */
	virtual register_info *find(const char *name);

	/**
	 * Unregister the component from this OMX IL library.
	 *
	 * @param name  Name of component.
	 * @return true if successful, false if failed.
	 */
	virtual bool erase(const char *name);

	/**
	 * Unregister all components from this OMX IL library.
	 */
	virtual void clear();

	/**
	 * Print all components.
	 * (for debug)
	 */
	virtual void dump() const;

protected:
	virtual void load_components(void);
	virtual void unload_components(void);

private:
	register_component();
	virtual ~register_component();

private:
	bool f_init;
	mutable std::recursive_mutex mut_map;
	map_library_type map_lib_name;
	map_component_type map_comp_name;


public:
	/**
	 * Get the singleton instance of this class.
	 *
	 * 1st time          : Create instance, call init() 
	 * and return the pointer of instance.
	 *
	 * 2nd, 3rd, ... time: Return the pointer of instance 
	 * created at 1st time.
	 *
	 * @return Pointer of singleton instance of this class.
	 */
	static register_component *get_instance(void);

private:
	static void create_instance_once(void);
};

} //namespace mf

#endif //OMX_MF_REGISTER_COMPONENT_HPP__
