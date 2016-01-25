
//にほんご

#include <cstdint>
#include <cinttypes>

#include <omxil_mf/ring/ring_buffer.hpp>
#include <omxil_mf/ring/bounded_buffer.hpp>
#include <omxil_mf/ring/bit_stream.hpp>

#define FORMAT_STRING     "    %40s: 0x%08x\n"
#define FORMAT_STRING_LL  "    %40s: 0x%08" PRIx64 "\n"

namespace mf {

typedef ring_buffer<uint8_t *, uint8_t> avring;
typedef bounded_buffer<avring, uint8_t> avbuffer;
typedef bit_stream<avring::iterator> avbit;

/**
 * ビットストリームに対して読み取り、書き込み可能なデータを表すクラスです。
 *
 * 何らかのサイズを持ちます。
 *
 * サイズはビットストリームから決めても構いませんし、
 * コンストラクタなどから与えても構いません。
 */
class unit {
public:
	explicit unit();

	virtual ~unit();

	/**
	 * ユニットのサイズを取得する。
	 *
	 * @return ユニットのサイズ
	 */
	virtual uint64_t size() const;

	/**
	 * ユニットのサイズを設定する。
	 *
	 * @param newsize 新たなユニットのサイズ
	 */
	virtual void size(uint64_t newsize);

	/**
	 * バイト列からユニットを読み込む。
	 *
	 * @param b バイト列
	 */
	virtual void read(avbuffer& b);

	/**
	 * バイト列からユニットを読み込む。
	 *
	 * @param b バイト列
	 * @param p 読み込むユニットへの参照
	 */
	static void read(avbuffer& b, unit& p);

	/**
	 * バイト列にユニットを書き込む。
	 *
	 * @param b バイト列
	 */
	virtual void write(avbuffer& b);

	/**
	 * バイト列からユニットを読み込む。
	 *
	 * @param b バイト列
	 * @param p 書き込むユニットへの参照
	 */
	static void write(avbuffer& b, unit& p);

	/**
	 * ユニットの情報を表示する。
	 */
	virtual void dump() const;

	/**
	 * ユニットの情報を表示する。
	 *
	 * @param p ユニットへの参照
	 */
	static void dump(const unit& p);

private:
	uint64_t unit_size;

};

}
