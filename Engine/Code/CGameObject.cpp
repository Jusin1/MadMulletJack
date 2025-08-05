#include "CGameObject.h"
#include "CComponentMgr.h"

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphicDev)
    : m_pGraphicDev(pGraphicDev), m_pTransformCom(nullptr)
{
    m_pGraphicDev->AddRef();
}

CGameObject::CGameObject(const CGameObject& rhs)
    : m_pGraphicDev(rhs.m_pGraphicDev), m_pTransformCom(rhs.m_pTransformCom)
{
    m_pGraphicDev->AddRef();
}


CGameObject::~CGameObject()
{
}


HRESULT CGameObject::Ready_GameObject()
{
    return S_OK;
}

_int CGameObject::Update_GameObject(const _float& fTimeDelta)
{
    return NO_EVENT;
}

void CGameObject::LateUpdate_GameObject(const _float& fTimeDelta)
{

}

// 컴포넌츠 추가 - 복제 기반
HRESULT CGameObject::Add_Components(const _tchar* pComponentTag, _uint iSceneIdx, const _tchar* pPrototypeTag, CComponent** ppOut, void* pArg)
{
    if (nullptr != Find_Component(pComponentTag)) // 이미 존재하면 실패
        return E_FAIL;


    CComponent* pComponent = CComponentMgr::GetInstance()->Clone_Component(iSceneIdx, pPrototypeTag, pArg);
    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent.emplace(pComponentTag, pComponent);

    *ppOut = pComponent;

    return S_OK;
}

// 컴포넌트 참조만 교체
HRESULT CGameObject::Change_Component(const _tchar* pComponentTag, CComponent** ppOut)
{
    CComponent* pComponent = Find_Component(pComponentTag);
    if (nullptr == pComponent)
        return E_FAIL;

    *ppOut = pComponent;

    return S_OK;
}

// 컴포넌트 검색
CComponent* CGameObject::Find_Component(const _tchar* pComponentTag)
{
    auto	iter = find_if(m_mapComponent.begin(), m_mapComponent.end(), CTag_Finder(pComponentTag));
    if (iter == m_mapComponent.end())
        return nullptr;

    return iter->second;
}

void CGameObject::Free()
{
    for (auto& Pair : m_mapComponent)
        Safe_Release(Pair.second);
    m_mapComponent.clear();

    Safe_Release(m_pGraphicDev);
}
