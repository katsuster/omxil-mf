#ifndef BUFFER_BASE_HPP__
#define BUFFER_BASE_HPP__

#include <cstddef>
#include <cstring>

#include <omxil_mf/base.h>

namespace mf {

template <class RandomIterator, class T>
class OMX_MF_API_CLASS buffer_base {
public:
	//reference to an element
	typedef T& reference;
	//const reference to an element
	typedef const T& const_reference;
	//size type(unsigned)
	typedef size_t size_type;

protected:
	//constructor for derived classes
	buffer_base(RandomIterator buf, size_type l)
		: start(buf), nelements(l) {
		//do nothing
	}

	//disable copy constructor
	buffer_base(const buffer_base& obj) = delete;

	//disable operator=
	buffer_base& operator=(const buffer_base& obj) = delete;

	//destructor for derived classes
	virtual ~buffer_base() {
		//do nothing
	}

	/**
	 * バッファのベースとなる配列の要素数を返します。
	 *
	 * @return バッファのベース配列の要素数
	 */
	size_type elems() const {
		return nelements;
	}

	/**
	 * リングバッファの範囲内に丸めたインデックスを取得する。
	 *
	 * @param n インデックス
	 * @return 丸めた後のインデックス
	 */
	size_type get_index(size_type n) const {
		if (n < nelements) {
			return n;
		} else {
			return n % nelements;
		}
	}

	/**
	 * 指定されたインデックスの要素を取得します。
	 *
	 * @param  n インデックス
	 * @return 指定されたインデックスが指す要素
	 */
	reference get_elem(size_type n) {
		return start[get_index(n)];
	}

	/**
	 * 指定されたインデックスの要素を取得します。
	 *
	 * @param  n インデックス
	 * @return 指定されたインデックスが指す要素
	 */
	const_reference get_elem(size_type n) const {
		return start[get_index(n)];
	}

	/**
	 * 配列をリングバッファの指定した位置から読み込みます。
	 *
	 * @param index   読み出しを開始する位置
	 * @param buf     リングバッファから読み込んだ要素を格納する配列
	 * @param count   リングバッファから読み込む数
	 * @return リングバッファから読み込んだ数
	 */
	size_type read_array(size_type index, T *buf, size_type count) const {
		size_type n, nwrite = 0;

		index = get_index(index);
		while (count - nwrite > 0) {
			n = count - nwrite;
			n = std::min(n, nelements - index);

			std::copy(start + index, start + index + n, buf + nwrite);
			nwrite += n;
			index += n;
			if (index >= nelements) {
				index -= nelements;
			}
		}

		return nwrite;
	}

	/**
	 * 配列をリングバッファの指定した位置に書き込みます。
	 *
	 * @param index   書き込みを開始する位置
	 * @param buf     リングバッファに書き込む要素の配列
	 * @param count   リングバッファに書き込む数
	 * @return リングバッファに書き込んだ数
	 */
	size_type write_array(size_type index, const T *buf, size_type count) {
		size_type n, nread = 0;

		index = get_index(index);
		while (count - nread > 0) {
			n = count - nread;
			n = std::min(n, nelements - index);

			std::copy(buf + nread, buf + nread + n, start + index);
			nread += n;
			index += n;
			if (index >= nelements) {
				index -= nelements;
			}
		}

		return nread;
	}

	/**
	 * Get the remain elements of buffer.
	 *
	 * @param rd  read position of buffer
	 * @param wr  write position of buffer
	 * @param len length of buffer
	 * @return the remain elements of buffer.
	 */
	size_type get_remain(size_type rd, size_type wr, size_type len) const {
		size_type rest;

		if (rd <= wr) {
			rest = wr - rd;
		} else {
			rest = wr - rd + len;
		}

		return rest;
	}

	/**
	 * Get the continuous remain elements of buffer.
	 *
	 * @param rd  read position of buffer
	 * @param wr  write position of buffer
	 * @param len length of buffer
	 * @return the remain elements of buffer.
	 */
	size_type get_remain_continuous(size_type rd, size_type wr, size_type len) const {
		size_type rest;

		if (rd <= wr) {
			rest = wr - rd;
		} else {
			rest = len - rd;
		}

		return rest;
	}

	/**
	 * Get the space elements of buffer.
	 *
	 * @param rd  read position of buffer
	 * @param wr  write position of buffer
	 * @param len length of buffer
	 * @param gap length of forbidden area
	 * @return the space elements of buffer.
	 */
	size_type get_space(size_type rd, size_type wr, size_type len, size_type gap) const {
		size_type rest;

		if (rd <= wr) {
			//(len - 1 - gap) - (wr - rd)
			rest = len + rd - wr - 1 - gap;
		} else {
			//(len - 1 - gap) - (wr - rd + len)
			rest = rd - wr - 1 - gap;
		}

		return rest;
	}

	/**
	 * Get the continuous space elements of buffer.
	 *
	 * @param rd  read position of buffer
	 * @param wr  write position of buffer
	 * @param len length of buffer
	 * @param gap length of forbidden area
	 * @return the continuous space elements of buffer.
	 */
	size_type get_space_continuous(size_type rd, size_type wr, size_type len, size_type gap) const {
		size_type rest;

		if (rd > wr + gap) {
			rest = rd - wr - 1 - gap;
		} else if (rd > gap) {
			rest = len - wr - gap;
		} else {
			rest = len - wr - 1 - gap;
		}

		return rest;
	}

private:
	RandomIterator start;
	size_type nelements;

};

} //namespace mf

#endif //BUFFER_BASE_HPP__
