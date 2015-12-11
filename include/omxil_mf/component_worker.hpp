#ifndef OMX_MF_COMPONENT_WORKER_HPP__
#define OMX_MF_COMPONENT_WORKER_HPP__

#include <mutex>
#include <condition_variable>
#include <thread>

namespace mf {

class component;

/**
 * コンポーネント内で生成するワーカスレッドクラス。
 */
class component_worker {
public:
	//親クラス
	//typedef xxxxx super;

	//disable default constructor
	component_worker() = delete;

	/**
	 * ワーカースレッドを生成します。
	 *
	 * NOTE:
	 * std::thread とは異なり、
	 * start() を呼び出さない限りスレッドは開始されません。
	 *
	 * @param c ワーカースレッドを保持するコンポーネント
	 */
	component_worker(component *c);

	/**
	 * ワーカースレッドを停止させ、破棄します。
	 */
	virtual ~component_worker();

	//disable copy constructor
	component_worker(const component_worker& obj) = delete;
	//disable operator=
	component_worker& operator=(const component_worker& obj) = delete;

	/**
	 * ワーカースレッド名を取得します。
	 *
	 * @return ワーカースレッド名
	 */
	virtual const char *get_name() const;

	/**
	 * ワーカースレッドのミューテックスを取得します。
	 *
	 * @return ワーカースレッドのミューテックス
	 */
	virtual std::mutex& mutex() const;

	/**
	 * ワーカースレッドの条件変数を取得します。
	 *
	 * @return ワーカースレッドの条件変数
	 */
	virtual std::condition_variable& condition() const;

	/**
	 * ワーカースレッドを保持するコンポーネントを取得します。
	 *
	 * @return コンポーネント
	 */
	virtual const component *get_component() const;

	/**
	 * ワーカースレッドを保持するコンポーネントを取得します。
	 *
	 * @return コンポーネント
	 */
	virtual component *get_component();

	/**
	 * ワーカースレッドのメイン処理の実行を続けるべきか取得します。
	 *
	 * @return 実行を続けるべきなら true、
	 * 停止すべきなら false
	 */
	bool is_running() const;

	/**
	 * ワーカースレッドのメイン処理の実行を続けるべきか設定します。
	 *
	 * @param f 実行を続けるべきなら true、
	 * 停止すべきなら false
	 */
	void set_running(bool f);

	/**
	 * このワーカースレッドの資源を待っているスレッドの待機状態を
	 * 強制的に解除すべきかを取得します。
	 *
	 * @return 強制解除すべきならば true、
	 * 強制解除すべきでなければ false
	 */
	virtual bool is_broken() const;

	/**
	 * このワーカースレッドの資源を待っているスレッドの待機状態を
	 * 強制的に解除すべきかを設定します。
	 *
	 * @param f 強制解除すべきならば true、
	 * 強制解除すべきでなければ false
	 */
	virtual void set_broken(bool f);

	/**
	 * フラッシュ処理を要求されているかどうかを取得します。
	 *
	 * @return フラッシュ処理を要求されていれば true、
	 * 要求されていなければ false
	 */
	virtual bool is_request_flush() const;

	/**
	 * フラッシュ処理を要求されているかどうかを設定します。
	 *
	 * @param f フラッシュ処理を要求されていれば true、
	 * 要求されていなければ false
	 */
	virtual void set_request_flush(bool f);

	/**
	 * フラッシュ処理が完了したかどうかを取得します。
	 *
	 * @return フラッシュ処理が完了していれば true、
	 * 完了していなければ false
	 */
	virtual bool is_flush_done() const;

	/**
	 * フラッシュ処理が完了したかどうかを設定します。
	 *
	 * @return フラッシュ処理が完了していれば true、
	 * 完了していなければ false
	 */
	virtual void set_flush_done(bool f);

	/**
	 * フラッシュ後のリスタート処理を要求されているかどうかを取得します。
	 *
	 * @return リスタート処理を要求されていれば true、
	 * 要求されていなければ false
	 */
	virtual bool is_request_restart() const;

	/**
	 * フラッシュ後のリスタート処理を要求されているかどうかを設定します。
	 *
	 * @param f リスタート処理を要求されていれば true、
	 * 要求されていなければ false
	 */
	virtual void set_request_restart(bool f);

	/**
	 * フラッシュ後のリスタート処理が完了したかどうかを取得します。
	 *
	 * @param f リスタート処理が完了していれば true、
	 * 完了していなければ false
	 */
	virtual bool is_restart_done() const;

	/**
	 * フラッシュ後のリスタート処理が完了したかどうかを設定します。
	 *
	 * @param f リスタート処理が完了していれば true、
	 * 完了していなければ false
	 */
	virtual void set_restart_done(bool f);

	/**
	 * フラッシュ処理が要求されるまで待ちます。
	 *
	 * フラグは自動的にクリアされます。
	 */
	virtual void wait_request_flush();

	/**
	 * フラッシュ処理が完了するまで待ちます。
	 *
	 * フラグは自動的にクリアされます。
	 */
	virtual void wait_flush_done();

	/**
	 * フラッシュ後のリスタート処理が要求されるまで待ちます。
	 *
	 * フラグは自動的にクリアされます。
	 */
	virtual void wait_request_restart();

	/**
	 * フラッシュ後のリスタート処理が完了するまで待ちます。
	 *
	 * フラグは自動的にクリアされます。
	 */
	virtual void wait_restart_done();

	/**
	 * ワーカースレッドを開始します。
	 */
	virtual void start();

	/**
	 * ワーカースレッドが終了するまで待ちます。
	 */
	virtual void join();

	/**
	 * ワーカースレッドのメイン処理を行います。
	 *
	 * 継承先のクラスでオーバーライドしてください。
	 */
	virtual void run();

protected:
	/**
	 * ワーカースレッドを保持するコンポーネントの状態をチェックし、
	 * 待ちを強制キャンセルすべきなら例外をスローします。
	 *
	 * @param lock コンポーネントのロック
	 * @see component::is_broken()
	 */
	virtual void error_if_broken(std::unique_lock<std::mutex>& lock) const;

protected:
	/**
	 * ワーカースレッドのメイン関数です。
	 *
	 * @param ワーカースレッドへのポインタ
	 * @return 常に nullptr
	 */
	static void *component_worker_thread_main(component_worker *arg);

private:
	//ワーカースレッドのロック
	mutable std::mutex mut;
	//ワーカースレッドの状態変数
	mutable std::condition_variable cond;

	//ワーカースレッドを保持するコンポーネント
	component *comp;

	//スレッド
	std::thread *th_work;

	//メイン処理を続けるかどうかのフラグ
	bool f_running;
	//待機の強制解除フラグ
	bool f_broken;
	//フラッシュ要求フラグ
	bool f_request_flush;
	//フラッシュ完了フラグ
	bool f_flush_done;
	//リスタート要求フラグ
	bool f_request_restart;
	//リスタート完了フラグ
	bool f_restart_done;

};

} //namespace mf

#endif //OMX_MF_COMPONENT_WORKER_HPP__
