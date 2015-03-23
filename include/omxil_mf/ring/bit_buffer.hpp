#ifndef BIT_BUFFER_HPP__
#define BIT_BUFFER_HPP__

#include <algorithm>
#include <stdexcept>
#include <string>
#include <cstdio>
#include <cstdint>
#include <cstddef>

#include "buffer_base.hpp"

#define BUFGAP    (8 * 1 - 1)

namespace mf {

template <class T>
class bit_buffer : public buffer_base<T> {
public:
	//type of this
	typedef bit_buffer<T> this_type;
	//reference to an element
	typedef typename buffer_base<T>::reference reference;
	//const reference to an element
	typedef typename buffer_base<T>::const_reference const_reference;
	//size type(unsigned)
	typedef typename buffer_base<T>::size_type size_type;

	typedef int (* transform_func_t)(T *dest, const T *src, size_type n, size_type *ntrans);

	using buffer_base<T>::no_transform;
	using buffer_base<T>::elems;
	using buffer_base<T>::get_index;
	using buffer_base<T>::read_array;
	using buffer_base<T>::write_array;
	using buffer_base<T>::get_remain;
	using buffer_base<T>::get_space;

	bit_buffer(T *buf, size_type l)
	: buffer_base<T>(buf, l), len(l * sizeof(T) * 8),
	rd(0), wr(0) {
		switch (sizeof(T)) {
		case 1:
			//8bits
			elem_shift = 3;
			break;
		case 2:
			//16bits
			elem_shift = 4;
			break;
		case 4:
			//32bits
			elem_shift = 5;
			break;
		case 8:
			//TODO: 64bits is not supported
			//elem_shift = 6;
			//break;
		case 16:
			//TODO: 128bits is not supported
			//elem_shift = 7;
			//break;
		default:
			std::string msg(__func__);
			msg += "illegal size of T.";
			throw std::runtime_error(msg);
		}
		elem_bits = 1 << elem_shift;
		elem_mask = elem_bits - 1;
	}

	//disable copy constructor
	bit_buffer(const bit_buffer& obj) = delete;

	//disable operator=
	bit_buffer& operator=(const bit_buffer& obj) = delete;

	virtual ~bit_buffer() {
		//do nothing
	}

	//----------------------------------------
	// iterators
	//----------------------------------------

	//begin
	//end
	//rbegin
	//rend

	//----------------------------------------
	// const iterators
	//----------------------------------------

	//cbegin
	//cend
	//crbegin
	//crend

	//----------------------------------------
	// capacity
	//----------------------------------------

	/**
	 * The number of bits in this buffer.
	 */
	size_type size() const {
		return get_remain(rd, wr, len);
	}

	/**
	 * The largest possible bits of this buffer.
	 */
	size_type max_size() const {
		return len - 1 - BUFGAP;
	}

	/**
	 * Is this buffer empty?
	 */
	bool empty() const {
		return size() == 0;
	}

	/**
	 * Is this buffer full?
	 */
	bool full() const {
		return size() == capacity();
	}

	/**
	 * Change the size of this buffer.
	 *
	 * This is not supported.
	 */
	//void resize(size_type new_size) {
		//cannot set
	//}

	/**
	 * The maximum number of bits that can be stored in this buffer.
	 */
	size_type capacity() const {
		return len - 1 - BUFGAP;
	}

	/**
	 * Change the capacity bits of this buffer.
	 *
	 * This is not supported.
	 */
	//void set_capacity(size_type new_cap) {
		//cannot set
	//}

	/**
	 * The maximum number of bits in this buffer without overwriting.
	 */
	size_type reserve() const {
		return get_space(rd, wr, len, BUFGAP);
	}

	//----------------------------------------
	// element access
	//----------------------------------------

	//front
	//back
	//operator[]
	//at

	//----------------------------------------
	// modifiers
	//----------------------------------------

	//assign
	//emplace
	//insert
	//erase
	//emplace_back
	//push_back
	//pop_back
	//emplace_front
	//push_front
	//pop_front
	//clear
	//swap

	//----------------------------------------
	// vendor specific
	//----------------------------------------

	/**
	 * バックバッファの 1要素のサイズを、
	 * 2 のべき乗で表したときの乗数を取得します。
	 *
	 * 1 << get_element_shift() とすることで、
	 * 1要素のサイズを計算できます。
	 *
	 * @return 2 のべき乗で表したときの乗数
	 */
	int get_element_shift() const {
		return elem_shift;
	}

	/**
	 * バックバッファの 1要素のサイズを、
	 * ビット単位で取得します。
	 *
	 * @return バックバッファの 1要素のサイズ（ビット単位）
	 */
	int get_element_bits() const {
		return elem_bits;
	}

	size_type get_read_position() const {
		return rd;
	}

	void set_read_position(size_type new_pos) {
		rd = new_pos % len;
	}

	size_type get_write_position() const {
		return wr;
	}

	void set_write_position(size_type new_pos) {
		wr = new_pos % len;
	}

	/**
	 * ビットを読み飛ばします。
	 *
	 * @param count 読み飛ばす数
	 * @return 読み飛ばした数
	 */
	size_type skip(size_type count) {
		count = std::min(count, get_remain(rd, wr, len));

		rd += count;
		if (rd >= len) {
			rd -= len;
		}

		return count;
	}

	/**
	 * 0 ～ 32ビットまでの任意のビット列を読み出し、
	 * uint32_t 型の LSB 側に詰めた値を取得します。
	 *
	 * ビットバッファに十分なデータがない場合、
	 * runtime_error をスローします。
	 *
	 * @param n       読み出すビット数
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 * @return 読み出したビット列を詰めた値
	 */
	uint32_t get_bits32(size_type n, transform_func_t rdtrans = no_transform, transform_func_t wrtrans = no_transform) {
		uint32_t result;

		if (n > 32) {
			std::string msg(__func__);
			msg += ": n is too large.";
			throw std::runtime_error(msg);
		}
		if (n > size()) {
			std::string msg(__func__);
			msg += ": underflow.";
			throw std::runtime_error(msg);
		}

		result = get_bits32_inner(rd, n, rdtrans, wrtrans);
		rd += n;
		if (rd >= len) {
			rd -= len;
		}

		return result;
	}

	/**
	 * val の LSB 側から 0 ～ 32ビットまでの、
	 * 任意のビット数を書き込みます。
	 *
	 * ビットバッファに十分な空きがない場合、
	 * runtime_error をスローします。
	 *
	 * @param n       書き込むビット数
	 * @param val     書き込むビット列を含んだ値
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 */
	void set_bits32(size_type n, uint32_t val, transform_func_t rdtrans = no_transform, transform_func_t wrtrans = no_transform) {
		if (n > 32) {
			std::string msg(__func__);
			msg += ": n is too large.";
			throw std::runtime_error(msg);
		}
		if (n > reserve()) {
			std::string msg(__func__);
			msg += ": overflow.";
			throw std::runtime_error(msg);
		}

		set_bits32_inner(wr, n, val, rdtrans, wrtrans);
		wr += n;
		if (wr >= len) {
			wr -= len;
		}
	}

	/**
	 * 配列をビットバッファから読み込みます。
	 *
	 * @param buf     ビットバッファから読み込んだ要素を格納する配列
	 * @param count   ビットバッファから読み込むビット数
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 * @return ビットバッファから読み込んだビット数
	 */
	size_type read_array(T *buf, size_type count, transform_func_t rdtrans = no_transform, transform_func_t wrtrans = no_transform) {
		size_type result;

		count = std::min(count, get_remain(rd, wr, len));

		if ((rd & elem_mask) != 0) {
			//not aligned, run slow mode
printf("rd: slow\n");
			result = 0;
			while (count >= (size_type)elem_bits) {
				buf[result >> elem_shift] = get_bits32(elem_bits, rdtrans, wrtrans);
				result += elem_bits;
				count -= elem_bits;
			}
		} else {
			//aligned, run fast mode
			result = read_array(rd >> elem_shift, buf, count >> elem_shift, rdtrans, wrtrans);
			result <<= elem_shift;
			count -= result;

			rd += result;
			if (rd >= len) {
				rd -= len;
			}
		}
		if (count != 0) {
			buf[result >> elem_shift] = get_bits32(count, rdtrans, wrtrans);
			result += count;
		}

		return result;
	}

	/**
	 * 配列をビットバッファに書き込みます。
	 *
	 * @param buf   ビットバッファに書き込む要素の配列
	 * @param count ビットバッファに書き込むビット数
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 * @return ビットバッファに書き込んだビット数
	 */
	size_type write_array(const T *buf, size_type count, transform_func_t rdtrans = no_transform, transform_func_t wrtrans = no_transform) {
		size_type result;

		count = std::min(count, get_space(rd, wr, len, BUFGAP));

		if ((wr & elem_mask) != 0) {
			//not aligned, run slow mode
printf("wr: slow\n");
			result = 0;
			while (count >= (size_type)elem_bits) {
				set_bits32(elem_bits, buf[result >> elem_shift], rdtrans, wrtrans);
				result += elem_bits;
				count -= elem_bits;
			}
		} else {
			//aligned, run fast mode
			result = write_array(wr >> elem_shift, buf, count >> elem_shift, rdtrans, wrtrans);
			result <<= elem_shift;
			count -= result;

			wr += result;
			if (wr >= len) {
				wr -= len;
			}
		}
		if (count != 0) {
			set_bits32(count, buf[result >> elem_shift], rdtrans, wrtrans);
			result += count;
		}

		return result;
	}

	/**
	 * 別のビットバッファからコピーします。
	 *
	 * @param src     コピー元のビットバッファ
	 * @param count   ビットバッファから読み込むビット数
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 * @return ビットバッファに書き込んだビット数
	 */
	size_type copy_array(this_type *src, size_type count, transform_func_t rdtrans = no_transform, transform_func_t wrtrans = no_transform) {
		size_type result;

		count = std::min(count, get_remain_continuous(src->rd, src->wr, src->len));

		result = write_array(&(*src)[0], count >> elem_shift, rdtrans, wrtrans);
		src->skip(result << elem_shift);

		return result << elem_shift;
	}

private:
	void check_position(size_type n) const {
		if (n >= size()) {
			throw std::out_of_range("bit_buffer");
		}
	}

	/**
	 * 指定された位置から 0 ～ 32ビットまでの任意のビット列を読み出し、
	 * uint32_t 型の LSB 側に詰めた値を取得します。
	 *
	 * 内部呼び出し用です。容量のチェックなどは行いません。
	 *
	 * @param index   読み出し開始位置（ビット）
	 * @param n       読み出すビット数
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 * @return 読み出したビット列を詰めた値
	 */
	uint32_t get_bits32_inner(size_type index, size_type n, transform_func_t rdtrans, transform_func_t wrtrans) const {
		T elem;
		size_type pos, remain;
		uint32_t result = 0;

		pos = get_element_index(index);
		remain = elem_bits - get_element_bitpos(index);
		read_array(pos, &elem, 1, rdtrans, wrtrans);

		while (n > remain) {
			//get all bits in the element
			n -= remain;
			index += remain;
			result |= get_right_bits32(remain, elem) << n;
			//read next element
			pos = get_index(pos + 1);
			read_array(pos, &elem, 1, rdtrans, wrtrans);
			remain = elem_bits;
		}

		if (n > 0) {
			//read n bits
			result |= get_right_bits32(n, (uint32_t)(elem) >> (remain - n));
		}

		return result;
	}

	/**
	 * 指定された位置に val の LSB 側から 0 ～ 32ビットまでの、
	 * 任意のビット数を書き込みます。
	 *
	 * 内部呼び出し用です。容量のチェックなどは行いません。
	 *
	 * @param index   書き込み開始位置（ビット）
	 * @param n       書き込むビット数
	 * @param val     書き込むビット列を含んだ値
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 */
	void set_bits32_inner(size_type index, size_type n, uint32_t val, transform_func_t rdtrans, transform_func_t wrtrans) {
		T elem;
		size_type pos, remain;

		pos = get_element_index(index);
		remain = elem_bits - get_element_bitpos(index);
		read_array(pos, &elem, 1, rdtrans, wrtrans);

		while (n > remain) {
			//put all bits in the space of element
			n -= remain;
			elem &= ~get_mask_bits32(remain);
			elem |= get_right_bits32(remain, val >> n);
			write_array(pos, &elem, 1, rdtrans, wrtrans);
			//write next element
			pos = get_index(pos + 1);
			read_array(pos, &elem, 1, rdtrans, wrtrans);
			remain = elem_bits;
		}

		if (n > 0) {
			//write n bits
			elem &= ~(get_right_bits32(n, -1) << (remain - n));
			elem |= get_right_bits32(n, val) << (remain - n);
			write_array(pos, &elem, 1, rdtrans, wrtrans);
		}
	}

	/**
	 * n ビット目のビットが、
	 * バックバッファの何要素目に格納されているか取得する。
	 *
	 * @param n ビット位置
	 * @return バックバッファのインデックス
	 */
	size_type get_element_index(size_type n) const {
		return get_index(n >> elem_shift);
	}

	/**
	 * n ビット目のビットがバックバッファのある要素の、
	 * 最上位ビット側から何ビット目に格納されているか取得する。
	 *
	 * @param n ビット位置
	 * @return バックバッファの要素の、
	 * 最上位ビット側から何ビット目かを表す位置
	 */
	size_type get_element_bitpos(size_type n) const {
		return n & elem_mask;
	}

	/**
	 * 指定された unsigned int 値の右 n ビットを取得します。
	 *
	 * ただし、n が 0 のときは val を返します。
	 *
	 * n に 32 より大きい値を指定した場合、
	 * 32 で割った余りを指定したときと同じ結果になります。
	 *
	 * @param n 取得するビット数
	 * @param val ビットを取得する整数値（32 ビットまで有効）
	 * @return val の右 n ビットの数値、n が 0 ビットの場合は val そのもの
	 */
	uint32_t get_right_bits32(size_type n, uint32_t val) const {
		size_type s = (32 - n) & 0x1f;

		return (val << s) >> s;
	}

	/**
	 * 指定されたビット数だけ下位ビットをマスクするための、
	 * マスク値を取得します。
	 *
	 * n に 32 より大きい値を指定した場合、
	 * 32 で割った余りを指定したときと同じ結果になります。
	 *
	 * <pre>
	 * 例:
	 *
	 * n = 0  なら 0 (0b0)
	 * n = 1  なら 1 (0b1)
	 * n = 5  なら 31 (0b11111)
	 * n = 33 なら 1 (0b1)
	 * </pre>
	 *
	 * @param n マスクするビット数
	 * @return マスク値、n が負の数の場合は 0
	 */
	uint32_t get_mask_bits32(size_type n) const {
		const static uint32_t res[] = {
			0x00000000, 0x00000001, 0x00000003, 0x00000007,
			0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f,
			0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff,
			0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff,
			0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff,
			0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff,
			0x00ffffff, 0x01ffffff, 0x03ffffff, 0x07ffffff,
			0x0fffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff,
		};
		return res[(int)(n & 0x1f)];
	}

private:
	size_type len;
	size_type rd, wr;

	int elem_shift;
	int elem_bits;
	int elem_mask;
};

} //namespace mf

#endif //BIT_BUFFER_HPP__
