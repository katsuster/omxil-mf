
#ifndef OMX_MF_COMPONENT_SIMPLE__WRITER_BINARY_HPP__
#define OMX_MF_COMPONENT_SIMPLE__WRITER_BINARY_HPP__

#include <omxil_mf/omxil_mf.h>
#include <omxil_mf/component.hpp>
#include <omxil_mf/port_video.hpp>
#include <omxil_mf/scoped_log.hpp>

namespace mf {

class writer_binary : public component {
public:
	class worker_main : public component_worker {
	public:
		//親クラス
		typedef component_worker super;

		worker_main(writer_binary *c);
		virtual ~worker_main();

		virtual const char *get_name() const;

		virtual port *get_in_port();
		virtual const port *get_in_port() const;
		virtual void set_in_port(port *p);

		virtual void write_file(int fd);
		virtual void run();

	private:
		writer_binary *comp;
		port *in_port;

	};


public:
	//親クラス
	typedef component super;

	writer_binary(OMX_COMPONENTTYPE *c, const char *cname);
	virtual ~writer_binary();

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
	 * writer_binary クラスのインスタンスへのポインタを取得します。
	 *
	 * @param hComponent OpenMAX コンポーネント
	 * @return component インスタンスへのポインタ
	 */
	static writer_binary *get_instance(OMX_HANDLETYPE hComponent);

private:
	worker_main wk_main;

	std::string uri_target;
};

} //namespace mf

#endif //OMX_MF_COMPONENT_EMPTY__RENDERER_NULL_HPP__

