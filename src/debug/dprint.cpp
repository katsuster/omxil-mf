
#include <cstdio>
#include <cstdarg>

#include <omxil_mf/dprint.h>

//For autoconf
#if defined(__linux__)
#include "config.h"
#endif

//For VC++
#if defined(_MSC_VER) && defined(_DEBUG)
#define ENABLE_DEBUG
#endif


#ifdef ENABLE_DEBUG
#define DPRINT_LEVEL_DEFAULT    DPRINT_LEVEL_DEBUG
#else
#define DPRINT_LEVEL_DEFAULT    DPRINT_LEVEL_ERROR
#endif

//Debug print level
static int debug_level = DPRINT_LEVEL_DEFAULT;

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
