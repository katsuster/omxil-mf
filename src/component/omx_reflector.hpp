#ifndef OMX_REFLECTOR_HPP__
#define OMX_REFLECTOR_HPP__

#include <OMX_Component.h>
#include <OMX_Core.h>

#include "consts.h"
#include "util/omx_enum_name.hpp"
#include "debug/scoped_log.hpp"

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
class omx_reflector {
public:
	//親クラス
	//typedef xxxx super;

	//disable default constructor
	omx_reflector() = delete;

	omx_reflector(OMX_COMPONENTTYPE *c, OMX_STRING cname)
	: omx_comp(c), omx_name(cname) {
		scoped_log_begin;

		if (c == NULL) {
			std::string msg(__func__);
			msg += ": failed to construct component.";
			throw std::runtime_error(msg);
		}

		//set this component pointer
		omx_comp->pComponentPrivate      = this;

		//set OpenMAX member functions
		omx_comp->GetComponentVersion    = comp_GetComponentVersion;
		omx_comp->SendCommand            = comp_SendCommand;
		omx_comp->GetParameter           = comp_GetParameter;
		omx_comp->SetParameter           = comp_SetParameter;
		omx_comp->GetConfig              = comp_GetConfig;
		omx_comp->SetConfig              = comp_SetConfig;
		omx_comp->GetExtensionIndex      = comp_GetExtensionIndex;
		omx_comp->GetState               = comp_GetState;
		omx_comp->ComponentTunnelRequest = comp_ComponentTunnelRequest;
		omx_comp->UseBuffer              = comp_UseBuffer;
		omx_comp->AllocateBuffer         = comp_AllocateBuffer;
		omx_comp->FreeBuffer             = comp_FreeBuffer;
		omx_comp->EmptyThisBuffer        = comp_EmptyThisBuffer;
		omx_comp->FillThisBuffer         = comp_FillThisBuffer;
		omx_comp->SetCallbacks           = comp_SetCallbacks;
		omx_comp->ComponentDeInit        = comp_ComponentDeInit;
		omx_comp->UseEGLImage            = comp_UseEGLImage;
		omx_comp->ComponentRoleEnum      = comp_ComponentRoleEnum;
	}

	virtual ~omx_reflector() {
		scoped_log_begin;

		omx_comp->pComponentPrivate = nullptr;
	}

	//disable copy constructor
	omx_reflector(const omx_reflector& obj) = delete;

	//disable operator=
	omx_reflector& operator=(const omx_reflector& obj) = delete;

	virtual const char *get_name() const {
		return "omx_reflector";
	}

	/**
	 * OpenMAX コンポーネントへのポインタを取得します。
	 *
	 * @return OpenMAX コンポーネントへのポインタ
	 */
	virtual const OMX_COMPONENTTYPE *get_omx_component() const {
		return omx_comp;
	}

	/**
	 * OpenMAX コンポーネントへのポインタを取得します。
	 *
	 * @return OpenMAX コンポーネントへのポインタ
	 */
	virtual OMX_COMPONENTTYPE *get_omx_component() {
		return omx_comp;
	}

	/**
	 * OpenMAX コンポーネント名を取得します。
	 *
	 * @return OpenMAX コンポーネント名
	 */
	virtual const std::string& get_component_name() const {
		return omx_name;
	}


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
	static inline omx_reflector *get_instance(OMX_HANDLETYPE hComponent) {
		OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
		omx_reflector *comp = (omx_reflector *) omx_comp->pComponentPrivate;

		return comp;
	}

	//----------
	//OpenMAX member function reflectors
	//----------

	static OMX_ERRORTYPE comp_GetComponentVersion(OMX_HANDLETYPE hComponent, OMX_STRING pComponentName, OMX_VERSIONTYPE *pComponentVersion, OMX_VERSIONTYPE *pSpecVersion, OMX_UUIDTYPE *pComponentUUID) {
		scoped_log_begin;

		if (hComponent == nullptr) {
			return OMX_ErrorInvalidComponent;
		}
		omx_reflector *comp = get_instance(hComponent);

		return comp->GetComponentVersion(hComponent, pComponentName, pComponentVersion, pSpecVersion, pComponentUUID);
	}

	static OMX_ERRORTYPE comp_SendCommand(OMX_HANDLETYPE hComponent, OMX_COMMANDTYPE Cmd, OMX_U32 nParam1, OMX_PTR pCmdData) {
		scoped_log_begin;

		if (hComponent == nullptr) {
			return OMX_ErrorInvalidComponent;
		}
		omx_reflector *comp = get_instance(hComponent);

		return comp->SendCommand(hComponent, Cmd, nParam1, pCmdData);
	}

	static OMX_ERRORTYPE comp_GetParameter(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) {
		scoped_log_begin;

		if (hComponent == nullptr) {
			return OMX_ErrorInvalidComponent;
		}
		omx_reflector *comp = get_instance(hComponent);

		return comp->GetParameter(hComponent, nParamIndex, pComponentParameterStructure);
	}

	static OMX_ERRORTYPE comp_SetParameter(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentParameterStructure) {
		scoped_log_begin;

		if (hComponent == nullptr) {
			return OMX_ErrorInvalidComponent;
		}
		omx_reflector *comp = get_instance(hComponent);

		return comp->SetParameter(hComponent, nIndex, pComponentParameterStructure);
	}

	static OMX_ERRORTYPE comp_GetConfig(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure) {
		scoped_log_begin;

		if (hComponent == nullptr) {
			return OMX_ErrorInvalidComponent;
		}
		omx_reflector *comp = get_instance(hComponent);

		return comp->GetConfig(hComponent, nIndex, pComponentConfigStructure);
	}

	static OMX_ERRORTYPE comp_SetConfig(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure) {
		scoped_log_begin;

		if (hComponent == nullptr) {
			return OMX_ErrorInvalidComponent;
		}
		omx_reflector *comp = get_instance(hComponent);

		return comp->SetConfig(hComponent, nIndex, pComponentConfigStructure);
	}

	static OMX_ERRORTYPE comp_GetExtensionIndex(OMX_HANDLETYPE hComponent, OMX_STRING cParameterName, OMX_INDEXTYPE *pIndexType) {
		scoped_log_begin;

		if (hComponent == nullptr) {
			return OMX_ErrorInvalidComponent;
		}
		omx_reflector *comp = get_instance(hComponent);

		return comp->GetExtensionIndex(hComponent, cParameterName, pIndexType);
	}

	static OMX_ERRORTYPE comp_GetState(OMX_HANDLETYPE hComponent, OMX_STATETYPE *pState) {
		scoped_log_begin;

		if (hComponent == nullptr) {
			return OMX_ErrorInvalidComponent;
		}
		omx_reflector *comp = get_instance(hComponent);

		return comp->GetState(hComponent, pState);
	}

	static OMX_ERRORTYPE comp_ComponentTunnelRequest(OMX_HANDLETYPE hComponent, OMX_U32 nPort, OMX_HANDLETYPE hTunneledComp, OMX_U32 nTunneledPort, OMX_TUNNELSETUPTYPE *pTunnelSetup) {
		scoped_log_begin;

		if (hComponent == nullptr) {
			return OMX_ErrorInvalidComponent;
		}
		omx_reflector *comp = get_instance(hComponent);

		return comp->ComponentTunnelRequest(hComponent, nPort, hTunneledComp, nTunneledPort, pTunnelSetup);
	}

	static OMX_ERRORTYPE comp_UseBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8 *pBuffer) {
		scoped_log_begin;

		if (hComponent == nullptr) {
			return OMX_ErrorInvalidComponent;
		}
		omx_reflector *comp = get_instance(hComponent);

		return comp->UseBuffer(hComponent, ppBufferHdr, nPortIndex, pAppPrivate, nSizeBytes, pBuffer);
	}

	static OMX_ERRORTYPE comp_AllocateBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBuffer, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes) {
		scoped_log_begin;

		if (hComponent == nullptr) {
			return OMX_ErrorInvalidComponent;
		}
		omx_reflector *comp = get_instance(hComponent);

		return comp->AllocateBuffer(hComponent, ppBuffer, nPortIndex, pAppPrivate, nSizeBytes);
	}

	static OMX_ERRORTYPE comp_FreeBuffer(OMX_HANDLETYPE hComponent, OMX_U32 nPortIndex, OMX_BUFFERHEADERTYPE *pBuffer) {
		scoped_log_begin;

		if (hComponent == nullptr) {
			return OMX_ErrorInvalidComponent;
		}
		omx_reflector *comp = get_instance(hComponent);

		return comp->FreeBuffer(hComponent, nPortIndex, pBuffer);
	}

	static OMX_ERRORTYPE comp_EmptyThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE *pBuffer) {
		scoped_log_begin;

		if (hComponent == nullptr) {
			return OMX_ErrorInvalidComponent;
		}
		omx_reflector *comp = get_instance(hComponent);

		return comp->EmptyThisBuffer(hComponent, pBuffer);
	}

	static OMX_ERRORTYPE comp_FillThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE *pBuffer) {
		scoped_log_begin;

		if (hComponent == nullptr) {
			return OMX_ErrorInvalidComponent;
		}
		omx_reflector *comp = get_instance(hComponent);

		return comp->FillThisBuffer(hComponent, pBuffer);
	}

	static OMX_ERRORTYPE comp_SetCallbacks(OMX_HANDLETYPE hComponent, OMX_CALLBACKTYPE *pCallbacks, OMX_PTR pAppData) {
		scoped_log_begin;

		if (hComponent == nullptr) {
			return OMX_ErrorInvalidComponent;
		}
		omx_reflector *comp = get_instance(hComponent);

		return comp->SetCallbacks(hComponent, pCallbacks, pAppData);
	}

	static OMX_ERRORTYPE comp_ComponentDeInit(OMX_HANDLETYPE hComponent) {
		scoped_log_begin;

		if (hComponent == nullptr) {
			return OMX_ErrorInvalidComponent;
		}
		omx_reflector *comp = get_instance(hComponent);
		OMX_ERRORTYPE err;

		err = comp->ComponentDeInit(hComponent);

		//NOTE: ComponentDeInit メンバ関数内で delete this; すると、
		//this ポインタが無効になり SEGV の危険あり。
		//必ず static 関数内で delete すること
		dprint("delete component:%s, ptr:%p.\n",
			comp->get_component_name().c_str(), comp);
		delete comp;

		return err;
	}

	static OMX_ERRORTYPE comp_UseEGLImage(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE **ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, void *eglImage) {
		scoped_log_begin;

		if (hComponent == nullptr) {
			return OMX_ErrorInvalidComponent;
		}
		omx_reflector *comp = get_instance(hComponent);

		return comp->UseEGLImage(hComponent, ppBufferHdr, nPortIndex, pAppPrivate, eglImage);
	}

	static OMX_ERRORTYPE comp_ComponentRoleEnum(OMX_HANDLETYPE hComponent, OMX_U8 *cRole, OMX_U32 nIndex) {
		scoped_log_begin;

		if (hComponent == nullptr) {
			return OMX_ErrorInvalidComponent;
		}
		omx_reflector *comp = get_instance(hComponent);

		return comp->ComponentRoleEnum(hComponent, cRole, nIndex);
	}


private:
	//OpenMAX コンポーネント
	OMX_COMPONENTTYPE *omx_comp;
	//コンポーネント名
	std::string omx_name;

};

} //namespace mf

#endif //OMX_REFLECTOR_HPP__
