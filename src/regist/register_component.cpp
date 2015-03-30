
#include <fstream>
#include <mutex>

#include <dlfcn.h>

#include <OMX_Core.h>

#include <omxil_mf/omxil_mf.h>
#include <omxil_mf/component.hpp>

#include "regist/register_component.hpp"
#include "debug/scoped_log.hpp"

namespace mf {

register_component::register_component() : f_init(false)
{
	scoped_log_begin;
	//do nothing
}

register_component::~register_component()
{
	scoped_log_begin;

	clear();
}

void register_component::init()
{
	load_components();

	f_init = true;
}

bool register_component::is_init()
{
	return f_init;
}

bool register_component::insert(const char *name, const OMX_MF_COMPONENT_INFO *info)
{
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lock(mut_map);
	std::string strname = name;
	register_info *reginfo = new register_info();
	std::pair<map_component_type::iterator, bool> pairret;

	reginfo->info = info;
	pairret = map_comp_name.insert(map_component_pair(strname, reginfo));
	if (!pairret.second) {
		//already existed
		errprint("Component '%s' already existed.\n", 
			strname.c_str());
		return false;
	}

	return true;
}

register_info *register_component::find(const char *name)
{
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lock(mut_map);
	std::string strname = name;
	map_component_type::iterator it;

	it = map_comp_name.find(strname);
	if (it == map_comp_name.end()) {
		//not found
		errprint("Component '%s' not found.\n", 
			strname.c_str());
		return nullptr;
	}

	return it->second;
}

bool register_component::erase(const char *name)
{
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lock(mut_map);
	std::string strname = name;
	map_component_type::iterator it;

	it = map_comp_name.find(strname);
	if (it == map_comp_name.end()) {
		//not found
		errprint("Component '%s' not erased.\n", 
			strname.c_str());
		return false;
	}

	delete it->second;
	map_comp_name.erase(it);

	return true;
}

void register_component::clear(const char *name)
{
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lock(mut_map);
	std::string strname = name;
	map_component_type::iterator it;

	for (it = map_comp_name.begin(); it != map_comp_name.end(); it++) {
		delete it->second;
	}

	map_comp_name.clear();
}


//----------------------------------------
//protected methods
//----------------------------------------
void register_component::load_components(void)
{
	scoped_log_begin;
	const char *homedir = getenv("HOME");
	std::string rcfilename = "/.omxilmfrc";

	if (homedir != nullptr) {
		rcfilename.insert(0, homedir);
	}
	dprint("rcfile:%s\n", rcfilename.c_str());

	std::ifstream ifs(rcfilename, std::ifstream::in);

	while (ifs.good()) {
		std::string libname;
		void *libhandle = nullptr;
		OMX_MF_ENTRY_FUNC entry_func = nullptr;
		OMX_ERRORTYPE libresult;
		int result;

		std::getline(ifs, libname);
		if (libname.compare("") == 0) {
			//ignore empty line
			continue;
		}

		//load component
		libhandle = dlopen(libname.c_str(), RTLD_LAZY);
		if (libhandle == nullptr) {
			//not found or error
			errprint("Library '%s' is not found. "
					"Skipped.\n", 
				libname.c_str());
			continue;
		}

		entry_func = (OMX_MF_ENTRY_FUNC)
			dlsym(libhandle, OMX_MF_ENTRY_FUNCNAME);
		if (entry_func == nullptr) {
			//not have entry func
			errprint("Library '%s' does not have entry '%s'. "
					"Skipped.\n", 
				libname.c_str(), OMX_MF_ENTRY_FUNCNAME);
			goto err_load;
		}

		//register component
		libresult = entry_func();
		if (libresult != OMX_ErrorNone) {
			//failed to regist
			errprint("Library '%s' entry '%s' was failed. "
					"Skipped.\n", 
				libname.c_str(), OMX_MF_ENTRY_FUNCNAME);
			goto err_load;
		}

err_load:
		//lazy close
		result = dlclose(libhandle);
		if (result != 0) {
			errprint("Library '%s' cannot close. "
					"Ignored.\n", 
				libname.c_str());
		}
	}
}


//----------------------------------------
//static public methods
//----------------------------------------

static std::once_flag once_instance;
static register_component *g_reg_comp = nullptr;

register_component *register_component::get_instance(void)
{
	scoped_log_begin;

	std::call_once(once_instance, create_instance_once);

	return g_reg_comp;
}

//----------------------------------------
//static private methods
//----------------------------------------
void register_component::create_instance_once(void)
{
	scoped_log_begin;

	g_reg_comp = new register_component();
}

} //namespace mf

