#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "CComponent.h"


// 모든 게임오브젝트와 계층을 관리
BEGIN(Engine)

class CObjectPool;

class ENGINE_DLL CObjectManager :public CBase
{
	DECLARE_SINGLETON(CObjectManager)
	friend CObjectPool;
private:
	explicit CObjectManager();
	virtual ~CObjectManager();

public:
	HRESULT Readay_ObjectManager(_uint iSceneNum); // 씬 개수 만큼 Layer 컨테이너 생성
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, CGameObject* pProtoType); // 원본 등록
	HRESULT Add_GameObject(const _tchar* pPrototypeTag, _uint iSceneIdx, const _tchar* pLayerTag, void* pArg = nullptr); // 복제 후 추가
	CGameObject* Clone_GameObject(const _tchar* pPrototypeTag, _uint iSceneIdx, const _tchar* pLayerTag, void* pArg = nullptr); // 복제 후 추가 후 반환
	void Update(_float fTimeDelta); 
	void Late_Update(_float fTImeDelta);
	void Clear(_uint iSceneIdx); // 특정 씬 정리
	void Clear_Layer(_uint iSceneIdx, const _tchar* pLayerTag); // 특정 Layer 정리

public:
	CGameObject* Find_Object(_uint iSceneIdx, const _tchar* pLayerTag, _uint iIdx = 0); // 오브젝트 찾기
	list<CGameObject*>* Get_ObjectList(_uint iSceneID, const _tchar* pLayerTag); // 레이어 안의 오브젝트 찾기
	CComponent* Get_Component(_uint iSceneIdx, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIdx = 0); // 오브젝트 안에 있는 컴포넌트 반환
	std::vector<MAPOBJECTDATA> ExportObjectData(_uint iSceneID, const _tchar* pLayerTag);
	std::vector<PREFABDATA> ExportPrefabData();
	std::vector<PREFABDATA> Export_InstancedPrefabData(_uint iSceneID);

	
private:
	CGameObject* Find_Prototype(const _tchar* pProtoTypeTag);  // 원본 찾기
	CLayer* Find_Layer(_uint iSceneIdx, const _tchar* pLayerTag); // 레이어 찾기

private:
	map<const _tchar*, CGameObject*>	m_objMap;
	typedef map<const _tchar*, CGameObject*>	OBJMAP;

private: // 그룹을 지어 씬 별로 객체 추가
	_uint			m_iSceneNum = 0;
	map<const _tchar*, CLayer*>*	m_pLayers = nullptr;	
	typedef map<const _tchar*, class CLayer*>	LAYERS;
public:
	virtual void Free() override;
};

END

