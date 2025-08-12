#include "CGameObject.h"
#include "CComponentMgr.h"

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphicDev)
    : m_pGraphicDev(pGraphicDev)
    , m_pTransformCom(nullptr)
    , m_pRendererCom(nullptr)
    , m_vPosition(0.f, 0.f, 0.f)
    , m_fRadius(0.5f)
    , m_bDead(false)
    , m_CollisionMatrix{}
    , m_bActive(false)
    , m_bRenderOn(true)
{
    if (m_pGraphicDev)
        m_pGraphicDev->AddRef();
}

CGameObject::CGameObject(const CGameObject& rhs)
    : m_pGraphicDev(rhs.m_pGraphicDev)
    , m_pTransformCom(rhs.m_pTransformCom)
    , m_vPosition(rhs.m_vPosition)
    , m_fRadius(rhs.m_fRadius)
    , m_bDead(rhs.m_bDead)
    , m_CollisionMatrix(rhs.m_CollisionMatrix)
    , m_mapComponent(rhs.m_mapComponent) // 얕복임 (주의: 컴포넌트 깊복 필요시 따로 처리해야 함)
    , m_bRenderOn(rhs.m_bRenderOn)
{
    if (m_pGraphicDev)
        m_pGraphicDev->AddRef();
}


CGameObject::~CGameObject()
{
}


HRESULT CGameObject::Ready_GameObject()
{
    return S_OK;
}
HRESULT CGameObject::Initialize(void* pArg)
{
    Set_Component();
    return S_OK;
}


_int CGameObject::Update_GameObject(const _float& fTimeDelta)
{
    return NO_EVENT;
}

void CGameObject::LateUpdate_GameObject(const _float& fTimeDelta)
{

}

HRESULT CGameObject::Set_Component()
{
    if (FAILED(Add_Components(L"Com_Transform", 0, L"Proto_Transform", (CComponent**)&m_pTransformCom)))
        return E_FAIL;

    if (FAILED(Add_Components(L"Com_Renderer", 0, L"Proto_Renderer", (CComponent**)&m_pRendererCom)))
        return E_FAIL;
    return S_OK;
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
