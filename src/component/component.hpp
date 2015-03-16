#ifndef COMPONENT_HPP__
#define COMPONENT_HPP__

#include <cstdarg>
#include <map>
#include <mutex>
#include <condition_variable>
#include <thread>

#include <sys/prctl.h>

#include <OMX_Component.h>
#include <OMX_Core.h>

#include "consts.h"
#include "util/omx_enum_name.hpp"
#include "ring/ring_buffer.hpp"
#include "ring/bounded_buffer.hpp"
#include "component/omx_reflector.hpp"
#include "component/port.hpp"


//OpenMAX バッファのヘッダチェック用
struct OMX_PARAM_HEADERTYPE {
	OMX_U32 nSize;
	OMX_VERSIONTYPE nVersion;
};

//コマンドを受け渡すバッファの深さ
#define OMX_SEND_CMD_DEPTH    1


namespace mf {

//OMX_SendCommand コマンドを受け渡すための構造体
struct OMX_SEND_CMD {
	//OMX_SendCommand の第 2引数です
	OMX_COMMANDTYPE cmd;
	//OMX_SendCommand の第 3引数です
	OMX_U32 param;
	//OMX_SendCommand の第 4引数です
	OMX_PTR data;
};

/**
 * コンポーネントの基底クラス。
 *
 * スレッド一覧
 * omx:cmd:xxxx: OMX_SendCommand を処理するためのスレッド。
 *    OpenMAX の規格上 OMX_SendCommand はブロックしてはいけないので、
 *    コマンドを受け付けたらすぐ呼び出し元に制御を返す。
 *    実際の処理は cmd_xxxx スレッドで処理し、処理が終わったら
 *    イベントコールバックで呼び出し元に通知する。
 *
 * omx:run:xxxx: コンポーネントの処理を行うスレッド。
 */
class component : public omx_reflector {
public:
	//親クラス
	typedef omx_reflector super;
	//ポート一覧表の型
	typedef std::map<OMX_U32, port&> portmap_t;

	//disable default constructor
	component() = delete;

	component(OMX_COMPONENTTYPE *c, OMX_STRING cname)
	: omx_reflector(c, cname), broken(false),
	state(OMX_StateInvalid), omx_cbs(nullptr), omx_cbs_priv(nullptr),
	th_accept(nullptr), ring_accept(nullptr), bound_accept(nullptr),
	th_main(nullptr), running_main(false) {
		scoped_log_begin;

		try {
			//create ring buffer
			ring_accept = new ring_buffer<OMX_SEND_CMD>(nullptr, OMX_SEND_CMD_DEPTH + 1);
			bound_accept = new bounded_buffer<ring_buffer<OMX_SEND_CMD>, OMX_SEND_CMD>(*ring_accept);

			//start command accept thread
			th_accept = new std::thread(accept_command_thread_main, get_omx_component());
		} catch (const std::bad_alloc& e) {
			errprint("failed to construct '%s'.\n", e.what());

			delete th_accept;
			th_accept = nullptr;

			delete ring_accept;
			ring_accept = nullptr;
			delete bound_accept;
			bound_accept = nullptr;

			throw;
		}

		//component is ready to work
		set_state(OMX_StateLoaded);
	}

	virtual ~component() {
		scoped_log_begin;

		//shutdown accept thread
		if (bound_accept) {
			bound_accept->shutdown();
		}
		if (th_accept) {
			th_accept->join();
		}
		delete th_accept;
		delete bound_accept;
		delete ring_accept;
	}

	//disable copy constructor
	component(const component& obj) = delete;

	//disable operator=
	component& operator=(const component& obj) = delete;

	virtual const char *get_name() const {
		return "component";
	}

	/**
	 * OpenMAX コンポーネントの状態を取得します。
	 *
	 * @return コンポーネントの状態
	 */
	virtual OMX_STATETYPE get_state() const {
		return state;
	}

	/**
	 * OpenMAX コンポーネントの状態を強制的に設定します。
	 *
	 * @param s 新たなコンポーネントの状態
	 */
	virtual void set_state(OMX_STATETYPE s) {
		std::unique_lock<std::mutex> lock(mut);

		state = s;
		cond.notify_all();
	}

	/**
	 * OpenMAX コンポーネントの状態変化を待ちます。
	 *
	 * wait_state_multiple(1, s) と同様の効果を持ちます。
	 *
	 * @param s 待ちたいコンポーネントの状態
	 */
	virtual void wait_state(OMX_STATETYPE s) {
		std::unique_lock<std::mutex> lock(mut);

		cond.wait(lock, [&] { return broken || state == s; });
		error_if_broken(lock);
	}

	/**
	 * OpenMAX コンポーネントの状態変化を待ちます。
	 * 引数は 16個まで指定できます。
	 *
	 * 指定した複数の状態のうち、どれか一つに当てはまるまで待ちます。
	 *
	 * NOTE: ベンダ定義の状態がなければ、状態は 6個です。
	 *
	 * @param cnt 待ちたいコンポーネントの状態の数（16個以下）
	 * @param ... 待ちたいコンポーネントの状態
	 */
	virtual void wait_state_multiple(int cnt, ...) {
		std::unique_lock<std::mutex> lock(mut);
		va_list ap;
		OMX_STATETYPE states[16];

		if (cnt <= 0) {
			return;
		}
		if (cnt > 16) {
			errprint("count:%d is too much.\n", cnt);
			return;
		}

		va_start(ap, cnt);
		for (int i = 0; i < cnt; i++) {
			//enum は暗黙の型変換により int にキャストされ、
			//関数に渡される。
			//int から enum への暗黙のキャストは出来ないので、
			//static_cast が必要。
			states[i] = static_cast<OMX_STATETYPE>(va_arg(ap, int));
		}
		va_end(ap);

		cond.wait(lock, [&] {
			for (int i = 0; i < cnt; i++) {
				if (broken || state == states[i]) {
					return true;
				}
			}
			return false; });
		error_if_broken(lock);
	}

	/**
	 * 全ての待機しているスレッドを強制的に解除します。
	 *
	 * 強制解除されたスレッドは runtime_error をスローします。
	 */
	void shutdown() {
		std::unique_lock<std::mutex> lock(mut);

		broken = true;
		cond.notify_all();
	}

	/**
	 * OpenMAX コンポーネントのコールバック関数を取得します。
	 *
	 * @return OpenMAX コンポーネントのコールバック関数
	 */
	virtual const OMX_CALLBACKTYPE *get_callbacks() const {
		return omx_cbs;
	}

	/**
	 * OpenMAX コンポーネントのコールバック関数に渡す、
	 * アプリケーション定義のデータを取得します。
	 *
	 * @return OpenMAX コールバック関数のアプリケーションデータ
	 */
	virtual const void *get_callbacks_data() const {
		return omx_cbs_priv;
	}

	/**
	 * 指定された番号のポートを検索します。
	 *
	 * @param index ポート番号
	 * @return ポートへのポインタ、存在しない場合は nullptr
	 */
	virtual port *find_port(OMX_U32 index) {
		portmap_t::iterator ret;

		ret = map_ports.find(index);
		if (ret == map_ports.end()) {
			//not found
			return nullptr;
		}

		return &ret->second;
	}

	/**
	 * コンポーネントのメイン処理を行います。
	 */
	virtual void run() //= 0;
	{}

	/**
	 * コンポーネントのメイン処理の実行を続けるべきか取得します。
	 *
	 * @return 実行を続けるべきなら true、停止すべきなら false
	 */
	virtual bool should_run() {
		return running_main;
	}

	/**
	 * コンポーネントのメイン処理の中断を要求します。
	 */
	virtual void stop_running() {
		running_main = false;
	}


	//----------
	//OpenMAX member functions
	//----------
	virtual OMX_ERRORTYPE GetComponentVersion(OMX_HANDLETYPE hComponent, OMX_STRING pComponentName, OMX_VERSIONTYPE *pComponentVersion, OMX_VERSIONTYPE *pSpecVersion, OMX_UUIDTYPE *pComponentUUID);
	virtual OMX_ERRORTYPE SendCommand(OMX_HANDLETYPE hComponent, OMX_COMMANDTYPE Cmd, OMX_U32 nParam, OMX_PTR pCmdData);
	virtual OMX_ERRORTYPE GetParameter(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);
	virtual OMX_ERRORTYPE SetParameter(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);
	virtual OMX_ERRORTYPE GetConfig(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure);
	virtual OMX_ERRORTYPE SetConfig(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure);
	virtual OMX_ERRORTYPE GetExtensionIndex(OMX_HANDLETYPE hComponent, OMX_STRING cParameterName, OMX_INDEXTYPE *pIndexType);
	virtual OMX_ERRORTYPE GetState(OMX_HANDLETYPE hComponent, OMX_STATETYPE *pState);
	virtual OMX_ERRORTYPE ComponentTunnelRequest(OMX_HANDLETYPE hComponent, OMX_U32 nPort, OMX_HANDLETYPE hTunneledComp, OMX_U32 nTunneledPort, OMX_TUNNELSETUPTYPE *pTunnelSetup);
	virtual OMX_ERRORTYPE UseBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8 *pBuffer);
	virtual OMX_ERRORTYPE AllocateBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBuffer, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes);
	virtual OMX_ERRORTYPE FreeBuffer(OMX_HANDLETYPE hComponent, OMX_U32 nPortIndex, OMX_BUFFERHEADERTYPE *pBuffer);
	virtual OMX_ERRORTYPE EmptyThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE *pBuffer);
	virtual OMX_ERRORTYPE FillThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE *pBuffer);
	virtual OMX_ERRORTYPE SetCallbacks(OMX_HANDLETYPE hComponent, OMX_CALLBACKTYPE *pCallbacks, OMX_PTR pAppData);
	virtual OMX_ERRORTYPE ComponentDeInit(OMX_HANDLETYPE hComponent);
	virtual OMX_ERRORTYPE UseEGLImage(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, void *eglImage);
	virtual OMX_ERRORTYPE ComponentRoleEnum(OMX_HANDLETYPE hComponent, OMX_U8 *cRole, OMX_U32 nIndex);

	//----------
	//OpenMAX callback function wrappers
	//----------
	virtual OMX_ERRORTYPE EventHandler(OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
	virtual OMX_ERRORTYPE EmptyBufferDone(OMX_BUFFERHEADERTYPE *pBuffer);
	virtual OMX_ERRORTYPE EmptyBufferDone(port_buffer *pb);
	virtual OMX_ERRORTYPE FillBufferDone(OMX_BUFFERHEADERTYPE *pBuffer);
	virtual OMX_ERRORTYPE FillBufferDone(port_buffer *pb);


protected:
	/**
	 * OpenMAX コンポーネントの待ちを強制キャンセルすべきか、
	 * そうでないかをチェックし、キャンセルすべきなら例外をスローします。
	 *
	 * @param lock コンポーネントのロック
	 */
	virtual void error_if_broken(std::unique_lock<std::mutex>& lock) {
		if (broken) {
			std::string msg(__func__);
			msg += ": interrupted.";
			throw std::runtime_error(msg);
		}
	}

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OpenMAX API の処理関数と別のスレッドで実行されます。
	 *
	 * @return 常に nullptr
	 */
	virtual void *accept_command() {
		scoped_log_begin;
		OMX_SEND_CMD cmd;
		OMX_STATETYPE new_state;
		OMX_U32 port_index;
		//callback するかしないか
		bool f_callback;
		//EventHandler の引数
		OMX_U32 data1, data2;
		OMX_PTR event_data;
		OMX_ERRORTYPE err, err_handler;

		while (1) {
			//blocked read
			bound_accept->read_fully(&cmd, 1);

			dprint("cmd:%s, param1:0x%08x, data:%p\n",
				omx_enum_name::get_OMX_COMMANDTYPE_name(cmd.cmd),
				(int)cmd.param, cmd.data);

			//process command
			err = OMX_ErrorNone;
			err_handler = OMX_ErrorNone;
			f_callback = true;

			switch (cmd.cmd) {
			case OMX_CommandStateSet:
				new_state = (OMX_STATETYPE) cmd.param;
				err = command_state_set(new_state);

				data1 = OMX_CommandStateSet;
				data2 = new_state;
				event_data = nullptr;

				break;
			case OMX_CommandFlush:
				port_index = cmd.param;
				err = command_flush(port_index);

				data1 = OMX_CommandFlush;
				data2 = port_index;
				event_data = nullptr;

				break;
			case OMX_CommandPortDisable:
				port_index = cmd.param;
				err = command_port_disable(port_index);

				data1 = OMX_CommandPortDisable;
				data2 = port_index;
				event_data = nullptr;

				break;
			case OMX_CommandPortEnable:
				port_index = cmd.param;
				err = command_port_enable(port_index);

				data1 = OMX_CommandPortEnable;
				data2 = port_index;
				event_data = nullptr;

				break;
			case OMX_CommandMarkBuffer:
				port_index = cmd.param;
				err = command_mark_buffer(port_index);

				data1 = OMX_CommandMarkBuffer;
				data2 = port_index;
				event_data = nullptr;

				break;
			default:
				errprint("unsupported index:%d.\n",
					(int)cmd.cmd);
				f_callback = false;
				break;
			}

			//done/error event callback
			if (f_callback && err == OMX_ErrorNone) {
				//done
				err_handler = EventHandler(OMX_EventCmdComplete,
					data1, data2, event_data);
			} else if (f_callback) {
				//error
				err_handler = EventHandler(OMX_EventError,
					err_handler, 0, nullptr);
			}
			if (err_handler != OMX_ErrorNone) {
				errprint("event handler returns error: %s\n",
					omx_enum_name::get_OMX_ERRORTYPE_name(err_handler));
			}
		}

		return nullptr;
	}

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandStateSet, ...)
	 * に対応します。
	 *
	 * @param new_state 遷移を要求されている状態
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_state_set(OMX_STATETYPE new_state) {
		scoped_log_begin;
		OMX_ERRORTYPE err;

		dprint("state:%s -> %s\n",
			omx_enum_name::get_OMX_STATETYPE_name(get_state()),
			omx_enum_name::get_OMX_STATETYPE_name(new_state));

		switch (new_state) {
		case OMX_StateInvalid:
			err = command_state_set_to_invalid();
			break;
		case OMX_StateLoaded:
			err = command_state_set_to_loaded();
			break;
		case OMX_StateIdle:
			err = command_state_set_to_idle();
			break;
		case OMX_StateExecuting:
			err = command_state_set_to_executing();
			break;
		case OMX_StatePause:
			err = command_state_set_to_pause();
			break;
		case OMX_StateWaitForResources:
			err = command_state_set_to_wait_for_resources();
			break;
		default:
			errprint("unsupported state:%d.\n", (int)new_state);
			err = OMX_ErrorIncorrectStateTransition;
			break;
		}

		return err;
	}

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandStateSet, OMX_StateInvalid...)
	 * に対応します。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_state_set_to_invalid() {
		scoped_log_begin;

		set_state(OMX_StateInvalid);

		return OMX_ErrorNone;
	}

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandStateSet, OMX_StateLoaded...)
	 * に対応します。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_state_set_to_loaded() {
		scoped_log_begin;
		OMX_ERRORTYPE err;

		switch (get_state()) {
		case OMX_StateLoaded:
			err = OMX_ErrorSameState;
			break;
		case OMX_StateIdle:
			//FIXME: port flush not implemented yet

			//stop & wait main thread
			stop_running();
			th_main->join();
			delete th_main;
			th_main = nullptr;

			err = OMX_ErrorNone;

			break;
		default:
			errprint("invalid state:%s.\n",
				omx_enum_name::get_OMX_STATETYPE_name(get_state()));
			err = OMX_ErrorInvalidState;
			break;
		}

		if (err == OMX_ErrorNone) {
			set_state(OMX_StateLoaded);
		}

		return OMX_ErrorNone;
	}

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandStateSet, OMX_StateIdle...)
	 * に対応します。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_state_set_to_idle() {
		scoped_log_begin;
		OMX_ERRORTYPE err;

		switch (get_state()) {
		case OMX_StateLoaded:
			try {
				//start main thread
				th_main = new std::thread(component_thread_main, get_omx_component());
			} catch (const std::bad_alloc& e) {
				errprint("failed to create main thread '%s'.\n", e.what());
				err = OMX_ErrorInsufficientResources;
				break;
			}

			err = OMX_ErrorNone;

			break;
		case OMX_StateIdle:
			err = OMX_ErrorSameState;
			break;
		case OMX_StateExecuting:
			//FIXME: flush not implemented yet

			err = OMX_ErrorNone;

			break;
		default:
			errprint("invalid state:%s.\n",
				omx_enum_name::get_OMX_STATETYPE_name(get_state()));
			err = OMX_ErrorInvalidState;
			break;
		}

		if (err == OMX_ErrorNone) {
			set_state(OMX_StateIdle);
		}

		return err;
	}

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandStateSet, OMX_StateExecuting...)
	 * に対応します。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_state_set_to_executing() {
		scoped_log_begin;
		OMX_ERRORTYPE err;

		switch (get_state()) {
		case OMX_StateIdle:
			//メイン処理が始まってから状態を切り替える
			{
				std::unique_lock<std::mutex> lock(mut);
				cond.wait(lock, [&] { return broken || should_run(); });
				error_if_broken(lock);
			}

			err = OMX_ErrorNone;
			break;
		case OMX_StateExecuting:
			err = OMX_ErrorSameState;
			break;
		default:
			errprint("invalid state:%s.\n",
				omx_enum_name::get_OMX_STATETYPE_name(get_state()));
			err = OMX_ErrorInvalidState;
			break;
		}

		set_state(OMX_StateExecuting);

		return err;
	}

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandStateSet, OMX_StatePause...)
	 * に対応します。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_state_set_to_pause() {
		scoped_log_begin;

		set_state(OMX_StatePause);

		return OMX_ErrorNone;
	}

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandStateSet, OMX_StateWaitForResources...)
	 * に対応します。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_state_set_to_wait_for_resources() {
		scoped_log_begin;

		set_state(OMX_StateWaitForResources);

		return OMX_ErrorNone;
	}

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandFlush, ...)
	 * に対応します。
	 *
	 * @param port_index ポートのインデックス
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_flush(OMX_U32 port_index) {
		scoped_log_begin;

		return OMX_ErrorNone;
	}

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandPortDisable, ...)
	 * に対応します。
	 *
	 * @param port_index ポートのインデックス
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_port_disable(OMX_U32 port_index) {
		scoped_log_begin;

		return OMX_ErrorNone;
	}

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandPortEnable, ...)
	 * に対応します。
	 *
	 * @param port_index ポートのインデックス
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_port_enable(OMX_U32 port_index) {
		scoped_log_begin;

		return OMX_ErrorNone;
	}

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandMarkBuffer, ...)
	 * に対応します。
	 *
	 * @param port_index ポートのインデックス
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_mark_buffer(OMX_U32 port_index) {
		scoped_log_begin;

		return OMX_ErrorNone;
	}

	/**
	 * ポート一覧表にポートを追加します。
	 *
	 * @param p     ポート
	 * @return ポートを追加できた場合は true, できなければ false
	 */
	virtual bool insert_port(port& p) {
		std::pair<portmap_t::iterator, bool> ret;

		ret = map_ports.insert(portmap_t::value_type(p.get_index(), p));

		return ret.second;
	}

	/**
	 * ポート一覧表から、指定された番号のポートを削除します。
	 *
	 * @param index ポート番号
	 * @return ポートを削除できた場合は true, できなければ false
	 */
	virtual bool erase_port(OMX_U32 index) {
		size_t ret;

		ret = map_ports.erase(index);

		return (ret != 0);
	}

	/**
	 * ポート一覧表を取得します。
	 *
	 * @return ポート一覧表
	 */
	virtual const portmap_t& get_map_ports() const {
		return map_ports;
	}

	/**
	 * ポート一覧表を取得します。
	 *
	 * ポート一覧表に対して、検索、追加、削除、
	 * 以外の操作を行う場合に使います。
	 *
	 * @return ポート一覧表
	 */
	virtual portmap_t& get_map_ports() {
		return map_ports;
	}

	virtual OMX_ERRORTYPE check_omx_header(const void *p, size_t size) const {
		const OMX_PARAM_HEADERTYPE *h = (const OMX_PARAM_HEADERTYPE *)p;

		if (h == nullptr || h->nSize != size) {
			return OMX_ErrorBadParameter;
		}
		if (h->nVersion.s.nVersionMajor != OMX_MF_IL_MAJOR ||
			h->nVersion.s.nVersionMinor != OMX_MF_IL_MINOR) {
			return OMX_ErrorVersionMismatch;
		}

		return OMX_ErrorNone;
	}


public:
	/**
	 * OpenMAX コンポーネントのハンドルから、
	 * component クラスのインスタンスへのポインタを取得します。
	 *
	 * @param hComponent OpenMAX コンポーネント
	 * @return component インスタンスへのポインタ
	 */
	static inline component *get_instance(OMX_HANDLETYPE hComponent) {
		OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
		component *comp = (component *) omx_comp->pComponentPrivate;

		return comp;
	}

protected:
	/**
	 * OMX_SendCommand のコマンドを受け取るスレッドの main 関数です。
	 *
	 * @param arg OpenMAX コンポーネントへのポインタ
	 * @return 常に nullptr
	 */
	static void *accept_command_thread_main(OMX_COMPONENTTYPE *arg) {
		scoped_log_begin;
		std::string thname;
		component *comp = get_instance(arg);

		try {
			//スレッド名をつける
			thname = "omx:cmd:";
			thname += comp->get_name();
			prctl(PR_SET_NAME, thname.c_str());

			comp->accept_command();
		} catch (const std::runtime_error& e) {
			errprint("runtime_error: %s\n", e.what());
		}

		return nullptr;
	}

	/**
	 * コンポーネントのメイン処理を行うスレッドの main 関数です。
	 *
	 * @param arg OpenMAX コンポーネントへのポインタ
	 * @return 常に nullptr
	 */
	static void *component_thread_main(OMX_COMPONENTTYPE *arg) {
		scoped_log_begin;
		std::string thname;
		component *comp = get_instance(arg);

		try {
			//スレッド名をつける
			thname = "omx:run:";
			thname += comp->get_name();
			prctl(PR_SET_NAME, thname.c_str());

			//Idle 状態になってから開始する
			comp->wait_state(OMX_StateIdle);

			//メイン処理が始まったことを通知する
			{
				std::unique_lock<std::mutex> lock(comp->mut);
				comp->running_main = true;
				comp->cond.notify_all();
			}
			comp->run();
		} catch (const std::runtime_error& e) {
			errprint("runtime_error: %s\n", e.what());
		}

		return nullptr;
	}


private:
	//コンポーネントのロック
	std::mutex mut;
	//コンポーネントの状態変数
	std::condition_variable cond;
	//待機の強制解除フラグ
	bool broken;

	//OpenMAX コンポーネントの状態
	OMX_STATETYPE state;
	//OpenMAX コールバック関数
	OMX_CALLBACKTYPE *omx_cbs;
	//OpenMAX コールバック関数に渡すアプリケーション定義のデータ
	void *omx_cbs_priv;

	//コマンド受理スレッド
	std::thread *th_accept;
	//コマンド受け渡し用リングバッファ
	ring_buffer<OMX_SEND_CMD> *ring_accept;
	bounded_buffer<ring_buffer<OMX_SEND_CMD>, OMX_SEND_CMD> *bound_accept;

	//メイン処理スレッド
	std::thread *th_main;
	//メイン処理を続けるかどうかのフラグ
	bool running_main;

	//ポート一覧表
	portmap_t map_ports;

};

} //namespace mf

#endif //COMPONENT_HPP__
