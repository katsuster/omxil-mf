#ifndef OMX_MF_PORT_HPP__
#define OMX_MF_PORT_HPP__

#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>

#include <OMX_Component.h>
#include <OMX_Core.h>

#include <omxil_mf/ring/ring_buffer.hpp>
#include <omxil_mf/ring/bounded_buffer.hpp>
#include <omxil_mf/port_format.hpp>


//OpenMAX バッファを受け渡すバッファの深さ
#define OMX_MF_BUFS_DEPTH    10


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




class port {
public:
	//親クラス
	//typedef xxxx super;

	//disable default constructor
	port() = delete;

	port(int ind, component *c);

	virtual ~port();

	//disable copy constructor
	port(const port& obj) = delete;

	//disable operator=
	port& operator=(const port& obj) = delete;

	virtual const char *get_name() const;

	/**
	 * ポートからの読み出し、または書き込みを禁止し、
	 * 全ての待機しているスレッドを強制的に解除（シャットダウン）します。
	 *
	 * 強制解除されたスレッドは runtime_error をスローします。
	 *
	 * @param rd 以降の読み出しを禁止し、
	 * 	読み出しの待機状態を解除する場合は true、
	 * 	変更しない場合は false を指定します
	 * @param wr 以降の書き込みを禁止し、
	 * 	書き込みの待機状態を解除する場合は true、
	 * 	変更しない場合は false を指定します
	 */
	virtual void shutdown(bool rd, bool wr);

	/**
	 * シャットダウン処理を中止し、
	 * ポートからの読み出し、または書き込みを許可します。
	 *
	 * FIXME: シャットダウン処理の中止後、
	 * ポートがシャットダウン前と同様に動作するかどうかは、
	 * 各ポートの実装に依存します。
	 *
	 * @param rd 以降の読み出しを許可する場合は true、
	 * 	変更しない場合は false を指定します
	 * @param wr 以降の書き込みを許可する場合は true、
	 * 	変更しない場合は false を指定します
	 */
	virtual void abort_shutdown(bool rd, bool wr);

	/**
	 * 読み出し側のシャットダウン処理中かどうかを取得します。
	 *
	 * @return 処理中ならば true、そうでなければ false
	 */
	virtual bool is_shutting_read();

	/**
	 * 書き込み側のシャットダウン処理中かどうかを取得します。
	 *
	 * @return 処理中ならば true、そうでなければ false
	 */
	virtual bool is_shutting_write();

	/**
	 * ポートが所属するコンポーネントを取得します。
	 *
	 * @return コンポーネントへのポインタ、属していなければ nullptr
	 */
	virtual const component *get_component() const;

	/**
	 * ポートが所属するコンポーネントを取得します。
	 *
	 * @return コンポーネントへのポインタ、属していなければ nullptr
	 */
	virtual component *get_component();

	/**
	 * ポートのインデックスを取得します。
	 *
	 * @return ポートのインデックス
	 */
	virtual OMX_U32 get_port_index() const;

	/**
	 * ポートのインデックスを設定します。
	 *
	 * @param v ポートのインデックス
	 */
	virtual void set_port_index(OMX_U32 v);

	virtual OMX_DIRTYPE get_dir() const;
	virtual void set_dir(OMX_DIRTYPE v);

	virtual OMX_U32 get_buffer_count_actual() const;
	virtual void set_buffer_count_actual(OMX_U32 v);

	virtual OMX_U32 get_buffer_count_min() const;
	virtual void set_buffer_count_min(OMX_U32 v);

	virtual OMX_U32 get_buffer_size() const;
	virtual void set_buffer_size(OMX_U32 v);

	virtual OMX_BOOL get_enabled() const;
	virtual void set_enabled(OMX_BOOL v);

	virtual OMX_BOOL get_populated() const;
	virtual void set_populated(OMX_BOOL v);

	/**
	 * ポートが 'populated' になるまで待ちます。
	 *
	 * 'populated' とはポートが enabled であり、なおかつ、
	 * nBufferCountActual で示す数だけ、
	 * OMX_UseBuffer または OMX_AllocateBuffer にてバッファが確保され、
	 * OMX_StateIdle に遷移する準備ができている状態を指します。
	 *
	 * <pre>
	 * バッファ確保数        | populated | no buffer | 状態
	 * ----------------------+-----------+-----------+---------------------
	 * 0                     | No        | Yes       | StateLoaded 遷移可能
	 * < nBufferCountActual  | No        | No        |
	 * >= nBufferCountActual | Yes       | No        | StateIdle 遷移可能
	 * ----------------------+-----------+-----------+---------------------
	 * </pre>
	 *
	 * @param v 待ちたい状態
	 * 	OMX_TRUE なら populated になるまで待ち、
	 * 	OMX_FALSE なら populated ではなくなるまで待ちます。
	 */
	virtual void wait_populated(OMX_BOOL v);

	virtual OMX_PORTDOMAINTYPE get_domain() const;
	virtual void set_domain(OMX_PORTDOMAINTYPE v);

	virtual OMX_BOOL get_buffers_contiguous() const;
	virtual void set_buffers_contiguous(OMX_BOOL v);

	virtual OMX_U32 get_buffer_alignment() const;
	virtual void set_buffer_alignment(OMX_U32 v);


	virtual OMX_BOOL get_no_buffer() const;
	virtual void set_no_buffer(OMX_BOOL v);

	/**
	 * ポートのバッファが全て解放されるまで待ちます。
	 *
	 * 注: 'no buffer' はこのライブラリ独自の用語です。
	 * OpenMAX IL の用語ではありません。
	 *
	 * 'no buffer' とはポートが enabled であり、なおかつ、
	 * OMX_FreeBuffer にて全てのバッファが解放され、
	 * OMX_StateLoaded に遷移する準備ができている状態を指します。
	 *
	 * <pre>
	 * バッファ確保数        | populated | no buffer | 状態
	 * ----------------------+-----------+-----------+---------------------
	 * 0                     | No        | Yes       | StateLoaded 遷移可能
	 * < nBufferCountActual  | No        | No        |
	 * >= nBufferCountActual | Yes       | No        | StateIdle 遷移可能
	 * ----------------------+-----------+-----------+---------------------
	 * </pre>
	 *
	 * @param v 待ちたい状態
	 * 	OMX_TRUE なら no buffer になるまで待ち、
	 * 	OMX_FALSE なら no buffer ではなくなるまで待ちます。
	 */
	virtual void wait_no_buffer(OMX_BOOL v);

	/**
	 * ポートの 'populated', 'no buffer' 状態を更新します。
	 *
	 * 注: 'no buffer' はこのライブラリ独自の用語です。
	 * OpenMAX IL の用語ではありません。
	 *
	 * 'populated' とはポートが enabled であり、なおかつ、
	 * nBufferCountActual で示す数だけ、
	 * OMX_UseBuffer または OMX_AllocateBuffer にてバッファが確保され、
	 * OMX_StateIdle に遷移する準備ができている状態を指します。
	 *
	 * 'no buffer' とはポートが enabled であり、なおかつ、
	 * OMX_FreeBuffer にて全てのバッファが解放され、
	 * OMX_StateLoaded に遷移する準備ができている状態を指します。
	 *
	 * <pre>
	 * バッファ確保数        | populated | no buffer | 状態
	 * ----------------------+-----------+-----------+---------------------
	 * 0                     | No        | Yes       | StateLoaded 遷移可能
	 * < nBufferCountActual  | No        | No        |
	 * >= nBufferCountActual | Yes       | No        | StateIdle 遷移可能
	 * ----------------------+-----------+-----------+---------------------
	 * </pre>
	 */
	virtual void update_buffer_status();

	/**
	 * 全てのバッファを EmptyBufferDone あるいは FillBufferDone にて、
	 * 返却するまで待ちます。
	 */
	virtual void wait_buffer_returned();

	/**
	 * Get OpenMAX IL definition data of this port.
	 *
	 * OMX_PARAM_PORTDEFINITIONTYPE の各メンバに設定値がセットされます。
	 * ただし format メンバは 0 で埋められます。
	 *
	 * <pre>
	 * struct OMX_PARAM_PORTDEFINITIONTYPE {
	 *     OMX_U32 nSize;
	 *     OMX_VERSIONTYPE nVersion;
	 *     OMX_U32 nPortIndex;
	 *     OMX_DIRTYPE eDir;
	 *     OMX_U32 nBufferCountActual;
	 *     OMX_U32 nBufferCountMin;
	 *     OMX_U32 nBufferSize;
	 *     OMX_BOOL bEnabled;
	 *     OMX_BOOL bPopulated;
	 *     OMX_PORTDOMAINTYPE eDomain;
	 *     union {
	 *         OMX_AUDIO_PORTDEFINITIONTYPE audio;
	 *         OMX_VIDEO_PORTDEFINITIONTYPE video;
	 *         OMX_IMAGE_PORTDEFINITIONTYPE image;
	 *         OMX_OTHER_PORTDEFINITIONTYPE other;
	 *     } format;
	 *     OMX_BOOL bBuffersContiguous;
	 *     OMX_U32 nBufferAlignment;
	 * }
	 * </pre>
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
	 * OMX_PARAM_PORTDEFINITIONTYPE の各メンバに対応して、
	 * このポートのメンバ変数が更新されます。
	 * ただし nSize, nVersion, format メンバは無視されます。
	 *
	 * <pre>
	 * struct OMX_PARAM_PORTDEFINITIONTYPE {
	 *     OMX_U32 nSize;
	 *     OMX_VERSIONTYPE nVersion;
	 *     OMX_U32 nPortIndex;
	 *     OMX_DIRTYPE eDir;
	 *     OMX_U32 nBufferCountActual;
	 *     OMX_U32 nBufferCountMin;
	 *     OMX_U32 nBufferSize;
	 *     OMX_BOOL bEnabled;
	 *     OMX_BOOL bPopulated;
	 *     OMX_PORTDOMAINTYPE eDomain;
	 *     union {
	 *         OMX_AUDIO_PORTDEFINITIONTYPE audio;
	 *         OMX_VIDEO_PORTDEFINITIONTYPE video;
	 *         OMX_IMAGE_PORTDEFINITIONTYPE image;
	 *         OMX_OTHER_PORTDEFINITIONTYPE other;
	 *     } format;
	 *     OMX_BOOL bBuffersContiguous;
	 *     OMX_U32 nBufferAlignment;
	 * }
	 * </pre>
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
	 * OMX_PARAM_PORTDEFINITIONTYPE の各メンバに対応した、
	 * このポートのメンバ変数が更新されます。
	 * ただし nBufferCountActual メンバ以外は全て無視されます。
	 *
	 * <pre>
	 * struct OMX_PARAM_PORTDEFINITIONTYPE {
	 *     OMX_U32 nSize;
	 *     OMX_VERSIONTYPE nVersion;
	 *     OMX_U32 nPortIndex;
	 *     OMX_DIRTYPE eDir;
	 *     OMX_U32 nBufferCountActual;
	 *     OMX_U32 nBufferCountMin;
	 *     OMX_U32 nBufferSize;
	 *     OMX_BOOL bEnabled;
	 *     OMX_BOOL bPopulated;
	 *     OMX_PORTDOMAINTYPE eDomain;
	 *     union {
	 *         OMX_AUDIO_PORTDEFINITIONTYPE audio;
	 *         OMX_VIDEO_PORTDEFINITIONTYPE video;
	 *         OMX_IMAGE_PORTDEFINITIONTYPE image;
	 *         OMX_OTHER_PORTDEFINITIONTYPE other;
	 *     } format;
	 *     OMX_BOOL bBuffersContiguous;
	 *     OMX_U32 nBufferAlignment;
	 * }
	 * </pre>
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE set_definition_from_client(const OMX_PARAM_PORTDEFINITIONTYPE& v);

	/**
	 * ポートがサポートするデータ形式を追加します。
	 *
	 * @param f データ形式へのポインタ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE add_supported_format(const port_format& f);

	/**
	 * ポートがサポートするオーディオデータ形式を追加します。
	 *
	 * @param f データ形式へのポインタ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE add_supported_format(const OMX_AUDIO_PARAM_PORTFORMATTYPE *f);

	/**
	 * ポートがサポートするビデオデータ形式を追加します。
	 *
	 * @param f データ形式へのポインタ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE add_supported_format(const OMX_VIDEO_PARAM_PORTFORMATTYPE *f);

	/**
	 * ポートがサポートする画像データ形式を追加します。
	 *
	 * @param f データ形式へのポインタ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE add_supported_format(const OMX_IMAGE_PARAM_PORTFORMATTYPE *f);

	/**
	 * ポートがサポートするその他のデータ形式を追加します。
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
	 * ポートがサポートするデータの形式を取得します。
	 *
	 * 取得した port_format は Audio/Video/Image/Other いずれかの、
	 * OMX_XXXXX_PARAM_PORTFORMATTYPE を保持しています。
	 *
	 * 下記のようにすれば必要な種類の PORTFORMATTYPE を取得できます。
	 *
	 * <pre>
	 * const port_format *pf = get_supported_format(index);
	 * //If you need audio port format type
	 * const OMX_AUDIO_PARAM_PORTFORMATTYPE *pf_audio = pf->get_audio();
	 * </pre>
	 *
	 * @param index データ形式のインデックス
	 * @return データ形式へのポインタ、取得できなければ nullptr
	 */
	virtual const port_format *get_supported_format(size_t index) const;

	/**
	 * ポートがデフォルトでサポートするデータの形式を取得します。
	 *
	 * デフォルトに指定したフォーマットの index を
	 * default_format_index と置いたとき、
	 * 下記の呼び出しに等しいです。
	 *
	 * <pre>
	 * get_supported_format(default_format_index);
	 * </pre>
	 *
	 * @param index データ形式のインデックス
	 * @return データ形式へのポインタ
	 */
	virtual const port_format *get_default_format() const;

	/**
	 * ポートがデフォルトでサポートするデータの形式を設定します。
	 *
	 * @param index データ形式のインデックス
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE set_default_format(size_t index);

	/**
	 * ポートを無効にします。
	 *
	 * empty_buffer または fill_buffer によって渡されているが、
	 * 未処理のバッファは全て返却されます。
	 *
	 * 確保していたバッファを全て解放します。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE disable_port();

	/**
	 * ポートを有効にします。
	 *
	 * use_buffer または allocate_buffer によって、
	 * 確保すべきバッファが全て確保されるまで、待機します。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE enable_port();

	/**
	 * 一時的にポートへのバッファ処理要求を禁止し、
	 * 未処理のバッファを全て返却（フラッシュ）する準備をします。
	 *
	 * empty_buffer または fill_buffer によって渡されているが、
	 * 未処理のバッファは全て返却されます。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE begin_flush();

	/**
	 * 未処理のバッファを全て返却（フラッシュ）します。
	 *
	 * empty_buffer または fill_buffer によって渡されているが、
	 * 未処理のバッファは全て返却されます。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE flush_buffers();

	/**
	 * ポートへのバッファ処理要求を許可します。
	 *
	 * 未処理のバッファを全て返却（フラッシュ）した後に呼び出します。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE end_flush();

	virtual OMX_ERRORTYPE component_tunnel_request(OMX_HANDLETYPE omx_comp, OMX_U32 index, OMX_TUNNELSETUPTYPE *setup);

	virtual OMX_ERRORTYPE allocate_tunnel_buffer(OMX_U32 index);

	virtual OMX_ERRORTYPE free_tunnel_buffer(OMX_U32 index);

	/**
	 * コンポーネント外部で確保されたバッファを、
	 * コンポーネントとのデータのやり取りに使用します。
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
	 *
	 * @param bufhead OpenMAX バッファヘッダを受け取るためのポインタ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE use_buffer(OMX_BUFFERHEADERTYPE **bufhead, OMX_PTR priv, OMX_U32 size, OMX_U8 *buf);

	/**
	 * コンポーネントでバッファを確保し、
	 * コンポーネントとのデータのやり取りに使用します。
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
	 *
	 * @param bufhead OpenMAX バッファヘッダを受け取るためのポインタ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE allocate_buffer(OMX_BUFFERHEADERTYPE **bufhead, OMX_PTR priv, OMX_U32 size);

	/**
	 * バッファを解放します。
	 *
	 * @param bufhead OpenMAX バッファヘッダ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE free_buffer(OMX_BUFFERHEADERTYPE *bufhead);

	/**
	 * バッファを解放します。
	 *
	 * @param pb ポートバッファ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE free_buffer(port_buffer *pb);

	/**
	 * バッファを検索します。
	 *
	 * 指定したバッファヘッダが保持している pBuffer が、
	 * このポートに登録されているかどうかを調べます。
	 *
	 * @param bufhead OpenMAX バッファヘッダ
	 * @return 指定したバッファが見つかれば true、見つからなければ false
	 */
	virtual bool find_buffer(OMX_BUFFERHEADERTYPE *bufhead);

	/**
	 * バッファを検索します。
	 *
	 * 指定したポートバッファの header->pBuffer が、
	 * このポートに登録されているかどうかを調べます。
	 *
	 * @param pb ポートバッファ
	 * @return 指定したバッファが見つかれば true、見つからなければ false
	 */
	virtual bool find_buffer(port_buffer *pb);


	//----------------------------------------
	// コンポーネント利用者 → コンポーネントへのバッファ送付
	//----------------------------------------

	/**
	 * ポートとポートが所属しているコンポーネントに対し、
	 * 指定した OpenMAX バッファからデータを読み出すことを要求します。
	 *
	 * @param bufhead OpenMAX バッファヘッダ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE empty_buffer(OMX_BUFFERHEADERTYPE *bufhead);

	/**
	 * ポートとポートが所属しているコンポーネントに対し、
	 * 指定した OpenMAX バッファにデータを書き込むことを要求します。
	 *
	 * @param bufhead OpenMAX バッファヘッダ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE fill_buffer(OMX_BUFFERHEADERTYPE *bufhead);

	/**
	 * OpenMAX バッファを受け付け、バッファ処理スレッドに送出します。
	 *
	 * @param bufhead OpenMAX バッファヘッダ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE push_buffer(OMX_BUFFERHEADERTYPE *bufhead);


	/**
	 * 受け付けた OpenMAX バッファを持つポートバッファを引き出します。
	 *
	 * バッファ処理スレッドにて使用します。
	 *
	 * @param pb ポートバッファ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE pop_buffer(port_buffer *pb);

	/**
	 * 受付中の OpenMAX バッファ数を取得します。
	 *
	 * バッファ処理スレッドにて使用します。
	 *
	 * @return 受付中のバッファ数
	 */
	virtual int get_buffer_count();


	//----------------------------------------
	// コンポーネント → コンポーネント利用者へのバッファ返却
	//----------------------------------------

	/**
	 * ポートとポートが所属しているコンポーネントに対し、
	 * データを読み出し終わった OpenMAX バッファの返却を要求します。
	 *
	 * @param bufhead OpenMAX バッファヘッダ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE empty_buffer_done(OMX_BUFFERHEADERTYPE *bufhead);

	/**
	 * ポートとポートが所属しているコンポーネントに対し、
	 * データを読み出し終わったポートバッファの返却を要求します。
	 *
	 * @param pb ポートバッファ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE empty_buffer_done(port_buffer *pb);

	/**
	 * ポートとポートが所属しているコンポーネントに対し、
	 * データを書き込み終わった OpenMAX バッファの返却を要求します。
	 *
	 * @param bufhead OpenMAX バッファヘッダ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE fill_buffer_done(OMX_BUFFERHEADERTYPE *bufhead);

	/**
	 * ポートとポートが所属しているコンポーネントに対し、
	 * データを書き込み終わったポートバッファの返却を要求します。
	 *
	 * @param pb ポートバッファ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE fill_buffer_done(port_buffer *pb);

	/**
	 * 使用後の OpenMAX バッファを、バッファ返却スレッドに送出します。
	 *
	 * @param bufhead OpenMAX バッファヘッダ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE push_buffer_done(OMX_BUFFERHEADERTYPE *bufhead);


protected:
	/**
	 * ポートの待ちを強制キャンセルすべきか、
	 * そうでないかをチェックし、キャンセルすべきなら例外をスローします。
	 *
	 * @param lock ポートのロック
	 */
	virtual void error_if_broken(std::unique_lock<std::recursive_mutex>& lock);

	/**
	 * ポートの待ちを強制キャンセルすべきかを取得します。
	 *
	 * shutdown_read または shutdown_write により、
	 * 強制キャンセルが発生します。
	 *
	 * @return 強制キャンセルすべきであれば true、そうでなければ false
	 */
	virtual bool is_broken();

	//----------------------------------------
	// コンポーネント利用者へのバッファ返却スレッド
	//----------------------------------------

	/**
	 * 使用後の OpenMAX バッファを返却します。
	 *
	 * 使用後の OpenMAX バッファを受け取り EmptyThisBufferDone または
	 * FillThisBufferDone コールバックにて、バッファの供給元に返します。
	 *
	 * @return 常に nullptr
	 */
	virtual void *buffer_done();

	/**
	 * 使用後の OpenMAX バッファを返却するスレッドの main 関数です。
	 *
	 * @param p ポートへのポインタ
	 * @return 常に nullptr
	 */
	static void *buffer_done_thread_main(port *p);


protected:
	//for get_definition() member function.
	mutable OMX_PARAM_PORTDEFINITIONTYPE definition;


private:
	//ポートのロック
	std::recursive_mutex mut;
	//ポートの状態変数
	std::condition_variable_any cond;
	//待機の強制解除フラグ
	bool shutting_read, shutting_write;

	component *comp;

	//以下 OMX_PARAM_PORTDEFINITIONTYPE に基づくメンバ

	OMX_U32 port_index;
	OMX_DIRTYPE dir;
	OMX_U32 buffer_count_actual;
	OMX_U32 buffer_count_min;
	OMX_U32 buffer_size;
	OMX_BOOL f_enabled;
	OMX_BOOL f_populated;
	OMX_PORTDOMAINTYPE domain;
	OMX_BOOL buffers_contiguous;
	OMX_U32 buffer_alignment;

	//以上 OMX_PARAM_PORTDEFINITIONTYPE に基づくメンバ

	OMX_BOOL f_no_buffer;

	//ポートがサポートするフォーマットのリスト
	//OMX_GetParameter(OMX_IndexParamXxxxxPortFormat) にて使用します。
	//Xxxxx は Audio | Video | Image | Other のいずれかです。
	std::vector<port_format> formats;
	int default_format;

	//使用可能バッファ登録リスト
	std::vector<port_buffer *> list_bufs;
	std::recursive_mutex mut_list_bufs;

	//バッファ送出用リングバッファ
	ring_buffer<port_buffer> *ring_send;
	bounded_buffer<ring_buffer<port_buffer>, port_buffer> *bound_send;

	//使用後のバッファ返却用リングバッファ
	ring_buffer<port_buffer> *ring_ret;
	bounded_buffer<ring_buffer<port_buffer>, port_buffer> *bound_ret;
	//使用後のバッファ返却スレッド
	std::thread *th_ret;

};

} //namespace mf

#endif //OMX_MF_PORT_HPP__
