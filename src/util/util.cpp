
#include <vector>

#if defined(__linux__)
#include <unistd.h>
#include <dlfcn.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#elif defined(_WINDOWS)
#include <windows.h>
#endif

#include "util/util.hpp"

namespace mf {

int get_process_id()
{
#if defined(__linux__)
	//Linux
	return (int)getpid();
#elif defined(_WINDOWS)
	//Windows
	return (int)GetCurrentProcessId();
#endif
	//Other
	return 0;
}

int get_thread_id()
{
#if defined(__linux__)
	//Linux
	return (int)syscall(SYS_gettid);
#elif defined(_WINDOWS)
	//Windows
	return (int)GetCurrentThreadId();
#endif
	//Other
	return 0;
}

int set_thread_name(const char *name)
{
#if defined(__linux__)
	//Linux
	return prctl(PR_SET_NAME, name);
#endif
	//Other
	return 0;
}

void *open_library(const char *name)
{
#if defined(__linux__)
	//Linux
	return dlopen(name, RTLD_LAZY);
#elif defined(_WINDOWS)
	//Windows
	std::vector<TCHAR> buf(strlen(name) + 1);
	mbstowcs(&buf.front(), name, strlen(name));
	return LoadLibraryEx(&buf.front(), nullptr, 0);
#endif
	//Other, always failed
	return nullptr;
}

void *get_symbol(void *handle, const char *name)
{
#if defined(__linux__)
	//Linux
	return dlsym(handle, name);
#elif defined(_WINDOWS)
	//Windows
	return GetProcAddress((HMODULE)handle, name);
#endif
	//Other, always failed
	return nullptr;
}

int close_library(void *handle)
{
#if defined(__linux__)
	//Linux
	return dlclose(handle);
#elif defined(_WINDOWS)
	//Windows
	BOOL ret = FreeLibrary((HMODULE)handle);
	if (ret) {
		return 0;
	} else {
		return -1;
	}
#endif
	//Other, always failed
	return -1;
}

uint32_t rev32(uint32_t v)
{
#ifdef __arm__
	asm volatile ("rev %0, %1\n\t" : "=r"(v) : "r"(v));
#else
	v = ((v & 0x00ff00ff) <<  8) | ((v & 0xff00ff00) >>  8);
	v = ((v & 0x0000ffff) << 16) | ((v & 0xffff0000) >> 16);
#endif

	return v;
}

uint16_t rev16(uint16_t v)
{
	v = ((v & 0x00ff) <<  8) | ((v & 0xff00) >>  8);

	return v;
}

} //namespace mf
