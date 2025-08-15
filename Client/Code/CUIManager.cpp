#include "pch.h"
#include "CUIManager.h"
#include "CObjectManager.h"
#include "CHeartUI.h"
#include "CLisaUI.h"
#include "CPanelUI.h"
#include "CChatUI.h"
#include "CBlackGackGround.h"
#include "CBannerUI.h"

IMPLEMENT_SINGLETON(CUIManager)

CUIManager::CUIManager() {}
CUIManager::~CUIManager() {}

static inline float EaseOutCubic(float t) {
    float u = 1.f - t;
    return 1.f - u * u * u;
}

void CUIManager::AddSlideIn(CUI* ui,
    float x, float yTarget, float w, float h,
    float offsetX, float offsetY, float delay, float dur)
{
    if (!ui) return;

    const float xStart = x + offsetX;
    const float yStart = yTarget + offsetY;

    ui->Set_UIPosition(xStart, yStart, w, h);

    m_slideTasks.push_back({
        ui,
        w, h,
        xStart, x,
        yStart, yTarget,        
        0.f, delay, dur,
        false
        });
}

void CUIManager::Update(const _float& dt)
{
    for (auto& t : m_slideTasks) {
        if (t.done || !t.ui) continue;

        t.elapsed += dt;
        if (t.elapsed < t.delay) continue;

        float u = (t.elapsed - t.delay) / max(0.0001f, t.dur);
        if (u >= 1.f) { u = 1.f; t.done = true; }

        float k = EaseOutCubic(u);

        const float x = t.xStart + (t.xEnd - t.xStart) * k;
        const float y = t.yStart + (t.yEnd - t.yStart) * k;

        t.ui->Set_UIPosition(x, y, t.w, t.h);
    }
    m_slideTasks.erase(
        std::remove_if(m_slideTasks.begin(), m_slideTasks.end(),
            [](const SlideTask& t) { return t.done || t.ui == nullptr; }),
        m_slideTasks.end());
}

void CUIManager::CreateEnterUI()
{
    constexpr float SLIDE_OFFSET_X = +220.f;
    constexpr float SLIDE_OFFSET_Y = +200.f;
    constexpr float SLIDE_DUR = 0.22f; 
    constexpr float STAGGER = 0.03f;
    float delay = 0.0f;

    m_pEnterUI = dynamic_cast<CUIBase*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIRoot", SCENE_STAGE, L"UI_Layer"));
    if (!m_pEnterUI) return;

    auto slide = [&](CUI* ui, float x, float y, float w, float h, float extraDelay = 0.f)
        {
            if (!ui) return;
            AddSlideIn(ui, x, y, w, h, SLIDE_OFFSET_X, SLIDE_OFFSET_Y, delay + extraDelay, SLIDE_DUR);
            delay += STAGGER;
        };

    auto* pBlack = dynamic_cast<CBlackGackGround*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_BlackBackground", SCENE_STAGE, L"UI_Layer"));
    pBlack->Set_UIPosition(0.f, 0.f, (float)WINCX, (float)WINCY);
    pBlack->SetAlpha(0);
    pBlack->FadeTo(190, 0.0f, 0.25f);
    m_pEnterUI->Add_Child(pBlack);
    slide(pBlack, 0.f, 0.f, (float)WINCX, (float)WINCY);

    auto* pFrame = dynamic_cast<CPanelUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_PanelUI", SCENE_STAGE, L"UI_Layer"));
    pFrame->UseGreenFramePreset(-130.f, -70.f, 1080.f, 600.f, 3.f, true);
    m_pEnterUI->Add_Child(pFrame);
    slide(pFrame, -130.f, -70.f, 1080.f, 600.f);

    pBlack->SetHoleRect(-130.f, -70.f, 1080.f, 600.f);

    const float x = 550.f, y = -210.f, w = 220.f, h = 320.f;

    auto* pLisa = dynamic_cast<CLisaUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_LisaUI", SCENE_STAGE, L"UI_Layer"));
    pLisa->Set_UIPosition(x, y, w, h);
    m_pEnterUI->Add_Child(pLisa);
    slide(pLisa, x, y, w, h);

    auto* pFaceFrame = dynamic_cast<CPanelUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_PanelUI", SCENE_STAGE, L"UI_Layer"));
    pFaceFrame->UseGreenFramePreset(x, y, w, h, 3.f, true);
    m_pEnterUI->Add_Child(pFaceFrame);
    slide(pFaceFrame, x, y, w, h);

    auto* pChat = dynamic_cast<CChatUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_ChatUI", SCENE_STAGE, L"UI_Layer"));
    pChat->Set_UIPosition(550.f, 200.f, 220.f, 320.f);
    m_pEnterUI->Add_Child(pChat);
    slide(pChat, 550.f, 200.f, 220.f, 320.f);
    const float lisaBottom = y + h * 0.5f;            
    const float chatTop = 200.f - 320.f * 0.5f;    
    const float bx = x, by = (lisaBottom + chatTop) * 0.5f, bw = w, bh = 46.f;

    auto* pBanner = dynamic_cast<CBannerUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_BannerUI", SCENE_STAGE, L"UI_Layer"));
    pBanner->SetBannerRect(bx, by, bw, bh);
    pBanner->SetText(L"FLOOR 01");
    pBanner->SetAccentColor(D3DCOLOR_ARGB(255, 60, 255, 60));
    pBanner->SetTextColorCycle(true, 120.f);
    pBanner->SetPadding(10, 12, 6, 6);
    pBanner->SetFontHeight(40);
    pBanner->SetTextOffset(13.f, -20.f);
    pBanner->SetArrowSizePx(26.f);
    pBanner->SetArrowOffset(-2.f, 0.f);
    pBanner->SetStripeBarAnchor(CBannerUI::StripeAnchor::Bottom);
    pBanner->SetStripeYOffsetPx(3.f);
    pBanner->SetStripeBarHeightPx(18.f);
    pBanner->SetStripeCount(7);
    pBanner->SetStripeTileWidthPx(20.f);
    pBanner->SetStripeAngleDeg(26.f);
    pBanner->SetStripeSpeed(-120.f);
    m_pEnterUI->Add_Child(pBanner);
    slide(pBanner, bx, by, bw, bh);

    const float X = -100.f, Y = 300.f, H2 = 96.f, W2 = 600.f;
    const float CAP_L = 260.f, CAP_R = 260.f;

    auto addPanel = [&](float cx, float cy, float pw, float ph, D3DCOLOR tint) -> CPanelUI*
        {
            auto* p = dynamic_cast<CPanelUI*>(
                CObjectManager::GetInstance()->Clone_GameObject(
                    L"Prototype_GameObject_PanelUI", SCENE_STAGE, L"UI_Layer"));
            p->SetPanelPos(cx, cy);
            p->SetPanelSize(pw, ph);
            p->SetStyle(L"Com_Texture_Panel_FileGrid", tint, false);
            p->SetUVRepeat(1.f, 1.f);
            m_pEnterUI->Add_Child(p);
            return p;
        };

    auto* pPink = addPanel(X, Y, W2, H2, D3DCOLOR_ARGB(190, 255, 120, 180));
    auto* pCapL = addPanel(X - (W2 * 0.5f - CAP_L * 0.5f), Y, CAP_L, H2, D3DCOLOR_ARGB(220, 255, 60, 60));
    auto* pCapR = addPanel(X + (W2 * 0.5f - CAP_R * 0.5f), Y, CAP_R, H2, D3DCOLOR_ARGB(220, 255, 60, 60));

    slide(pPink, X, Y, W2, H2);
    slide(pCapL, X - (W2 * 0.5f - CAP_L * 0.5f), Y, CAP_L, H2, 0.01f);
    slide(pCapR, X + (W2 * 0.5f - CAP_R * 0.5f), Y, CAP_R, H2, 0.02f);

    auto* heart = dynamic_cast<CHeartUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_HeartUI", SCENE_STAGE, L"UI_Layer"));
    heart->Set_UIPosition(X, Y, W2, H2);

    heart->SetHeartSizePx(80.f);
    heart->SetHeartManual(-W2 * 0.5f + 60.f, +W2 * 0.5f - 60.f, 0.f);

    heart->SetLineHeightPx(15.f);
    heart->SetLineYOffset(-2.f);
    heart->SetLineRangePx(-W2 * 0.5f + 70.f, +W2 * 0.5f - 70.f);
    heart->SetLineTint(D3DCOLOR_ARGB(255, 255, 230, 240));

    heart->SetPulseStyle(110.f, 60.f, 0.f, D3DCOLOR_ARGB(255, 255, 230, 240));
    heart->SetPulseSpeed(400.f);
    {
        float trackL = -W2 * 0.5f + 120.f + 110.f * 0.5f;
        float trackR = +W2 * 0.5f - 120.f - 110.f * 0.5f;
        heart->SetBeatTrackPx(trackL, trackR);
        heart->SetPulseStartOffsetPx(110.f + 40.f);
    }
    m_pEnterUI->Add_Child(heart);
    slide(heart, X, Y, W2, H2, 0.03f);
}

void CUIManager::Free()
{
    m_slideTasks.clear();
    Safe_Release(m_pEnterUI);
}