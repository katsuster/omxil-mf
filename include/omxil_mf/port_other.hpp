﻿#ifndef OMX_MF_PORT_OTHER_HPP__
#define OMX_MF_PORT_OTHER_HPP__

#include <OMX_Component.h>
#include <OMX_Core.h>
#include <OMX_Other.h>

#include <omxil_mf/base.h>
#include <omxil_mf/port.hpp>

namespace mf {

class OMX_MF_API_CLASS port_other : public port {
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
	 * <pre>
	 * struct OMX_OTHER_PORTDEFINITIONTYPE {
	 *     OMX_OTHER_FORMATTYPE eFormat;
	 * }
	 * </pre>
	 *
	 * @return Port definition data
	 */
	virtual const OMX_PARAM_PORTDEFINITIONTYPE *get_definition() const override;

	/**
	 * Set OpenMAX IL definition data of this port.
	 *
	 * コンポーネントや、ポート自身が definition data を設定する場合、
	 * こちらのメンバ関数を呼び出します。
	 *
	 * OMX_PARAM_PORTDEFINITIONTYPE の各メンバに加え、
	 * format.other の各メンバに対応する、
	 * このポートのメンバ変数が更新されます。
	 *
	 * <pre>
	 * struct OMX_OTHER_PORTDEFINITIONTYPE {
	 *     OMX_OTHER_FORMATTYPE eFormat;
	 * }
	 * </pre>
	 *
	 * @param v Port definition data
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE set_definition(const OMX_PARAM_PORTDEFINITIONTYPE& v) override;

	/**
	 * Set OpenMAX IL definition data of this port by IL Client.
	 *
	 * OpenMAX IL クライアントから definition data を設定された場合、
	 * こちらのメンバ関数を呼び出します。
	 *
	 * OMX_PARAM_PORTDEFINITIONTYPE の各メンバに加え、
	 * format.other の各メンバに対応する、
	 * このポートのメンバ変数が更新されます。
	 *
	 * <pre>
	 * struct OMX_OTHER_PORTDEFINITIONTYPE {
	 *     OMX_OTHER_FORMATTYPE eFormat;
	 * }
	 * </pre>
	 *
	 * @param v Port definition data
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE set_definition_from_client(const OMX_PARAM_PORTDEFINITIONTYPE& v) override;

	/**
	 * ポートがサポートするデータの形式のインデクスを取得します。
	 *
	 * 指定した条件が複数のデータ形式に該当する場合、
	 * index の値が小さいデータ形式が優先されます。
	 *
	 * @param f   データ形式
	 * @param ind データ形式のインデクス、取得できなければ (~0)
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE get_port_format_index(const port_format& f, size_t *ind) const override;

	/**
	 * ポートがデフォルトでサポートするその他のデータ形式を取得します。
	 *
	 * @return データ形式へのポインタ
	 */
	virtual const OMX_OTHER_PARAM_PORTFORMATTYPE *get_default_format_other() const;

private:
	//下記メンバについては、直接設定できません
	//OMX_XXXXX_PARAM_PORTFORMATTYPE のリスト
	//（add_supported_format, get_supported_format）
	//デフォルトフォーマットの設定機能（set_default_format）
	//を使用してください
	//  OMX_OTHER_FORMATTYPE format;

};

} //namespace mf

#endif //OMX_MF_PORT_OTHER_HPP__
