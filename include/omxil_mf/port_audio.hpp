#ifndef OMX_MF_PORT_AUDIO_HPP__
#define OMX_MF_PORT_AUDIO_HPP__

#include <OMX_Component.h>
#include <OMX_Core.h>
#include <OMX_Audio.h>

#include <omxil_mf/port.hpp>

namespace mf {

class port_audio : public port {
public:
	//親クラス
	typedef port super;

	//disable default constructor
	port_audio() = delete;

	port_audio(int ind, component *c);

	virtual ~port_audio();

	//disable copy constructor
	port_audio(const port_audio& obj) = delete;

	//disable operator=
	port_audio& operator=(const port_audio& obj) = delete;

	virtual const char *get_name() const;

	virtual OMX_STRING get_mime_type() const;
	virtual void set_mime_type(OMX_STRING v);

	virtual OMX_NATIVE_DEVICETYPE get_native_render() const;
	virtual void set_native_render(OMX_NATIVE_DEVICETYPE v);

	virtual OMX_BOOL get_flag_error_concealment() const;
	virtual void set_flag_error_concealment(OMX_BOOL v);

	virtual OMX_AUDIO_CODINGTYPE get_encoding() const;

	/**
	 * Get OpenMAX IL definition data of this port.
	 *
	 * OMX_PARAM_PORTDEFINITIONTYPE の各メンバに加え、
	 * format.audio メンバにも設定値がセットされます。
	 *
	 * struct OMX_AUDIO_PORTDEFINITIONTYPE {
	 *     OMX_STRING cMIMEType;
	 *     OMX_NATIVE_DEVICETYPE pNativeRender;
	 *     OMX_BOOL bFlagErrorConcealment;
	 *     OMX_AUDIO_CODINGTYPE eEncoding;
	 * }
	 *
	 * @return port definition data of OpenMAX IL
	 */
	virtual const OMX_PARAM_PORTDEFINITIONTYPE *get_definition() const;

	/**
	 * Set OpenMAX IL definition data of this port.
	 *
	 * コンポーネントや、ポート自身が definition data を設定する場合、
	 * こちらのメンバ関数を呼び出します。
	 *
	 * OMX_PARAM_PORTDEFINITIONTYPE の各メンバに加え、
	 * format.audio の各メンバに対応する、
	 * このポートのメンバ変数が更新されます。
	 *
	 * struct OMX_AUDIO_PORTDEFINITIONTYPE {
	 *     OMX_STRING cMIMEType;
	 *     OMX_NATIVE_DEVICETYPE pNativeRender;
	 *     OMX_BOOL bFlagErrorConcealment;
	 *     OMX_AUDIO_CODINGTYPE eEncoding;
	 * }
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE set_definition(const OMX_PARAM_PORTDEFINITIONTYPE& v);

	/**
	 * Set OpenMAX IL definition data of this port by IL Client.
	 *
	 * OpenMAX IL クライアントから definition data を設定された場合、
	 * こちらのメンバ関数を呼び出します。
	 *
	 * OMX_PARAM_PORTDEFINITIONTYPE の各メンバに加え、
	 * format.audio の各メンバに対応する、
	 * このポートのメンバ変数が更新されます。
	 *
	 * struct OMX_AUDIO_PORTDEFINITIONTYPE {
	 *     OMX_STRING cMIMEType;
	 *     OMX_NATIVE_DEVICETYPE pNativeRender;
	 *     OMX_BOOL bFlagErrorConcealment;
	 *     OMX_AUDIO_CODINGTYPE eEncoding;
	 * }
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE set_definition_from_client(const OMX_PARAM_PORTDEFINITIONTYPE& v);

	/**
	 * ポートがサポートするデータの形式を取得します。
	 *
	 * @param index データ形式のインデックス
	 * @return データ形式へのポインタ、取得できなければ nullptr
	 */
	virtual const OMX_AUDIO_PARAM_PORTFORMATTYPE *get_supported_format(size_t index) const;

	/**
	 * ポートがサポートするデータの形式を追加します。
	 *
	 * @param f データ形式へのポインタ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE add_supported_format(const OMX_AUDIO_PARAM_PORTFORMATTYPE *f);

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
	virtual const OMX_AUDIO_PARAM_PORTFORMATTYPE *get_default_format() const;

	/**
	 * ポートがデフォルトでサポートするデータの形式を設定します。
	 *
	 * @param index データ形式のインデックス
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE set_default_format(size_t index);

private:
	OMX_STRING mime_type;
	OMX_NATIVE_DEVICETYPE native_render;
	OMX_BOOL flag_error_concealment;
	//下記メンバについては、直接設定できません
	//OMX_XXXXX_PARAM_PORTFORMATTYPE のリスト
	//（add_supported_format, get_supported_format）
	//デフォルトフォーマットの設定機能（set_default_format）
	//を使用してください
	//  OMX_AUDIO_CODINGTYPE encoding;

	std::vector<OMX_AUDIO_PARAM_PORTFORMATTYPE> formats;
	int default_format;

};

} //namespace mf

#endif //OMX_MF_PORT_AUDIO_HPP__
