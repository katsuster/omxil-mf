#ifndef OMX_MF_UTIL_HPP__
#define OMX_MF_UTIL_HPP__

#include <cstdint>

#include "debug/dbgprint.h"

namespace mf {

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
