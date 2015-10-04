
#include <algorithm>
#include <fstream>
#include <mutex>

#include <OMX_Core.h>

#include <omxil_mf/omxil_mf.h>
#include <omxil_mf/component.hpp>
#include <omxil_mf/scoped_log.hpp>

#include "regist/register_component.hpp"
#include "util/util.hpp"

namespace mf {

register_component::register_component() : f_init(false)
{
	scoped_log_begin;

	//do nothing
}

register_component::~register_component()
{
	scoped_log_begin;

	deinit();
}

void register_component::init()
{
	std::lock_guard<std::recursive_mutex> lock(mut_map);

	load_components();

	f_init = true;
}

void register_component::deinit()
{
	std::lock_guard<std::recursive_mutex> lock(mut_map);

	clear();
	unload_components();

	f_init = false;
}

bool register_component::is_init()
{
	return f_init;
}

std::size_t register_component::size() const
{
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lock(mut_map);

	return map_comp_name.size();
}

bool register_component::insert(const char *name, const char *cano, const OMX_MF_COMPONENT_INFO *info)
{
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lock(mut_map);
	std::string strname = name;
	register_info *rinfo = new register_info();
	std::pair<map_component_type::iterator, bool> pairret;

	rinfo->name = strname;
	rinfo->cano_name = cano;
	rinfo->comp_info = info;
	pairret = map_comp_name.insert(map_component_type::value_type(strname, rinfo));
	if (!pairret.second) {
		//already existed
		errprint("Component '%s' already existed.\n",
			strname.c_str());
		delete rinfo;
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

register_info *register_component::find_index(int index)
{
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lock(mut_map);
	int i = 0;

	if (i < 0 || (std::size_t)i >= map_comp_name.size()) {
		return nullptr;
	}

	for (auto& elem : map_comp_name) {
		if (i == index) {
			return elem.second;
		}
		i++;
	}

	//not found
	errprint("Component index:%d not found.\n",
		index);

	return nullptr;
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

void register_component::clear()
{
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lock(mut_map);

	for (auto& elem : map_comp_name) {
		delete elem.second->comp_info;
		delete elem.second;
	}

	map_comp_name.clear();
}

bool register_component::insert_role(const char *name, const char *role)
{
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lock(mut_map);
	std::string strname = name;
	std::string strrole = role;
	register_info *reginfo;
	map_role_type::iterator itr;
	std::pair<map_role_type::iterator, bool> pairret;

	//Insert role of component
	auto itc = map_comp_name.find(strname);
	if (itc == map_comp_name.end()) {
		//not found
		errprint("Component '%s' not found. role '%s'.\n",
			strname.c_str(), strrole.c_str());
		return false;
	}

	reginfo = itc->second;
	reginfo->roles.push_back(strrole);

	//Insert component of role
	itr = map_role_name.find(strrole);
	if (itr == map_role_name.end()) {
		pairret = map_role_name.insert(map_role_type::value_type(strrole, std::vector<std::string>()));
		itr = pairret.first;
	}
	itr->second.push_back(strname);

	return true;
}

bool register_component::erase_role(const char *name, const char *role)
{
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lock(mut_map);
	std::string strname = name;
	std::string strrole = role;
	register_info *reginfo;

	//Erase role of component
	auto itrolec = map_comp_name.find(strname);
	if (itrolec == map_comp_name.end()) {
		//not found
		errprint("Role '%s' of Component '%s' not found.\n",
			strrole.c_str(), strname.c_str());
		return false;
	}

	reginfo = itrolec->second;
	auto itroler = std::find(reginfo->roles.begin(), reginfo->roles.end(), strname);
	if (itroler == reginfo->roles.end()) {
		//not found
		errprint("Role '%s' not found.\n",
			strrole.c_str());
		return false;
	}
	reginfo->roles.erase(itroler);

	//Erase component of role
	auto itcompr = map_role_name.find(strrole);
	if (itcompr == map_role_name.end()) {
		errprint("Component '%s' of Role '%s' not found.\n",
			strname.c_str(), strrole.c_str());
		return false;
	}

	std::vector<std::string>& it_vec = itcompr->second;
	auto itcompc = std::find(it_vec.begin(), it_vec.end(), strname);
	if (itcompc == it_vec.end()) {
		errprint("Component '%s' not found.\n",
			strname.c_str());
		return false;
	}
	it_vec.erase(itcompc);

	return true;
}

const std::vector<std::string> *register_component::find_by_role(const char *role)
{
	//Erase component of role
	auto itr = map_role_name.find(role);
	if (itr == map_role_name.end()) {
		errprint("Role '%s' not found.\n",
			role);
		return nullptr;
	}

	return &itr->second;
}

void register_component::dump() const
{
	std::lock_guard<std::recursive_mutex> lock(mut_map);

	for (auto& elem : map_comp_name) {
		const register_info *reginfo = elem.second;
		const OMX_MF_COMPONENT_INFO *comp_info = reginfo->comp_info;
		std::string roles;

		//Enum roles
		for (auto& elem_r : reginfo->roles) {
			roles += elem_r + ", ";
		}
		if (roles.size() == 0) {
			roles = "(empty)";
		} else if (roles.size() > 2) {
			roles = roles.substr(0, roles.size() - 2);
		}

		dprint("name      : %s\n"
			"reginfo  : %p\n"
			"  name     : %s\n"
			"  cano_name: %s\n"
			"  comp_info: %p\n"
			"    version    : 0x%08x\n"
			"    constructor: %p\n"
			"    destructor : %p\n"
			" roles     : %s\n",
			elem.first.c_str(),
			reginfo,
			reginfo->name.c_str(),
			reginfo->cano_name.c_str(),
			comp_info,
			(int)comp_info->version.nVersion,
			comp_info->constructor,
			comp_info->destructor,
			roles.c_str());
	}

	for (auto& elem : map_role_name) {
		std::string comps;

		//Enum components
		for (auto& elem_r : elem.second) {
			comps += elem_r + ", ";
		}
		if (comps.size() == 0) {
			comps = "(empty)";
		} else if (comps.size() > 2) {
			comps = comps.substr(0, comps.size() - 2);
		}

		dprint("role      : %s\n"
			"  comps    : %s\n",
			elem.first.c_str(),
			comps.c_str());
	}
}


//----------------------------------------
//protected methods
//----------------------------------------

void register_component::load_components(void)
{
	scoped_log_begin;
	const char *homedir = getenv("HOME");
	std::string rcfilename = "/.omxilmfrc";
	bool flag_err = false;

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

		//load library, get entry function
		libhandle = open_library(libname.c_str());
		if (libhandle == nullptr) {
			//not found or error
			errprint("Library '%s' is not found. "
					"Skipped.\n",
				libname.c_str());
			continue;
		}

		entry_func = (OMX_MF_ENTRY_FUNC)
			get_symbol(libhandle, OMX_MF_ENTRY_FUNCNAME);
		if (entry_func == nullptr) {
			//not have entry func
			errprint("Library '%s' does not have entry '%s'. "
					"Skipped.\n",
				libname.c_str(), OMX_MF_ENTRY_FUNCNAME);
			flag_err = true;
		}

		//register components (by library)
		libresult = entry_func();
		if (libresult != OMX_ErrorNone) {
			//failed to regist
			errprint("Library '%s' entry '%s' was failed. "
					"Skipped.\n",
				libname.c_str(), OMX_MF_ENTRY_FUNCNAME);
			flag_err = true;
		}

		if (!flag_err) {
			//success, remember it
			map_lib_name.insert(map_library_type::value_type(libname, libhandle));
		} else {
			//failed, clean up
			result = close_library(libhandle);
			if (result != 0) {
				errprint("Library '%s' cannot close. "
						"Ignored.\n",
					libname.c_str());
			}
			flag_err = false;
			continue;
		}
	}
}

void register_component::unload_components(void)
{
	int result;

	for (auto& elem: map_lib_name) {
		result = close_library(elem.second);
		if (result != 0) {
			errprint("Library '%s' cannot close. "
					"Ignored.\n",
				elem.first.c_str());
		}
	}
	map_lib_name.clear();
}

//----------------------------------------
//static public methods
//----------------------------------------

static std::once_flag once_instance;
static register_component *g_reg_comp = nullptr;

register_component *register_component::get_instance(void)
{
	scoped_log_begin;

	//1回目の呼び出し時にシングルトンを生成してポインタを返す。
	//2回目以降の呼び出しでは 1回目で生成したポインタを返す。
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

