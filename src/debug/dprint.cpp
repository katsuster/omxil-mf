
#include <cstdio>
#include <cstdarg>

#include <omxil_mf/dprint.h>

#if defined(__linux__) && defined(__ANDROID__)
//For Android
#include <utils/Log.h>
#elif defined(__linux__)
//For autoconf
#include "config.h"
#endif

#if defined(_MSC_VER) && defined(_DEBUG)
//For VC++
#define ENABLE_DEBUG
#endif


#ifdef ENABLE_DEBUG
#define DPRINT_LEVEL_DEFAULT    DPRINT_LEVEL_TRACE
#else
#define DPRINT_LEVEL_DEFAULT    DPRINT_LEVEL_INFO
#endif

//Debug print level
static int debug_level = DPRINT_LEVEL_DEFAULT;

int OMX_MF_get_debug_level()
{
	return debug_level;
}

int OMX_MF_set_debug_level(int level)
{
	debug_level = level;

	return debug_level;
}

int OMX_MF_print_cont(int level, const char *fmt, ...)
{
	va_list ap;
	int result;

	if (level > debug_level) {
		return 0;
	}

	va_start(ap, fmt);
	result = vfprintf(stderr, fmt, ap);
	fflush(stderr);
	va_end(ap);

	return result;
}
