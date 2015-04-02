#ifndef OMX_MF_COMPONENT_HPP__
#define OMX_MF_COMPONENT_HPP__

#include <cstdarg>
#include <map>
#include <mutex>
#include <condition_variable>
#include <thread>

#include <sys/prctl.h>

#include <OMX_Component.h>
#include <OMX_Core.h>

#include <omxil_mf/ring/ring_buffer.hpp>
#include <omxil_mf/ring/bounded_buffer.hpp>
#include <omxil_mf/omx_reflector.hpp>
#include <omxil_mf/port.hpp>

//コマンドを受け渡すバッファの深さ
#define OMX_SEND_CMD_DEPTH    1


namespace mf {

//OpenMAX バッファのヘッダチェック用
struct OMX_PARAM_HEADERTYPE {
	OMX_U32 nSize;
	OMX_VERSIONTYPE nVersion;
};

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
 */
class component : public omx_reflector {
public:
	//親クラス
	typedef omx_reflector super;
	//ポート一覧表の型
	typedef std::map<OMX_U32, port&> portmap_t;

	//disable default constructor
	component() = delete;

	component(OMX_COMPONENTTYPE *c, const char *cname);

	virtual ~component();

	//disable copy constructor
	component(const component& obj) = delete;

	//disable operator=
	component& operator=(const component& obj) = delete;

	virtual const char *get_name() const;

	/**
	 * OpenMAX コンポーネントの状態を取得します。
	 *
	 * @return コンポーネントの状態
	 */
	virtual OMX_STATETYPE get_state() const;

	/**
	 * OpenMAX コンポーネントの状態を強制的に設定します。
	 *
	 * @param s 新たなコンポーネントの状態
	 */
	virtual void set_state(OMX_STATETYPE s);

	/**
	 * OpenMAX コンポーネントの状態変化を待ちます。
	 *
	 * wait_state_multiple(1, s) と同様の効果を持ちます。
	 *
	 * @param s 待ちたいコンポーネントの状態
	 */
	virtual void wait_state(OMX_STATETYPE s);

	/**
	 * OpenMAX コンポーネントの状態変化を待ちます。
	 * 引数は 16個まで指定できます。
	 *
	 * 指定した複数の状態のうち、どれか一つに当てはまるまで待ちます。
	 *
	 * NOTE: ベンダ定義の状態がなければ、
	 * コンポーネントの状態は 6種類です。
	 *
	 * @param cnt 待ちたいコンポーネントの状態の数（16個以下）
	 * @param ... 待ちたいコンポーネントの状態
	 */
	virtual void wait_state_multiple(int cnt, ...);

	/**
	 * 全ての待機しているスレッドを強制的に解除します。
	 *
	 * 強制解除されたスレッドは runtime_error をスローします。
	 */
	virtual void shutdown();

	/**
	 * OpenMAX コンポーネントのコールバック関数を取得します。
	 *
	 * @return OpenMAX コンポーネントのコールバック関数
	 */
	virtual const OMX_CALLBACKTYPE *get_callbacks() const;

	/**
	 * OpenMAX コンポーネントのコールバック関数に渡す、
	 * アプリケーション定義のデータを取得します。
	 *
	 * @return OpenMAX コールバック関数のアプリケーションデータ
	 */
	virtual const void *get_callbacks_data() const;

	/**
	 * 指定された番号のポートを検索します。
	 *
	 * @param index ポート番号
	 * @return ポートへのポインタ、存在しない場合は nullptr
	 */
	virtual port *find_port(OMX_U32 index);

	/**
	 * コンポーネントのメイン処理を行います。
	 */
	virtual void run() = 0;

	/**
	 * コンポーネントのメイン処理の実行を続けるべきか取得します。
	 *
	 * @return 実行を続けるべきなら true、停止すべきなら false
	 */
	virtual bool should_run();

	/**
	 * コンポーネントのメイン処理の中断を要求します。
	 */
	virtual void stop_running();


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
	virtual void error_if_broken(std::unique_lock<std::mutex>& lock);

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OpenMAX API の処理関数と別のスレッドで実行されます。
	 *
	 * @return 常に nullptr
	 */
	virtual void *accept_command();

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandStateSet, ...)
	 * に対応します。
	 *
	 * @param new_state 遷移を要求されている状態
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_state_set(OMX_STATETYPE new_state);

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandStateSet, OMX_StateInvalid...)
	 * に対応します。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_state_set_to_invalid();

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandStateSet, OMX_StateLoaded...)
	 * に対応します。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_state_set_to_loaded();

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandStateSet, OMX_StateIdle...)
	 * に対応します。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_state_set_to_idle();

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandStateSet, OMX_StateExecuting...)
	 * に対応します。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_state_set_to_executing();

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandStateSet, OMX_StatePause...)
	 * に対応します。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_state_set_to_pause();

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandStateSet, OMX_StateWaitForResources...)
	 * に対応します。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_state_set_to_wait_for_resources();

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandFlush, ...)
	 * に対応します。
	 *
	 * @param port_index ポートのインデックス
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_flush(OMX_U32 port_index);

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandPortDisable, ...)
	 * に対応します。
	 *
	 * @param port_index ポートのインデックス
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_port_disable(OMX_U32 port_index);

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandPortEnable, ...)
	 * に対応します。
	 *
	 * @param port_index ポートのインデックス
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_port_enable(OMX_U32 port_index);

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandMarkBuffer, ...)
	 * に対応します。
	 *
	 * @param port_index ポートのインデックス
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_mark_buffer(OMX_U32 port_index);

	/**
	 * ポート一覧表にポートを追加します。
	 *
	 * @param p     ポート
	 * @return ポートを追加できた場合は true, できなければ false
	 */
	virtual bool insert_port(port& p);

	/**
	 * ポート一覧表から、指定された番号のポートを削除します。
	 *
	 * @param index ポート番号
	 * @return ポートを削除できた場合は true, できなければ false
	 */
	virtual bool erase_port(OMX_U32 index);

	/**
	 * ポート一覧表を取得します。
	 *
	 * @return ポート一覧表
	 */
	virtual const portmap_t& get_map_ports() const;

	/**
	 * ポート一覧表を取得します。
	 *
	 * ポート一覧表に対して、検索、追加、削除、
	 * 以外の操作を行う場合に使います。
	 *
	 * @return ポート一覧表
	 */
	virtual portmap_t& get_map_ports();

	virtual OMX_ERRORTYPE check_omx_header(const void *p, size_t size) const;


public:
	/**
	 * OpenMAX コンポーネントのハンドルから、
	 * component クラスのインスタンスへのポインタを取得します。
	 *
	 * @param hComponent OpenMAX コンポーネント
	 * @return component インスタンスへのポインタ
	 */
	static component *get_instance(OMX_HANDLETYPE hComponent);

protected:
	/**
	 * OMX_SendCommand のコマンドを受け取るスレッドの main 関数です。
	 *
	 * @param arg OpenMAX コンポーネントへのポインタ
	 * @return 常に nullptr
	 */
	static void *accept_command_thread_main(OMX_COMPONENTTYPE *arg);

	/**
	 * コンポーネントのメイン処理を行うスレッドの main 関数です。
	 *
	 * @param arg OpenMAX コンポーネントへのポインタ
	 * @return 常に nullptr
	 */
	static void *component_thread_main(OMX_COMPONENTTYPE *arg);


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

#endif //OMX_MF_COMPONENT_HPP__
