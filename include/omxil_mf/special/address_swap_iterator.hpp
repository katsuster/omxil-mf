#ifndef ADDRESS_SWAP_ITERATOR_HPP__
#define ADDRESS_SWAP_ITERATOR_HPP__

#include <cstdint>
#include <cstddef>
#include <iterator>

#include <omxil_mf/base.h>

namespace mf {

template <class RandomIterator, class T>
class OMX_MF_API_CLASS address_swap_iterator : public std::iterator<std::random_access_iterator_tag, T> {
public:
	typedef typename std::iterator<std::random_access_iterator_tag, T>::reference reference;
	typedef typename std::iterator<std::random_access_iterator_tag, T>::difference_type difference_type;

	address_swap_iterator()
		: buf(), pos(0) {
	}

	explicit address_swap_iterator(RandomIterator buffer)
		: address_swap_iterator(buffer, 0) {
	}

	address_swap_iterator(RandomIterator buffer, difference_type position)
		: buf(buffer), pos(position) {
	}

	~address_swap_iterator() {
	}

	address_swap_iterator(const address_swap_iterator& obj) {
		*this = obj;
	}

	const address_swap_iterator& operator=(const address_swap_iterator& obj) {
		if (this == &obj) {
			return *this;
		}

		buf = obj.buf;
		pos = obj.pos;

		return *this;
	}

	reference operator*() const {
		return buf[pos ^ 3];
	}

	address_swap_iterator& operator++() {
		pos++;
		return *this;
	}

	address_swap_iterator operator++(int) {
		return address_swap_iterator(buf, pos++);
	}

	address_swap_iterator& operator+=(difference_type off) {
		pos += off;
		return *this;
	}

	address_swap_iterator& operator--() {
		pos--;
		return *this;
	}

	address_swap_iterator operator--(int) {
		return address_swap_iterator(buf, pos--);
	}

	address_swap_iterator& operator-=(difference_type off) {
		pos -= off;
		return *this;
	}

	address_swap_iterator operator+(int off) const {
		return address_swap_iterator(buf, pos + off);
	}

	address_swap_iterator operator-(int off) const {
		return address_swap_iterator(buf, pos - off);
	}

	int operator-(const address_swap_iterator& obj) const {
		return pos - obj.pos;
	}

	reference operator[](difference_type off) const {
		return buf[(pos + off) ^ 3];
	}

	bool operator==(const address_swap_iterator& obj) const {
		return (buf == obj.buf) && (pos == obj.pos);
	}

	bool operator!=(const address_swap_iterator& obj) const {
		return !(*this == obj);
	}

	bool operator<(const address_swap_iterator& obj) const {
		return (buf == obj.buf) && (pos < obj.pos);
	}

	bool operator>(const address_swap_iterator& obj) const {
		return !(*this < obj) && (*this != obj);
	}

	bool operator<=(const address_swap_iterator& obj) const {
		return (*this < obj) && (*this == obj);
	}

	bool operator>=(const address_swap_iterator& obj) const {
		return !(*this < obj);
	}

private:
	RandomIterator buf;
	difference_type pos;

};

} //namespace mf

#endif //ADDRESS_SWAP_ITERATOR_HPP__
