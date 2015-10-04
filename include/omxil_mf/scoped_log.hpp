#ifndef OMX_MF_SCOPED_LOG_HPP__
#define OMX_MF_SCOPED_LOG_HPP__

#include <omxil_mf/dprint.h>

#define scoped_log_begin    mf::scoped_log __tmp_scoped_log__(DPRINT_FUNC, __LINE__)

namespace mf {

/**
 * 関数の In/Out を出力するためのデバッグ用オブジェクトです。
 */
class scoped_log {
public:
	scoped_log(const char *f, int n) : func(f), num(n) {
		dprint_cont("[% 5d] in  %s:%d\n", (int)thread_id(), func, num);
	}

	~scoped_log() {
		dprint_cont("[% 5d] out %s\n", (int)thread_id(), func);
	}

private:
	const char *func;
	int num;
};

} //namespace mf

#endif //OMX_MF_SCOPED_LOG_HPP__
