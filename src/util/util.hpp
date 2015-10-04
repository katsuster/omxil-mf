#ifndef OMX_MF_UTIL_HPP__
#define OMX_MF_UTIL_HPP__

#include <cstdint>

namespace mf {

/**
 * Get process ID.
 *
 * @return Converted Process ID to int value.
 */
int get_process_id();

/**
 * Get thread ID.
 *
 * @return Converted Thread ID to int value.
 */
int get_thread_id();

/**
 * Set thread name.
 *
 * @param name Name of thread.
 * @return 0 is success, -1 is error.
 */
int set_thread_name(const char *name);

/**
 * Load a dynamic link library.
 *
 * @param name Path of library.
 * @return Handle of loaded library, nullptr is error.
 */
void *open_library(const char *name);

/**
 * Get specified function address of the library.
 *
 * @param handle Handle of the loaded library.
 * @param name   Name of function
 * @return Pointer of the function, nullptr is error.
 */
void *get_symbol(void *handle, const char *name);

/**
 * Unload a dynamic link library.
 *
 * @param handle Handle of the loaded library.
 * @return 0 is success, -1 is error.
 */
int close_library(void *handle);

/**
 * do 4bytes-swap.
 *
 * v      : 0x00010203
 * return : 0x03020100
 *
 * @param v unsigned value
 * @return swapped value
 */
uint32_t rev32(uint32_t v);

/**
 * do 2bytes-swap.
 *
 * v      : 0x0001
 * return : 0x0100
 *
 * @param v unsigned value
 * @return swapped value
 */
uint16_t rev16(uint16_t v);

} //namespace mf

#endif //OMX_MF_UTIL_HPP__
