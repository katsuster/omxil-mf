#ifndef OMX_MF_PORT_OTHER_HPP__
#define OMX_MF_PORT_OTHER_HPP__

#include <OMX_Component.h>
#include <OMX_Core.h>
#include <OMX_Other.h>

#include <omxil_mf/port.hpp>

namespace mf {

class port_other : public port {
public:
	//親クラス
	typedef port super;

	//disable default constructor
	port_other() = delete;

	port_other(int ind, component *c);

	virtual ~port_other();

	//disable copy constructor
	port_other(const port_other& obj) = delete;

	//disable operator=
	port_other& operator=(const port_other& obj) = delete;

	virtual const char *get_name() const;

	virtual OMX_OTHER_FORMATTYPE get_format() const;

	/**
	 * Get OpenMAX IL definition data of this port.
	 *
	 * OMX_PARAM_PORTDEFINITIONTYPE の各メンバに加え、
	 * format.other メンバにも設定値がセットされます。
	 *
	 * struct OMX_OTHER_PORTDEFINITIONTYPE {
	 *     OMX_OTHER_FORMATTYPE eFormat;
	 * }
	 *
	 * @return port definition data of OpenMAX IL
	 */
	virtual const OMX_PARAM_PORTDEFINITIONTYPE *get_definition() const;

	/**
	 * ポートがサポートするデータの形式を取得します。
	 *
	 * @param index データ形式のインデックス
	 * @return データ形式へのポインタ、取得できなければ nullptr
	 */
	virtual const OMX_OTHER_PARAM_PORTFORMATTYPE *get_supported_format(size_t index) const;

	/**
	 * ポートがサポートするデータの形式を追加します。
	 *
	 * @param f データ形式へのポインタ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE add_supported_format(const OMX_OTHER_PARAM_PORTFORMATTYPE *f);

	/**
	 * ポートがサポートするデータの形式を削除します。
	 *
	 * @param index データ形式のインデックス
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE remove_supported_format(size_t index);

	/**
	 * ポートがデフォルトでサポートするデータの形式を取得します。
	 *
	 * @param index データ形式のインデックス
	 * @return データ形式へのポインタ
	 */
	virtual const OMX_OTHER_PARAM_PORTFORMATTYPE *get_default_format() const;

	/**
	 * ポートがデフォルトでサポートするデータの形式を設定します。
	 *
	 * @param index データ形式のインデックス
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE set_default_format(size_t index);

private:
	//下記メンバについては、直接設定できません
	//OMX_XXXXX_PARAM_PORTFORMATTYPE のリスト
	//（add_supported_format, get_supported_format）
	//デフォルトフォーマットの設定機能（set_default_format）
	//を使用してください
	//  OMX_OTHER_FORMATTYPE format;

	std::vector<OMX_OTHER_PARAM_PORTFORMATTYPE> formats;
	int default_format;

};

} //namespace mf

#endif //OMX_MF_PORT_OTHER_HPP__
