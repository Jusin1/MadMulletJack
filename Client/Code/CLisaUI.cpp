#include "pch.h"
#include "CLisaUI.h"
#include "CImageUI.h"
#include "CObjectManager.h"


CLisaUI::CLisaUI(LPDIRECT3DDEVICE9 dev) : CUI(dev) {}
CLisaUI::CLisaUI(const CLisaUI& rhs)
    : CUI(rhs), m_state(rhs.m_state), m_pHair(nullptr) {
}
CLisaUI::~CLisaUI() {}

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

    // ----- 본체 텍스처 준비 -----
    if (FAILED(Texture_Clone()))
        return E_FAIL;

    if (FAILED(Create_HairPart()))
        return E_FAIL;


    // 초기 상태는 무조건 Default
    Change_Texture(L"Com_Texture_Lisa_Default");
    SetState(AnimState::Default);
    Add_Child(m_pHair);
    return S_OK;
}

HRESULT CLisaUI::Texture_Clone()
{
    // Default
    {
        CTexture::TEXINFO texInfo{};
        texInfo.m_iStart = 0;
        texInfo.m_iEndTex = 12;
        texInfo.m_fSpeed = 6.f;
        texInfo.m_bLoop = true;

        CTexture* pTex = nullptr;
        if (FAILED(Add_Components(L"Com_Texture_Lisa_Default", SCENE_STATIC,
            L"Prototype_Component_Texture_LisaUI",
            (CComponent**)&pTex, &texInfo)))
            return E_FAIL;

        m_mapTextures.insert({ L"Com_Texture_Lisa_Default", pTex });
    }

    // Bye
    {
        CTexture::TEXINFO texInfo{};
        texInfo.m_iStart = 0;
        texInfo.m_iEndTex = 10;
        texInfo.m_fSpeed = 8.f;
        texInfo.m_bLoop = true;

        CTexture* pTex = nullptr;
        if (FAILED(Add_Components(L"Com_Texture_Lisa_Bye", SCENE_STATIC,
            L"Prototype_Component_Texture_LisaByeUI",
            (CComponent**)&pTex, &texInfo)))
            return E_FAIL;

        m_mapTextures.insert({ L"Com_Texture_Lisa_Bye", pTex });
    }

    return S_OK;
}

HRESULT CLisaUI::Change_Texture(const _tchar* pTextureTag)
{
    auto it = m_mapTextures.find(pTextureTag);
    if (it == m_mapTextures.end())
        return E_FAIL;

    m_pTextureCom = it->second;      // 해당 텍스처로 교체
    m_pTextureCom->Set_Zero_Frame(); // 애니메이션 처음부터
    m_CurrentAnimTag = pTextureTag;
    return S_OK;
}

HRESULT CLisaUI::Create_HairPart()
{
    m_pHair = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", SCENE_STATIC, L"UI_Layer"));

    if (!m_pHair) return E_FAIL;

    m_pHair->Set_UIPosition(550.f, -320.f, 230.f, 150.f);

    m_pHair->RegisterTexture(
        L"Lisa_Hair_Default",
        L"Prototype_Component_Texture_LisaHair_Default",
        0, 3, 6.f, true);

    m_pHair->RegisterTexture(
        L"Lisa_Hair_Bye",
        L"Prototype_Component_Texture_LisaHair_Bye",
        0, 3, 8.f, true);

    m_pHair->ChangeTexture(L"Lisa_Hair_Default");
    return S_OK;
}

_int CLisaUI::Update_GameObject(const _float& dt)
{
    m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_Info(INFO_POS, _vec3(m_fX, -m_fY, 0.f));

    if (GetAsyncKeyState('T'))
        SetState(AnimState::Bye);

    if (m_pTextureCom) m_pTextureCom->MoveFrame();

    __super::Update_GameObject(dt);
    return NO_EVENT;
}

void CLisaUI::LateUpdate_GameObject(const _float& dt)
{
    __super::LateUpdate_GameObject(dt);
}

void CLisaUI::Render_GameObject()
{
    // 머리를 먼저 그린다
    if (m_pHair)
        m_pHair->Render_GameObject();

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

void CLisaUI::SetState(AnimState st)
{
    if (m_state == st) return;
    m_state = st;
    ApplyStateToParts(st);
}

void CLisaUI::ApplyStateToParts(AnimState st)
{
    // --- 본체 ---
    if (st == AnimState::Default)
        Change_Texture(L"Com_Texture_Lisa_Default");
    else if (st == AnimState::Bye)
        Change_Texture(L"Com_Texture_Lisa_Bye");

    // --- 머리카락 ---
    if (m_pHair)
    {
        if (st == AnimState::Default)
            m_pHair->ChangeTexture(L"Lisa_Hair_Default");
        else if (st == AnimState::Bye)
            m_pHair->ChangeTexture(L"Lisa_Hair_Bye");

        m_pHair->Play(true);
    }
}

CLisaUI* CLisaUI::Create(LPDIRECT3DDEVICE9 dev)
{
    CLisaUI* p = new CLisaUI(dev);
    if (FAILED(p->Ready_GameObject()))
    {
        MSG_BOX("CLisaUI Create Failed");
        Safe_Release(p);
    }
    return p;
}

CGameObject* CLisaUI::Clone(void* pArg)
{
    CLisaUI* p = new CLisaUI(*this);
    if (FAILED(p->Initialize(pArg)))
    {
        MSG_BOX("CLisaUI Clone Failed");
        Safe_Release(p);
    }
    return p;
}

void CLisaUI::Free()
{
    CUIBase::Free();
}