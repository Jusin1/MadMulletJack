#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "CComponent.h"
// 생성된 오브젝트들을 보관 : 내가 기준 정해서
BEGIN(Engine)

class ENGINE_DLL CObjectManager :public CBase
{
	DECLARE_SINGLETON(CObjectManager)

private:
	explicit CObjectManager();
	virtual ~CObjectManager();

public:
	HRESULT Readay_ObjectManager(_uint iSceneNum);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, CGameObject* pProtoType);
	HRESULT Add_GameObject(const _tchar* pPrototypeTag, _uint iSceneIdx, const _tchar* pLayerTag, void* pArg = nullptr);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTImeDelta);
	void Clear(_uint iSceneIdx);
	void Clear_Layer(_uint iSceneIdx, const _tchar* pLayerTag);

public:
	CGameObject* Find_Object(_uint iSceneIdx, const _tchar* pLayerTag, _uint iIdx = 0);
	list<CGameObject*>* Get_ObjectList(_uint iSceneID, const _tchar* pLayerTag);
	CComponent* Get_Component(_uint iSceneIdx, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIdx = 0);


private:
	map<const _tchar*, CGameObject*>	m_objMap;
	typedef map<const _tchar*, CGameObject*>	OBJMAP;

private: // 그룹을 지어 씬 별로 객체 추가
	_uint			m_iSceneNum = 0;
	map<const _tchar*, CLayer*>*	m_pLayers = nullptr;	
	typedef map<const _tchar*, class CLayer*>	LAYERS;

private:
	CGameObject* Find_Prototype(const _tchar* pProtoTypeTag); 
	CLayer* Find_Layer(_uint iSceneIdx, const _tchar* pLayerTag);

public:
	virtual void Free() override;
};

END

