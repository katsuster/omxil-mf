#ifndef RING_BUFFER_HPP__
#define RING_BUFFER_HPP__

#include <algorithm>
#include <stdexcept>
#include <cstddef>
#include <iterator>

#include <omxil_mf/base.h>

#include "buffer_base.hpp"

namespace mf {

template <class RandomIterator, class T>
class OMX_MF_API_CLASS ring_buffer : public buffer_base<RandomIterator, T> {
public:
	//type of this
	typedef ring_buffer<RandomIterator, T> this_type;
	//reference to an element
	typedef typename buffer_base<RandomIterator, T>::reference reference;
	//const reference to an element
	typedef typename buffer_base<RandomIterator, T>::const_reference const_reference;
	//size type(unsigned)
	typedef typename buffer_base<RandomIterator, T>::size_type size_type;

	using buffer_base<RandomIterator, T>::elems;
	using buffer_base<RandomIterator, T>::get_elem;
	using buffer_base<RandomIterator, T>::read_array;
	using buffer_base<RandomIterator, T>::write_array;
	using buffer_base<RandomIterator, T>::get_remain;
	using buffer_base<RandomIterator, T>::get_remain_continuous;
	using buffer_base<RandomIterator, T>::get_space;
	using buffer_base<RandomIterator, T>::get_space_continuous;

	class const_iterator : public std::iterator<std::forward_iterator_tag, T> {
	public:
		explicit const_iterator(const ring_buffer *buffer)
			: const_iterator(buffer, 0) {
		}

		const_iterator(const ring_buffer *buffer, size_type position)
			: buf(buffer), pos(position) {
		}

		~const_iterator() {
		}

		const_iterator(const const_iterator& obj) {
			*this = obj;
		}

		const const_iterator& operator=(const const_iterator& obj) {
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

		const_iterator& operator++() {
			pos++;
			return *this;
		}

		const_iterator operator++(int) {
			return const_iterator(buf, pos++);
		}

		const_iterator& operator+=(size_type off) {
			pos += off;
			return *this;
		}

		const_iterator& operator--() {
			pos--;
			return *this;
		}

		const_iterator operator--(int) {
			return const_iterator(buf, pos--);
		}

		const_iterator& operator-=(size_type off) {
			pos -= off;
			return *this;
		}

		const_iterator operator+(int off) const {
			return const_iterator(buf, pos + off);
		}

		const_iterator operator-(int off) const {
			return const_iterator(buf, pos - off);
		}

		int operator-(const const_iterator& obj) const {
			return pos - obj.pos;
		}

		const_reference operator[](size_type off) const {
			return (*buf)[pos + off];
		}

		bool operator==(const const_iterator& obj) const {
			return (buf == obj.buf) && (pos == obj.pos);
		}

		bool operator!=(const const_iterator& obj) const {
			return !(*this == obj);
		}

		bool operator<(const const_iterator& obj) const {
			return (buf == obj.buf) && (pos < obj.pos);
		}

		bool operator>(const const_iterator& obj) const {
			return !(*this < obj) && (*this != obj);
		}

		bool operator<=(const const_iterator& obj) const {
			return (*this < obj) && (*this == obj);
		}

		bool operator>=(const const_iterator& obj) const {
			return !(*this < obj);
		}

	private:
		const ring_buffer *buf;
		size_type pos;
	};

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

		reference operator*() const {
			return (*buf)[pos];
		}

		iterator& operator++() {
			pos++;
			return *this;
		}

		iterator operator++(int) {
			return iterator(buf, pos++);
		}

		iterator& operator+=(size_type off) {
			pos += off;
			return *this;
		}

		iterator& operator--() {
			pos--;
			return *this;
		}

		iterator operator--(int) {
			return iterator(buf, pos--);
		}

		iterator& operator-=(size_type off) {
			pos -= off;
			return *this;
		}

		iterator operator+(int off) const {
			return iterator(buf, pos + off);
		}

		iterator operator-(int off) const {
			return iterator(buf, pos - off);
		}

		int operator-(const iterator& obj) const {
			return pos - obj.pos;
		}

		reference operator[](size_type off) const {
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

	ring_buffer(RandomIterator buf, size_type l)
		: buffer_base<RandomIterator, T>(buf, l), rd(0), wr(0) {
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

	/**
	 * Returns an iterator pointing to the first element.
	 *
	 * @return An iterator to the first element.
	 */
	iterator begin() {
		return iterator(this, 0);
	}

	/**
	 * Returns an iterator pointing to the past-the-end element.
	 *
	 * @return An iterator pointing to the past-the-end element
	 */
	iterator end() {
		return iterator(this, size());
	}

	//rbegin
	//rend

	//----------------------------------------
	// const iterators
	//----------------------------------------

	/**
	 * Returns an const_iterator pointing to the first element.
	 *
	 * @return An const_iterator to the first element.
	 */
	const_iterator cbegin() {
		return const_iterator(this, 0);
	}

	/**
	 * Returns an const_iterator pointing to the past-the-end element.
	 *
	 * @return An const_iterator pointing to the past-the-end element
	 */
	const_iterator cend() {
		return const_iterator(this, size());
	}

	//crbegin
	//crend

	//----------------------------------------
	// capacity
	//----------------------------------------

	/**
	 * The number of elements in this buffer.
	 *
	 * @return Number of elements in this buffer
	 */
	size_type size() const {
		return get_remain(rd, wr, elems());
	}

	/**
	 * The largest possible size of this buffer.
	 *
	 * @return The largest possible size of this buffer
	 */
	size_type max_size() const {
		return elems() - 1;
	}

	/**
	 * Is this buffer empty?
	 *
	 * @return true if this buffer is empty, false otherwise
	 */
	bool empty() const {
		return size() == 0;
	}

	/**
	 * Is this buffer full?
	 *
	 * @return true if this buffer is full, false otherwise
	 */
	bool full() {
		return size() == capacity();
	}

	/**
	 * Change the size of this buffer.
	 *
	 * This function is not supported.
	 *
	 * @param new_size New buffer size
	 */
	void resize(size_type new_size) {
		//cannot set
	}

	/**
	 * The maximum number of elements that can be stored in this buffer.
	 *
	 * In this class, capacity() returns same value as max_size().
	 *
	 * @return The size of currently allocated
	 */
	size_type capacity() const {
		return elems() - 1;
	}

	/**
	 * Change the capacity of this buffer.
	 *
	 * This function is not supported.
	 *
	 * @param new_cap New buffer capacity
	 */
	void set_capacity(size_type new_cap) {
		//cannot set
	}

	/**
	 * The maximum number of elements in this buffer without overwriting.
	 *
	 * @return The maximum number of elements in this buffer without overwriting
	 */
	size_type reserve() const {
		return get_space(rd, wr, elems(), 0);
	}

	//----------------------------------------
	// element access
	//----------------------------------------

	/**
	 * Access the first element in this buffer.
	 *
	 * @return A reference of first element
	 */
	reference front() {
		return get_elem(rd);
	}

	/**
	 * Access the first element in this buffer.
	 *
	 * @return A const reference of first element
	 */
	const_reference front() const {
		return get_elem(rd);
	}

	/**
	 * Access the last element in this buffer.
	 *
	 * @return A reference of last element
	 */
	reference back() {
		return get_elem(rd + size() - 1);
	}

	/**
	 * Access the last element in this buffer.
	 *
	 * @return A const reference of last element
	 */
	const_reference back() const {
		return get_elem(rd + size() - 1);
	}

	/**
	 * Access the element in this buffer.
	 *
	 * The position of first element is 0.
	 *
	 * @param n Position of element
	 * @return A reference of specified element
	 */
	reference operator[](size_type n) {
		return get_elem(rd + n);
	}

	/**
	 * Access the element in this buffer.
	 *
	 * The position of first element is 0.
	 *
	 * @param n Position of element
	 * @return A const reference of specified element
	 */
	const_reference operator[](size_type n) const {
		return get_elem(rd + n);
	}

	/**
	 * Access the element in this buffer.
	 *
	 * Throw the out_of_range exception if n is out of bounds.
	 *
	 * @param n Position of element
	 * @return A reference of specified element
	 */
	reference at(size_type n) {
		check_position(n);
		return get_elem(rd + n);
	}

	/**
	 * Access the element in this buffer.
	 *
	 * Throw the out_of_range exception if n is out of bounds.
	 *
	 * @param n Position of element
	 * @return A const reference of specified element
	 */
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

	/**
	 * Remove all elements in this buffer.
	 */
	void clear() {
		rd = wr = 0;
	}

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
	 * 配列をリングバッファから読み込みますが、
	 * 読み込み位置を変更しません。
	 *
	 * @param buf     リングバッファから読み込んだ要素を格納する配列
	 * @param count   リングバッファから読み込む数
	 * @return リングバッファから読み込んだ数
	 */
	size_type peek_array(T *buf, size_type count) {
		size_type result;

		count = std::min(count, get_remain(rd, wr, elems()));

		result = read_array(rd, buf, count);

		return result;
	}

	/**
	 * 配列をリングバッファから読み込みます。
	 *
	 * @param buf     リングバッファから読み込んだ要素を格納する配列
	 * @param count   リングバッファから読み込む数
	 * @return リングバッファから読み込んだ数
	 */
	size_type read_array(T *buf, size_type count) {
		size_type result;

		count = std::min(count, get_remain(rd, wr, elems()));

		result = read_array(rd, buf, count);
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
	 * @return リングバッファに書き込んだ数
	 */
	size_type write_array(const T *buf, size_type count) {
		size_type result;

		count = std::min(count, get_space(rd, wr, elems(), 0));

		result = write_array(wr, buf, count);
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
	 * @return リングバッファに書き込んだ数
	 */
	size_type copy_array(this_type *src, size_type count) {
		size_type result;

		count = std::min(count, get_remain_continuous(src->rd, src->wr, src->elems()));

		result = write_array(&(*src)[0], count);
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
