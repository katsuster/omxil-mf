#ifndef BUFFER_BASE_HPP__
#define BUFFER_BASE_HPP__

#include <cstddef>
#include <cstring>

namespace mf {

template <class T>
class buffer_base {
public:
	//reference to an element
	typedef T& reference;
	//const reference to an element
	typedef const T& const_reference;
	//size type(unsigned)
	typedef size_t size_type;

	/**
	 * 変換関数です。
	 *
	 * read 時は、
	 *   dest: 外部バッファ
	 *   src : 自身のバッファ
	 * write 時は、
	 *   dest: 自身のバッファ
	 *   src : 外部バッファ
	 * となります。
	 *
	 * @param dest   コピー先バッファ
	 * @param src    コピー元バッファ
	 * @param n      コピーする要素数
	 * @param ntrans コピーした要素数
	 * @return 成功ならば 0、失敗ならば -1
	 */
	typedef int (* transform_func_t)(T *dest, const T *src, size_type n, size_type *ntrans);

	/**
	 * 何も変換しない transform 関数です。
	 *
	 * 内部で memcpy を呼ぶだけです。
	 *
	 * @param dest   コピー先バッファ
	 * @param src    コピー元バッファ
	 * @param n      コピーする要素数
	 * @param ntrans コピーした要素数
	 * @return 成功ならば 0、失敗ならば -1
	 */
	static int no_transform(T *dest, const T *src, size_type n, size_type *ntrans) {
		if (!dest || !src || !ntrans) {
			return -1;
		}

		memcpy(dest, src, n * sizeof(T));
		*ntrans = n;

		return 0;
	}

protected:
	//constructor for derived classes
	buffer_base(T *buf, size_type l) : start(buf), nelements(l), alloc_self(false) {
		if (start == nullptr) {
			start = new T[l];
			alloc_self = true;
		}
	}

	//disable copy constructor
	buffer_base(const buffer_base& obj) = delete;

	//disable operator=
	buffer_base& operator=(const buffer_base& obj) = delete;

	//destructor for derived classes
	virtual ~buffer_base() {
		if (alloc_self) {
			delete[] start;
		}
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
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 * @return リングバッファから読み込んだ数
	 */
	size_type read_array(size_type index, T *buf, size_type count, transform_func_t rdtrans, transform_func_t wrtrans) const {
		size_type n, ntrans, nwrite = 0;
		int res;

		index = get_index(index);
		while (count - nwrite > 0) {
			n = count - nwrite;
			n = std::min(n, nelements - index);

			res = rdtrans(&buf[nwrite], &start[index], n, &ntrans);
			if (res != 0) {
				//error
				break;
			}

			nwrite += ntrans;
			index += ntrans;
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
	 * @param rdtrans read 用の変換関数
	 * @param wrtrans write 用の変換関数
	 * @return リングバッファに書き込んだ数
	 */
	size_type write_array(size_type index, const T *buf, size_type count, transform_func_t rdtrans, transform_func_t wrtrans) {
		size_type n, ntrans, nread = 0;
		int res;

		index = get_index(index);
		while (count - nread > 0) {
			n = count - nread;
			n = std::min(n, nelements - index);

			res = wrtrans(&start[index], &buf[nread], n, &ntrans);
			if (res != 0) {
				//error
				break;
			}

			nread += ntrans;
			index += ntrans;
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
	T *start;
	size_type nelements;
	bool alloc_self;
};

} //namespace mf

#endif //BUFFER_BASE_HPP__
