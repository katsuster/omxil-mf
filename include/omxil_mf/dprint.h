
#ifndef OMX_MF_DPRINT_HPP__
#define OMX_MF_DPRINT_HPP__

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

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
#define DPRINT_LEVEL_ERROR     3
#define DPRINT_LEVEL_DEBUG     5

#define gettid()    ((pid_t)syscall(SYS_gettid))

int OMX_MF_set_debug_level(int level);
int OMX_MF_print_cont(int level, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

#define fatalprint_cont(fmt, ...)    OMX_MF_print_cont(DPRINT_LEVEL_FATAL, fmt, ##__VA_ARGS__)
#define errprint_cont(fmt, ...)      OMX_MF_print_cont(DPRINT_LEVEL_ERROR, fmt, ##__VA_ARGS__)
#define dprint_cont(fmt, ...)        OMX_MF_print_cont(DPRINT_LEVEL_DEBUG, fmt, ##__VA_ARGS__)

#ifdef __GNUC__
#define DPRINT_FUNC    __func__
/* #define DPRINT_FUNC    __PRETTY_FUNCTION__ */
#else
#define DPRINT_FUNC    __func__
#endif

#define fatalprint(fmt, ...)    fatalprint_cont("[% 5d]     %s:%d: " fmt, (int)gettid(), DPRINT_FUNC, __LINE__, ##__VA_ARGS__)
#define errprint(fmt, ...)      errprint_cont("[% 5d]     %s:%d: " fmt, (int)gettid(), DPRINT_FUNC, __LINE__, ##__VA_ARGS__)
#define dprint_in(fmt, ...)     dprint_cont("[% 5d] in  %s:%d: " fmt, (int)gettid(), DPRINT_FUNC, __LINE__, ##__VA_ARGS__)
#define dprint_out(fmt, ...)    dprint_cont("[% 5d] out %s:%d: " fmt, (int)gettid(), DPRINT_FUNC, __LINE__, ##__VA_ARGS__)
#define dprint(fmt, ...)        dprint_cont("[% 5d]     %s:%d: " fmt, (int)gettid(), DPRINT_FUNC, __LINE__, ##__VA_ARGS__)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* OMX_MF_DPRINT_H__ */
