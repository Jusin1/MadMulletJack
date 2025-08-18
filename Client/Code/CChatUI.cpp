#include "pch.h"
#include "CChatUI.h"

CChatUI::CChatUI(LPDIRECT3DDEVICE9 pGraphicDev)
    : CUI(pGraphicDev)
{
}

CChatUI::CChatUI(const CChatUI& rhs)
    : CUI(rhs)
{
}

CChatUI::~CChatUI()
{
}

HRESULT CChatUI::Ready_GameObject()
{
    if (FAILED(__super::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

HRESULT CChatUI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Texture_Clone()))
        return E_FAIL;

    Change_Texture(TEXT("Com_Texture_Lisa"));
    return S_OK;
}

_int CChatUI::Update_GameObject(const _float& fTimeDelta)
{
    m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_Info(INFO_POS, _vec3(m_fX, -m_fY, 0.f));

    __super::Update_GameObject(fTimeDelta);
    return NO_EVENT;
}

void CChatUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
    __super::LateUpdate_GameObject(fTimeDelta);

}

void CChatUI::Render_GameObject()
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


HRESULT CChatUI::Texture_Clone()
{
    CTexture::TEXINFO texInfo = {};
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 7;
    texInfo.m_fSpeed = 7.f;
    texInfo.m_bLoop = true;

    if (FAILED(Add_Components(L"Com_Texture_Lisa", SCENE_STATIC, L"Prototype_Component_Texture_ChatUI", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_Lisa"), m_pTextureCom });
    return S_OK;
}

HRESULT CChatUI::Change_Texture(const _tchar* pTextureTag)
{
    if (FAILED(__super::Change_Component(pTextureTag, (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    m_pTextureCom->Set_Zero_Frame();
    m_CurrentAnimTag = pTextureTag;
    return S_OK;
}

CChatUI* CChatUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CChatUI* pInstance = new CChatUI(pGraphicDev);
    if (FAILED(pInstance->Ready_GameObject()))
    {
        MSG_BOX("CHATUI Create Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CChatUI::Clone(void* pArg)
{
    CChatUI* pInstance = new CChatUI(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CHATUI Clone Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CChatUI::Free()
{
    __super::Free();
}