
#include <cstdio>
#include <cstdarg>

#include <omxil_mf/dprint.hpp>

//Debug print level.
static int debug_level = DPRINT_LEVEL_DEBUG;

int OMX_MF_set_debug_level(int level)
{
	debug_level = level;

	return debug_level;
}

int OMX_MF_print_cont(int level, const char *fmt, ...)
{
	va_list ap;
	int result;

	if (level <= debug_level) {
		return 0;
	}

	va_start(ap, fmt);
	result = vfprintf(stderr, fmt, ap);
	fflush(stderr);
	va_end(ap);

	return result;
}
