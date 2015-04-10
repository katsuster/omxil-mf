#include "util/util.hpp"

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

/**
 * do 2bytes-swap.
 *
 * v      : 0x0001
 * return : 0x0100
 *
 * @param v unsigned value
 * @return swapped value
 */
uint16_t rev16(uint16_t v)
{
	v = ((v & 0x00ff) <<  8) | ((v & 0xff00) >>  8);

	return v;
}

} //namespace mf
