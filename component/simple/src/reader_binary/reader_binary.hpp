
#ifndef OMX_MF_COMPONENT_SIMPLE__READER_BINARY_HPP__
#define OMX_MF_COMPONENT_SIMPLE__READER_BINARY_HPP__

#include <omxil_mf/omxil_mf.h>
#include <omxil_mf/component.hpp>
#include <omxil_mf/port_video.hpp>
#include <omxil_mf/scoped_log.hpp>

namespace mf {

class reader_binary : public component {
public:
	class worker_main : public component_worker {
	public:
		//親クラス
		typedef component_worker super;

		worker_main(reader_binary *c);
		virtual ~worker_main();

		virtual const char *get_name() const;

		virtual port *get_out_port();
		virtual const port *get_out_port() const;
		virtual void set_out_port(port *p);

		virtual void run();

	private:
		reader_binary *comp;
		port *out_port;
	};


public:
	//親クラス
	typedef component super;

	reader_binary(OMX_COMPONENTTYPE *c, const char *cname);
	virtual ~reader_binary();

	virtual const char *get_name() const;

	virtual std::string& get_uri();
	virtual const std::string& get_uri() const;
	virtual void set_uri(char *uri);
	virtual void set_uri(std::string& uri);

	OMX_ERRORTYPE GetParameter(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);
	OMX_ERRORTYPE SetParameter(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);

protected:
	virtual worker_main& get_worker();
	virtual const worker_main& get_worker() const;

	virtual OMX_U32 get_audio_start_port();
	virtual OMX_U32 get_video_start_port();

public:
	/**
	 * OpenMAX コンポーネントのハンドルから、
	 * reader_binary クラスのインスタンスへのポインタを取得します。
	 *
	 * @param hComponent OpenMAX コンポーネント
	 * @return component インスタンスへのポインタ
	 */
	static reader_binary *get_instance(OMX_HANDLETYPE hComponent);

private:
	worker_main wk_main;

	std::string uri_target;
};

} //namespace mf

#endif //OMX_MF_COMPONENT_SIMPLE__READER_BINARY_HPP__

