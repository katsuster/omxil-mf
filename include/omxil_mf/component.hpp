#ifndef OMX_MF_COMPONENT_HPP__
#define OMX_MF_COMPONENT_HPP__

#include <map>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <vector>

#include <OMX_Component.h>
#include <OMX_Core.h>

#include <omxil_mf/ring/ring_buffer.hpp>
#include <omxil_mf/ring/bounded_buffer.hpp>
#include <omxil_mf/omx_reflector.hpp>
#include <omxil_mf/component_worker.hpp>
#include <omxil_mf/port.hpp>

//コマンドを受け渡すバッファの深さ
#define OMX_MF_CMD_DEPTH    1


namespace mf {

//OpenMAX バッファのヘッダチェック用
struct OMX_MF_HEADERTYPE {
	OMX_U32 nSize;
	OMX_VERSIONTYPE nVersion;
};

//OMX_SendCommand コマンドを受け渡すための構造体
struct OMX_MF_CMD {
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
	//ワーカースレッド一覧表の型
	typedef std::vector<component_worker *> workerlist_t;
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

	/**
	 * コンポーネント名を取得します。
	 *
	 * @return コンポーネント名
	 */
	virtual const char *get_name() const override;

	/**
	 * 全ての待機しているスレッドを強制的に解除します。
	 *
	 * 強制解除されたスレッドは runtime_error をスローします。
	 */
	virtual void shutdown();

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
	virtual void wait_state(OMX_STATETYPE s) const;

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
	virtual void wait_state_multiple(int cnt, ...) const;

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
	virtual const port *find_port(OMX_U32 index) const;

	/**
	 * 指定された番号のポートを検索します。
	 *
	 * @param index ポート番号
	 * @return ポートへのポインタ、存在しない場合は nullptr
	 */
	virtual port *find_port(OMX_U32 index);

	/**
	 * 全ての有効なポートがバッファを返却するまで待ちます。
	 */
	virtual void wait_all_port_buffer_returned() const;

	//----------
	//OpenMAX member functions
	//----------
	virtual OMX_ERRORTYPE GetComponentVersion(OMX_HANDLETYPE hComponent, OMX_STRING pComponentName, OMX_VERSIONTYPE *pComponentVersion, OMX_VERSIONTYPE *pSpecVersion, OMX_UUIDTYPE *pComponentUUID) override;
	virtual OMX_ERRORTYPE SendCommand(OMX_HANDLETYPE hComponent, OMX_COMMANDTYPE Cmd, OMX_U32 nParam, OMX_PTR pCmdData) override;
	virtual OMX_ERRORTYPE GetParameter(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) override;
	virtual OMX_ERRORTYPE SetParameter(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) override;
	virtual OMX_ERRORTYPE GetConfig(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure) override;
	virtual OMX_ERRORTYPE SetConfig(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure) override;
	virtual OMX_ERRORTYPE GetExtensionIndex(OMX_HANDLETYPE hComponent, OMX_STRING cParameterName, OMX_INDEXTYPE *pIndexType) override;
	virtual OMX_ERRORTYPE GetState(OMX_HANDLETYPE hComponent, OMX_STATETYPE *pState) override;
	virtual OMX_ERRORTYPE ComponentTunnelRequest(OMX_HANDLETYPE hComponent, OMX_U32 nPort, OMX_HANDLETYPE hTunneledComp, OMX_U32 nTunneledPort, OMX_TUNNELSETUPTYPE *pTunnelSetup) override;
	virtual OMX_ERRORTYPE UseBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8 *pBuffer) override;
	virtual OMX_ERRORTYPE AllocateBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBuffer, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes) override;
	virtual OMX_ERRORTYPE FreeBuffer(OMX_HANDLETYPE hComponent, OMX_U32 nPortIndex, OMX_BUFFERHEADERTYPE *pBuffer) override;
	virtual OMX_ERRORTYPE EmptyThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE *pBuffer) override;
	virtual OMX_ERRORTYPE FillThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE *pBuffer) override;
	virtual OMX_ERRORTYPE SetCallbacks(OMX_HANDLETYPE hComponent, OMX_CALLBACKTYPE *pCallbacks, OMX_PTR pAppData) override;
	virtual OMX_ERRORTYPE ComponentDeInit(OMX_HANDLETYPE hComponent) override;
	virtual OMX_ERRORTYPE UseEGLImage(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, void *eglImage) override;
	virtual OMX_ERRORTYPE ComponentRoleEnum(OMX_HANDLETYPE hComponent, OMX_U8 *cRole, OMX_U32 nIndex) override;

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
	 * コンポーネントが使用する静的リソースの確保を行います。
	 *
	 * StateLoaded から StateIdle に遷移する際に呼び出されます。
	 *
	 * デフォルトでは何も行いません。
	 * 必要に応じて派生クラスにてオーバライドしてください。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE allocate_static_resouces();

	/**
	 * コンポーネントが使用する静的リソースの確保を行います。
	 *
	 * StateIdle から StateLoaded に遷移する際に呼び出されます。
	 *
	 * デフォルトでは何も行いません。
	 * 必要に応じて派生クラスにてオーバライドしてください。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE free_static_resouces();

	/**
	 * コンポーネント内の未処理のバッファを全て破棄、
	 * 返却（フラッシュ）するように要求します。
	 *
	 * このメソッドはブロックせず、速やかに制御を返します。
	 *
	 * デフォルトでは、
	 * component::set_request_flush(true)
	 * を呼び出すだけです。
	 *
	 * @param port_index フラッシュを要求されているポート番号、
	 * 	OMX_ALL は全てのポートを表す
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE begin_flush(OMX_U32 port_index);

	/**
	 * コンポーネント内の未処理のバッファを全て破棄、
	 * 返却（フラッシュ）します。
	 *
	 * このメソッドはバッファを全て破棄、
	 * もしくは返却するまでブロックします。
	 *
	 * begin_restart により処理の再開が要求されるまで、
	 * コンポーネントの処理を停止する必要があります。
	 *
	 * デフォルトでは、
	 * component::wait_flush_done()
	 * を呼び出すだけです。
	 *
	 * 必要に応じて派生クラスにてオーバライドしてください。
	 *
	 * @param port_index フラッシュを要求されているポート番号、
	 * 	OMX_ALL は全てのポートを表す
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE end_flush(OMX_U32 port_index);

	/**
	 * フラッシュによって停止していた、
	 * コンポーネントの処理を再開するよう要求します。
	 *
	 * このメソッドはブロックせず、速やかに制御を返します。
	 *
	 * デフォルトでは、
	 * component::set_request_restart(true)
	 * を呼び出すだけです。
	 *
	 * 必要に応じて派生クラスにてオーバライドしてください。
	 *
	 * @param port_index フラッシュを要求されているポート番号、
	 * 	OMX_ALL は全てのポートを表す
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE begin_restart(OMX_U32 port_index);

	/**
	 * フラッシュによって停止していた、
	 * コンポーネントの処理を再開します。
	 *
	 * このメソッドはコンポーネントの処理を再開するまでブロックします。
	 *
	 * デフォルトでは、
	 * component::wait_request_done()
	 * を呼び出すだけです。
	 *
	 * 必要に応じて派生クラスにてオーバライドしてください。
	 *
	 * @param port_index フラッシュを要求されているポート番号、
	 * 	OMX_ALL は全てのポートを表す
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE end_restart(OMX_U32 port_index);

	/**
	 * OMX_GetParameter(OMX_IndexParamAudioInit) にて返す、
	 * ポート数を取得します。
	 *
	 * 初期値は 0 個です。
	 *
	 * 必要に応じて派生クラスにてオーバライドしてください。
	 *
	 * @return オーディオポートの個数
	 */
	virtual OMX_U32 get_audio_ports();

	/**
	 * OMX_GetParameter(OMX_IndexParamAudioInit) にて返す、
	 * ポートの開始インデックスを取得します。
	 *
	 * 初期値は 0 です。
	 *
	 * 必要に応じて派生クラスにてオーバライドしてください。
	 *
	 * @return オーディオポートの開始インデックス
	 */
	virtual OMX_U32 get_audio_start_port();

	/**
	 * OMX_GetParameter(OMX_IndexParamVideoInit) にて返す、
	 * ポート数を取得します。
	 *
	 * 初期値は 0 個です。
	 *
	 * 必要に応じて派生クラスにてオーバライドしてください。
	 *
	 * @return ビデオポートの個数
	 */
	virtual OMX_U32 get_video_ports();

	/**
	 * OMX_GetParameter(OMX_IndexParamVideoInit) にて返す、
	 * ポートの開始インデックスを取得します。
	 *
	 * 初期値は 0 です。
	 *
	 * 必要に応じて派生クラスにてオーバライドしてください。
	 *
	 * @return ビデオポートの開始インデックス
	 */
	virtual OMX_U32 get_video_start_port();

	/**
	 * OMX_GetParameter(OMX_IndexParamImageInit) にて返す、
	 * ポート数を取得します。
	 *
	 * 初期値は 0 個です。
	 *
	 * 必要に応じて派生クラスにてオーバライドしてください。
	 *
	 * @return イメージポートの個数
	 */
	virtual OMX_U32 get_image_ports();

	/**
	 * OMX_GetParameter(OMX_IndexParamImageInit) にて返す、
	 * ポートの開始インデックスを取得します。
	 *
	 * 初期値は 0 です。
	 *
	 * 必要に応じて派生クラスにてオーバライドしてください。
	 *
	 * @return イメージポートの開始インデックス
	 */
	virtual OMX_U32 get_image_start_port();

	/**
	 * OMX_GetParameter(OMX_IndexParamOtherInit) にて返す、
	 * ポート数を取得します。
	 *
	 * 初期値は 0 個です。
	 *
	 * 必要に応じて派生クラスにてオーバライドしてください。
	 *
	 * @return その他のポートの個数
	 */
	virtual OMX_U32 get_other_ports();

	/**
	 * OMX_GetParameter(OMX_IndexParamOtherInit) にて返す、
	 * ポートの開始インデックスを取得します。
	 *
	 * 初期値は 0 です。
	 *
	 * 必要に応じて派生クラスにてオーバライドしてください。
	 *
	 * @return その他のポートの開始インデックス
	 */
	virtual OMX_U32 get_other_start_port();


protected:
	/**
	 * OpenMAX コンポーネントのミューテックスを取得します。
	 *
	 * @return コンポーネントのミューテックス
	 */
	virtual std::mutex& mutex() const;

	/**
	 * OpenMAX コンポーネントの条件変数を取得します。
	 *
	 * @return コンポーネントの条件変数
	 */
	virtual std::condition_variable& condition() const;

	/**
	 * OpenMAX コンポーネントの待ちを強制キャンセルすべきか、
	 * そうでないかをチェックし、キャンセルすべきなら例外をスローします。
	 *
	 * @param lock コンポーネントのロック
	 */
	virtual void error_if_broken(std::unique_lock<std::mutex>& lock) const;

	/**
	 * 全ての待機しているスレッドを強制的に解除すべきか、
	 * を取得します。
	 *
	 * @return 強制解除すべきならば true、
	 * 強制解除すべきでなければ false
	 */
	virtual bool is_broken() const;

	/**
	 * 全ての待機しているスレッドを強制的に解除すべきか、
	 * を設定します。
	 *
	 * @param f 強制解除すべきならば true、
	 * 強制解除すべきでなければ false
	 */
	virtual void set_broken(bool f);

	/**
	 * コンポーネントにワーカースレッドを登録します。
	 *
	 * @param wr 登録したいワーカースレッド
	 * @return ワーカースレッドを登録できた場合は true,
	 * できなければ false
	 */
	virtual bool register_worker_thread(component_worker *wr);

	/**
	 * コンポーネントからワーカースレッドの登録を解除します。
	 *
	 * 登録解除の際、スレッドを停止します。
	 *
	 * @param wr 削除したいワーカースレッド
	 * @return ワーカースレッドを登録解除できた場合は true,
	 * できなければ false
	 */
	virtual bool unregister_worker_thread(component_worker *wr);

	/**
	 * コンポーネントのワーカースレッドのリストを取得します。
	 *
	 * @return ワーカースレッドのリスト
	 */
	virtual const workerlist_t *get_worker_threads() const;

	/**
	 * コンポーネントのワーカースレッドのリストを取得します。
	 *
	 * @return ワーカースレッドのリスト
	 */
	virtual workerlist_t *get_worker_threads();

	/**
	 * 登録されたワーカースレッドを全て開始します。
	 */
	virtual void start_all_worker_threads();

	/**
	 * 登録されたワーカースレッドを全て停止します。
	 */
	virtual void stop_all_worker_threads();

	/**
	 * 登録されたワーカースレッドを強制的に停止します。
	 *
	 * 可能な限り待機状態も解除します。
	 */
	virtual void break_all_worker_threads();

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
	 * OMX_SendCommand(OMX_CommandStateSet, OMX_StateLoaded...) かつ
	 * 現在の状態が OMX_StateIdle のとき
	 * に対応します。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_state_set_to_loaded_from_idle();

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
	 * OMX_SendCommand(OMX_CommandStateSet, OMX_StateIdle...) かつ
	 * 現在の状態が OMX_StateLoaded のとき
	 * に対応します。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_state_set_to_idle_from_loaded();

	/**
	 * OMX_SendCommand にて送られたコマンドを処理します。
	 *
	 * OMX_SendCommand(OMX_CommandStateSet, OMX_StateIdle...) かつ
	 * 現在の状態が OMX_StateExecuting のとき
	 * に対応します。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_state_set_to_idle_from_executing();

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
	 * OMX_SendCommand(OMX_CommandStateSet, OMX_StateExecuting...) かつ
	 * 現在の状態が OMX_StateIdle のとき
	 * に対応します。
	 *
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE command_state_set_to_executing_from_idle();

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
	 * ポートに受付済みで、
	 * コンポーネントが未処理のバッファを全て返却します。
	 *
	 * 下記の順に実行されます。
	 *
	 * <pre>
	 * - コンポーネントのバッファフラッシュ
	 *   - クライアントからポートへのバッファ処理要求の受付を禁止します。
	 *     以降、OMX_EmptyThisBuffer, OMX_FillThisBuffer がエラーとなります。
	 *     参照: port::plug_client_request
	 *   - フラッシュ要求処理（func_request_flush で指定した処理）
	 *     処理の中で component::begin_flush を呼ぶ必要があります。
	 *   - コンポーネントからポートへのバッファ処理要求の受付を禁止します。
	 *     以降、port::pop_buffer がエラーとなります。
	 *     参照: port::plug_component_request
	 *   - フラッシュ処理（func_wait_flush_done で指定した処理）
	 *     処理の中で component::end_flush を呼ぶ必要があります。
	 *
	 * - ポートのバッファフラッシュ
	 *   - ポートで EmptyThisBuffer, FillThisBuffer を受け付けたが、
	 *     コンポーネントが処理していないバッファを全て返却します。
	 * </pre>
	 *
	 * @param port_index             フラッシュするポートのインデックス
	 * @param func_request_flush     フラッシュ要求処理
	 * @param func_wait_flush_done   フラッシュ終了待ち処理
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE execute_flush(OMX_U32 port_index,
		std::function<OMX_ERRORTYPE(OMX_U32)> func_request_flush, std::function<OMX_ERRORTYPE(OMX_U32)> func_wait_flush_done);

	/**
	 * フラッシュ処理で停止したコンポーネントを再開します。
	 *
	 * 下記の順に実行されます。
	 *
	 * <pre>
	 * - コンポーネントのリスタート
	 *   - コンポーネントからポートへのバッファ処理要求の受付を許可します。
	 *     参照: port::unplug_component_request
	 *   - リスタート要求処理（func_request_restart で指定した処理）
	 *     処理の中で component::request_restart を呼ぶ必要があります。
	 *   - クライアントからのバッファ処理要求の受付を許可します。
	 *     参照: port::unplug_client_request
	 *   - リスタート終了待ち処理（func_wait_restart_done で指定した処理）
	 *     処理の中で component::wait_restart_done を呼ぶ必要があります。
	 * </pre>
	 *
	 * @param port_index             フラッシュするポートのインデックス
	 * @param func_request_restart   リスタート要求処理
	 * @param func_wait_restart_done リスタート終了待ち処理
	 * @return OpenMAX エラー値
	 */
	virtual OMX_ERRORTYPE execute_restart(OMX_U32 port_index,
		std::function<OMX_ERRORTYPE(OMX_U32)> func_request_restart, std::function<OMX_ERRORTYPE(OMX_U32)> func_wait_restart_done);

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


private:
	//コンポーネントの状態を変更するときに使用するロック
	mutable std::mutex mut;
	mutable std::condition_variable cond;
	//待機の強制解除フラグ
	bool f_broken;

	//OpenMAX コンポーネントの状態
	OMX_STATETYPE state;
	//OpenMAX コールバック関数
	OMX_CALLBACKTYPE omx_cbs;
	//OpenMAX コールバック関数に渡すアプリケーション定義のデータ
	void *omx_cbs_priv;

	//コマンド受理スレッド
	std::thread *th_accept;
	//コマンド受け渡し用リングバッファ
	ring_buffer<OMX_MF_CMD> *ring_accept;
	bounded_buffer<ring_buffer<OMX_MF_CMD>, OMX_MF_CMD> *bound_accept;

	//ワーカースレッド一覧表
	workerlist_t list_workers;

	//ポート一覧表
	portmap_t map_ports;

};

} //namespace mf

#endif //OMX_MF_COMPONENT_HPP__
