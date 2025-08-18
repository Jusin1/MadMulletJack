#include "pch.h"
#include "CPhoneUI.h"
#include "CImageUI.h"
#include "CObjectManager.h"


CPhoneUI::CPhoneUI(LPDIRECT3DDEVICE9 dev) : CUI(dev) {}
CPhoneUI::CPhoneUI(const CPhoneUI& rhs)
    : CUI(rhs), m_state(rhs.m_state)
{

}
CPhoneUI::~CPhoneUI() {}

HRESULT CPhoneUI::Ready_GameObject()
{
    if (FAILED(__super::Ready_GameObject()))
        return E_FAIL;
    return S_OK;
}

HRESULT CPhoneUI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    // ----- 본체 텍스처 준비 -----
    if (FAILED(Texture_Clone()))
        return E_FAIL;



    // 초기 상태는 무조건 Default
    Change_Texture(L"Com_Texture_Phone_Default");
    return S_OK;
}

HRESULT CPhoneUI::Texture_Clone()
{
    // Default
    {
        CTexture::TEXINFO texInfo{};
        texInfo.m_iStart = 0;
        texInfo.m_iEndTex = 3;
        texInfo.m_fSpeed = 6.f;
        texInfo.m_bLoop = true;

        CTexture* pTex = nullptr;
        if (FAILED(Add_Components(L"Com_Texture_Phone_Default", SCENE_STAGE_1,
            L"Prototype_Component_Texture_PhoneUI",
            (CComponent**)&pTex, &texInfo)))
            return E_FAIL;

        m_mapTextures.insert({ L"Com_Texture_Phone_Default", pTex });
    }


    return S_OK;
}

HRESULT CPhoneUI::Change_Texture(const _tchar* pTextureTag)
{
    auto it = m_mapTextures.find(pTextureTag);
    if (it == m_mapTextures.end())
        return E_FAIL;

    m_pTextureCom = it->second;      // 해당 텍스처로 교체
    m_pTextureCom->Set_Zero_Frame(); // 애니메이션 처음부터
    m_CurrentAnimTag = pTextureTag;
    return S_OK;
}


_int CPhoneUI::Update_GameObject(const _float& dt)
{
    m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_Info(INFO_POS, _vec3(m_fX, -m_fY, 0.f));


    if (m_pTextureCom) m_pTextureCom->MoveFrame();

    __super::Update_GameObject(dt);
    return NO_EVENT;
}

void CPhoneUI::LateUpdate_GameObject(const _float& dt)
{
    __super::LateUpdate_GameObject(dt);
}

void CPhoneUI::Render_GameObject()
{
    // --- 본체 ---
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    if (m_pTextureCom)
        m_pTextureCom->Set_Texture(m_pTextureCom->Get_Frame().m_iCurrentTex);

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

void CPhoneUI::SetState(AnimState st)
{
    if (m_state == st) return;
    m_state = st;
}



CPhoneUI* CPhoneUI::Create(LPDIRECT3DDEVICE9 dev)
{
    CPhoneUI* p = new CPhoneUI(dev);
    if (FAILED(p->Ready_GameObject()))
    {
        MSG_BOX("CPhoneUI Create Failed");
        Safe_Release(p);
    }
    return p;
}

CGameObject* CPhoneUI::Clone(void* pArg)
{
    CPhoneUI* p = new CPhoneUI(*this);
    if (FAILED(p->Initialize(pArg)))
    {
        MSG_BOX("CPhoneUI Clone Failed");
        Safe_Release(p);
    }
    return p;
}

void CPhoneUI::Free()
{
    CUIBase::Free();
}