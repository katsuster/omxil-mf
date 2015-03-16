#ifndef PORT_HPP__
#define PORT_HPP__

#include <vector>
#include <mutex>
#include <thread>
#include <sys/prctl.h>

#include <OMX_Component.h>
#include <OMX_Core.h>

#include "consts.h"
#include "util/omx_enum_name.hpp"
#include "ring/ring_buffer.hpp"
#include "ring/bounded_buffer.hpp"
#include "debug/scoped_log.hpp"


//OpenMAX バッファを受け渡すバッファの深さ
#define OMX_MF_BUFS_DEPTH    1


namespace mf {

class component;
class port;

/*
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
 */

/**
 * ポートに渡された OpenMAX バッファです。
 */
struct port_buffer {
	//バッファを所持しているポート
	port *p;
	//OMX_AllocateBuffer によって確保されたバッファであれば true
	//OMX_UseBuffer によるバッファであれば false
	bool f_allocate;
	//OpenMAX のバッファヘッダ
	OMX_BUFFERHEADERTYPE *header;
	//現在位置（初期値は nOffset）コンポーネント内で、
	//入力/出力ポートに渡されたバッファの nOffset は変更しない
	//ただしコンポーネント自身が生成する場合（クロックなど）は、
	//nOffset の初期値だけ設定する
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

	port(int ind, component *c)
	: index(ind), comp(c),
	ring_send(nullptr), bound_send(nullptr),
	ring_ret(nullptr), bound_ret(nullptr), th_ret(nullptr),
	dir(OMX_DirMax),
	buffer_count_actual(0), buffer_count_min(0), buffer_size(0),
	f_enabled(OMX_FALSE), f_populated(OMX_FALSE),
	domain(OMX_PortDomainMax),
	buffers_contiguous(OMX_FALSE), buffer_alignment(0) {
		scoped_log_begin;

		try {
			//creating ring buffer for sending OpenMAX buffers
			ring_send  = new ring_buffer<port_buffer>(nullptr, OMX_MF_BUFS_DEPTH + 1);
			bound_send = new bounded_buffer<ring_buffer<port_buffer>, port_buffer>(*ring_send);

			//creating ring buffer for returning OpenMAX buffers
			ring_ret  = new ring_buffer<port_buffer>(nullptr, OMX_MF_BUFS_DEPTH + 1);
			bound_ret = new bounded_buffer<ring_buffer<port_buffer>, port_buffer>(*ring_ret);

			//start returning OpenMAX buffers thread
			th_ret = new std::thread(buffer_done_thread_main, this);
		} catch (const std::bad_alloc& e) {
			errprint("failed to construct '%s'.\n", e.what());

			delete th_ret;
			th_ret = nullptr;

			delete bound_ret;
			bound_ret = nullptr;
			delete ring_ret;
			ring_ret = nullptr;

			delete bound_send;
			bound_send = nullptr;
			delete ring_send;
			ring_send = nullptr;

			throw;
		}
	}

	virtual ~port() {
		scoped_log_begin;

		//shutdown returning OpenMAX buffers thread
		if (bound_ret) {
			bound_ret->shutdown();
		}
		if (th_ret) {
			th_ret->join();
		}
		delete th_ret;
		delete bound_ret;
		delete ring_ret;

		//shutdown sending OpenMAX buffers thread
		if (bound_send) {
			bound_send->shutdown();
		}
		delete bound_send;
		delete ring_send;
	}

	//disable copy constructor
	port(const port& obj) = delete;

	//disable operator=
	port& operator=(const port& obj) = delete;

	virtual const char *get_name() const {
		return "port";
	}

	/**
	 * ポートが所属するコンポーネントを取得します。
	 *
	 * @return コンポーネントへのポインタ、属していなければ nullptr
	 */
	virtual const component *get_component() const {
		return comp;
	}

	/**
	 * ポートが所属するコンポーネントを取得します。
	 *
	 * @return コンポーネントへのポインタ、属していなければ nullptr
	 */
	virtual component *get_component() {
		return comp;
	}

	/**
	 * ポートのインデックスを取得します。
	 *
	 * @return ポートのインデックス
	 */
	virtual OMX_U32 get_index() const {
		return index;
	}

	/**
	 * ポートのインデックスを設定します。
	 *
	 * @param v ポートのインデックス
	 */
	virtual void set_index(OMX_U32 v) {
		index = v;
	}

	virtual OMX_DIRTYPE get_dir() const {
		return dir;
	}

	virtual void set_dir(OMX_DIRTYPE v) {
		dir = v;
	}

	virtual OMX_U32 get_buffer_count_actual() const {
		return buffer_count_actual;
	}

	virtual void set_buffer_count_actual(OMX_U32 v) {
		buffer_count_actual = v;
	}

	virtual OMX_U32 get_buffer_count_min() const {
		return buffer_count_min;
	}

	virtual void set_buffer_count_min(OMX_U32 v) {
		buffer_count_min = v;
	}

	virtual OMX_U32 get_buffer_size() const {
		return buffer_size;
	}

	virtual void set_buffer_size(OMX_U32 v) {
		buffer_size = v;
	}

	virtual OMX_BOOL get_enabled() const {
		return f_enabled;
	}

	virtual void set_enabled(OMX_BOOL v) {
		f_enabled = v;
	}

	virtual OMX_BOOL get_populated() const {
		return f_populated;
	}

	virtual void set_populated(OMX_BOOL v) {
		f_populated = v;
	}

	virtual OMX_PORTDOMAINTYPE get_domain() const {
		return domain;
	}

	virtual void set_domain(OMX_PORTDOMAINTYPE v) {
		domain = v;
	}

	virtual OMX_BOOL get_buffers_contiguous() const {
		return buffers_contiguous;
	}

	virtual void set_buffers_contiguous(OMX_BOOL v) {
		buffers_contiguous = v;
	}

	virtual OMX_U32 get_buffer_alignment() const {
		return buffer_alignment;
	}

	virtual void set_buffer_alignment(OMX_U32 v) {
		buffer_alignment = v;
	}

	/**
	 * Get OpenMAX IL definition data of this port.
	 *
	 * OMX_PARAM_PORTDEFINITIONTYPE の各メンバに設定値がセットされます。
	 * ただし format メンバは 0 で埋められます。
	 *
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
	 *
	 * @return port definition data of OpenMAX IL
	 */
	virtual const OMX_PARAM_PORTDEFINITIONTYPE *get_definition() const {
		scoped_log_begin;

		memset(&definition, 0, sizeof(definition));

		definition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
		definition.nVersion.s.nVersionMajor = OMX_MF_IL_MAJOR;
		definition.nVersion.s.nVersionMinor = OMX_MF_IL_MINOR;
		definition.nVersion.s.nRevision     = OMX_MF_IL_REVISION;
		definition.nVersion.s.nStep         = OMX_MF_IL_STEP;
		definition.eDir               = dir;
		definition.nBufferCountActual = buffer_count_actual;
		definition.nBufferCountMin    = buffer_count_min;
		definition.nBufferSize        = buffer_size;
		definition.bEnabled           = f_enabled;
		definition.bPopulated         = f_populated;
		definition.eDomain            = domain;
		//definition.format is not set
		definition.bBuffersContiguous = buffers_contiguous;
		definition.nBufferAlignment   = buffer_alignment;

		return &definition;
	}

	/**
	 * Set OpenMAX IL definition data of this port.
	 *
	 * OMX_PARAM_PORTDEFINITIONTYPE の各メンバに対応して、
	 * このポートのメンバが更新されます。
	 * ただし nSize, nVersion, format メンバは無視されます。
	 *
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
	 *
	 * @return port definition data of OpenMAX IL
	 */
	virtual void set_definition(const OMX_PARAM_PORTDEFINITIONTYPE& v) {
		scoped_log_begin;

		dir                 = v.eDir;
		buffer_count_actual = v.nBufferCountActual;
		buffer_count_min    = v.nBufferCountMin;
		buffer_size         = v.nBufferSize;
		f_enabled           = v.bEnabled;
		f_populated         = v.bPopulated;
		domain              = v.eDomain;
		//definition.format is ignored
		buffers_contiguous  = v.bBuffersContiguous;
		buffer_alignment    = v.nBufferAlignment;
	}

	virtual void disable_port() {
		scoped_log_begin;
		//do nothing
	}

	virtual void enable_port() {
		scoped_log_begin;
		//do nothing
	}

	virtual void flush_buffers() {
		scoped_log_begin;
		//do nothing
	}

	virtual void component_tunnel_request(OMX_HANDLETYPE omx_comp, OMX_U32 index, OMX_TUNNELSETUPTYPE *setup) {
		scoped_log_begin;
		//do nothing
	}

	virtual void allocate_tunnel_buffer(OMX_U32 index) {
		scoped_log_begin;
		//do nothing
	}

	virtual void free_tunnel_buffer(OMX_U32 index) {
		scoped_log_begin;
		//do nothing
	}

	/**
	 * コンポーネント外部で確保されたバッファを、
	 * コンポーネントとのデータのやり取りに使用します。
	 *
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
	 *
	 * @param bufhead OpenMAX バッファヘッダを受け取るためのポインタ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE use_buffer(OMX_BUFFERHEADERTYPE **bufhead, OMX_PTR priv, OMX_U32 size, OMX_U8 *buf) {
		scoped_log_begin;
		port_buffer *pb = nullptr;
		OMX_BUFFERHEADERTYPE *header = nullptr;
		OMX_ERRORTYPE err;

		try {
			//allocate buffer of port
			pb = new port_buffer();

			//allocate OpenMAX BUFFERHEADER
			header = new OMX_BUFFERHEADERTYPE();
		} catch (const std::bad_alloc& e) {
			errprint("failed to allocate '%s'.\n", e.what());
			err = OMX_ErrorInsufficientResources;
			goto err_out;
		}

		{
			std::lock_guard<std::recursive_mutex> lk(mut_list_bufs);

			//init buffer of port
			pb->p           = this;
			pb->f_allocate  = false;
			pb->header      = header;
			pb->index       = 0;

			list_bufs.push_back(pb);

			//init OpenMAX BUFFERHEADER
			header->nSize = sizeof(OMX_BUFFERHEADERTYPE);
			header->nVersion.s.nVersionMajor = OMX_MF_IL_MAJOR;
			header->nVersion.s.nVersionMinor = OMX_MF_IL_MINOR;
			header->nVersion.s.nRevision     = OMX_MF_IL_REVISION;
			header->nVersion.s.nStep         = OMX_MF_IL_STEP;
			header->pBuffer              = buf;
			header->nAllocLen            = size;
			header->nFilledLen           = 0;
			header->nOffset              = 0;
			header->pAppPrivate          = priv;
			header->pPlatformPrivate     = pb;
			if (get_dir() == OMX_DirInput) {
				header->pInputPortPrivate  = nullptr;
				header->nInputPortIndex    = get_index();
			} else if (get_dir() == OMX_DirOutput) {
				header->pOutputPortPrivate = nullptr;
				header->nOutputPortIndex   = get_index();
			} else {
				errprint("unknown direction.\n");
				err = OMX_ErrorBadPortIndex;
				goto err_out;
			}
			header->hMarkTargetComponent = nullptr;
			header->pMarkData            = nullptr;
			header->nTickCount           = 0;
			header->nTimeStamp           = 0;
			header->nFlags               = 0;
		}

		//return result
		*bufhead = header;

		//success
		err = OMX_ErrorNone;

	err_out:
		if (err != OMX_ErrorNone) {
			delete header;
			delete pb;
		}

		return err;
	}

	/**
	 * コンポーネントでバッファを確保し、
	 * コンポーネントとのデータのやり取りに使用します。
	 *
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
	 *
	 * @param bufhead OpenMAX バッファヘッダを受け取るためのポインタ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE allocate_buffer(OMX_BUFFERHEADERTYPE **bufhead, OMX_PTR priv, OMX_U32 size) {
		scoped_log_begin;
		OMX_U8 *backbuf = nullptr;
		port_buffer *pb = nullptr;
		OMX_BUFFERHEADERTYPE *header = nullptr;
		OMX_ERRORTYPE err;

		try {
			//allocate back buffer of OpenMAX buffer
			backbuf = new OMX_U8[size];

			//allocate buffer of port
			pb = new port_buffer();

			//allocate OpenMAX BUFFERHEADER
			header = new OMX_BUFFERHEADERTYPE();
		} catch (const std::bad_alloc& e) {
			errprint("failed to allocate '%s'.\n", e.what());
			err = OMX_ErrorInsufficientResources;
			goto err_out;
		}

		{
			std::lock_guard<std::recursive_mutex> lk(mut_list_bufs);

			//init buffer of port
			pb->p           = this;
			pb->f_allocate  = true;
			pb->header      = header;
			pb->index       = 0;

			list_bufs.push_back(pb);

			//init OpenMAX BUFFERHEADER
			header->nSize = sizeof(OMX_BUFFERHEADERTYPE);
			header->nVersion.s.nVersionMajor = OMX_MF_IL_MAJOR;
			header->nVersion.s.nVersionMinor = OMX_MF_IL_MINOR;
			header->nVersion.s.nRevision     = OMX_MF_IL_REVISION;
			header->nVersion.s.nStep         = OMX_MF_IL_STEP;
			header->pBuffer              = backbuf;
			header->nAllocLen            = size;
			header->nFilledLen           = 0;
			header->nOffset              = 0;
			header->pAppPrivate          = priv;
			header->pPlatformPrivate     = pb;
			if (get_dir() == OMX_DirInput) {
				header->pInputPortPrivate  = nullptr;
				header->nInputPortIndex    = get_index();
			} else if (get_dir() == OMX_DirOutput) {
				header->pOutputPortPrivate = nullptr;
				header->nOutputPortIndex   = get_index();
			} else {
				errprint("unknown direction.\n");
				err = OMX_ErrorBadPortIndex;
				goto err_out;
			}
			header->hMarkTargetComponent = nullptr;
			header->pMarkData            = nullptr;
			header->nTickCount           = 0;
			header->nTimeStamp           = 0;
			header->nFlags               = 0;
		}

		//return result
		*bufhead = header;

		//success
		err = OMX_ErrorNone;

	err_out:
		if (err != OMX_ErrorNone) {
			delete header;
			delete pb;
			delete[] backbuf;
		}

		return err;
	}

	/**
	 * バッファを解放します。
	 *
	 * @param bufhead OpenMAX バッファヘッダ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE free_buffer(OMX_BUFFERHEADERTYPE *bufhead) {
		scoped_log_begin;
		std::lock_guard<std::recursive_mutex> lk(mut_list_bufs);
		std::vector<port_buffer *>::iterator it;
		port_buffer *pb;

		for (it = list_bufs.begin(); it != list_bufs.end(); it++) {
			pb = *it;

			if (pb->header != bufhead) {
				//not match
				continue;
			}

			list_bufs.erase(it);
			if (pb->f_allocate) {
				delete[] pb->header->pBuffer;
				pb->header->pBuffer = nullptr;
			}
			delete pb->header;
			pb->header = nullptr;
			delete pb;

			break;
		}

		return OMX_ErrorNone;
	}

	/**
	 * バッファを解放します。
	 *
	 * @param pb ポートバッファ
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE free_buffer(port_buffer *pb) {
		scoped_log_begin;
		return free_buffer(pb->header);
	}

	/**
	 * バッファを検索します。
	 *
	 * 指定したバッファヘッダが保持している pBuffer が、
	 * このポートに登録されているかどうかを調べます。
	 *
	 * @param bufhead OpenMAX バッファヘッダ
	 * @return OpenMAX エラー値、
	 * 指定したバッファが見つかれば OMX_ErrorNone、見つからなければそれ以外
	 */
	virtual bool find_buffer(OMX_BUFFERHEADERTYPE *bufhead) {
		std::lock_guard<std::recursive_mutex> lk(mut_list_bufs);

		for (port_buffer *pb : list_bufs) {
			if (pb->header->pBuffer == bufhead->pBuffer) {
				//found
				return OMX_ErrorNone;
			}
		}

		return OMX_ErrorBadParameter;
	}

	/**
	 * バッファを検索します。
	 *
	 * 指定したポートバッファの header->pBuffer が、
	 * このポートに登録されているかどうかを調べます。
	 *
	 * @param pb ポートバッファ
	 * @return OpenMAX エラー値、
	 * 指定したバッファが見つかれば OMX_ErrorNone、見つからなければそれ以外
	 */
	virtual bool find_buffer(port_buffer *pb) {
		return find_buffer(pb->header);
	}


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
	virtual int get_buffer_count() {
		return bound_send->size();
	}


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
	OMX_U32 index;
	component *comp;

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

	OMX_DIRTYPE dir;
	OMX_U32 buffer_count_actual;
	OMX_U32 buffer_count_min;
	OMX_U32 buffer_size;
	OMX_BOOL f_enabled;
	OMX_BOOL f_populated;
	OMX_PORTDOMAINTYPE domain;
	OMX_BOOL buffers_contiguous;
	OMX_U32 buffer_alignment;

};

} //namespace mf

#endif //PORT_HPP__
