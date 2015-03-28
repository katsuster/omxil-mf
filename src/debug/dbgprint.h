
#ifndef OMX_MF_DBGPRINT_H__
#define OMX_MF_DBGPRINT_H__

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#define gettid()    ((pid_t)syscall(SYS_gettid))

#define errprint_cont(fmt, ...)    \
	do { \
		fprintf(stderr, fmt, ##__VA_ARGS__); \
		fflush(stderr); \
	} while (0)

#if 1 //#ifdef ENABLE_DEBUG
#define dprint_cont(fmt, ...)    errprint_cont(fmt, ##__VA_ARGS__)
#else
#define dprint_cont(fmt, ...)    no_printf(fmt, ##__VA_ARGS__)
int no_printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
#endif

#ifdef __GNUC__
#define DPRINT_FUNC    __func__
//#define DPRINT_FUNC    __PRETTY_FUNCTION__
#else
#define DPRINT_FUNC    __func__
#endif

#define dprint_in(fmt, ...)     dprint_cont("[% 5d] in  %s:%d: " fmt, (int)gettid(), DPRINT_FUNC, __LINE__, ##__VA_ARGS__)
#define dprint_out(fmt, ...)    dprint_cont("[% 5d] out %s:%d: " fmt, (int)gettid(), DPRINT_FUNC, __LINE__, ##__VA_ARGS__)
#define dprint(fmt, ...)        dprint_cont("[% 5d]     %s:%d: " fmt, (int)gettid(), DPRINT_FUNC, __LINE__, ##__VA_ARGS__)
#define errprint(fmt, ...)    errprint_cont("[% 5d]     %s:%d: " fmt, (int)gettid(), DPRINT_FUNC, __LINE__, ##__VA_ARGS__)

#endif //OMX_MF_DBGPRINT_H__
