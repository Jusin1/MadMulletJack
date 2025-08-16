#include "pch.h"
#include "CLisaUI.h"

CLisaUI::CLisaUI(LPDIRECT3DDEVICE9 pGraphicDev)
    : CUI(pGraphicDev)
{
}

CLisaUI::CLisaUI(const CLisaUI& rhs)
    : CUI(rhs)
{
}

CLisaUI::~CLisaUI()
{
}

HRESULT CLisaUI::Ready_GameObject()
{
    if (FAILED(__super::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLisaUI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Texture_Clone()))
        return E_FAIL;

    Change_Texture(TEXT("Com_Texture_Lisa"));
    return S_OK;
}

_int CLisaUI::Update_GameObject(const _float& fTimeDelta)
{
    m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_Info(INFO_POS, _vec3(m_fX, -m_fY, 0.f));

    __super::Update_GameObject(fTimeDelta);
    return NO_EVENT;
}

void CLisaUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
    __super::LateUpdate_GameObject(fTimeDelta);

}

void CLisaUI::Render_GameObject()
{
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


    m_pTextureCom->Set_Texture(m_pTextureCom->Get_Frame().m_iCurrentTex);
    m_pTextureCom->MoveFrame();

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x01);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    __super::Render_GameObject();

    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}


HRESULT CLisaUI::Texture_Clone()
{
    CTexture::TEXINFO texInfo = {};
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 12;
    texInfo.m_fSpeed = 3.f;
    texInfo.m_bLoop = true;

    if (FAILED(Add_Components(L"Com_Texture_Lisa", SCENE_STAGE_1, L"Prototype_Component_Texture_LisaUI", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_Lisa"), m_pTextureCom });
    return S_OK;
}

HRESULT CLisaUI::Change_Texture(const _tchar* pTextureTag)
{
    if (FAILED(__super::Change_Component(pTextureTag, (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    m_pTextureCom->Set_Zero_Frame();
    m_CurrentAnimTag = pTextureTag; // 현재 상태 저장
    return S_OK;
}

CLisaUI* CLisaUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CLisaUI* pInstance = new CLisaUI(pGraphicDev);
    if (FAILED(pInstance->Ready_GameObject()))
    {
        MSG_BOX("CLisaUI Create Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CLisaUI::Clone(void* pArg)
{
    CLisaUI* pInstance = new CLisaUI(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CLisaUI Clone Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CLisaUI::Free()
{
    __super::Free();
}