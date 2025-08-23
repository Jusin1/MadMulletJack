#include "CObjectManager.h"
#include "CFileManager.h"
#include "CLayer.h"


IMPLEMENT_SINGLETON(CObjectManager)

CObjectManager::CObjectManager()
	: m_iSceneNum(0)
	, m_pLayers(nullptr)
{
}

CObjectManager::~CObjectManager()
{
	Free();
}

// Layer 전부 초기화
HRESULT CObjectManager::Readay_ObjectManager(_uint iSceneNum)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_pLayers = new LAYERS[iSceneNum];

	m_iSceneNum = iSceneNum;

	return S_OK;
}

// 오브젝트 원형 등록
HRESULT CObjectManager::Add_Prototype(const _tchar* pPrototypeTag, CGameObject* pPrototype) // 원본 생성
{
	if (nullptr != Find_Prototype(pPrototypeTag))
		return E_FAIL;

	m_objMap.emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

// 원본 복사하여 실제 게임오브젝트 추가
HRESULT CObjectManager::Add_GameObject(const _tchar* pPrototypeTag, _uint iSceneIdx, const _tchar* pLayerTag, void* pArg)
{
	CGameObject* pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iSceneIdx, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pGameObject);

		m_pLayers[iSceneIdx].emplace(pLayerTag, pLayer);
	}
	else
	{
		pLayer->Add_GameObject(pGameObject);
	}
	return S_OK;
}

CGameObject* CObjectManager::Clone_GameObject(const _tchar* pPrototypeTag, _uint iSceneIdx, const _tchar* pLayerTag, void* pArg)
{
	CGameObject* pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return nullptr;

	CLayer* pLayer = Find_Layer(iSceneIdx, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pGameObject);
		m_pLayers[iSceneIdx].emplace(pLayerTag, pLayer);
	}
	else
	{
		pLayer->Add_GameObject(pGameObject);
	}

	return pGameObject;
}

void CObjectManager::Update(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iSceneNum; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Update_Layer(fTimeDelta);
		}
	}
}

void CObjectManager::Late_Update(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iSceneNum; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->LateUpdate_Layer(fTimeDelta);
		}
	}
}

// 특정 씬 제거
void CObjectManager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iSceneNum ||
		nullptr == m_pLayers)
		return;

	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();
}

// 특정 레이어 제거
void CObjectManager::Clear_Layer(_uint iLevelIndex, const _tchar* pLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);
	if (pLayer == nullptr)
		return;

	pLayer->Free();
}

// 오브젝트 반환
CGameObject* CObjectManager::Find_Object(_uint iSceneIdx, const _tchar* pLayerTag, _uint iIdx)
{
	CLayer* pLayer = Find_Layer(iSceneIdx, pLayerTag);
	if (pLayer == nullptr)
		return nullptr;

	return pLayer->Get_Object(iIdx);
}

// 오브젝트 리스트 반환
list<CGameObject*>* CObjectManager::Get_ObjectList(_uint iSceneID, const _tchar* pLayerTag)
{
	CLayer* pLayer = Find_Layer(iSceneID, pLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_ObjectList();
}

// 컴포넌트 가져오기
CComponent* CObjectManager::Get_Component(_uint iSceneIdx, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIdx)
{
	CLayer* pLayer = Find_Layer(iSceneIdx, pLayerTag);
	if (nullptr == pLayer)
		return nullptr;


	return pLayer->Get_Component(pComponentTag, iIdx);
}

std::vector<MAPOBJECTDATA> CObjectManager::ExportObjectData(_uint iSceneID, const _tchar *pLayerTag)
{
	list<CGameObject *> *pList = Get_ObjectList(iSceneID, pLayerTag);
	if (!pList || (*pList).size() <= 0)
		return {};

	std::vector<MAPOBJECTDATA> returnData;
	returnData.reserve((*pList).size());

	for (CGameObject *element : (*pList))
	{
		MAPOBJECTDATA pSrc;
		element->ExportData(&pSrc);
		returnData.push_back(pSrc);
	}

	return returnData;
}

std::vector<PREFABDATA> CObjectManager::ExportPrefabData()
{
	list<CGameObject *> *pList = Get_ObjectList(SCENE_PREFAB, L"Prefab_Layer");
	if (!pList || (*pList).size() <= 0)
		return {};

	std::vector<PREFABDATA> returnData;
	returnData.reserve((*pList).size());

	for (CGameObject *element : (*pList))
	{
		PREFABDATA pSrc;
		element->ExportData(&pSrc);
		returnData.push_back(pSrc);
	}

	return returnData;
}

// 원본 검색
CGameObject* CObjectManager::Find_Prototype(const _tchar* pProtoTypeTag)
{
	auto	iter = find_if(m_objMap.begin(), m_objMap.end(), CTag_Finder(pProtoTypeTag));

	if (iter == m_objMap.end())
		return nullptr;

	return iter->second;
}

// 레이어 검색
CLayer* CObjectManager::Find_Layer(_uint iSceneIdx, const _tchar* pLayerTag)
{
	if (iSceneIdx >= m_iSceneNum)
		return nullptr;

	auto	iter = find_if(m_pLayers[iSceneIdx].begin(), m_pLayers[iSceneIdx].end(), CTag_Finder(pLayerTag));
	if (iter == m_pLayers[iSceneIdx].end())
		return nullptr;

	return iter->second;
}

void CObjectManager::Free()
{
	for (_uint i = 0; i < m_iSceneNum; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();
	}

	for (auto& Pair : m_objMap)
		Safe_Release(Pair.second);

	m_objMap.clear();


	Safe_Delete_Array(m_pLayers);

}
