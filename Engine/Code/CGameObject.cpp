#include "CGameObject.h"
#include "CComponentMgr.h"

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphicDev)
    : m_pGraphicDev(pGraphicDev)
    , m_pTransformCom(nullptr)
    , m_pRendererCom(nullptr)
    , m_pParent(nullptr)
    , m_vPosition(0.f, 0.f, 0.f)
    , m_fRadius(0.5f)
    , m_bDead(false)
    , m_CollisionMatrix{}
    , m_bActive(false)
    , m_bRenderOn(true)
    , m_fCamDistance(0.f)
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
    , m_fCamDistance(rhs.m_fCamDistance)
    , m_pParent(nullptr)
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
    if (FAILED(Add_Components(L"Com_Transform", SCENE_STATIC, L"Proto_Transform", (CComponent**)&m_pTransformCom)))
        return E_FAIL;

    if (FAILED(Add_Components(L"Com_Renderer", SCENE_STATIC, L"Proto_Renderer", (CComponent**)&m_pRendererCom)))
        return E_FAIL;
    return S_OK;
}

void CGameObject::Compute_CamDistance(_vec3 WorldPos)
{
    _matrix view, invView;
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &view);

    // 역행렬은 별도 변수에 받는 게 안전
    if (D3DXMatrixInverse(&invView, nullptr, &view) == nullptr) {
        m_fCamDistance = 0.f;
        return;
    }

    _vec3 camPos(invView._41, invView._42, invView._43);

    _vec3 diff = camPos - WorldPos;
    m_fCamDistance = D3DXVec3Length(&diff);

}

HRESULT CGameObject::Despawn_Pooling()
{
    if (m_pTransformCom)
    {
        m_pTransformCom->Set_Info(INFO::INFO_POS, _vec3{ -999.f, 999.f, -999.f });
    }
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
