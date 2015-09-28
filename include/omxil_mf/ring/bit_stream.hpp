#ifndef BIT_STREAM_HPP__
#define BIT_STREAM_HPP__

#include <cstdint>

namespace mf {

template <class RandomIterator>
class bit_stream {
public:
	bit_stream(RandomIterator buffer, size_t offset, size_t length)
		: buf(buffer), off(offset), len(length), pos(0)
	{
	}

	RandomIterator buffer()
	{
		return buf;
	}

	const RandomIterator buffer() const
	{
		return buf;
	}

	int offset() const
	{
		return off;
	}

	int length() const
	{
		return len;
	}

	int position() const
	{
		if (!is_byte_align()) {
			fprintf(stderr, "bit position %d is not byte aligned.\n",
				(int)bit_position());
		}

		return pos >> 3;
	}

	int bit_position() const
	{
		return pos;
	}

	void bit_position(int newpos)
	{
		pos = newpos;
	}

	bool is_byte_align() const
	{
		return (pos & 0x7) == 0;
	}

	void skip(size_t n)
	{
		bit_position(bit_position() + (n << 3));
	}

	void skip_bits(size_t n)
	{
		bit_position(bit_position() + n);
	}

	uint64_t get_bits(size_t n)
	{
		uint64_t result;

		result = get_bits(bit_position(), n);
		bit_position(bit_position() + n);

		return result;
	}

	uint64_t get_bits(size_t st, size_t n)
	{
		size_t epos, remain;
		uint8_t elem;
		int result = 0;

		epos = st >> 3;
		remain = 8 - (st & 0x7);
		elem = buf[off + epos];

		while (n > remain) {
			//Add all remain bits
			n -= remain;
			result |= get_right_bits(remain, elem) << n;
			//Read next
			epos += 1;
			elem = buf[off + epos];
			remain = 8;
		}

		if (n > 0) {
			//Read n bits from current
			result |= get_right_bits(n, elem >> (remain - n));
		}

		return result;
	}

protected:
	uint64_t get_right_bits(size_t n, const uint64_t val)
	{
		size_t s = 64 - n;

		if (n == 0) {
			return 0;
		} else {
			return (val << s) >> s;
		}
	}

private:
	RandomIterator buf;
	//in bytes
	size_t off;
	//in bytes
	size_t len;
	//in bits
	size_t pos;

};

} //namespace mf

#endif //BIT_STREAM_HPP__
