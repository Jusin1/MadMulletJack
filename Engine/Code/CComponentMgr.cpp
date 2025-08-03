#include "CComponentMgr.h"

IMPLEMENT_SINGLETON(CComponentMgr)

CComponentMgr::CComponentMgr()
{
}

CComponentMgr::~CComponentMgr()
{
    Free();
}

HRESULT CComponentMgr::Ready_Prototype(_uint iSceneNum)
{
    if (nullptr != m_mapPrototype)
        return E_FAIL;

    m_mapPrototype = new PROTOTYPES[iSceneNum];

    m_iSceneNum = iSceneNum;

    return S_OK;
}

HRESULT CComponentMgr::Add_Prototype(_uint iSceneIdx, const _tchar* pPrototypeTag, CComponent* pPrototype)
{
    if (nullptr == m_mapPrototype ||
        iSceneIdx >= m_iSceneNum)
        return E_FAIL;

    if (nullptr != Find_Component(iSceneIdx, pPrototypeTag))
        return E_FAIL;

    m_mapPrototype[iSceneIdx].emplace(pPrototypeTag, pPrototype);

    return S_OK;
}

CComponent* CComponentMgr::Clone_Component(_uint iSceneIdx, const _tchar* pPrototypeTag, void* pArg)
{
    if (nullptr == m_mapPrototype ||
        iSceneIdx >= m_iSceneNum)
        return nullptr;

    CComponent* pPrototype = Find_Component(iSceneIdx, pPrototypeTag);
    if (nullptr == pPrototype)
        return nullptr;

    CComponent* pComponent = pPrototype->Clone(pArg);
    if (nullptr == pComponent)
        return nullptr;

    return pComponent;
}

void CComponentMgr::Clear(_uint iSceneIdx)
{
    if (iSceneIdx >= m_iSceneNum)
        return;

    for (auto& Pair : m_mapPrototype[iSceneIdx])
        Safe_Release(Pair.second);

    m_mapPrototype[iSceneIdx].clear();
}

CComponent* CComponentMgr::Find_Component(_uint iSceneIdx, const _tchar* pPrototypeTag)
{
    auto	iter = find_if(m_mapPrototype[iSceneIdx].begin(), m_mapPrototype[iSceneIdx].end(), CTag_Finder(pPrototypeTag));

    if (iter == m_mapPrototype[iSceneIdx].end())
        return nullptr;

    return iter->second;
}



void CComponentMgr::Free()
{
    for (_uint i = 0; i < m_iSceneNum; ++i)
    {
        for (auto& Pair : m_mapPrototype[i])
        {
            Safe_Release(Pair.second);
        }
        m_mapPrototype[i].clear();
    }
    Safe_Delete_Array(m_mapPrototype);
}
