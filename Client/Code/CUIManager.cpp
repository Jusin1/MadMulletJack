#include "pch.h"
#include "CUIManager.h"
#include "CObjectManager.h"
#include "CHeartUI.h"
#include "CLisaUI.h"
#include "CPanelUI.h"
#include "CChatUI.h"
#include "CBlackGackGround.h"
#include "CBannerUI.h"
#include "CImageUI.h"
#include "CTextUI.h"
#include "CTalkUI.h"

IMPLEMENT_SINGLETON(CUIManager)

CUIManager::CUIManager() {}
CUIManager::~CUIManager() {}

static inline float EaseOutCubic(float t) {
    float u = 1.f - t;
    return 1.f - u * u * u;
}

void CUIManager::AddSlideIn(
    CUI* ui, float x, float yTarget, float w, float h,
    float offsetX, float offsetY, float delay, float dur)
{
    if (!ui) return;

    const float xStart = x + offsetX;
    const float yStart = yTarget + offsetY;

    ui->Set_Active(true);                
    ui->Set_RenderOn(true);
    ui->Set_UIPosition(xStart, yStart, w, h);

    m_slideTasks.push_back({
        ui, w, h,
        xStart, x,
        yStart, yTarget,
        0.f, delay, dur, false
        });
}

void CUIManager::AddSlideTo(CUI* ui, float xEnd, float yEnd, float delay, float dur)
{
    if (!ui) return;

    float x, y, w, h;
    ui->Get_UIPosition(x, y);
    ui->Get_UISize(w, h);

    m_slideTasks.push_back({
        ui, w, h,
        x,  xEnd,
        y,  yEnd,
        0.f, delay, dur, false
        });
}

void CUIManager::Update(const _float& dt)
{
    // 슬라이드 진행
    for (auto& t : m_slideTasks) {
        if (t.done || !t.ui) continue;

        t.elapsed += dt;
        if (t.elapsed < t.delay) continue;

        float u = (t.elapsed - t.delay) / max(0.0001f, t.dur);
        if (u >= 1.f) { u = 1.f; t.done = true; }

        const float k = EaseOutCubic(u);
        const float x = t.xStart + (t.xEnd - t.xStart) * k;
        const float y = t.yStart + (t.yEnd - t.yStart) * k;

        t.ui->Set_UIPosition(x, y, t.w, t.h);
    }

    // 끝난 작업 정리
    m_slideTasks.erase(
        std::remove_if(m_slideTasks.begin(), m_slideTasks.end(),
            [](const SlideTask& t) { return t.done || t.ui == nullptr; }),
        m_slideTasks.end());

    if (m_exitingEnter && m_slideTasks.empty()) {
        if (m_pEnterUI) {
            CancelSlidesForSubtree(m_pEnterUI); 

            std::vector<CUIBase*> stk{ m_pEnterUI };
            while (!stk.empty()) {
                CUIBase* n = stk.back(); stk.pop_back();
                if (!n) continue;

                n->Set_Active(false);
                n->Set_Dead(true);       

                for (auto* ch : n->GetChildren())
                    if (ch) stk.push_back(ch);
            }

            m_pEnterUI = nullptr;
        }
        m_exitingEnter = false;
    }
}

void CUIManager::CreateClearUI()
{
    if (m_pEnterUI || m_exitingEnter) return;

    constexpr float SLIDE_OFFSET_X = +220.f;
    constexpr float SLIDE_OFFSET_Y = +200.f;
    constexpr float SLIDE_DUR = 0.12f;
    constexpr float STAGGER = 0.02f;

    float baseDelay = 0.f;

    m_pEnterUI = dynamic_cast<CUIBase*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIRoot", SCENE_STAGE_1, L"UI_Layer"));
    if (!m_pEnterUI) return;

    m_pEnterUI->Set_Active(true);
    auto attachAndSlide = [&](CUI* ui, float x, float y, float w, float h, float extraDelay = 0.f)
        {
            if (!ui) return;
            ui->Set_Active(true);
            ui->Set_RenderOn(true);
            m_pEnterUI->Add_Child(ui);

            AddSlideIn(ui, x, y, w, h,
                SLIDE_OFFSET_X, SLIDE_OFFSET_Y,
                baseDelay + extraDelay, SLIDE_DUR);
            baseDelay += STAGGER;
        };

    if (auto* pBlack = dynamic_cast<CBlackGackGround*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_BlackBackground", SCENE_STAGE_1, L"UI_Layer"))) {
        pBlack->Set_UIPosition(0.f, 0.f, (float)WINCX, (float)WINCY);
        pBlack->SetAlpha(0);
        pBlack->FadeTo(190, 0.0f, 0.25f);
        attachAndSlide(pBlack, 0.f, 0.f, (float)WINCX, (float)WINCY);
        pBlack->SetHoleRect(-130.f, -70.f, 1080.f, 600.f); // 프레임 구멍
    }

    if (auto* pFrame = dynamic_cast<CPanelUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_PanelUI", SCENE_STAGE_1, L"UI_Layer"))) {
        pFrame->UseGreenFramePreset(-130.f, -70.f, 1080.f, 600.f, 3.f, true);
        attachAndSlide(pFrame, -130.f, -70.f, 1080.f, 600.f);
    }

    const float faceX = 550.f, faceY = -210.f, faceW = 220.f, faceH = 320.f;

    if (auto* pLisa = dynamic_cast<CLisaUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_LisaUI", SCENE_STAGE_1, L"UI_Layer")))
        attachAndSlide(pLisa, faceX, faceY, faceW, faceH);

    if (auto* pFaceFrame = dynamic_cast<CPanelUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_PanelUI", SCENE_STAGE_1, L"UI_Layer"))) {
        pFaceFrame->UseGreenFramePreset(faceX, faceY, faceW, faceH, 3.f, true);
        attachAndSlide(pFaceFrame, faceX, faceY, faceW, faceH);
    }

    if (auto* pChat = dynamic_cast<CChatUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_ChatUI", SCENE_STAGE_1, L"UI_Layer")))
        attachAndSlide(pChat, 550.f, 200.f, 220.f, 320.f);

    const float lisaBottom = faceY + faceH * 0.5f;
    const float chatTop = 200.f - 320.f * 0.5f;
    const float bx = faceX, by = (lisaBottom + chatTop) * 0.5f, bw = faceW, bh = 46.f;

    if (auto* pBanner = dynamic_cast<CBannerUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_BannerUI", SCENE_STAGE_1, L"UI_Layer"))) {
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
        attachAndSlide(pBanner, bx, by, bw, bh);
    }

    const float X = -100.f, Y = 300.f, H2 = 96.f, W2 = 600.f;
    const float CAP_L = 260.f, CAP_R = 260.f;

    auto mkPanel = [&](float cx, float cy, float pw, float ph, D3DCOLOR tint) -> CPanelUI* {
        auto* p = dynamic_cast<CPanelUI*>(
            CObjectManager::GetInstance()->Clone_GameObject(
                L"Prototype_GameObject_PanelUI", SCENE_STAGE_1, L"UI_Layer"));
        if (!p) return nullptr;
        p->SetPanelPos(cx, cy);
        p->SetPanelSize(pw, ph);
        p->SetStyle(L"Com_Texture_Panel_FileGrid", tint, false);
        p->SetUVRepeat(1.f, 1.f);
        attachAndSlide(p, cx, cy, pw, ph);
        return p;
        };

    mkPanel(X, Y, W2, H2, D3DCOLOR_ARGB(190, 255, 120, 180));
    mkPanel(X - (W2 * 0.5f - CAP_L * 0.5f), Y, CAP_L, H2, D3DCOLOR_ARGB(220, 255, 60, 60));
    mkPanel(X + (W2 * 0.5f - CAP_R * 0.5f), Y, CAP_R, H2, D3DCOLOR_ARGB(220, 255, 60, 60));

    if (auto* heart = dynamic_cast<CHeartUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_HeartUI", SCENE_STAGE_1, L"UI_Layer"))) {
        heart->Set_UIPosition(X, Y, W2, H2);
        heart->SetHeartSizePx(80.f);
        heart->SetHeartManual(-W2 * 0.5f + 60.f, +W2 * 0.5f - 60.f, 0.f);
        heart->SetLineHeightPx(15.f);
        heart->SetLineYOffset(-2.f);
        heart->SetLineRangePx(-W2 * 0.5f + 70.f, +W2 * 0.5f - 70.f);
        heart->SetLineTint(D3DCOLOR_ARGB(255, 255, 230, 240));
        heart->SetPulseStyle(110.f, 60.f, 0.f, D3DCOLOR_ARGB(255, 255, 230, 240));
        heart->SetPulseSpeed(400.f);
        float trackL = -W2 * 0.5f + 120.f + 110.f * 0.5f;
        float trackR = +W2 * 0.5f - 120.f - 110.f * 0.5f;
        heart->SetBeatTrackPx(trackL, trackR);
        heart->SetPulseStartOffsetPx(110.f + 40.f);
        attachAndSlide(heart, X, Y, W2, H2, 0.03f);
    }

    if (auto* img = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", SCENE_STAGE_1, L"UI_Layer"))) {
        img->RegisterTexture(L"Com_Texture_Logo", L"Prototype_Component_Texture_LogoUI", 0, 0, 0.f, false);
        img->ChangeTexture(L"Com_Texture_Logo");
        img->SetTintRGBA(100, 255, 120, 255);
        img->SetColorMode(CImageUI::ColorMode::TintMultiply);
        img->SetAdditive(false);
        attachAndSlide(img, 320.f, 300.f, 100.f, 100.f);
    }

    if (auto* titleImage = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", SCENE_STAGE_1, L"UI_Layer"))) {
        titleImage->RegisterTexture(L"Com_Texture_Title", L"Prototype_Component_Texture_SmallTitleUI", 0, 0, 0.f, false);
        titleImage->ChangeTexture(L"Com_Texture_Title");
        attachAndSlide(titleImage, -550.f, 300.f, 200.f, 100.f);
    }

    if (auto* textImage = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", SCENE_STAGE_1, L"UI_Layer"))) {
        textImage->RegisterTexture(L"Com_Texture_Text", L"Prototype_Component_Texture_SmallTextUI", 0, 0, 0.f, false);
        textImage->ChangeTexture(L"Com_Texture_Text");
        attachAndSlide(textImage, -530.f, 360.f, 250.f, 30.f);
    }

    if (auto* liveIcon1 = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", SCENE_STAGE_1, L"UI_Layer"))) {
        liveIcon1->RegisterTexture(L"Com_Texture_Text", L"Prototype_Component_Texture_LiveIconUI", 0, 0, 0.f, false);
        liveIcon1->ChangeTexture(L"Com_Texture_Text");
        liveIcon1->SetTintRGBA(57, 255, 20, 255);
        liveIcon1->SetColorMode(CImageUI::ColorMode::TintMultiply);
        liveIcon1->SetAdditive(false);
        attachAndSlide(liveIcon1, -630.f, -330.f, 25.f, 25.f);
    }

    if (auto* txt = dynamic_cast<CTextUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_TextUI", SCENE_STAGE_1, L"UI_Layer"))) {
        txt->SetFontTag(L"UIFont");
        txt->SetText(L"LIVESTREAM");
        txt->SetColor(D3DXCOLOR(0.22f, 1.f, 0.08f, 1.f));
        txt->SetScale(1.f);
        txt->SetCentered(false);
        txt->SetLetterSpacing(1.f);
        attachAndSlide(txt, -590.f, 350.f, 70.f, 40.f);
    }

    if (auto* liveIcon2 = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", SCENE_STAGE_1, L"UI_Layer"))) {
        liveIcon2->RegisterTexture(L"Com_Texture_Text", L"Prototype_Component_Texture_LiveIconUI", 0, 0, 0.f, false);
        liveIcon2->ChangeTexture(L"Com_Texture_Text");
        liveIcon2->SetTintRGBA(255, 0, 0, 255);
        liveIcon2->SetColorMode(CImageUI::ColorMode::TintMultiply);
        liveIcon2->SetAdditive(false);
        attachAndSlide(liveIcon2, 470.f, -340.f, 20.f, 20.f);
    }

    if (auto* txt1 = dynamic_cast<CTextUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_TextUI", SCENE_STAGE_1, L"UI_Layer"))) {
        txt1->SetFontTag(L"UIFont");
        txt1->SetText(L"PEACE CROP CODEC");
        txt1->SetColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
        txt1->SetScale(0.4f);
        txt1->SetCentered(false);
        txt1->SetLetterSpacing(1.f);
        attachAndSlide(txt1, 485.f, 350.f, 17.f, 17.f);
    }

    if (auto* talk = dynamic_cast<CTalkUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_TalkUI", SCENE_STAGE_1, L"UI_Layer"))) {
        std::vector<std::wstring> dialogues = {
            L"대화테스트 1.", L"대화테스트 2", L"대화테스트 3", L"종료"
        };
        talk->LoadDialogues(dialogues);
        talk->Set_TextPos(400.f, -500.f);
        talk->Set_TextScale(0.5f);
        talk->Set_Active(true);
        attachAndSlide(talk, 430.f, -250.f, 600.f, 100.f);
    }
}  // 게임 클리어 UI 생성

void CUIManager::DestroyEnterUI()
{
    if (m_exitingEnter || !m_pEnterUI) return;

    CancelSlidesForSubtree(m_pEnterUI);

    std::vector<CUI*> uis;
    std::function<void(CUIBase*)> dfs = [&](CUIBase* n) {
        if (!n) return;
        if (auto asUI = dynamic_cast<CUI*>(n)) uis.push_back(asUI);
        for (auto* ch : n->GetChildren()) dfs(ch);
        };
    dfs(m_pEnterUI);

    const float OUT_X = WINCX * 1.3f;
    const float OUT_Y = WINCY * 1.3f;
    const float DUR = 0.18f;
    const float STAG = 0.015f;

    float delay = 0.f;
    for (auto* ui : uis) {
        float x, y; ui->Get_UIPosition(x, y);
        AddSlideTo(ui, x + OUT_X, y + OUT_Y, delay, DUR);
        delay += STAG;

        if (auto* bg = dynamic_cast<CBlackGackGround*>(ui)) {
            bg->FadeTo(0, 0.0f, DUR * 0.9f);
        }
    }

    m_exitingEnter = true;
}

void CUIManager::CancelSlidesForSubtree(CUIBase* root)
{
    if (!root) return;

    std::unordered_set<CUIBase*> nodes;
    std::function<void(CUIBase*)> dfs = [&](CUIBase* n) {
        if (!n) return;
        nodes.insert(n);
        for (auto* ch : n->GetChildren()) dfs(ch);
        };
    dfs(root);

    m_slideTasks.erase(
        std::remove_if(m_slideTasks.begin(), m_slideTasks.end(),
            [&](const SlideTask& t) { return t.ui && nodes.count(t.ui) > 0; }),
        m_slideTasks.end());
}

void CUIManager::Free()
{
    m_slideTasks.clear();
    Safe_Release(m_pEnterUI);
}
