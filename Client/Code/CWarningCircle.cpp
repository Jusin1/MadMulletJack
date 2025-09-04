#include "pch.h"
#include "Engine_Define.h"
#include "CCullingManager.h"
#include "CRenderer.h"
#include "CVIBuffer_Circle.h"
#include "CWarningCircle.h"

CWarningCircle::CWarningCircle(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CWarningCircle::CWarningCircle(const CWarningCircle &rhs)
    : CGameObject(rhs)
{
}

CWarningCircle::~CWarningCircle()
{
}

void CWarningCircle::Free()
{
    CGameObject::Free();
}

CGameObject *CWarningCircle::Clone(void *pArg)
{
    CWarningCircle *pInstance = new CWarningCircle(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CWarningCircle Create Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CWarningCircle *CWarningCircle::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CWarningCircle *pInstance = new CWarningCircle(pGraphicDev);
    if (FAILED(pInstance->Ready_GameObject()))
    {
        MSG_BOX("CWarningCircle Create Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

HRESULT CWarningCircle::Ready_GameObject()
{
    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

HRESULT CWarningCircle::Initialize(void *pArg)
{
    if (FAILED(CGameObject::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Set_Component()))
        return E_FAIL;

    Setup_OnGround();

    return S_OK;
}

_int CWarningCircle::Update_GameObject(const _float &fTimeDelta)
{
    if (m_bDead)
        return DEAD;
    CGameObject::Update_GameObject(fTimeDelta);

    Update_Position(m_pTransformCom->Get_Info(INFO::INFO_POS));

    Compute_CamDistance(Get_Position());
    // 카메라 컬링 등
    if (CCullingManager::GetInstance()->Is_In_Frustum(Get_Position(), m_fEnd_Radius * 1.2f))
        if (m_pRendererCom) m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this);

    m_fElapsed += fTimeDelta;
    _float fT = (std::min)(1.f, m_fElapsed / m_fGoalTime);
    _float fRadius = m_fStart_Radius + (m_fEnd_Radius - m_fStart_Radius) * fT;

    
    m_pBufferCom->Set_Radius(fRadius);
    return NO_EVENT;
}

void CWarningCircle::LateUpdate_GameObject(const _float &fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CWarningCircle::Render_GameObject()
{
    CGameObject::Render_GameObject();

    RenderStates_Begin();

    m_pTransformCom->Apply_WorldMatrix();

    m_pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &m_prevTFactor);

    m_pBufferCom->Render_Buffer();

    RenderStages_End();
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, m_prevTFactor);
}

HRESULT CWarningCircle::Spawn_Pooling(void *pArg)
{
    if (FAILED(CGameObject::Spawn_Pooling(pArg)))
        return E_FAIL;

    m_pBufferCom->Set_Radius(m_fStart_Radius);
    Setup_OnGround();

    return S_OK;
}

HRESULT CWarningCircle::Despawn_Pooling()
{
    if (FAILED(CGameObject::Despawn_Pooling()))
        return E_FAIL;

    m_fElapsed = 0.f;

    return S_OK;
}


HRESULT CWarningCircle::Set_Component()
{
    CircleBufferData tData;
    tData.fRadius = m_fStart_Radius;
    tData.iSegments = 16;
    if (FAILED(Add_Components(L"Com_Buffer", SCENE_STATIC, L"Proto_Buffer_Circle", (CComponent **)&m_pBufferCom, &tData)))
        return E_FAIL;

    return S_OK;
}

void CWarningCircle::Setup_OnGround()
{
    m_pTransformCom->Set_Info(INFO::INFO_RIGHT, _vec3{ 1.f, 0.f, 0.f });
    m_pTransformCom->Set_Info(INFO::INFO_UP, _vec3{ 0.f, 0.f, 1.f });
    m_pTransformCom->Set_Info(INFO::INFO_LOOK, _vec3{ 0.f, 1.f, 0.f });
}

void CWarningCircle::RenderStates_Begin()
{
    m_pGraphicDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &m_prevAlphaBlend);
    m_pGraphicDev->GetRenderState(D3DRS_ZWRITEENABLE, &m_prevZWrite);
    m_pGraphicDev->GetRenderState(D3DRS_SRCBLEND, &m_prevSrcBlend);
    m_pGraphicDev->GetRenderState(D3DRS_DESTBLEND, &m_prevDstBlend);
    m_pGraphicDev->GetRenderState(D3DRS_CULLMODE, &m_prevCullMode);

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}

void CWarningCircle::RenderStages_End()
{
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, m_prevAlphaBlend);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, m_prevZWrite);
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, m_prevSrcBlend);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, m_prevDstBlend);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, m_prevCullMode);
}