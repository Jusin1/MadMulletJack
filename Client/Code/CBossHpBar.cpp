#include "pch.h"
#include "CBossHpBar.h"
#include "CBoss.h"
#include "CObjectManager.h"

CBossHpBar::CBossHpBar(LPDIRECT3DDEVICE9 dev) : CImageUI(dev) 
{
}
CBossHpBar::CBossHpBar(const CBossHpBar& rhs) : CImageUI(rhs), m_fMaxHp(rhs.m_fMaxHp) {}
CBossHpBar::~CBossHpBar()
{
}

HRESULT CBossHpBar::Ready_GameObject()
{
    if (FAILED(__super::Ready_GameObject()))
        return E_FAIL;

    // 기본 프레임 텍스처 (보스 HP 바 배경)
    ChangeTexture(L"Com_Texture_BossHpFrame");

    return S_OK;
}

HRESULT CBossHpBar::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
  
    //// ? 프레임 텍스처 적용
    RegisterTexture(L"Com_Tex_BossHpBar", L"Prototype_Component_Texture_BossHpBar");
    ChangeTexture(L"Com_Tex_BossHpBar");

    // === 아이콘 생성 ===
    auto* icon = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", SCENE_STATIC, L"UI_Layer"));
    if (icon)
    {
        // HpBar 기준 왼쪽 끝 (아이콘 크기 40x40)
        icon->Set_UIPosition(WINCX * 0.5f - 785.f, -200.f, 55.f, 55.f);
        icon->RegisterTexture(L"Com_Tex_BossIcon", L"Prototype_Component_Texture_BossIcon");
        icon->ChangeTexture(L"Com_Tex_BossIcon");
        icon->SetTint(D3DCOLOR_ARGB(255, 0, 0, 0));
        Add_Child(icon);
        m_pIcon = icon;
    }

    // === 텍스트 생성 ===
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
        m_fMaxHp = max(m_fMaxHp, m_pBoss->Get_Helath()); // 초기 HP 기록
    }



    return __super::Update_GameObject(dt);
}

void CBossHpBar::LateUpdate_GameObject(const _float& dt)
{
    __super::LateUpdate_GameObject(dt);
}

void CBossHpBar::Render_GameObject()
{
    // 프레임 (BOSS LIFE BAR.png) 출력
    __super::Render_GameObject();

    // 체력 채움 부분
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

    // === 상태 저장 ===
    LPDIRECT3DSTATEBLOCK9 pStateBlock = nullptr;
    if (SUCCEEDED(m_pGraphicDev->CreateStateBlock(D3DSBT_ALL, &pStateBlock)))
        pStateBlock->Capture();

    // === 우리가 쓸 상태 세팅 ===
    m_pGraphicDev->SetTexture(0, nullptr);
    m_pGraphicDev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE); // 알파 허용
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

    // === 체력 칸 그리기 (하얀색만) ===
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

    // === 상태 복구 ===
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