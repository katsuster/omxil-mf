#ifndef RING_BUFFER_HPP__
#define RING_BUFFER_HPP__

#include <algorithm>
#include <stdexcept>
#include <cstddef>
#include <iterator>

#include "buffer_base.hpp"

namespace mf {

template <class T>
class ring_buffer : public buffer_base<T> {
public:
	//type of this
	typedef ring_buffer<T> this_type;
	//reference to an element
	typedef typename buffer_base<T>::reference reference;
	//const reference to an element
	typedef typename buffer_base<T>::const_reference const_reference;
	//size type(unsigned)
	typedef typename buffer_base<T>::size_type size_type;

	typedef int (* transform_func_t)(T *dest, const T *src, size_t n, size_t *ntrans);

	using buffer_base<T>::no_transform;
	using buffer_base<T>::elems;
	using buffer_base<T>::get_elem;
	using buffer_base<T>::read_array;
	using buffer_base<T>::write_array;
	using buffer_base<T>::get_remain;
	using buffer_base<T>::get_remain_continuous;
	using buffer_base<T>::get_space;
	using buffer_base<T>::get_space_continuous;

	class iterator : public std::iterator<std::forward_iterator_tag, T> {
	public:
		explicit iterator(ring_buffer *buffer)
			: iterator(buffer, 0) {
		}

		iterator(ring_buffer *buffer, size_type position)
			: buf(buffer), pos(position) {
		}

		~iterator() {
		}

		iterator(const iterator& obj) {
			*this = obj;
		}

		const iterator& operator=(const iterator& obj) {
			if (this == &obj) {
				return *this;
			}

			buf = obj.buf;
			pos = obj.pos;

			return *this;
		}

		const_reference operator*() const {
			return (*buf)[pos];
		}

		reference operator*() {
			return (*buf)[pos];
		}

		iterator& operator++() {
			pos++;
			return *this;
		}

		iterator operator++(int) {
			return iterator(buf, pos++);
		}

		iterator& operator--() {
			pos--;
			return *this;
		}

		iterator operator--(int) {
			return iterator(buf, pos--);
		}

		iterator operator+(int off) const {
			return iterator(buf, pos + off);
		}

		iterator operator-(int off) const {
			return iterator(buf, pos - off);
		}

		int operator-(const iterator& obj) {
			return pos - obj.pos;
		}

		const_reference operator[](size_type off) const {
			return (*buf)[pos + off];
		}

		reference operator[](size_type off) {
			return (*buf)[pos + off];
		}

		bool operator==(const iterator& obj) const {
			return (buf == obj.buf) && (pos == obj.pos);
		}

		bool operator!=(const iterator& obj) const {
			return !(*this == obj);
		}

		bool operator<(const iterator& obj) const {
			return (buf == obj.buf) && (pos < obj.pos);
		}

		bool operator>(const iterator& obj) const {
			return !(*this < obj) && (*this != obj);
		}

		bool operator<=(const iterator& obj) const {
			return (*this < obj) && (*this == obj);
		}

		bool operator>=(const iterator& obj) const {
			return !(*this < obj);
		}

	private:
		ring_buffer *buf;
		size_type pos;

	};

	ring_buffer(T *buf, size_type l)
	: buffer_base<T>(buf, l), rd(0), wr(0) {
		//do nothing
	}

	//disable copy constructor
	ring_buffer(const ring_buffer& obj) = delete;

	//disable operator=
	ring_buffer& operator=(const ring_buffer& obj) = delete;

	virtual ~ring_buffer() {
		//do nothing
	}

	//----------------------------------------
	// iterators
	//----------------------------------------

	iterator begin() {
		return iterator(this, 0);
	}

	iterator end() {
		return iterator(this, size());
	}

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
	size_type size() const {
		return get_remain(rd, wr, elems());
	}

	/**
	 * The largest possible size of this buffer.
	 */
	size_type max_size() const {
		return elems() - 1;
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
	bool full() {
		return size() == capacity();
	}

	/**
	 * Change the size of this buffer.
	 *
	 * This is not supported.
	 */
	void resize(size_type new_size) {
		//cannot set
	}

	/**
	 * The maximum number of elements that can be stored in this buffer.
	 */
	size_type capacity() const {
		return elems() - 1;
	}

	/**
	 * Change the capacity of this buffer.
	 *
	 * This is not supported.
	 */
	void set_capacity(size_type new_cap) {
		//cannot set
	}

	/**
	 * The maximum number of elements in this buffer without overwriting.
	 */
	size_type reserve() const {
		return get_space(rd, wr, elems(), 0);
	}

	//----------------------------------------
	// element access
	//----------------------------------------

	reference front() {
		return get_elem(rd);
	}

	const_reference front() const {
		return get_elem(rd);
	}

	reference back() {
		return get_elem(rd + size() - 1);
	}

	const_reference back() const {
		return get_elem(rd + size() - 1);
	}

	reference operator[](size_type n) {
		return get_elem(rd + n);
	}

	const_reference operator[](size_type n) const {
		return get_elem(rd + n);
	}

	reference at(size_type n) {
		check_position(n);
		return get_elem(rd + n);
	}

	const_reference at(size_type n) const {
		check_position(n);
		return get_elem(rd + n);
	}

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

	size_type get_read_position() const {
		return rd;
	}

	void set_read_position(size_type new_pos) {
		rd = new_pos % elems();
	}

	size_type get_write_position() const {
		return wr;
	}

	void set_write_position(size_type new_pos) {
		wr = new_pos % elems();
	}

	/**
	 * 要素を読み飛ばします。
	 *
	 * @param count 読み飛ばす数
	 * @return 読み飛ばした数
	 */
	size_type skip(size_type count) {
		count = std::min(count, get_remain(rd, wr, elems()));

		rd += count;
		if (rd >= elems()) {
			rd -= elems();
		}

		return count;
	}

	/**
	 * 配列をリングバッファから読み込みます。
	 *
	 * @param buf     リングバッファから読み込んだ要素を格納する配列
	 * @param count   リングバッファから読み込む数
	 * @param rdtrans 読み出し用の変換関数
	 * @param wrtrans 書き込み用の変換関数
	 * @return リングバッファから読み込んだ数
	 */
	size_type read_array(T *buf, size_type count, transform_func_t rdtrans = no_transform, transform_func_t wrtrans = no_transform) {
		size_type result;

		count = std::min(count, get_remain(rd, wr, elems()));

		result = read_array(rd, buf, count, rdtrans, wrtrans);
		rd += result;
		if (rd >= elems()) {
			rd -= elems();
		}

		return result;
	}

	/**
	 * 配列をリングバッファに書き込みます。
	 *
	 * @param buf     リングバッファに書き込む要素の配列
	 * @param count   リングバッファに書き込む数
	 * @param rdtrans 読み出し用の変換関数
	 * @param wrtrans 書き込み用の変換関数
	 * @return リングバッファに書き込んだ数
	 */
	size_type write_array(const T *buf, size_type count, transform_func_t rdtrans = no_transform, transform_func_t wrtrans = no_transform) {
		size_type result;

		count = std::min(count, get_space(rd, wr, elems(), 0));

		result = write_array(wr, buf, count, rdtrans, wrtrans);
		wr += result;
		if (wr >= elems()) {
			wr -= elems();
		}

		return result;
	}

	/**
	 * 別のリングバッファからコピーします。
	 *
	 * @param src     コピー元のリングバッファ
	 * @param count   リングバッファから読み込む数
	 * @param rdtrans 読み出し用の変換関数
	 * @param wrtrans 書き込み用の変換関数
	 * @return リングバッファに書き込んだ数
	 */
	size_type copy_array(this_type *src, size_type count, transform_func_t rdtrans = no_transform, transform_func_t wrtrans = no_transform) {
		size_type result;

		count = std::min(count, get_remain_continuous(src->rd, src->wr, src->elems()));

		result = write_array(&(*src)[0], count, rdtrans, wrtrans);
		src->skip(result);

		return result;
	}

protected:
	void check_position(size_type n) const {
		if (n >= size()) {
			throw std::out_of_range("ring_buffer");
		}
	}

private:
	size_type rd, wr;
};

} //namespace mf

#endif //RING_BUFFER_HPP__
