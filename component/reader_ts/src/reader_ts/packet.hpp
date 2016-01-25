
//にほんご

#include <cstdint>
#include <cinttypes>

#include <omxil_mf/ring/ring_buffer.hpp>
#include <omxil_mf/ring/bounded_buffer.hpp>
#include <omxil_mf/ring/bit_stream.hpp>

#include "unit.hpp"

namespace mf {

/**
 * ヘッダ、ペイロードから構成されるデータを表すクラスです。
 *
 * 個々のパケットは何らかの ID を持ちます。
 */
class packet : public unit {
public:
	explicit packet(uint64_t unique_id);

	virtual ~packet();

	/**
	 * パケットの ID を取得する。
	 *
	 * @return パケットの ID
	 */
	virtual uint64_t get_id() const;

	/**
	 * パケットのヘッダを取得する。
	 *
	 * @return パケットのヘッダ
	 */
	virtual const unit& header() const = 0;

	/**
	 * パケットのヘッダを取得する。
	 *
	 * @return パケットのヘッダ
	 */
	virtual unit& header() = 0;

	/**
	 * パケットのペイロードを取得する。
	 *
	 * @return パケットのペイロード
	 */
	virtual const uint8_t *payload() const = 0;

	/**
	 * パケットのペイロードを取得する。
	 *
	 * @return パケットのペイロード
	 */
	virtual uint8_t *payload() = 0;

	/**
	 * パケットのペイロードのサイズを取得する。
	 *
	 * @return パケットのペイロードのサイズ
	 */
	virtual size_t payload_size() const = 0;

	virtual void dump() const;

	static void dump(const packet& p);

private:
	uint64_t id;

};

}
