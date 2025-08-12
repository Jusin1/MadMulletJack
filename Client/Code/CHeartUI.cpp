#include "pch.h"
#include "CHeartUI.h"



CHeartUI::CHeartUI(LPDIRECT3DDEVICE9 pGraphicDev)
    : CUI(pGraphicDev)
{
}

CHeartUI::CHeartUI(const CHeartUI& rhs)
    : CUI(rhs)
{
}

CHeartUI::~CHeartUI()
{
}

HRESULT CHeartUI::Ready_GameObject()
{
    if (FAILED(__super::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

HRESULT CHeartUI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    
    //m_fSizeX = 50.f;
    //m_fSizeY = 50.f;

    //m_fX = WINCX * 0.5f - 300.f;
    //m_fY = WINCY * 0.5f + 250.f;

    if (FAILED(Texture_Clone()))
        return E_FAIL;

    Change_Texture(TEXT("Com_Texture_Heart"));
    return S_OK;
}

_int CHeartUI::Update_GameObject(const _float& fTimeDelta)
{
    m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_Info(INFO_POS, _vec3(m_fX - WINCX * 0.5f, -m_fY + WINCY * 0.5f, 0.f));

    __super::Update_GameObject(fTimeDelta);
    return NO_EVENT;
}

void CHeartUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
    __super::LateUpdate_GameObject(fTimeDelta);

}

void CHeartUI::Render_GameObject()
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

HRESULT CHeartUI::Texture_Clone()
{
    CTexture::TEXINFO texInfo = {};
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 5;
    texInfo.m_fSpeed = 3.f;
    texInfo.m_bLoop = true;

    if (FAILED(Add_Components(L"Com_Texture_Heart", SCENE_STAGE, L"Prototype_Component_Texture_HeartUI", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_Heart"), m_pTextureCom });
    return S_OK;
}

HRESULT CHeartUI::Change_Texture(const _tchar* pTextureTag)
{
    if (FAILED(__super::Change_Component(pTextureTag, (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    m_pTextureCom->Set_Zero_Frame();
    m_CurrentAnimTag = pTextureTag; // 현재 상태 저장
    return S_OK;
}

CHeartUI* CHeartUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CHeartUI* pInstance = new CHeartUI(pGraphicDev);
    if (FAILED(pInstance->Ready_GameObject()))
    {
        MSG_BOX("CHeartUI Create Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CHeartUI::Clone(void* pArg)
{
    CHeartUI* pInstance = new CHeartUI(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CHeartUI Clone Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CHeartUI::Free()
{
    __super::Free();
}