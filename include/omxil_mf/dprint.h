
#ifndef OMX_MF_DPRINT_HPP__
#define OMX_MF_DPRINT_HPP__

#if defined(__linux__)
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#elif defined(_WINDOWS)
#define _WIN32_WINNT    _WIN32_WINNT_WINXP
#include <WinSDKVer.h>
//#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Debug print level.
 *
 * 0:
 * 1: Fatal : see fatalprint()
 * 2:
 * 3: Error : see errprint()
 * 4:
 * 5: Debug : see dprint()
 */
#define DPRINT_LEVEL_FATAL     1
#define DPRINT_LEVEL_ERROR     2
#define DPRINT_LEVEL_WARN      3
#define DPRINT_LEVEL_INFO      4
#define DPRINT_LEVEL_DEBUG     5
#define DPRINT_LEVEL_TRACE     6

#if defined(__linux__)
/* Linux */
#define thread_id()    ((pid_t)syscall(SYS_gettid))
#elif defined(_WINDOWS)
/* Windows */
#define thread_id()    GetCurrentThreadId()
#endif

#if defined(__GNUC__)
/* For GCC */
#define DPRINT_FUNC    __func__
/* #define DPRINT_FUNC    __PRETTY_FUNCTION__ */

int OMX_MF_set_debug_level(int level);
int OMX_MF_print_cont(int level, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
#else
/* Others */
#define DPRINT_FUNC    __func__

int OMX_MF_set_debug_level(int level);
int OMX_MF_print_cont(int level, const char *fmt, ...);
#endif

#define fatalprint_cont(fmt, ...)    OMX_MF_print_cont(DPRINT_LEVEL_FATAL, fmt, ##__VA_ARGS__)
#define errprint_cont(fmt, ...)      OMX_MF_print_cont(DPRINT_LEVEL_ERROR, fmt, ##__VA_ARGS__)
#define warnprint_cont(fmt, ...)     OMX_MF_print_cont(DPRINT_LEVEL_WARN, fmt, ##__VA_ARGS__)
#define infoprint_cont(fmt, ...)     OMX_MF_print_cont(DPRINT_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define dprint_cont(fmt, ...)        OMX_MF_print_cont(DPRINT_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#define traceprint_cont(fmt, ...)    OMX_MF_print_cont(DPRINT_LEVEL_TRACE, fmt, ##__VA_ARGS__)

#define fatalprint(fmt, ...)    fatalprint_cont("[% 5d] %-5s: %s:%d: " fmt, (int)thread_id(), "fatal", DPRINT_FUNC, __LINE__, ##__VA_ARGS__)
#define errprint(fmt, ...)      errprint_cont(  "[% 5d] %-5s: %s:%d: " fmt, (int)thread_id(), "error", DPRINT_FUNC, __LINE__, ##__VA_ARGS__)
#define warnprint(fmt, ...)     warnprint_cont( "[% 5d] %-5s: %s:%d: " fmt, (int)thread_id(), "warn ", DPRINT_FUNC, __LINE__, ##__VA_ARGS__)
#define infoprint(fmt, ...)     infoprint_cont( "[% 5d] %-5s: %s:%d: " fmt, (int)thread_id(), "info ", DPRINT_FUNC, __LINE__, ##__VA_ARGS__)
#define dprint(fmt, ...)        dprint_cont(    "[% 5d] %-5s: %s:%d: " fmt, (int)thread_id(), "dbg  ", DPRINT_FUNC, __LINE__, ##__VA_ARGS__)
#define traceprint(fmt, ...)    traceprint_cont("[% 5d] %-5s: %s:%d: " fmt, (int)thread_id(), "trace", DPRINT_FUNC, __LINE__, ##__VA_ARGS__)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* OMX_MF_DPRINT_H__ */
