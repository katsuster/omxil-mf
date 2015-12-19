#ifndef OMX_MF_REFLECTOR_HPP__
#define OMX_MF_REFLECTOR_HPP__

#include <string>

#include <OMX_Component.h>
#include <OMX_Core.h>

#include <omxil_mf/base.h>

namespace mf {

/**
 * OpenMAX コンポーネント、
 * 独自コンポーネントクラスの接続部分となる基底クラス。
 *
 * OpenMAX コンポーネントの
 * 初期化、解放処理を行います。
 *
 * OpenMAX コンポーネントの各メソッド（OMX_SendCommand など）を
 * コンポーネントクラスのメンバ関数呼び出しに変換します。
 * メンバ関数は全て純粋仮想関数として宣言していますので、
 * 継承先のクラスで実装する必要があります。
 *
 *
 * （参考）OpenMAX コンポーネントと、
 * このクラスの関係について。
 *
 * OpenMAX の API には常に OpenMAX コンポーネント
 * OMX_COMPONENTTYPE へのポインタが渡され、
 * 独自コンポーネントのポインタは渡されません。
 *
 * 独自コンポーネントには 1つルールがあり、
 * OpenMAX コンポーネントから独自コンポーネントを取得可能にしています。
 *
 * ルール 1-1: OpenMAX コンポーネントの pComponentPrivate に、
 * 独自コンポーネントを継承した構造体のポインタを設定する。
 *
 * これにより、下記のようにこのクラスのインスタンスを取得可能です。
 *
 * <pre>
 * OMX_ERRORTYPE comp_GetState(OMX_HANDLETYPE hComponent, OMX_STATETYPE *pState) {
 *     OMX_COMPONENTTYPE *a = (OMX_COMPONENTTYPE *)hComponent;
 *     omx_reflector *b = (omx_reflector *)a->pComponentPrivate;
 *     ...
 * </pre>
 *
 * しかし、将来、変数名や構造が変わる可能性に備えて、
 * キャストではなく get_instance() メンバ関数を使用してください。
 *
 * <pre>
 * OMX_ERRORTYPE comp_GetState(OMX_HANDLETYPE hComponent, OMX_STATETYPE *pState) {
 *     omx_reflector *comp = omx_reflector::get_instance(hComponent);
 *     ...
 * </pre>
 *
 */
class OMX_MF_API_CLASS omx_reflector {
public:
	//親クラス
	//typedef xxxx super;

	//disable default constructor
	omx_reflector() = delete;

	omx_reflector(OMX_COMPONENTTYPE *c, const char *cname);

	virtual ~omx_reflector();

	//disable copy constructor
	omx_reflector(const omx_reflector& obj) = delete;

	//disable operator=
	omx_reflector& operator=(const omx_reflector& obj) = delete;

	virtual const char *get_name() const;

	/**
	 * OpenMAX コンポーネントへのポインタを取得します。
	 *
	 * @return OpenMAX コンポーネントへのポインタ
	 */
	virtual const OMX_COMPONENTTYPE *get_omx_component() const;

	/**
	 * OpenMAX コンポーネントへのポインタを取得します。
	 *
	 * @return OpenMAX コンポーネントへのポインタ
	 */
	virtual OMX_COMPONENTTYPE *get_omx_component();

	/**
	 * OpenMAX コンポーネント名を取得します。
	 *
	 * @return OpenMAX コンポーネント名
	 */
	virtual const std::string& get_component_name() const;


	//----------
	//OpenMAX member functions
	//----------

	virtual OMX_ERRORTYPE GetComponentVersion(OMX_HANDLETYPE hComponent, OMX_STRING pComponentName, OMX_VERSIONTYPE *pComponentVersion, OMX_VERSIONTYPE *pSpecVersion, OMX_UUIDTYPE *pComponentUUID) = 0;
	virtual OMX_ERRORTYPE SendCommand(OMX_HANDLETYPE hComponent, OMX_COMMANDTYPE Cmd, OMX_U32 nParam, OMX_PTR pCmdData) = 0;
	virtual OMX_ERRORTYPE GetParameter(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) = 0;
	virtual OMX_ERRORTYPE SetParameter(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) = 0;
	virtual OMX_ERRORTYPE GetConfig(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure) = 0;
	virtual OMX_ERRORTYPE SetConfig(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure) = 0;
	virtual OMX_ERRORTYPE GetExtensionIndex(OMX_HANDLETYPE hComponent, OMX_STRING cParameterName, OMX_INDEXTYPE *pIndexType) = 0;
	virtual OMX_ERRORTYPE GetState(OMX_HANDLETYPE hComponent, OMX_STATETYPE *pState) = 0;
	virtual OMX_ERRORTYPE ComponentTunnelRequest(OMX_HANDLETYPE hComponent, OMX_U32 nPort, OMX_HANDLETYPE hTunneledComp, OMX_U32 nTunneledPort, OMX_TUNNELSETUPTYPE *pTunnelSetup) = 0;
	virtual OMX_ERRORTYPE UseBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8 *pBuffer) = 0;
	virtual OMX_ERRORTYPE AllocateBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBuffer, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes) = 0;
	virtual OMX_ERRORTYPE FreeBuffer(OMX_HANDLETYPE hComponent, OMX_U32 nPortIndex, OMX_BUFFERHEADERTYPE *pBuffer) = 0;
	virtual OMX_ERRORTYPE EmptyThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE *pBuffer) = 0;
	virtual OMX_ERRORTYPE FillThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE *pBuffer) = 0;
	virtual OMX_ERRORTYPE SetCallbacks(OMX_HANDLETYPE hComponent, OMX_CALLBACKTYPE *pCallbacks, OMX_PTR pAppData) = 0;
	virtual OMX_ERRORTYPE ComponentDeInit(OMX_HANDLETYPE hComponent) = 0;
	virtual OMX_ERRORTYPE UseEGLImage(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, void *eglImage) = 0;
	virtual OMX_ERRORTYPE ComponentRoleEnum(OMX_HANDLETYPE hComponent, OMX_U8 *cRole, OMX_U32 nIndex) = 0;


protected:
	/**
	 * OpenMAX コンポーネントのハンドルから、
	 * omx_reflector クラスのインスタンスへのポインタを取得します。
	 *
	 * @param hComponent OpenMAX コンポーネント
	 * @return omx_reflector インスタンスへのポインタ
	 */
	static omx_reflector *get_instance(OMX_HANDLETYPE hComponent);

	//----------
	//OpenMAX member function reflectors
	//----------

	static OMX_ERRORTYPE comp_GetComponentVersion(OMX_HANDLETYPE hComponent, OMX_STRING pComponentName, OMX_VERSIONTYPE *pComponentVersion, OMX_VERSIONTYPE *pSpecVersion, OMX_UUIDTYPE *pComponentUUID);
	static OMX_ERRORTYPE comp_SendCommand(OMX_HANDLETYPE hComponent, OMX_COMMANDTYPE Cmd, OMX_U32 nParam1, OMX_PTR pCmdData);
	static OMX_ERRORTYPE comp_GetParameter(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);
	static OMX_ERRORTYPE comp_SetParameter(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentParameterStructure);
	static OMX_ERRORTYPE comp_GetConfig(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure);
	static OMX_ERRORTYPE comp_SetConfig(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure);
	static OMX_ERRORTYPE comp_GetExtensionIndex(OMX_HANDLETYPE hComponent, OMX_STRING cParameterName, OMX_INDEXTYPE *pIndexType);
	static OMX_ERRORTYPE comp_GetState(OMX_HANDLETYPE hComponent, OMX_STATETYPE *pState);
	static OMX_ERRORTYPE comp_ComponentTunnelRequest(OMX_HANDLETYPE hComponent, OMX_U32 nPort, OMX_HANDLETYPE hTunneledComp, OMX_U32 nTunneledPort, OMX_TUNNELSETUPTYPE *pTunnelSetup);
	static OMX_ERRORTYPE comp_UseBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8 *pBuffer);
	static OMX_ERRORTYPE comp_AllocateBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBuffer, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes);
	static OMX_ERRORTYPE comp_FreeBuffer(OMX_HANDLETYPE hComponent, OMX_U32 nPortIndex, OMX_BUFFERHEADERTYPE *pBuffer);
	static OMX_ERRORTYPE comp_EmptyThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE *pBuffer);
	static OMX_ERRORTYPE comp_FillThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE *pBuffer);
	static OMX_ERRORTYPE comp_SetCallbacks(OMX_HANDLETYPE hComponent, OMX_CALLBACKTYPE *pCallbacks, OMX_PTR pAppData);
	static OMX_ERRORTYPE comp_ComponentDeInit(OMX_HANDLETYPE hComponent);
	static OMX_ERRORTYPE comp_UseEGLImage(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, void *eglImage);
	static OMX_ERRORTYPE comp_ComponentRoleEnum(OMX_HANDLETYPE hComponent, OMX_U8 *cRole, OMX_U32 nIndex);

private:
	//OpenMAX コンポーネント
	OMX_COMPONENTTYPE *omx_comp;
	//コンポーネント名
	std::string omx_name;

};

} //namespace mf

#endif //OMX_MF_REFLECTOR_HPP__
