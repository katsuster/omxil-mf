#ifndef OMX_MF_PORT_VIDEO_HPP__
#define OMX_MF_PORT_VIDEO_HPP__

#include <OMX_Component.h>
#include <OMX_Core.h>
#include <OMX_Video.h>

#include <omxil_mf/port.hpp>

namespace mf {

class port_video : public port {
public:
	//親クラス
	typedef port super;

	//disable default constructor
	port_video() = delete;

	port_video(int ind, component *c);

	virtual ~port_video();

	//disable copy constructor
	port_video(const port_video& obj) = delete;

	//disable operator=
	port_video& operator=(const port_video& obj) = delete;

	virtual const char *get_name() const;

	virtual OMX_STRING get_mime_type() const;
	virtual void set_mime_type(OMX_STRING v);

	virtual OMX_NATIVE_DEVICETYPE get_native_render() const;
	virtual void set_native_render(OMX_NATIVE_DEVICETYPE v);

	virtual OMX_U32 get_frame_width() const;
	virtual void set_frame_width(OMX_U32 v);

	virtual OMX_U32 get_frame_height() const;
	virtual void set_frame_height(OMX_U32 v);

	virtual OMX_S32 get_stride() const;
	virtual void set_stride(OMX_S32 v);

	virtual OMX_U32 get_slice_height() const;
	virtual void set_slice_height(OMX_U32 v);

	virtual OMX_U32 get_bitrate() const;
	virtual void set_bitrate(OMX_U32 v);

	virtual OMX_U32 get_framerate() const;

	virtual OMX_BOOL get_flag_error_concealment() const;
	virtual void set_flag_error_concealment(OMX_BOOL v);

	virtual OMX_VIDEO_CODINGTYPE get_compression_format() const;

	virtual OMX_COLOR_FORMATTYPE get_color_format() const;

	virtual OMX_NATIVE_WINDOWTYPE get_native_window() const;
	virtual void set_native_window(OMX_NATIVE_WINDOWTYPE v);

	/**
	 * Get OpenMAX IL definition data of this port.
	 *
	 * OMX_PARAM_PORTDEFINITIONTYPE の各メンバに加え、
	 * format.video メンバにも設定値がセットされます。
	 *
	 * <pre>
	 * struct OMX_VIDEO_PORTDEFINITIONTYPE {
	 *     OMX_STRING cMIMEType;
	 *     OMX_NATIVE_DEVICETYPE pNativeRender;
	 *     OMX_U32 nFrameWidth;
	 *     OMX_U32 nFrameHeight;
	 *     OMX_S32 nStride;
	 *     OMX_U32 nSliceHeight;
	 *     OMX_U32 nBitrate;
	 *     OMX_U32 xFramerate;
	 *     OMX_BOOL bFlagErrorConcealment;
	 *     OMX_VIDEO_CODINGTYPE eCompressionFormat;
	 *     OMX_COLOR_FORMATTYPE eColorFormat;
	 *     OMX_NATIVE_WINDOWTYPE pNativeWindow;
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
	 * format.video の各メンバに対応する、
	 * このポートのメンバ変数が更新されます。
	 *
	 * <pre>
	 * struct OMX_VIDEO_PORTDEFINITIONTYPE {
	 *     OMX_STRING cMIMEType;
	 *     OMX_NATIVE_DEVICETYPE pNativeRender;
	 *     OMX_U32 nFrameWidth;
	 *     OMX_U32 nFrameHeight;
	 *     OMX_S32 nStride;
	 *     OMX_U32 nSliceHeight;
	 *     OMX_U32 nBitrate;
	 *     OMX_U32 xFramerate;
	 *     OMX_BOOL bFlagErrorConcealment;
	 *     OMX_VIDEO_CODINGTYPE eCompressionFormat;
	 *     OMX_COLOR_FORMATTYPE eColorFormat;
	 *     OMX_NATIVE_WINDOWTYPE pNativeWindow;
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
	 * format.video の各メンバに対応する、
	 * このポートのメンバ変数が更新されます。
	 *
	 * <pre>
	 * struct OMX_VIDEO_PORTDEFINITIONTYPE {
	 *     OMX_STRING cMIMEType;
	 *     OMX_NATIVE_DEVICETYPE pNativeRender;
	 *     OMX_U32 nFrameWidth;
	 *     OMX_U32 nFrameHeight;
	 *     OMX_S32 nStride;
	 *     OMX_U32 nSliceHeight;
	 *     OMX_U32 nBitrate;
	 *     OMX_U32 xFramerate;
	 *     OMX_BOOL bFlagErrorConcealment;
	 *     OMX_VIDEO_CODINGTYPE eCompressionFormat;
	 *     OMX_COLOR_FORMATTYPE eColorFormat;
	 *     OMX_NATIVE_WINDOWTYPE pNativeWindow;
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
	 * ポートがデフォルトでサポートするビデオデータ形式を取得します。
	 *
	 * @return データ形式へのポインタ
	 */
	virtual const OMX_VIDEO_PARAM_PORTFORMATTYPE *get_default_format_video() const;

private:
	OMX_STRING mime_type;
	OMX_NATIVE_DEVICETYPE native_render;
	OMX_U32 frame_width;
	OMX_U32 frame_height;
	OMX_S32 stride;
	OMX_U32 slice_height;
	OMX_U32 bitrate;
	OMX_BOOL flag_error_concealment;
	//下記メンバについては、直接設定できません
	//OMX_XXXXX_PARAM_PORTFORMATTYPE のリスト
	//（add_supported_format, get_supported_format）
	//デフォルトフォーマットの設定機能（set_default_format）
	//を使用してください
	//  OMX_U32 framerate;
	//  OMX_VIDEO_CODINGTYPE compression_format;
	//  OMX_COLOR_FORMATTYPE color_format;
	OMX_NATIVE_WINDOWTYPE native_window;

};

} //namespace mf

#endif //OMX_MF_PORT_VIDEO_HPP__
