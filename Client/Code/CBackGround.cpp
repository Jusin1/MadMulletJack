#include "pch.h"
#include "CBackGround.h"
#include "CRenderer.h"

CBackGround::CBackGround(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CBackGround::CBackGround(const CGameObject& rhs)
    : CGameObject(rhs)
{
}

CBackGround::~CBackGround()
{

}

HRESULT CBackGround::Ready_GameObject()
{
    if (FAILED(__super::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBackGround::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_Scale(2.f, 2.f, 1.f);

    if (FAILED(Set_Component()))
        return E_FAIL;

    return S_OK;
}

_int CBackGround::Update_GameObject(const _float& fTimeDelta)
{
    CGameObject::Update_GameObject(fTimeDelta);
    if (m_pRendererCom)
        m_pRendererCom->Add_RenderGroup(RENDER_PRIORITY, this);

    return 0;
}

void CBackGround::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    SetWindowText(g_hWnd, TEXT("로고레벨입니다."));

}

void CBackGround::Render_GameObject()
{
    __super::Render_GameObject();
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTransformCom->Apply_WorldMatrix();

    m_pTextureCom->Set_Texture();

    // 알파 테스트 설정 추가
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    m_pBufferCom->Render_Buffer();

    // 원상복귀
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CBackGround::Set_Component()
{
    // VIBuffer
    if (FAILED(Add_Components(L"Com_Buffer", SCENE_STATIC, L"Proto_Rect_Buffer", (CComponent**)&m_pBufferCom)))
        return E_FAIL;

    // Texture
    if (FAILED(Add_Components(L"Com_Texture", SCENE_LOGO, L"Prototype_Component_Texture_BackGround", (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}


CBackGround* CBackGround::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CBackGround* pBackGround = new CBackGround(pGraphicDev);

    if (FAILED(pBackGround->Ready_GameObject()))
    {
        Safe_Release(pBackGround);
        MSG_BOX("BackGround Create Failed");
        return nullptr;
    }

    return pBackGround;
}

CGameObject* CBackGround::Clone(void* pArg)
{
    CBackGround* pInstance = new CBackGround(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("pBackGround Clone Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CBackGround::Free()
{
    Engine::CGameObject::Free();

}
