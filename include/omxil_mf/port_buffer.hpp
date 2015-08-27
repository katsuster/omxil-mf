#ifndef OMX_MF_PORT_BUFFER_HPP__
#define OMX_MF_PORT_BUFFER_HPP__

#include <cstdint>
#include <sys/types.h>

#include <OMX_Component.h>
#include <OMX_Core.h>

namespace mf {

class component;
class port;

/**
 * ポートに渡された OpenMAX バッファです。
 *
 * 主に OMX_BUFFERHEADERTYPE にアクセスするためのラッパークラスです。
 *
 * OMX_BUFFERHEADERTYPE の定義は下記の通りです。
 *
 * <pre>
 * struct OMX_BUFFERHEADERTYPE {
 *     OMX_U32 nSize;
 *     OMX_VERSIONTYPE nVersion;
 *     OMX_U8* pBuffer;
 *     OMX_U32 nAllocLen;
 *     OMX_U32 nFilledLen;
 *     OMX_U32 nOffset;
 *     OMX_PTR pAppPrivate;
 *     OMX_PTR pPlatformPrivate;
 *     OMX_PTR pInputPortPrivate;
 *     OMX_PTR pOutputPortPrivate;
 *     OMX_HANDLETYPE hMarkTargetComponent;
 *     OMX_PTR pMarkData;
 *     OMX_U32 nTickCount;
 *     OMX_TICKS nTimeStamp;
 *     OMX_U32 nFlags;
 *     OMX_U32 nOutputPortIndex;
 *     OMX_U32 nInputPortIndex;
 * }
 * </pre>
 */
struct port_buffer {
	//バッファを所持しているポート
	port *p;
	//OMX_AllocateBuffer によって確保されたバッファであれば true
	//OMX_UseBuffer によるバッファであれば false
	bool f_allocate;
	//OpenMAX のバッファヘッダ
	OMX_BUFFERHEADERTYPE *header;
	//現在位置（初期値は nOffset）
	//NOTE:
	//OpenMAX IL 1.2.0: 3.1.3.9.2 EmptyBufferDone
	//OpenMAX IL 1.2.0: 3.1.3.9.3 FillBufferDone によると、
	//コンポーネントは渡されたバッファの
	//nOffset と nFilledLen を更新して返す、とあるが、
	//gst-omx など実際に変更して返すと変な動作をする奴らが多い？
	OMX_U32 index;

public:
	/**
	 * OpenMAX バッファに格納されている要素数を取得します。
	 */
	bool size() const;

	/**
	 * OpenMAX バッファが空かどうかを返します。
	 *
	 * @return 空なら true、空でなければ false
	 */
	bool empty() const;

	/**
	 * OpenMAX バッファが満杯かどうかを返します。
	 *
	 * @return 満杯なら true、満杯でなければ false
	 */
	bool full() const;

	/**
	 * OpenMAX バッファの残り要素数を取得します。
	 *
	 * 読み込みのときは、
	 * 現在位置から読み出せる最大の要素数を返します。
	 *
	 * <pre>
	 * pBuffer
	 * |<-- nAllocLen ------------------------------------>|
	 * |<-- index   -->|<-- nFilledLen -->|                |
	 * |               |<-- remain()   -->|                |
	 * |               |                  |                |
	 * |xxxxxxxxxxxxxxx|++++++++++++++++++|xxxxxxxxxxxxxxxx|
	 *
	 * xxx: 無効なデータ、パディングなど
	 * +++: 有効なデータ、コンポーネントが処理すべきデータ
	 * </pre>
	 *
	 * 書き込みのときは、
	 * 現在位置から書き込める最大の要素数を返します。
	 *
	 * <pre>
	 * pBuffer
	 * |<-- nAllocLen ------------------------------------>|
	 * |<-- index   -->|<-- nFilledLen -->|<-- remain() -->|
	 * |               |                  |                |
	 * |xxxxxxxxxxxxxxx|******************|................|
	 *
	 * xxx: 無効なデータ、パディングなど
	 * ***: コンポーネントが生成したデータ
	 * ...: 空き領域
	 * </pre>
	 *
	 * 現在位置については get_index()/set_index() を参照ください。
	 *
	 * @return 残り要素数
	 */
	size_t remain() const;

	/**
	 * バッファの要素を読まずに飛ばします。
	 *
	 * 出力ポートを読み飛ばそうとした場合、
	 * 現在位置は変更されず、0 を返します。
	 *
	 * @param count 読み飛ばす要素数
	 * @return 読み飛ばした要素数
	 */
	size_t skip(size_t count);

	/**
	 * OpenMAX バッファの現在位置から、配列を読み込みます。
	 *
	 * 出力ポートから読み込もうとした場合、
	 * 現在位置、バッファは変更されず、0 を返します。
	 *
	 * @param buf   バッファから読み込んだ要素を格納する配列
	 * @param count バッファから読み込む数
	 * @return バッファから読み込んだ数
	 */
	size_t read_array(uint8_t *buf, size_t count);

	/**
	 * OpenMAX バッファの現在位置に、配列を書き込みます。
	 *
	 * 入力ポートに書き込もうとした場合、
	 * 現在位置、バッファは変更されず、0 を返します。
	 *
	 * @param buf   バッファに書き込む要素を格納する配列
	 * @param count バッファに書き込む数
	 * @return バッファに書き込んだ数
	 */
	size_t write_array(uint8_t *buf, size_t count);

	/**
	 * バッファの現在位置を指すポインタを取得します。
	 *
	 * @return バッファの現在位置を指すポインタ
	 */
	uint8_t *get_ptr();

	/**
	 * ダンプをデバッグ出力します。
	 *
	 * @param msg 一緒に出力する文字列
	 */
	void dump(const char *msg);

private:
	/**
	 * OpenMAX バッファの現在位置を取得します。
	 *
	 * OpenMAX IL 1.1.2: 3.1.2.9.2 EmptyThisBuffer によれば、
	 * 読み出しの際は、読み出した分だけ index を進め、
	 * nFilledLen を減じてから通知する仕様のため、
	 * 現在位置に index を使用します。
	 *
	 * <pre>
	 * pBuffer
	 * |<-- nAllocLen ------------------------------------>|
	 * |<-- index   -->|<-- nFilledLen -->|                |
	 * |               |                  |                |
	 * |xxxxxxxxxxxxxxx|++++++++++++++++++|xxxxxxxxxxxxxxxx|
	 *                 |
	 *                 現在位置
	 *
	 * xxx: 無効なデータ、パディングなど
	 * +++: 有効なデータ、コンポーネントが処理すべきデータ
	 * </pre>
	 *
	 * OpenMAX IL 1.1.2: 3.1.2.9.3 FillBufferDone によれば、
	 * 書き込みの際は index は書き込んだデータの先頭を指し、
	 * nFilledLen は書き込んだデータの総量を通知する仕様のため、
	 * 現在位置に index + nFilledLen を使用します。
	 *
	 * <pre>
	 * pBuffer
	 * |<-- nAllocLen ------------------------------------>|
	 * |<-- index ---->|<-- nFilledLen -->|                |
	 * |               |                  |                |
	 * |xxxxxxxxxxxxxxx|******************|................|
	 *                                    |
	 *                                    現在位置
	 *
	 * xxx: 無効なデータ、パディングなど
	 * ***: コンポーネントが生成したデータ
	 * ...: 空き領域
	 * </pre>
	 *
	 * @return 現在の読み込み、または、書き込み位置
	 */
	size_t get_index() const;

	/**
	 * OpenMAX バッファの現在位置を設定します。
	 *
	 * OpenMAX IL 1.1.2: 3.1.2.9.2 EmptyThisBuffer によれば、
	 * 読み出しの際は、読み出した分だけ index を進め、
	 * nFilledLen を減じてから通知する仕様のため、
	 * 現在位置に index を使用します。
	 * そのため index と nFilledLen を合わせて更新します。
	 *
	 * <pre>
	 * pBuffer
	 * |<-- nAllocLen -------------------------------------------->|
	 * |<-- index ---->|<-- nFilledLen ---------->|                |
	 * |               |                          |                |
	 * |xxxxxxxxxxxxxxx|++++++++++++++++++++++++++|xxxxxxxxxxxxxxxx|
	 *                 |
	 *                 現在位置
	 *
	 * X バイト読み込んだ場合（X が負の値でも成り立ちます）、
	 *
	 * |<-- nAllocLen -------------------------------------------->|
	 * |<-- index + X ---->|<-- nFilledLen - X -->|                |
	 * |                   |                      |                |
	 * |xxxxxxxxxxxxxxx|***|++++++++++++++++++++++|xxxxxxxxxxxxxxxx|
	 *                 |   |
	 *                 |   新たな現在位置
	 *                 元の現在位置
	 *
	 * xxx: 無効なデータ、パディングなど
	 * ***: 今回コンポーネントが処理したデータ
	 * +++: 有効なデータ、コンポーネントが処理すべきデータ
	 * </pre>
	 *
	 * OpenMAX IL 1.1.2: 3.1.2.9.3 FillBufferDone によれば、
	 * 書き込みの際は index は書き込んだデータの先頭を指し、
	 * nFilledLen は書き込んだデータの総量を通知する仕様のため、
	 * 現在位置に index + nFilledLen を使用します。
	 * そのため index は変更せず、nFilledLen のみを更新します。
	 *
	 * <pre>
	 * pBuffer
	 * |<-- nAllocLen ------------------------------------>|
	 * |<-- index ---->|<-- nFilledLen -->|                |
	 * |               |                  |                |
	 * |xxxxxxxxxxxxxxx|******************|................|
	 *                                    |
	 *                                    現在位置
	 *
	 * X バイト書き込んだ場合（X が負の値でも成り立ちます）、
	 *
	 * |<-- nAllocLen ------------------------------------>|
	 * |<-- index ---->|<-- nFilledLen + X -->|            |
	 * |               |                      |            |
	 * |xxxxxxxxxxxxxxx|******************|###|............|
	 *                                    |   |
	 *                                    |   新たな現在位置
	 *                                    元の現在位置
	 *
	 * xxx: 無効なデータ、パディングなど
	 * ***: コンポーネントが生成したデータ
	 * ###: 今回コンポーネントが生成したデータ
	 * ...: 空き領域
	 * </pre>
	 *
	 * @param new_index 新しい読み込み、または、書き込み位置
	 */
	void set_index(size_t new_index);

};

} //namespace mf

#endif //OMX_MF_PORT_BUFFER_HPP__
