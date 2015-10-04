#ifndef OMX_MF_UTIL_HPP__
#define OMX_MF_UTIL_HPP__

#include <cstdint>

namespace mf {

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
