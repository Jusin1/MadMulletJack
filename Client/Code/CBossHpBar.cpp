#include "pch.h"
#include "CBossHpBar.h"
#include "CBoss.h"
#include "CObjectManager.h"

CBossHpBar::CBossHpBar(LPDIRECT3DDEVICE9 dev) : CImageUI(dev) 
{
}
CBossHpBar::CBossHpBar(const CBossHpBar& rhs) : CImageUI(rhs), m_fMaxHp(rhs.m_fMaxHp)
{

}
CBossHpBar::~CBossHpBar()
{

}

HRESULT CBossHpBar::Ready_GameObject()
{
    if (FAILED(__super::Ready_GameObject()))
        return E_FAIL;

    ChangeTexture(L"Com_Texture_BossHpFrame");

    return S_OK;
}

HRESULT CBossHpBar::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
  
    RegisterTexture(L"Com_Tex_BossHpBar", L"Prototype_Component_Texture_BossHpBar");
    ChangeTexture(L"Com_Tex_BossHpBar");

    auto* icon = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", SCENE_STATIC, L"UI_Layer"));
    if (icon)
    {
        icon->Set_UIPosition(WINCX * 0.5f - 785.f, -200.f, 55.f, 55.f);
        icon->RegisterTexture(L"Com_Tex_BossIcon", L"Prototype_Component_Texture_BossIcon");
        icon->ChangeTexture(L"Com_Tex_BossIcon");
        icon->SetTint(D3DCOLOR_ARGB(255, 0, 0, 0));
        Add_Child(icon);
        m_pIcon = icon;
    }
    auto* text = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", SCENE_STATIC, L"UI_Layer"));
    if (text)
    {
        text->Set_UIPosition(WINCX * 0.5f - 640.f, -250.f, 180.f, 40.f);
        text->RegisterTexture(L"Com_Tex_BossText", L"Prototype_Component_Texture_BossText");
        text->ChangeTexture(L"Com_Tex_BossText");
        Add_Child(text);
        m_pText = text;
    }

    return S_OK;
}

_int CBossHpBar::Update_GameObject(const _float& dt)
{
    if (m_pBoss)
    {
        m_fMaxHp = max(m_fMaxHp, m_pBoss->Get_Helath()); 
    }



    return __super::Update_GameObject(dt);
}

void CBossHpBar::LateUpdate_GameObject(const _float& dt)
{
    __super::LateUpdate_GameObject(dt);
}

void CBossHpBar::Render_GameObject()
{
    __super::Render_GameObject();

    RenderHpBar();
    
}

void CBossHpBar::RenderHpBar()
{
    if (!m_pBoss) return;

    float fCurHp = m_pBoss->Get_Helath();
    float ratio = fCurHp / m_fMaxHp;
    ratio = max(0.f, min(1.f, ratio));

    const int CELL_COUNT = 15;
    int filledCells = (int)(CELL_COUNT * ratio);

    float screenX = WINCX * 0.5f + m_fX + 100.f;
    float screenY = WINCY * 0.5f + m_fY;

    float startX = screenX - (m_fBarW * 0.5f);
    float startY = screenY - (m_fBarH * 0.5f);

    float cellW = 14.f;
    float cellH = m_fBarH;
    float gap = 3.f;

    struct Vertex { float x, y, z, rhw; D3DCOLOR color; };
    Vertex rect[4];

    LPDIRECT3DSTATEBLOCK9 pStateBlock = nullptr;
    if (SUCCEEDED(m_pGraphicDev->CreateStateBlock(D3DSBT_ALL, &pStateBlock)))
        pStateBlock->Capture();

    m_pGraphicDev->SetTexture(0, nullptr);
    m_pGraphicDev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE); 
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

    for (int i = 0; i < filledCells; ++i)
    {
        float x1 = startX + i * (cellW + gap);
        float y1 = startY;
        float x2 = x1 + cellW;
        float y2 = y1 + cellH;

        D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255);

        rect[0] = { x1, y1, 0.f, 1.f, color };
        rect[1] = { x2, y1, 0.f, 1.f, color };
        rect[2] = { x1, y2, 0.f, 1.f, color };
        rect[3] = { x2, y2, 0.f, 1.f, color };

        m_pGraphicDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, rect, sizeof(Vertex));
    }

    if (pStateBlock) {
        pStateBlock->Apply();
        pStateBlock->Release();
    }
}

void CBossHpBar::BindBoss(CBoss* pBoss)
{
    m_pBoss = pBoss;
    m_fMaxHp = pBoss->Get_Helath();
}

void CBossHpBar::SetIconTexture(const _tchar* tag)
{
    m_pIcon = dynamic_cast<CImageUI*>(CImageUI::Create(m_pGraphicDev));
    if (m_pIcon) m_pIcon->ChangeTexture(tag);
}

void CBossHpBar::SetTextTexture(const _tchar* tag)
{
    m_pText = dynamic_cast<CImageUI*>(CImageUI::Create(m_pGraphicDev));
    if (m_pText) m_pText->ChangeTexture(tag);
}

CBossHpBar* CBossHpBar::Create(LPDIRECT3DDEVICE9 dev)
{
    CBossHpBar* p = new CBossHpBar(dev);
    if (FAILED(p->Ready_GameObject())) { Safe_Release(p); return nullptr; }
    return p;
}

CGameObject* CBossHpBar::Clone(void* pArg)
{
    CBossHpBar* p = new CBossHpBar(*this);
    if (FAILED(p->Initialize(pArg))) { Safe_Release(p); return nullptr; }
    return p;
}

void CBossHpBar::Free()
{
    __super::Free();
}