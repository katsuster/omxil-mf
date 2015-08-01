
#ifndef OMXIL_COMP_HPP__
#define OMXIL_COMP_HPP__

#include <string>
#include <vector>
#include <map>
#include <condition_variable>
#include <mutex>

#include <OMX_Core.h>
#include <OMX_Component.h>
#include <OMX_Audio.h>
#include <OMX_Image.h>
#include <OMX_Video.h>
#include <OMX_Other.h>

struct buffer_attr {
	bool used;
};

class omxil_comp {
public:
	typedef typename std::vector<OMX_BUFFERHEADERTYPE *> buflist_type;

	omxil_comp(const char *comp_name);
	virtual ~omxil_comp();

	virtual OMX_HANDLETYPE get_component();

	//OpenMAX IL API wapper

	virtual OMX_ERRORTYPE GetComponentVersion(OMX_STRING pComponentName, OMX_VERSIONTYPE *pComponentVersion, OMX_VERSIONTYPE *pSpecVersion, OMX_UUIDTYPE *pComponentUUID) const;
	virtual OMX_ERRORTYPE SendCommand(OMX_COMMANDTYPE Cmd, OMX_U32 nParam, OMX_PTR pCmdData);
	virtual OMX_ERRORTYPE GetParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) const;
	virtual OMX_ERRORTYPE SetParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);
	virtual OMX_ERRORTYPE GetConfig(OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure) const;
	virtual OMX_ERRORTYPE SetConfig(OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure);
	virtual OMX_ERRORTYPE GetExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE *pIndexType) const;
	virtual OMX_ERRORTYPE GetState(OMX_STATETYPE *pState) const;
	virtual OMX_ERRORTYPE UseBuffer(OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8 *pBuffer);
	virtual OMX_ERRORTYPE AllocateBuffer(OMX_BUFFERHEADERTYPE **ppBuffer, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes);
	virtual OMX_ERRORTYPE FreeBuffer(OMX_U32 nPortIndex, OMX_BUFFERHEADERTYPE *pBuffer);
	virtual OMX_ERRORTYPE EmptyThisBuffer(OMX_BUFFERHEADERTYPE *pBuffer);
	virtual OMX_ERRORTYPE FillThisBuffer(OMX_BUFFERHEADERTYPE *pBuffer);

	//OpenMAX IL Callbacks wrapper

	virtual OMX_ERRORTYPE EventHandler(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
	virtual OMX_ERRORTYPE EmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer);
	virtual OMX_ERRORTYPE FillBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer);

	//Utilities

	/**
	 * コンポーネントが指定した状態に変化するまで待ちます。
	 *
	 * @param s コンポーネントの状態
	 */
	virtual void wait_state_changed(OMX_STATETYPE s) const;

	virtual buflist_type *find_buflist(OMX_U32 port);
	virtual const buflist_type *find_buflist(OMX_U32 port) const;

	/**
	 * ポートとバッファを関連付けます。
	 *
	 * @param port ポート番号
	 * @param buf  OpenMAX IL バッファ
	 */
	virtual void register_buffer(OMX_U32 port, OMX_BUFFERHEADERTYPE *buf);

	/**
	 * 指定したポートに関連付けられているバッファのうち、
	 * 未使用のバッファを取得します。
	 *
	 * バッファには使用中の印が付けられます。
	 *
	 * @param port ポート番号
	 * @param buf  OpenMAX IL バッファ
	 */
	virtual OMX_BUFFERHEADERTYPE *get_free_buffer(OMX_U32 port) const;

	/**
	 * 全てのバッファが使用中かどうかを取得します。
	 *
	 * N 個のバッファがあったとき、
         *
	 * 使用中の
	 * バッファ数  | is_used_all | is_free_all |
	 * ------------+-------------+-------------+
	 * 0           | false       | true        |
	 * 1 ... N - 1 | false       | false       |
	 * N           | true        | false       |
	 * ------------+-------------+-------------+
	 *
	 * @param port ポート番号
	 * @return 全てのバッファが使用中ならば true、
	 * 1つでもバッファが使用可能ならば false
	 */
	virtual bool is_used_all_buffer(OMX_U32 port) const;

	/**
	 * 全てのバッファが使用可能かどうかを取得します。
	 *
	 * N 個のバッファがあったとき、
         *
	 * 使用中の
	 * バッファ数  | is_used_all | is_free_all |
	 * ------------+-------------+-------------+
	 * 0           | false       | true        |
	 * 1 ... N - 1 | false       | false       |
	 * N           | true        | false       |
	 * ------------+-------------+-------------+
	 *
	 * @param port ポート番号
	 * @return 全てのバッファが使用可能ならば true、
	 * 1つでもバッファが使用中ならば false
	 */
	virtual bool is_free_all_buffer(OMX_U32 port) const;

	/**
	 * 1つ以上のバッファが使用可能になるまで待ちます。
	 *
	 * @param port ポート番号
	 */
	virtual void wait_buffer_free(OMX_U32 port) const;

	/**
	 * 全てのバッファが使用可能になるまで待ちます。
	 *
	 * @param port ポート番号
	 */
	virtual void wait_all_buffer_free(OMX_U32 port) const;

	/**
	 * 全バッファの使用状況を表示します。
	 *
	 * @param port ポート番号
	 */
	virtual void dump_all_buffer(OMX_U32 port) const;

	virtual OMX_ERRORTYPE get_param_port_definition(OMX_U32 port_index, OMX_PARAM_PORTDEFINITIONTYPE *def) const;
	virtual OMX_ERRORTYPE get_param_audio_init(OMX_PORT_PARAM_TYPE *param) const;
	virtual OMX_ERRORTYPE get_param_image_init(OMX_PORT_PARAM_TYPE *param) const;
	virtual OMX_ERRORTYPE get_param_video_init(OMX_PORT_PARAM_TYPE *param) const;
	virtual OMX_ERRORTYPE get_param_other_init(OMX_PORT_PARAM_TYPE *param) const;
	virtual OMX_ERRORTYPE get_param_audio_format(OMX_U32 port_index, OMX_U32 fmt_index, OMX_AUDIO_PARAM_PORTFORMATTYPE *format) const;
	virtual OMX_ERRORTYPE get_param_image_format(OMX_U32 port_index, OMX_U32 fmt_index, OMX_IMAGE_PARAM_PORTFORMATTYPE *format) const;
	virtual OMX_ERRORTYPE get_param_video_format(OMX_U32 port_index, OMX_U32 fmt_index, OMX_VIDEO_PARAM_PORTFORMATTYPE *format) const;
	virtual OMX_ERRORTYPE get_param_other_format(OMX_U32 port_index, OMX_U32 fmt_index, OMX_OTHER_PARAM_PORTFORMATTYPE *format) const;

public:
	//Utilities

	static OMX_ERRORTYPE fill_version(OMX_VERSIONTYPE *v);

protected:
	static OMX_ERRORTYPE gate_EventHandler(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
	static OMX_ERRORTYPE gate_EmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer);
	static OMX_ERRORTYPE gate_FillBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer);


private:
	OMX_HANDLETYPE comp;
	OMX_CALLBACKTYPE callbacks;
	std::string name;

	mutable std::recursive_mutex mut_comp;
	mutable std::condition_variable_any cond_comp;
	OMX_STATETYPE state_done;
	std::map<OMX_U32, buflist_type *> map_buflist;

};

#endif //OMXIL_COMP_HPP__
