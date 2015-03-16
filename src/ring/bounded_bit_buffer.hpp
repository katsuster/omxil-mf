#ifndef BOUNDED_BIT_BUFFER_HPP__
#define BOUNDED_BIT_BUFFER_HPP__

#include <algorithm>
#include <stdexcept>
#include <string>
#include <mutex>
#include <condition_variable>
#include <cstdio>
//#include <cstdint>
#include <cstddef>

#include "buffer_base.hpp"

namespace mf {

template <class Container, class T>
class bounded_bit_buffer {
public:
	//type of this
	typedef bounded_bit_buffer<Container, T> this_type;
	//reference to an element
	typedef typename buffer_base<T>::reference reference;
	//const reference to an element
	typedef typename buffer_base<T>::const_reference const_reference;
	//size type(unsigned)
	typedef typename buffer_base<T>::size_type size_type;

	typedef int (* transform_func_t)(T *dest, const T *src, size_t n, size_t *ntrans);

	bounded_bit_buffer(Container& buffer)
	: bound(buffer), cnt_rd(0), cnt_wr(0), broken(false) {
		//do nothing
	}

	//disable copy constructor
	bounded_bit_buffer(const bounded_bit_buffer& obj) = delete;

	//disable operator=
	bounded_bit_buffer& operator=(const bounded_bit_buffer& obj) = delete;

	virtual ~bounded_bit_buffer() {
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
	 * The number of elements in this buffer.
	 */
	size_t size() const {
		return bound.size();
	}

	/**
	 * The largest possible size of this buffer.
	 */
	size_t max_size() const {
		return bound.max_size();
	}

	/**
	 * Is this buffer empty?
	 */
	bool empty() const {
		return bound.empty();
	}

	/**
	 * Is this buffer full?
	 */
	bool full() const {
		return bound.full();
	}

	/**
	 * Change the size of this buffer.
	 */
	//void resize(size_t new_size) {
	//	std::unique_lock<std::mutex> lock(mut);
	//	bound.resize(new_size);
	//}

	/**
	 * The maximum number of elements that can be stored in this buffer.
	 */
	size_t capacity() const {
		return bound.capacity();
	}

	/**
	 * Change the capacity of this buffer.
	 */
	//void set_capacity(size_t new_cap) {
	//	std::unique_lock<std::mutex> lock(mut);
	//	bound.set_capacity(new_cap);
	//}

	/**
	 * The maximum number of elements in this buffer without overwriting.
	 */
	size_t reserve() const {
		return bound.reserve();
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

	size_t get_read_position() const {
		return bound.get_read_position();
	}

	void set_read_position(size_t new_pos) {
		std::unique_lock<std::mutex> lock(mut);

		bound.set_read_position(new_pos);
		cnt_rd = 0;
		notify_with_lock();
	}

	size_t get_write_position() const {
		return bound.get_write_position();
	}

	void set_write_position(size_t new_pos) {
		std::unique_lock<std::mutex> lock(mut);

		bound.set_write_position(new_pos);
		cnt_wr = 0;
		notify_with_lock();
	}

	/**
	 * 読み出したビットの総数を取得します。
	 *
	 * @return 読み出したビットの総数
	 */
	uint64_t get_read_count() const {
		return cnt_rd;
	}

	/**
	 * 書き込んだビットの総数を取得します。
	 *
	 * @return 書き込んだビットの総数
	 */
	uint64_t get_write_count() const {
		return cnt_wr;
	}

	/**
	 * 0 ～ 32ビットまでの任意のビット列を読み出し、
	 * uint32_t 型の LSB 側に詰めた値を取得します。
	 *
	 * バッファに十分なデータがない場合、
	 * 指定したビット数を読み込むまでブロックします。
	 *
	 * @param n       読み出すビット数
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 * @return 読み出したビット列を詰めた値
	 */
	uint32_t get_bits32_fully(size_t n, transform_func_t rdtrans = buffer_base<T>::no_transform, transform_func_t wrtrans = buffer_base<T>::no_transform) {
		std::unique_lock<std::mutex> lock(mut);
		size_t pos = 0, remain;
		uint32_t result = 0;

		while (n - pos > 0) {
			wait_element_with_lock(lock);

			remain = std::min(n - pos, bound.size());
			result <<= remain;
			result |= bound.get_bits32(remain, rdtrans, wrtrans);
			pos += remain;

			notify_with_lock();
		}

		return result;
	}

	/**
	 * val の LSB 側から 0 ～ 32ビットまでの、
	 * 任意のビット数を書き込みます。
	 *
	 * バッファに十分な空きがない場合、
	 * 指定したビット数を書き込むまでブロックします。
	 *
	 * @param n       書き込むビット数
	 * @param val     書き込むビット列を含んだ値
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 */
	void set_bits32_fully(size_t n, uint32_t val, transform_func_t rdtrans = buffer_base<T>::no_transform, transform_func_t wrtrans = buffer_base<T>::no_transform) {
		std::unique_lock<std::mutex> lock(mut);
		size_t pos = 0, space;

		while (n - pos > 0) {
			wait_space_with_lock(lock);

			space = std::min(n - pos, bound.reserve());
			bound.set_bits32(space, val >> (n - space - pos), rdtrans, wrtrans);
			pos += space;

			notify_with_lock();
		}
	}

	/**
	 * 任意の要素をリングバッファから読み込みます。
	 *
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 * @return 任意の要素
	 */
	template <class U>
	U read(transform_func_t rdtrans = buffer_base<T>::no_transform, transform_func_t wrtrans = buffer_base<T>::no_transform) {
		U buf;

		read_array(reinterpret_cast<T *>(&buf),
			sizeof(U) << bound.get_element_shift(), rdtrans, wrtrans);

		return buf;
	}

	/**
	 * 配列をリングバッファから読み込みます。
	 *
	 * @param buf     リングバッファから読み込んだ要素を格納する配列
	 * @param count   リングバッファから読み込むビット数
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 * @return リングバッファから読み込んだビット数
	 */
	size_type read_array(T *buf, size_type count, transform_func_t rdtrans = buffer_base<T>::no_transform, transform_func_t wrtrans = buffer_base<T>::no_transform) {
		std::unique_lock<std::mutex> lock(mut);

		return read_array_with_lock(buf, count, rdtrans, wrtrans);
	}

	/**
	 * 任意の要素をリングバッファに書き込みます。
	 *
	 * @param buf     リングバッファに書き込む要素
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 */
	template <class U>
	void write(const U& buf, transform_func_t rdtrans = buffer_base<T>::no_transform, transform_func_t wrtrans = buffer_base<T>::no_transform) {
		write_array(reinterpret_cast<T *>(&buf),
			sizeof(U) << bound.get_element_shift(), rdtrans, wrtrans);
	}

	/**
	 * 配列をリングバッファに書き込みます。
	 *
	 * @param buf     リングバッファに書き込む要素の配列
	 * @param count   リングバッファに書き込むビット数
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 * @return リングバッファに書き込んだビット数
	 */
	size_type write_array(const T *buf, size_type count, transform_func_t rdtrans = buffer_base<T>::no_transform, transform_func_t wrtrans = buffer_base<T>::no_transform) {
		std::unique_lock<std::mutex> lock(mut);

		return write_array_with_lock(buf, count, rdtrans, wrtrans);
	}

	/**
	 * 別のリングバッファからコピーします。
	 *
	 * @param src     コピー元のリングバッファ
	 * @param count   リングバッファから読み込む数
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 * @return リングバッファに書き込んだビット数
	 */
	size_type copy_array(this_type *src, size_type count, transform_func_t rdtrans = buffer_base<T>::no_transform, transform_func_t wrtrans = buffer_base<T>::no_transform) {
		std::unique_lock<std::mutex> lock(mut);
		std::unique_lock<std::mutex> lock_src(src->mut);

		return copy_array_with_lock(src, count, rdtrans, wrtrans);
	}




	/**
	 * ビットを読み飛ばします。
	 *
	 * 指定した要素を読み飛ばすまでブロックします。
	 *
	 * @param count 読み飛ばすビット数
	 * @return 読み飛ばしたビット数
	 */
	size_type skip_fully(size_type count) {
		std::unique_lock<std::mutex> lock(mut);
		size_type pos = 0;

		while (count - pos > 0) {
			wait_element_with_lock(lock);

			pos += skip_with_lock(count - pos);
		}

		return pos;
	}

	/**
	 * 任意の要素をリングバッファから読み込みます。
	 *
	 * 指定した要素を読み込むまでブロックします。
	 *
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 * @return リングバッファから読み込んだ要素
	 */
	template <class U>
	U read_fully(transform_func_t rdtrans = buffer_base<T>::no_transform, transform_func_t wrtrans = buffer_base<T>::no_transform) {
		U buf;

		read_fully(reinterpret_cast<T *>(&buf),
			sizeof(U) << bound.get_element_shift(), rdtrans, wrtrans);

		return buf;
	}

	/**
	 * 配列をリングバッファから読み込みます。
	 *
	 * 指定したビット数を読み込むまでブロックします。
	 *
	 * @param buf     リングバッファから読み込んだ要素を格納する配列
	 * @param count   リングバッファから読み込むビット数
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 * @return リングバッファから読み込んだビット数
	 */
	size_type read_fully(T *buf, size_type count, transform_func_t rdtrans = buffer_base<T>::no_transform, transform_func_t wrtrans = buffer_base<T>::no_transform) {
		std::unique_lock<std::mutex> lock(mut);
		size_type pos = 0;
		size_type bits = bound.get_element_bits();
		int shift = bound.get_element_shift();

		while (count - pos >= bits) {
			wait_element_with_lock(lock);

			pos += read_array_with_lock(&buf[pos >> shift], count - pos, rdtrans, wrtrans);
		}

		return pos;
	}

	/**
	 * 任意の要素をリングバッファに書き込みます。
	 *
	 * 指定した要素を書き込むまでブロックします。
	 *
	 * @param buf     リングバッファに書き込む要素
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 */
	template <class U>
	void write_fully(const U& buf, transform_func_t rdtrans = buffer_base<T>::no_transform, transform_func_t wrtrans = buffer_base<T>::no_transform) {
		write_fully(reinterpret_cast<const T *>(&buf),
			sizeof(U) << bound.get_element_shift(), rdtrans, wrtrans);
	}

	/**
	 * 配列をリングバッファに書き込みます。
	 *
	 * 指定したビット数を書き込むまでブロックします。
	 *
	 * @param buf     リングバッファに書き込む要素の配列
	 * @param count   リングバッファに書き込むビット数
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 * @return リングバッファに書き込んだビット数
	 */
	size_type write_fully(const T *buf, size_type count, transform_func_t rdtrans = buffer_base<T>::no_transform, transform_func_t wrtrans = buffer_base<T>::no_transform) {
		std::unique_lock<std::mutex> lock(mut);
		size_type pos = 0;
		size_type bits = bound.get_element_bits();
		int shift = bound.get_element_shift();

		while (count - pos >= bits) {
			wait_space_with_lock(lock);

			pos += write_array_with_lock(&buf[pos >> shift], count - pos, rdtrans, wrtrans);
		}

		return pos;
	}

	/**
	 * 別のリングバッファからコピーします。
	 *
	 * 指定したビット数を書き込むまでブロックします。
	 *
	 * @param src     コピー元のリングバッファ
	 * @param count   リングバッファから読み込むビット数
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 * @return リングバッファに書き込んだビット数
	 */
	size_type copy_array_fully(this_type *src, size_type count, transform_func_t rdtrans = buffer_base<T>::no_transform, transform_func_t wrtrans = buffer_base<T>::no_transform) {
		std::unique_lock<std::mutex> lock(mut);
		std::unique_lock<std::mutex> lock_src(src->mut);
		size_type pos = 0;

		while (count - pos > 0) {
			if (bound.full()) {
				lock_src.unlock();
				wait_space_with_lock(lock);
				lock_src.lock();
			}
			if (src->bound.empty()) {
				lock.unlock();
				src->wait_element_with_lock(lock_src);
				lock.lock();
			}

			pos += copy_array_with_lock(src, count - pos, rdtrans, wrtrans);
		}

		return pos;
	}




	/**
	 * リングバッファに指定されたビット数が書き込まれるまでブロックします。
	 * リングバッファに要素が既に存在していればすぐに返ります。
	 *
	 * シャットダウンされた場合は runtime_error をスローします。
	 *
	 * @param n ビット数
	 */
	void wait_element(size_type n) {
		std::unique_lock<std::mutex> lock(mut);

		cond_not_empty.wait(lock, [&] { return broken || bound.size() >= n; });
		if (broken) {
			std::string msg(__func__);
			msg += ": interrupted.";
			throw std::runtime_error(msg);
		}
	}

	/**
	 * リングバッファに指定されたビット数の空きができるまでブロックします。
	 * リングバッファに空きが既に存在していればすぐに返ります。
	 *
	 * シャットダウンされた場合は runtime_error をスローします。
	 *
	 * @param n ビット数
	 */
	void wait_space(size_type n) {
		std::unique_lock<std::mutex> lock(mut);

		cond_not_full.wait(lock, [&] { return broken || bound.reserve() >= n; });
		if (broken) {
			std::string msg(__func__);
			msg += ": interrupted.";
			throw std::runtime_error(msg);
		}
	}

	/**
	 * 全ての待機しているスレッドを強制的に解除します。
	 *
	 * 強制解除されたスレッドは runtime_error をスローします。
	 */
	void shutdown() {
		std::unique_lock<std::mutex> lock(mut);

		broken = true;
		notify_with_lock();
	}

protected:
	/**
	 * ビットを読み飛ばします。
	 *
	 * ロックを確保してから呼び出します。
	 *
	 * @param count 読み飛ばすビット数
	 * @return 読み飛ばしたビット数
	 */
	size_type skip_with_lock(size_type count) {
		size_type result;

		result = bound.skip(count);
		cnt_rd += result;
		notify_with_lock();

		return result;
	}

	/**
	 * 配列をリングバッファから読み込みます。
	 *
	 * ロックを確保してから呼び出します。
	 *
	 * @param buf     リングバッファから読み込んだ要素を格納する配列
	 * @param count   リングバッファから読み込むビット数
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 * @return リングバッファから読み込んだビット数
	 */
	size_type read_array_with_lock(T *buf, size_type count, transform_func_t rdtrans, transform_func_t wrtrans) {
		size_type result;

		result = bound.read_array(buf, count, rdtrans, wrtrans);
		cnt_rd += result;
		notify_with_lock();

		return result;
	}

	/**
	 * 配列をリングバッファに書き込みます。
	 *
	 * ロックを確保してから呼び出します。
	 *
	 * @param buf     リングバッファに書き込む要素の配列
	 * @param count   リングバッファに書き込むビット数
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 * @return リングバッファに書き込んだビット数
	 */
	size_type write_array_with_lock(const T *buf, size_type count, transform_func_t rdtrans, transform_func_t wrtrans) {
		size_type result;

		result = bound.write_array(buf, count, rdtrans, wrtrans);
		cnt_wr += result;
		notify_with_lock();

		return result;
	}

	/**
	 * 別のリングバッファからコピーします。
	 *
	 * ロックを確保してから呼び出します。
	 *
	 * @param src     コピー元のリングバッファ
	 * @param count   リングバッファから読み込む数
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 * @return リングバッファに書き込んだ数
	 */
	size_type copy_array_with_lock(this_type *src, size_type count, transform_func_t rdtrans, transform_func_t wrtrans) {
		size_type result;

		result = bound.copy_array(&src->bound, count, rdtrans, wrtrans);
		cnt_wr += result;
		notify_with_lock();
		src->notify_with_lock();

		return result;
	}

	/**
	 * リングバッファに要素が書き込まれるまでブロックします。
	 * リングバッファに要素が既に存在していればすぐに返ります。
	 *
	 * ロックを確保してから呼び出します。
	 *
	 * シャットダウンされた場合は runtime_error をスローします。
	 *
	 * @param lock リングバッファのロックへの参照
	 */
	void wait_element_with_lock(std::unique_lock<std::mutex>& lock) {
		cond_not_empty.wait(lock, [&] { return broken || !bound.empty(); });
		if (broken) {
			std::string msg(__func__);
			msg += ": interrupted.";
			throw std::runtime_error(msg);
		}
	}

	/**
	 * リングバッファに空きができるまでブロックします。
	 * リングバッファに空きが既に存在していればすぐに返ります。
	 *
	 * ロックを確保してから呼び出します。
	 *
	 * シャットダウンされた場合は runtime_error をスローします。
	 *
	 * @param lock リングバッファのロックへの参照
	 */
	void wait_space_with_lock(std::unique_lock<std::mutex>& lock) {
		cond_not_full.wait(lock, [&] { return broken || !bound.full(); });
		if (broken) {
			std::string msg(__func__);
			msg += ": interrupted.";
			throw std::runtime_error(msg);
		}
	}

	void notify_with_lock() {
		if (!bound.full()) {
			cond_not_full.notify_all();
		}
		if (!bound.empty()) {
			cond_not_empty.notify_all();
		}
		if (broken) {
			cond_not_full.notify_all();
			cond_not_empty.notify_all();
		}
	}

private:
	Container& bound;
	mutable std::mutex mut;
	std::condition_variable cond_not_full;
	std::condition_variable cond_not_empty;
	uint64_t cnt_rd, cnt_wr;
	bool broken;
};

} //namespace mf

#endif //BOUNDED_BIT_BUFFER_HPP__
