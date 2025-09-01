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
#include "CPhoneUI.h"
#include "CButtonUI.h"
#include "CManagement.h"
#include "CLoading_Scene.h"
#include "CItemUI.h"
#include "CTextEffectUI.h"

// À¯Æ¿ - UI Á×ÀÌ±â
static void DetachAndKill(CUIBase* parent, CUIBase*& node)
{
    if (!node) return;
    if (parent) parent->Remove_Child(node); 
    node->Set_Active(false);
    node->Set_Dead(true);
    node = nullptr;
}

IMPLEMENT_SINGLETON(CUIManager)


CUIManager::CUIManager()
{
}
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

void CUIManager::AddScaleIn(CUI* ui,
    float xStart, float yStart, float wStart, float hStart,
    float xEnd, float yEnd, float wEnd, float hEnd,
    float delay, float dur)
{
    if (!ui) return;
    ui->Set_UIPosition(xStart, yStart, wStart, hStart);

    m_scaleTasks.push_back({
        ui,
        0.f, delay, dur, false,
        xStart, yStart, wStart, hStart,
        xEnd,   yEnd,   wEnd,   hEnd
        });
}

void CUIManager::Update(const _float& dt)
{
    // === slide ===
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
    // ¾Ö´Ï¸ÞÀÌ¼Ç ½½¶óÀÌµå (UI)
    m_slideTasks.erase(
        std::remove_if(m_slideTasks.begin(), m_slideTasks.end(),
            [](const SlideTask& t) { return t.done || t.ui == nullptr; }),
        m_slideTasks.end());

    // ¾Ö´Ï¸ÞÀÌ¼Ç Å©±â Å°¿ì±â ½½¶óÀÌµå
    for (auto& t : m_scaleTasks) {
        if (t.done || !t.ui) continue;
        t.elapsed += dt;
        if (t.elapsed < t.delay) continue;

        float u = (t.elapsed - t.delay) / max(0.0001f, t.dur);
        if (u >= 1.f) { u = 1.f; t.done = true; }

        const float k = EaseOutCubic(u);
        const float x = t.xStart + (t.xEnd - t.xStart) * k;
        const float y = t.yStart + (t.yEnd - t.yStart) * k;
        const float w = t.wStart + (t.wEnd - t.wStart) * k;
        const float h = t.hStart + (t.hEnd - t.hStart) * k;
        t.ui->Set_UIPosition(x, y, w, h);
    }

    m_scaleTasks.erase(
        std::remove_if(m_scaleTasks.begin(), m_scaleTasks.end(),
            [](const ScaleTask& t) { return t.done || t.ui == nullptr; }),
        m_scaleTasks.end());

    // Å¸ÀÓ ÅØ½ºÆ® ½Ã°£ ½ºÆù
    if (!m_spawnedTimeUI && m_pVictoryText && m_pFloorTimeText) {
        if (m_pVictoryText->IsAppearFinished() && m_pFloorTimeText->IsAppearFinished()) {
            CreateTimeTextUI(L"01:12:45");
            m_spawnedTimeUI = true;
        }
    }

    // ¦¡¦¡ ÇÚµåÆù ´ç±â±â
    if (m_phonePullStarted && !m_phonePullFinished && PhoneScaleDone()) {
        m_phonePullFinished = true;

        if (m_pPhoneScreen) {
            const wchar_t* tag = m_nextPhoneScreenTexTag.empty()
                ? L"Com_Texture_PhoneErrorUI"
                : m_nextPhoneScreenTexTag.c_str();

            if (m_changeScreenOnPullFinish) {
                m_pPhoneScreen->ChangeTexture(tag);
                m_pPhoneScreen->Play(true);

                // 1ÃÊ ÈÄ ÇÚµåÆù ´ó±â±â
                m_createPhoneScreenPending = true;
                m_createPhoneScreenTimer = 0.f;
                m_createPhoneScreenDelay = 1.0f;

                m_changeScreenOnPullFinish = false;
                m_nextPhoneScreenTexTag.clear();
            }
        }
    }

    // ÇÚµåÆù ´ó±â±â Å¸ÀÌ¸Ó
    if (m_createPhoneScreenPending) {
        m_createPhoneScreenTimer += dt;
        if (m_createPhoneScreenTimer >= m_createPhoneScreenDelay) {
            m_createPhoneScreenPending = false;
            m_createPhoneScreenTimer = 0.f;
            CreatePhoneScreen();
        }
    }

    // UI »èÁ¦ Å¸ÀÌ¸Ó
    if (m_bRemoveUI) {
        m_timeUIRemoveTimer += dt;
        if (m_timeUIRemoveTimer >= 1.F) {
            m_bRemoveUI = false;
            m_timeUIRemoveTimer = 0.f;
            DestroyEnterUI();
        }
    }

  
    if (m_timeAutoRemoveArmed) {
        m_timeAutoRemoveTimer += dt;
        if (m_timeAutoRemoveTimer >= 2.0f) {
            DetachAndKill(m_pEnterUI, reinterpret_cast<CUIBase*&>(m_pTimeText));
            DetachAndKill(m_pEnterUI, reinterpret_cast<CUIBase*&>(m_pTimeFrame));
            DetachAndKill(m_pEnterUI, reinterpret_cast<CUIBase*&>(m_pTimeBlack));
            DetachAndKill(m_pEnterUI, reinterpret_cast<CUIBase*&>(m_pVictoryText));
            DetachAndKill(m_pEnterUI, reinterpret_cast<CUIBase*&>(m_pFloorTimeText));
            auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
            if (auto* talk = dynamic_cast<CTalkUI*>(CObjectManager::GetInstance()->Clone_GameObject(
                L"Prototype_GameObject_TalkUI", sceneIdx, L"UI_Layer"))) {
                std::vector<std::wstring> dialogues = { L"Å¬¸®¾îÇÏ¼Ì±º¿ä...", L"ÁÁÀº °É º¸¿©µå¸±°Ô¿ä", L"ÁÁÀº ¼±ÅÃÀÌ±æ!!" };
                talk->LoadDialogues(dialogues);
                talk->Set_TextPos(420.f, -500.f);
                talk->Set_TextScale(0.5f);
                talk->Set_Active(true);
                talk->Set_OwnerLisa(m_pLisaUI);
                m_pTalkUI = talk;
                m_pEnterUI->Add_Child(m_pTalkUI);
            }
            m_timeAutoRemoveArmed = false;
            m_timeAutoRemoveTimer = 0.f;
        }
    }

    // ÇÚµåÆù ´ç±â±â Æ®¸®°Å
    if (m_phoneSlideActive && !m_phonePullArmed && PhoneSlidesDone()) {
        m_phonePullArmed = true;
        m_phonePullTimer = 0.f;
        m_phoneSlideActive = false;   // Áßº¹ ¹æÁö
    }

    if (m_phonePullArmed && !m_phonePullStarted) {
        m_phonePullTimer += dt;
        if (m_phonePullTimer >= m_phonePullDelay) {
            StartPhonePullAnim();                
            ChangePhoneScreenAfterPull(L"Com_Texture_PhoneErrorUI");
            m_phonePullStarted = true;
        }
    }


}

void CUIManager::CreateClearUI()
{
    DestroyItemUI();
    DestroyEffectUI();

    if (m_pEnterUI || m_exitingEnter) return;

    constexpr float SLIDE_OFFSET_X = +220.f;
    constexpr float SLIDE_OFFSET_Y = +200.f;
    constexpr float SLIDE_DUR = 0.12f;
    constexpr float STAGGER = 0.02f;

    float baseDelay = 0.f;
    auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
    m_pEnterUI = dynamic_cast<CUIBase*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIRoot", sceneIdx, L"UI_Layer"));
    if (!m_pEnterUI) return;

    CreatePhoneUI();
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
            L"Prototype_GameObject_BlackBackground", sceneIdx, L"UI_Layer"))) {
        pBlack->Set_UIPosition(0.f, 0.f, (float)WINCX, (float)WINCY);
        pBlack->SetAlpha(0);
        pBlack->FadeTo(190, 0.0f, 0.25f);
        attachAndSlide(pBlack, 0.f, 0.f, (float)WINCX, (float)WINCY);
        pBlack->SetHoleRect(-130.f, -70.f, 1080.f, 600.f); // ÇÁ·¹ÀÓ ±¸¸Û
    }

    if (auto* pFrame = dynamic_cast<CPanelUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_PanelUI", sceneIdx, L"UI_Layer"))) {
        pFrame->UseGreenFramePreset(-130.f, -70.f, 1080.f, 600.f, 3.f, true);
        attachAndSlide(pFrame, -130.f, -70.f, 1080.f, 600.f);
    }

    const float faceX = 550.f, faceY = -210.f, faceW = 220.f, faceH = 320.f;

    if (auto* pLisa = dynamic_cast<CLisaUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_LisaUI", sceneIdx, L"UI_Layer")))
    {
        m_pLisaUI = pLisa;
        attachAndSlide(pLisa, faceX, faceY, faceW, faceH);
    }

    if (auto* pFaceFrame = dynamic_cast<CPanelUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_PanelUI", sceneIdx, L"UI_Layer"))) {
        pFaceFrame->UseGreenFramePreset(faceX, faceY, faceW, faceH, 3.f, true);
        attachAndSlide(pFaceFrame, faceX, faceY, faceW, faceH);
    }

    if (auto* pChat = dynamic_cast<CChatUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_ChatUI", sceneIdx, L"UI_Layer")))
        attachAndSlide(pChat, 550.f, 200.f, 220.f, 320.f);

    const float lisaBottom = faceY + faceH * 0.5f;
    const float chatTop = 200.f - 320.f * 0.5f;
    const float bx = faceX, by = (lisaBottom + chatTop) * 0.5f, bw = faceW, bh = 46.f;

    if (auto* pBanner = dynamic_cast<CBannerUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_BannerUI", sceneIdx, L"UI_Layer"))) {
        pBanner->SetBannerRect(bx, by, bw, bh);
        pBanner->SetText(L"FLOOR 01");
        pBanner->SetAccentColor(D3DCOLOR_ARGB(255, 60, 255, 60));
        pBanner->SetTextColorCycle(true, 120.f);
        pBanner->SetPadding(10, 12, 6, 6);
        pBanner->SetFontHeight(32);
        pBanner->SetTextOffset(13.f, -20.f);
        pBanner->SetArrowSizePx(26.f);
        pBanner->SetArrowOffset(-2.f, 2.f);
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
                L"Prototype_GameObject_PanelUI", sceneIdx, L"UI_Layer"));
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
            L"Prototype_GameObject_HeartUI", sceneIdx, L"UI_Layer"))) {
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
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"))) {
        img->RegisterTexture(L"Com_Texture_Logo", L"Prototype_Component_Texture_LogoUI", 0, 0, 0.f, false);
        img->ChangeTexture(L"Com_Texture_Logo");
        img->SetTintRGBA(100, 255, 120, 255);
        img->SetColorMode(CImageUI::ColorMode::TintMultiply);
        img->SetAdditive(false);
        attachAndSlide(img, 320.f, 300.f, 100.f, 100.f);
    }

    if (auto* titleImage = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"))) {
        titleImage->RegisterTexture(L"Com_Texture_Title", L"Prototype_Component_Texture_SmallTitleUI", 0, 0, 0.f, false);
        titleImage->ChangeTexture(L"Com_Texture_Title");
        attachAndSlide(titleImage, -550.f, 300.f, 200.f, 100.f);
    }
    if (auto* textImage = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"))) {
        textImage->RegisterTexture(L"Com_Texture_Text", L"Prototype_Component_Texture_SmallTextUI", 0, 0, 0.f, false);
        textImage->ChangeTexture(L"Com_Texture_Text");
        attachAndSlide(textImage, -530.f, 360.f, 250.f, 30.f);
    }
    if (auto* liveIcon1 = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"))) {
        liveIcon1->RegisterTexture(L"Com_Texture_Text", L"Prototype_Component_Texture_LiveIconUI", 0, 0, 0.f, false);
        liveIcon1->ChangeTexture(L"Com_Texture_Text");
        liveIcon1->SetTintRGBA(57, 255, 20, 255);
        liveIcon1->SetColorMode(CImageUI::ColorMode::TintMultiply);
        liveIcon1->SetAdditive(false);
        attachAndSlide(liveIcon1, -630.f, -330.f, 25.f, 25.f);
    }
    if (auto* txt = dynamic_cast<CTextUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_TextUI", sceneIdx, L"UI_Layer"))) {
        txt->SetFontTag(L"Font_UI_Regular");
        txt->SetText(L"LIVESTREAM");
        txt->SetColor(D3DXCOLOR(0.22f, 1.f, 0.08f, 1.f));
        txt->SetScale(1.f);
        txt->SetCentered(false);
        txt->SetLetterSpacing(1.f);
        attachAndSlide(txt, -590.f, 350.f, 70.f, 40.f);
    }

    if (auto* liveIcon2 = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"))) {
        liveIcon2->RegisterTexture(L"Com_Texture_Text", L"Prototype_Component_Texture_LiveIconUI", 0, 0, 0.f, false);
        liveIcon2->ChangeTexture(L"Com_Texture_Text");
        liveIcon2->SetTintRGBA(255, 0, 0, 255);
        liveIcon2->SetColorMode(CImageUI::ColorMode::TintMultiply);
        liveIcon2->SetAdditive(false);
        attachAndSlide(liveIcon2, 470.f, -340.f, 20.f, 20.f);
    }

    if (auto* txt1 = dynamic_cast<CTextUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_TextUI", sceneIdx, L"UI_Layer"))) {
        txt1->SetFontTag(L"Font_UI_Regular");
        txt1->SetText(L"PEACE CROP CODEC");
        txt1->SetColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
        txt1->SetScale(0.35f);
        txt1->SetCentered(false);
        txt1->SetLetterSpacing(1.f);
        attachAndSlide(txt1, 485.f, 348.f, 17.f, 17.f);
    }

    CreateClearTextUI();

}  // °ÔÀÓ Å¬¸®¾î UI »ý¼º

void CUIManager::CreateEffectUI(const std::wstring& str)
{
    if (m_pEffectUI && m_pEffectUI->Get_Dead())
        m_pEffectUI = nullptr;

    if (m_pEffectUI) return; // Áßº¹ »ý¼º ¹æÁö

    auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
    m_pEffectUI = dynamic_cast<CUIBase*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIRoot", sceneIdx, L"UI_Layer"));
    if (!m_pEffectUI) return;

    if (auto* eff = dynamic_cast<CTextEffectUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_TextEffectUI", sceneIdx, L"UI_Layer")))
    {
        const float W = 80.f, H = 64.f, PAD_X = 12.f;
        const float SX = W + PAD_X, SY = H * 0.75f, cy = -260.f;
        auto row = [&](int n, float y) {
            float s = -((n - 1) * SX) * 0.5f;
            for (int i = 0; i < n; ++i)
                eff->AddImage(s + i * SX, y, D3DCOLOR_ARGB(255, 255, 255, 255), W, H);
            };
        row(4, cy - SY); row(5, cy); row(4, cy + SY);

        const float textY = 300.f;
        eff->SetTextBaseScale(2.f);
        eff->SetupText(str, 0.f, textY);
        eff->PlayTextOvershootMove(0.45f, 4.5f, 0.f, 50.f);
        eff->StartRainbow(0.2f);

        m_pEffectUI->Add_Child(eff);
    }
}

void CUIManager::CreateItemUI()
{

    if (m_pItemUI) return; // Áßº¹ ¹æÁö
    auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
    m_pItemUI = dynamic_cast<CUIBase*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIRoot", sceneIdx, L"UI_Layer"));
    if (!m_pItemUI) return;

    if (auto* effect = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"))) {
        effect->Set_UIPosition(0.f, 200.f, 130.f, 130.f);
        effect->RegisterTexture(L"Com_Texture_Text", L"Prototype_Component_Texture_WeaponUIBack", 0, 0, 0.f, false);
        effect->ChangeTexture(L"Com_Texture_Text");
        effect->SetAdditive(false);
        m_pItemUI->Add_Child(effect);
    }
    if (auto* weapon = dynamic_cast<CItemUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIItem", sceneIdx, L"UI_Layer")))
    {
        weapon->RegisterTexture(L"Com_Texture_Text", L"Prototype_Component_Texture_WeaponUI", 0, 0, 0.f, false);
        weapon->ChangeTexture(L"Com_Texture_Text");
        weapon->SetAdditive(false);

        weapon->PlayAppear(0.f, 200.f, 40.f, 70.f, 1.f);

        weapon->StartBlink(1.0f, 0.5f, true, 255, 0);
        m_pItemUI->Add_Child(weapon);
    }
    if (auto* pBlack = dynamic_cast<CBlackGackGround*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_BlackBackground", sceneIdx, L"UI_Layer"))) {
        pBlack->Set_UIPosition(0.f, 290.f, 170.f, 50.f);
        pBlack->SetAlpha(255);
        pBlack->SetColor(D3DCOLOR_ARGB(255, 255, 165, 0));
        m_pItemUI->Add_Child(pBlack);
    }

    if (auto* pBlack = dynamic_cast<CBlackGackGround*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_BlackBackground", sceneIdx, L"UI_Layer"))) {
        pBlack->Set_UIPosition(0.f, 280.f, 165.f, 25.f);
        pBlack->SetAlpha(255);
        m_pItemUI->Add_Child(pBlack);
    }


    if (auto* txt1 = dynamic_cast<CTextUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_TextUI", sceneIdx, L"UI_Layer"))) {
        txt1->SetFontTag(L"Font_UI_Bold");
        txt1->SetText(L"[MOUSE 2]");
        txt1->Set_UIPosition(0.f, -267.f, 165.f, 25.f);
        txt1->SetColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
        txt1->SetScale(0.5f);
        txt1->SetCentered(true);
        m_pItemUI->Add_Child(txt1);
    }

    if (auto* txt2 = dynamic_cast<CTextUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_TextUI", sceneIdx, L"UI_Layer"))) {
        txt2->SetFontTag(L"Font_UI_Bold");
        txt2->SetText(L"FINISH");
        txt2->Set_UIPosition(0.f, -290.F, 165.f, 25.f);
        txt2->SetColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
        txt2->SetScale(0.5f);
        txt2->SetCentered(true);
        m_pItemUI->Add_Child(txt2);
    }
}

void CUIManager::DestroyEnterUI()
{
    if (m_exitingEnter || !m_pEnterUI) return;


    vector<CUI*> uis;
    function<void(CUIBase*)> dfs = [&](CUIBase* n) {
        if (!n) return;
        if (auto asUI = dynamic_cast<CUI*>(n)) uis.push_back(asUI);
        for (auto* ch : n->GetChildren()) dfs(ch);
        };
    dfs(m_pEnterUI);

    const float OUT_X = WINCX * 1.3f;
    const float OUT_Y = WINCY * 1.3f;
    const float DUR = -0.4f;
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

    auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
    // ¾À ±³Ã¼ (·Îµù¾ÀÀ¸·Î)
    LPDIRECT3DDEVICE9 pDev = CManagement::GetInstance()->GetCurrentScene()->GetDevice();

    CManagement::GetInstance()->Open_Scene(SCENE_LOADING, CLoading_Scene::Create(pDev, SCENE_TUTORIAL));
    ClearAllUI();
    m_exitingEnter = false; 
}

void CUIManager::DestroyItemUI()
{
    if (!m_pItemUI) return;
    m_pItemUI->Set_Dead(true);
    m_pItemUI = nullptr; 
}

void CUIManager::DestroyEffectUI()
{
    if (!m_pEffectUI) return;
    m_pEffectUI->Set_Dead(true);
    m_pEffectUI = nullptr;
}

void CUIManager::DestroyReloadUI()
{
    if (!m_pReloadUI) return;
    m_pReloadUI->Set_Dead(true);
    m_pReloadUI = nullptr;
}


void CUIManager::Create_PlayerEff(PLAYEREFF _eEffect)
{
    auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();

    // ¸¸¾à player eff ui °¡ ¸¸µé¾îÁöÁö ¾Ê¾Ò´Ù¸é
    if (!m_pPlayerEffUI)
    {
        m_pPlayerEffUI = dynamic_cast<CUIBase*>(
            CObjectManager::GetInstance()->Clone_GameObject(
                L"Prototype_GameObject_UIRoot", sceneIdx, L"UI_Layer"));
    }

    auto* effect = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"));

    //if (!effect) return;

    // ¼ÂÆÃ °ª ÃÊ±âÈ­
    _vec4           vSizeOffset = {};
    const wchar_t* proto = L"";
    const wchar_t* childTag = L"";
    _uint           iIdx = 0;
    _float          fSpeed = 0.f;
    _bool           bLoop = false;

    // °ª ¼ÂÆÃ
    switch (_eEffect)
    {
    case PLAYEREFF::DASH:
        vSizeOffset = { 2048.f, 1248.f, 0.f,0.f }; //2048 1248
        proto = L"Prototype_Component_Texture_Effect_Dash";
        childTag = L"DashEff";
        iIdx = 6;
        fSpeed = 10.f;
        bLoop = true;
        break;

    case PLAYEREFF::BLOODR: //2048 1152
        vSizeOffset = { 2048.f,1152.f, 0.f,0.f }; //2048 1152
        proto = L"Prototype_Component_Texture_Effect_BloodR";
        childTag = L"BloodREff";
        iIdx = 6;
        fSpeed = 10.f;
        bLoop = false;
        break;

    case PLAYEREFF::BLOODG:
        vSizeOffset = { 1024.f,526.f, 0.f,0.f }; //2048 1152
        proto = L"Prototype_Component_Texture_Effect_BloodG";
        childTag = L"BloodGEff";
        iIdx = 6;
        fSpeed = 10.f;
        bLoop = false;
        break;
    }

    // Áßº¹ ¹æ¾î : ¸¸¾à ÇØ´ç tag°¡ child·Î ÀÖ´Ù¸é return
    if (m_pPlayerEffUI->Find_Child_ByTag(childTag))
        return;

    effect->Set_UISizeAndPos(vSizeOffset.x, vSizeOffset.y, vSizeOffset.z, vSizeOffset.w);
    effect->RegisterTexture(L"Com_Texture_PLayerEff", proto, 0, iIdx, fSpeed, bLoop);
    effect->ChangeTexture(L"Com_Texture_PLayerEff");

    effect->Set_ObjTag(childTag);

    m_pPlayerEffUI->Add_Child(effect);
}

void CUIManager::Create_CureEff()
{
}


void CUIManager::Destory_PlayerEff(PLAYEREFF _eEffect)
{
    if (!m_pPlayerEffUI) return;

    const wchar_t* childTag = L"";
    CImageUI* pEff = nullptr;
    // °ª ¼ÂÆÃ
    switch (_eEffect)
    {
    case PLAYEREFF::DASH:
        pEff = dynamic_cast<CImageUI*> (m_pPlayerEffUI->Find_Child_ByTag(L"DashEff"));
        if (!pEff)
            return;

        break;

    case PLAYEREFF::BLOODR:
        pEff = dynamic_cast<CImageUI*> (m_pPlayerEffUI->Find_Child_ByTag(L"BloodREff"));
        if (!pEff)
            return;
        if (!pEff->GetTextureCom()->Is_AnimFinished())
            return;

        break;

    case PLAYEREFF::BLOODG:
        pEff = dynamic_cast<CImageUI*> (m_pPlayerEffUI->Find_Child_ByTag(L"BloodGEff"));
        if (!pEff)
            return;
        if (!pEff->GetTextureCom()->Is_AnimFinished())
            return;

        break;
    }

    pEff->Set_Dead(true);                   // °´Ã¼ dead Ã³¸®
    m_pPlayerEffUI-> Remove_Child(pEff);    // child¿¡¼­ Á¦°Å
}

void CUIManager::Destory_CureEff()
{
}

void CUIManager::Destory_PlayerEff_ALL()
{
}

bool CUIManager::PhoneSlidesDone() const
{
    auto checkDone = [&](CUI* ui) {
        return std::none_of(m_slideTasks.begin(), m_slideTasks.end(),
            [&](const SlideTask& t) { return t.ui == ui; });
        };
    return checkDone(m_pPhone) &&
        checkDone(m_pLeftHand) &&
        checkDone(m_pRightHand) &&
        checkDone(m_pPhoneScreen);
}

void CUIManager::OpenShop()
{
    if (m_shopOpen) return;
    m_shopOpen = true;

    m_pShopRoot = m_pPhoeScreenBackGround
        ? static_cast<CUIBase*>(m_pPhoeScreenBackGround)
        : m_pEnterUI;
    if (!m_pShopRoot) return;

    float scx = -100.f, scy = 0.f;
    float sw = 630.f, sh = 300.f;
    if (auto* ui = dynamic_cast<CUI*>(m_pShopRoot)) {
        ui->Get_UIPosition(scx, scy);
        ui->Get_UISize(sw, sh);
        if (sw <= 0.f || sh <= 0.f) { sw = 630.f; sh = 420.f; }
    }

    // ÇöÀç ¾À °¡Á®¿À±â
    int sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();

    // ¾À¿¡ ¸Â´Â Ä«µå ¸®½ºÆ® Ã£±â
    auto it = gSceneShopCards.find(sceneIdx);
    if (it == gSceneShopCards.end()) return;

    const auto& cardsToShow = it->second;

    const float SAFE_L = 80.f;
    const float SAFE_R = 55.f;
    const float SAFE_T = 10.f;
    const float SAFE_B = 32.f;

    const float CARD_W = 170.f;
    const float CARD_H = 340.f;

    const int nCards = (int)cardsToShow.size();
    m_shopCards.clear();
    m_shopCards.reserve(nCards);

    const float innerW = sw - SAFE_L - SAFE_R;
    float gap = 16.f;
    if (nCards > 1)
        gap = max(16.f, (innerW - CARD_W * nCards) / (nCards - 1));

    const float left = scx - sw * 0.5f + SAFE_L + CARD_W * 0.5f;
    const float cy = scy - sh * 0.5f + SAFE_T + CARD_H * 0.5f + 40.f;

    for (int i = 0; i < nCards; ++i)
    {
        const float cx = (nCards == 1) ? scx : left + i * (CARD_W + gap);

        ShopCardUI card{};
        const UpgradeId id = cardsToShow[i];

        // ID·Î ShopItemDef Ã£±â
        if (const ShopItemDef* def = FindShopDef(id))
        {
            int poolIdx = (int)(def - &kShopPool[0]); // ÀÎµ¦½º ±¸ÇÏ±â
            CreateShopCardAt(poolIdx, cx, cy, card);

            if (card.btn) card.btn->Set_ButtonRect(cx, cy, CARD_W, CARD_H);

            m_shopCards.push_back(card);
        }
    }
}

void CUIManager::CloseShop()
{
    for (auto& c : m_shopCards)
    {
        if (m_pShopRoot && c.btn)
            m_pShopRoot->Remove_Child(c.btn);

        Safe_Release(c.btn);
        Safe_Release(c.icon);
        Safe_Release(c.pBack);
    }

    if (m_pShopRoot && m_pShopRoot != m_pEnterUI)
    {
        if (m_pEnterUI) m_pEnterUI->Remove_Child(m_pShopRoot);
        Safe_Release(m_pShopRoot);
    }

    m_pShopRoot = nullptr;
    m_shopOpen = false;
}


void CUIManager::CreateShopCardAt(int poolIdx, float cx, float cy, ShopCardUI& outCard)
{
    auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
    if (poolIdx < 0 || poolIdx >= (int)kShopPool.size()) return;
    const ShopItemDef& def = kShopPool[poolIdx];

    CUIBase* parent = m_pShopRoot ? m_pShopRoot : m_pEnterUI;
    if (!parent) return;

    auto attach = [&](CUIBase* ui)
        {
            if (!ui) return;
            ui->Set_RenderOn(true);
            parent->Add_Child(ui);
        };

    // - 2) ¹è°æÈ­¸é
    if (auto* back = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer")))
    {
        const float ICON_W = 160.f, ICON_H = 300.f;
        const float ICON_Y = cy;
        back->RegisterTexture(def.backTag.c_str(), def.backProto.c_str(), 0, 1, 0.f, false);
        back->ChangeTexture(def.backTag.c_str());
        back->SetAdditive(false);
        back->Set_UIPosition(cx, ICON_Y, ICON_W, ICON_H);
        attach(back);
        outCard.pBack = back;
    }

  
    // ¦¡¦¡ 1) Ä«µå ¹öÆ°(ÇÁ·¹ÀÓ)
    const float CARD_W = 200.f;
    const float CARD_H = 300.f;
    auto* btn = dynamic_cast<CButtonUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIButton", sceneIdx, L"UI_Layer"));
    if (!btn) return;

    btn->Set_ButtonRect(cx, cy, CARD_W, CARD_H);
    btn->SetSolidMode(false);
    btn->RegisterTexture(L"Com_Btn_Frame", L"Prototype_Component_Texture_PhoneShop_BoardFrameUI", 0, 1, 0.f, false);
    // ¸ðµç »óÅÂ°¡ °°Àº ÅØ½ºÃ³¸¦ ¾²µÇ, '»ö'Àº »óÅÂº° Æ¾Æ®·Î Á¦¾î
    btn->SetStateTextures(
        L"Com_Btn_Frame",   // Normal
        L"Com_Btn_Frame",   // Hover
        L"Com_Btn_Frame",   // Pressed
        L"Com_Btn_Frame"    // Disabled
    );

    btn->SetTextureTints(
        D3DXCOLOR(0.f, 0.f, 0.f, 1.f),  // Normal ¡æ °ËÁ¤(¹«Á¶°Ç ±î¸Ä°Ô)
        D3DXCOLOR(1.f, 1.f, 1.f, 1.f),  // Hover  ¡æ ¿øº»»ö
        D3DXCOLOR(1.f, 1.f, 1.f, 1.f),  // Pressed¡æ ¿øº»»ö (¿øÇÏ¸é 0.9·Î ¾à°£ ¾îµÓ°Ô)
        D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.f)// Disabled¡æ È¸»ö
    );

    // ³ª¸ÓÁö È¿°ú ±×´ë·Î
    btn->SetHoverScale(1.14f);
    btn->SetPressScale(1.04f);
    btn->SetLerpSpeeds(22.f, 14.f);
    attach(btn);
    outCard.btn = btn;
    outCard.id = def.id;



    // ¦¡¦¡ Ä«µå ¾ÆÀÌÄÜ
    if (auto* icon = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer")))
    {
        const float ICON_W = def.iconW;
        const float ICON_H = def.iconH;
        const float ICON_Y = cy + def.iconYOffset;

        icon->RegisterTexture(def.artTag.c_str(), def.artProto.c_str(), 0, 0, 0.f, false);
        icon->ChangeTexture(def.artTag.c_str());
        icon->SetAdditive(false);
        icon->Set_UIPosition(cx, ICON_Y, ICON_W, ICON_H);
        attach(icon);
        outCard.icon = icon;
    }

    //  Å¬¸¯½Ã µîÀå ½ÃÅ³ ÀÌ¹ÌÁö
    if (auto* sold = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer")))
    {
        const float SOLD_W = 96.f, SOLD_H = 40.f;
        const float SOLD_X = cx + 50.f, SOLD_Y = cy - 110.f;
        sold->RegisterTexture(L"Com_Tex_Sold", L"Prototype_Component_Texture_Shop_Sold",
            0, 0, 0.f, false);
        sold->ChangeTexture(L"Com_Tex_Sold");
        sold->Set_Active(false);
        sold->Set_UIPosition(SOLD_X, SOLD_Y, SOLD_W, SOLD_H);
        attach(sold);
    }

    CTextUI* buyLabel = dynamic_cast<CTextUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_TextUI", sceneIdx, L"UI_Layer"));
    if (buyLabel)
    {
        buyLabel->SetFontTag(L"Font_UI_Bold");
        buyLabel->SetText(L"±¸¸Å");
        buyLabel->SetColor(D3DXCOLOR(0.22f, 1.f, 0.08f, 1.f));
        buyLabel->SetScale(0.75f);
        buyLabel->SetCentered(true);
        buyLabel->SetLetterSpacing(1.f);

        const float BUY_Y = cy + CARD_H * 0.5f - 22.f;
        buyLabel->Set_UIPosition(cx, BUY_Y, 120.f, 26.f);


        buyLabel->Set_Active(false);
        buyLabel->Set_RenderOn(false);

        attach(buyLabel);


        btn->SetOnHoverEnter([buyLabel]() {
            if (buyLabel) {
                buyLabel->Set_Active(true);
                buyLabel->Set_RenderOn(true);
                buyLabel->m_bHovering = true;   // ¡ç Hover ½ÃÀÛ
            }
            });

        btn->SetOnHoverExit([buyLabel]() {
            if (buyLabel) {
                buyLabel->m_bHovering = false;  // ¡ç Hover ³¡
                buyLabel->SetColor(D3DXCOLOR(0.22f, 1.f, 0.08f, 1.f)); // ±âº»»ö º¹¿ø
                buyLabel->Set_RenderOn(false);
                buyLabel->Set_Active(false);
            }
            });
    }
    // ¦¡¦¡ Å¬¸¯ ·ÎÁ÷
    btn->SetOnClick([this, &outCard]()
        {
            m_bRemoveUI = true;
            m_pTalkUI->NextDialogue();
        });
}

void CUIManager::OnShopCardClicked(int slot)
{
    if (slot < 0 || slot >= (int)m_shopCards.size()) return;
    auto& card = m_shopCards[slot];
    if (!card.btn || card.bought) return;

    card.bought = true;
}

void CUIManager::LayoutShopCard(ShopCardUI& card)
{
    if (!card.btn) return;

    float cx, cy; card.btn->Get_UIPosition(cx, cy);
    float w, h;  card.btn->Get_UISize(w, h);

    // ¾ÆÀÌÄÜ¸¸ ¹èÄ¡
    if (card.icon) {
        const float iw = w * 0.62f, ih = iw;
        const float iy = cy - h * 0.23f;
        card.icon->Set_UIPosition(cx, iy, iw, ih);
    }
}

void CUIManager::ClearAllUI()
{
    // --- ±âº» UI Æ÷ÀÎÅÍ Á¤¸® ---
    Safe_Release(m_pEnterUI);
    Safe_Release(m_pFlooroUI);
    Safe_Release(m_pVictoryText);
    Safe_Release(m_pFloorTimeText);
    Safe_Release(m_pTimeBlack);
    Safe_Release(m_pTimeFrame);
    Safe_Release(m_pTimeText);
    Safe_Release(m_pLisaUI);
    Safe_Release(m_pTalkUI);
    Safe_Release(m_pPhone);
    Safe_Release(m_pLeftHand);
    Safe_Release(m_pRightHand);
    Safe_Release(m_pPhoneScreen);
    Safe_Release(m_pPhoeScreenBackGround);

    // --- »óÁ¡ UI ---
    Safe_Release(m_pShopRoot);
    for (auto& card : m_shopCards) {
        Safe_Release(card.btn);
        Safe_Release(card.icon);
        Safe_Release(card.pBack);
    }
    m_shopCards.clear();
    m_shopIndices.clear();
    m_shopOpen = false;

    // --- »óÅÂ º¯¼ö ÃÊ±âÈ­ ---
    m_spawnedTimeUI = false;
    m_timeAutoRemoveArmed = false;
    m_timeAutoRemoveTimer = 0.f;
    m_bRemoveUI = false;
    m_timeUIRemoveTimer = 0.f;
    m_phoneSlideActive = false;
    m_phonePullArmed = false;
    m_phonePullStarted = false;
    m_phonePullFinished = false;
    m_phonePullTimer = 0.f;
    m_createPhoneScreenPending = false;
    m_createPhoneScreenTimer = 0.f;
    m_createPhoneScreenDelay = 0.2f;
    m_exitingEnter = false;
    m_changeScreenOnPullFinish = false;
    m_nextPhoneScreenTexTag.clear();
    m_slideTasks.clear();
    m_scaleTasks.clear();
    m_pEnterUI = nullptr;
}


void CUIManager::CreateClearTextUI()
{
    auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
    if (auto* pBlack = dynamic_cast<CBlackGackGround*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_BlackBackground", sceneIdx, L"UI_Layer"))) {
        pBlack->Set_UIPosition(-130.f, -50.f, 1080.f, 100.f);
        pBlack->SetAlpha(0);
        pBlack->FadeTo(190, 0.0f, 0.25f);
        m_pEnterUI->Add_Child(pBlack);
        m_pTimeBlack = pBlack; 
    }

    if (auto* pTimeFrame = dynamic_cast<CPanelUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_PanelUI", sceneIdx, L"UI_Layer"))) {

        const float X = -130.f, Y = -50.f, W = 1080.F, H = 100.f;

        pTimeFrame->UseGreenFramePreset(X, Y, W, H, 3.f, true);
        m_pEnterUI->Add_Child(pTimeFrame);
        m_pTimeFrame = pTimeFrame;
    }

    if (auto* txt1 = dynamic_cast<CTextUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_TextUI", sceneIdx, L"UI_Layer"))) {

        txt1->SetFontTag(L"Font_UI_ROUGH");
        txt1->SetText(L"VICTORY");
        txt1->SetColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

        txt1->SetScale(3.f);
        txt1->SetCentered(true);
        txt1->SetLetterSpacing(10.f);

        txt1->PlayAppear(1.f, 1.f, 10.f);
        txt1->Set_UIPosition(-60.F, 300.f, 0.f, 0.f);

        m_pVictoryText = txt1;
        m_pEnterUI->Add_Child(txt1);
    }

    if (auto* txt2 = dynamic_cast<CTextUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_TextUI", sceneIdx, L"UI_Layer"))) {

        txt2->SetFontTag(L"Font_UI_Bold");
        txt2->SetText(L"FLOOR TIME");
        txt2->SetColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

        txt2->SetScale(1.5f);
        txt2->SetCentered(true);
        txt2->SetLetterSpacing(10.f);

        txt2->PlayAppear(1.f, 1.f, 10.f);
        txt2->Set_UIPosition(-60.F, 170.f, 0.f, 0.f);

        m_pFloorTimeText = txt2;
        m_pEnterUI->Add_Child(txt2);
    }
    m_spawnedTimeUI = false;
    m_timeAutoRemoveArmed = false;
    m_timeAutoRemoveTimer = 0.f;
    m_pTimeText = nullptr;
}

void CUIManager::CreateTimeTextUI(const std::wstring& timeStr)
{
    if (!m_pEnterUI) return;

    auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
    if (auto* timeTxt = dynamic_cast<CTextUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_TextUI", sceneIdx, L"UI_Layer")))
    {
        timeTxt->SetFontTag(L"Font_Time");
        timeTxt->SetText(timeStr);
        timeTxt->SetColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
        timeTxt->SetScale(2.0f);
        timeTxt->SetCentered(true);
        timeTxt->SetLetterSpacing(6.f);
        timeTxt->PlayAppear(0.5f, 1.2f, 1.05f);
        timeTxt->Set_UIPosition(-100.f, 90.f, 0.f, 0.f);
        m_pEnterUI->Add_Child(timeTxt);

        m_pTimeText = timeTxt;
        m_timeAutoRemoveArmed = true;
        m_timeAutoRemoveTimer = 0.f;
    }
}

void CUIManager::CreatePhoneUI()
{
    auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
    if (m_pPhoeScreenBackGround = dynamic_cast<CBlackGackGround*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_BlackBackground", sceneIdx, L"UI_Layer"))) {
        m_pPhoeScreenBackGround->Set_UIPosition(-100.f, 0.f, (float)650.f, (float)420.f);
        m_pPhoeScreenBackGround->SetAlpha(255);     
        m_pPhoeScreenBackGround->Set_Active(false);
        m_pEnterUI->Add_Child(m_pPhoeScreenBackGround);
    }

    // È­¸é ½ºÅ©¸° »ý¼º
    m_pPhoneScreen = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"));


    if (m_pPhoneScreen)
    {
        m_pPhoneScreen->RegisterTexture(L"Com_Texture_PhoneLogoUI", L"Prototype_Component_Texture_Phone_ScreenUI", 0, 6, 4.f, true);
        m_pPhoneScreen->RegisterTexture(L"Com_Texture_PhoneErrorUI", L"Prototype_Component_Texture_Phone_ErrorScreenUI", 0, 4, 5.f, true);
        m_pPhoneScreen->Play(true);
        m_pPhoneScreen->Set_UIPosition(-120.f, 600.f, 410.f, 250.f);
        m_pPhoneScreen->ChangeTexture(L"Com_Texture_PhoneLogoUI");
        m_pEnterUI->Add_Child(m_pPhoneScreen);
    }

    if (m_pPhone = dynamic_cast<CPhoneUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_PhoneUI", sceneIdx, L"UI_Layer")))
    {
        m_pPhone->Set_UIPosition(-160.f, 600.f, 600.f, 300.f);
        m_pEnterUI->Add_Child(m_pPhone);

        // ¿Þ¼Õ
        m_pLeftHand = dynamic_cast<CImageUI*>(
            CObjectManager::GetInstance()->Clone_GameObject(
                L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"));
        if (m_pLeftHand)
        {
            m_pLeftHand->RegisterTexture(L"Com_Texture_RightHandIDLE",
                L"Prototype_Component_Texture_Phone_RightHandUI", 0, 2, 4.f, true);
            m_pLeftHand->Play(true);

            m_pLeftHand->Set_UIPosition(190.f, 600.f + 80.f, 350.f, 410.f);
            m_pLeftHand->ChangeTexture(L"Com_Texture_RightHandIDLE");

            m_pPhone->Add_Child(m_pLeftHand);
        }

        // ¿À¸¥¼Õ
        m_pRightHand = dynamic_cast<CImageUI*>(
            CObjectManager::GetInstance()->Clone_GameObject(
                L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"));
        if (m_pRightHand)
        {
            m_pRightHand->RegisterTexture(L"Com_Texture_LeftHandIDLE",
                L"Prototype_Component_Texture_Phone_LeftHandUI", 0, 2, 4.f, true);
            m_pRightHand->Play(true);

            m_pRightHand->Set_UIPosition(-450.f, 600.f + 80.f, 350.f, 410.f);
            m_pRightHand->ChangeTexture(L"Com_Texture_LeftHandIDLE");

            m_pPhone->Add_Child(m_pRightHand);
        }
    }
}

void CUIManager::CreatePhoneScreen()
{
    // 1) ·Î°í ¾Ö´Ï ²ô°í, ¹è°æ ÆÐ³Î È°¼ºÈ­
    if (m_pPhoneScreen)           m_pPhoneScreen->Set_Active(false);
    if (m_pPhoeScreenBackGround)  m_pPhoeScreenBackGround->Set_Active(true);

    CUIBase* parent = m_pPhoeScreenBackGround ? static_cast<CUIBase*>(m_pPhoeScreenBackGround)
        : m_pEnterUI;
    if (!parent) return;

    auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
    // 2) ¿ÞÂÊ ¼¼·Î Å¸ÀÌÆ²
    if (auto* img = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer")))
    {
        img->Set_UIPosition(-380.f, -10.f, 60.f, 390.f);
        img->RegisterTexture(L"Com_Texture_Logo", L"Prototype_Component_Texture_Phone_ScreenTitleUI", 0, 1, 0.f, false);
        img->ChangeTexture(L"Com_Texture_Logo");
        parent->Add_Child(img);
    }

    // ±¸¸Å Å¸ÀÌÆ²
    if (auto* shopTitle = dynamic_cast<CTextUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_TextUI", sceneIdx, L"UI_Layer")))
    {
        shopTitle->SetFontTag(L"UIFont");
        shopTitle->SetText(L"CHOOSE YOUR UPGRADE!");
        shopTitle->SetColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
        shopTitle->SetScale(0.7f);
        shopTitle->SetCentered(true);
        shopTitle->SetLetterSpacing(4.f);
        shopTitle->Set_UIPosition(-100.f, 200.f, 0.f, 0.f);
        m_pEnterUI->Add_Child(shopTitle);
    }


    OpenShop();


    // 3) Æù ÇÁ·¹ÀÓ
    if (auto* img2 = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer")))
    {
        img2->Set_UIPosition(-100.f, 0.f, 630.f, 420.f); // È­¸é ÇÁ·¹ÀÓ
        img2->RegisterTexture(L"Com_Texture_Logo", L"Prototype_Component_Texture_Phone_FrameUI", 0, 4, 10.f, true);
        img2->ChangeTexture(L"Com_Texture_Logo");
        img2->Play(true);
        // ÇÁ·¹ÀÓÀ» ÃÖ»ó´ÜÀ¸·Î ¿Ã¸®°í ½ÍÀ¸¸é:
        parent->Add_ChildFront(img2);
    }
}

void CUIManager::CreateReloadUI()
{
    if (m_pReloadUI) return; // Áßº¹ ¹æÁö
    auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
    m_pReloadUI = dynamic_cast<CUIBase*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIRoot", sceneIdx, L"UI_Layer"));
    if (!m_pReloadUI) return;

    if (auto* pRed = dynamic_cast<CBlackGackGround*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_BlackBackground", sceneIdx, L"UI_Layer"))) {
        pRed->Set_UIPosition(0.f, 110.f, 170.f, 50.f);
        pRed->SetAlpha(255);
        pRed->SetColor(D3DCOLOR_ARGB(255, 220, 80, 80)); // Èå¸° ºÓÀº»ö
        m_pReloadUI->Add_Child(pRed);
    }

    if (auto* pBlack = dynamic_cast<CBlackGackGround*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_BlackBackground", sceneIdx, L"UI_Layer"))) {
        pBlack->Set_UIPosition(-5.f, 105.f, 160.f, 40.f);
        pBlack->SetAlpha(255);
        m_pReloadUI->Add_Child(pBlack);
    }

    if (auto* pGreen = dynamic_cast<CBlackGackGround*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_BlackBackground", sceneIdx, L"UI_Layer"))) {
        pGreen->Set_UIPosition(30.f, 105.f, 100.f, 40.f);
        pGreen->SetAlpha(255);
        pGreen->SetColor(D3DCOLOR_ARGB(255, 0, 255, 0)); // ¹àÀº ¿¬µÎ»ö (Lime Green)
        pGreen->EnableColorCycle(true, 0.8f); // ¡ç ÀÌ °´Ã¼¸¸ »ö»ó ÀÚµ¿ º¯°æ
        m_pReloadUI->Add_Child(pGreen);
    }


    if (auto* txt1 = dynamic_cast<CTextUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_TextUI", sceneIdx, L"UI_Layer"))) {
        txt1->SetFontTag(L"Font_UI_Regular");
        txt1->SetText(L"[R]");
        txt1->Set_UIPosition(-50.f, -95.f, 50.f, 25.f);
        txt1->SetColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
        txt1->SetScale(0.5f);
        txt1->SetCentered(true);
        m_pReloadUI->Add_Child(txt1);
    }

    if (auto* txt2 = dynamic_cast<CTextUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_TextUI", sceneIdx, L"UI_Layer"))) {
        txt2->SetFontTag(L"Font_UI_Bold");
        txt2->SetText(L"ÀçÀåÀü");
        txt2->Set_UIPosition(35.f, -95.f, 50.f, 25.f);
        txt2->SetColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
        txt2->SetScale(0.5f);
        txt2->SetCentered(true);
        m_pReloadUI->Add_Child(txt2);
    }
}

void CUIManager::SliderPhoneUI()
{
    m_phonePullFinished = false;
    m_changeScreenOnPullFinish = false;
    m_nextPhoneScreenTexTag.clear();
    // ½½¶óÀÌµå ¡°½ÃÀÛµÊ¡± ÇÃ·¡±×/Å¸ÀÌ¸Ó ¸®¼Â
    m_phoneSlideActive = true;
    m_phonePullArmed = false;
    m_phonePullStarted = false;
    m_phonePullTimer = 0.f;

    // º»Ã¼ / ¼Õ / ½ºÅ©¸° °¢°¢ À§·Î ½½¶óÀÌµå ÀÎ
    AddSlideInY(m_pPhone, -160.f, 50.f, 600.f, 300.f, +400.f, 0.f, 1.f);
    if (m_pLeftHand)  AddSlideInY(m_pLeftHand, 190.f, 80.f, 350.f, 410.f, +600.f, 0.f, 1.f);
    if (m_pRightHand) AddSlideInY(m_pRightHand, -450.f, 80.f, 350.f, 410.f, +600.f, 0.f, 1.f);
    if (m_pPhoneScreen)
        AddSlideInY(m_pPhoneScreen, -120.f, 50.f, 410.f, 250.f, +600.f, 0.f, 0.85f);
}

void CUIManager::StartPhonePullAnim()
{
    // ¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡
// ÇÁ·¹ÀÓ ±¸¸Û(center, size)  ¡Ø CreateClearUI()¿Í µ¿ÀÏÇØ¾ß ÇÔ
//   Black/Hole & Frame : (-130, -70) Áß½É, 1080 x 600 Å©±â
// ¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡
    const float FRAME_CX = -130.f;
    const float FRAME_CY = -70.f;
    const float HOLE_W = 1080.f;
    const float HOLE_H = 600.f;

    // ¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡
    // ½ºÅ©¸°À» ÇÁ·¹ÀÓ ±¸¸Ûº¸´Ù »ìÂ¦ ÀÛ°Ô (¿©¹é È®º¸)
    // ºñÀ²Àº ÅØ½ºÃ³(410x250 ? 1.64) À¯Áö
    //   - ´õ ÀÛ°Ô º¸ÀÌ°Ô ÇÏ°í ½ÍÀ¸¸é RATIO¸¦ ³·Ãç(¿¹: 0.70f)
    // ¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡
    const float SCREEN_RATIO = 0.74f;                 // 0.80f ¡æ 0.74f ·Î ÁÙÀÓ(´ú ²Ë Â÷°Ô)
    const float TARGET_SCR_W = SCREEN_RATIO * HOLE_W; // ? 799
    const float TARGET_SCR_H = TARGET_SCR_W * (250.f / 410.f); // ? 487

    // ¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡
    // Æù(º£Á©) Å©±â = ½ºÅ©¸° + ÆÐµù(ÁÂ¿ì/»óÇÏ ¿©¹é)
    // ¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡
    const float PHONE_PAD_W = 140.f;     // ÁÂ+¿ì ¿©¹é ÇÕ
    const float PHONE_PAD_H = 70.f;      // »ó+ÇÏ ¿©¹é ÇÕ
    const float TARGET_PHN_W = TARGET_SCR_W + PHONE_PAD_W; // ? 939
    const float TARGET_PHN_H = TARGET_SCR_H + PHONE_PAD_H; // ? 557

    // ¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡
    // ÃÖÁ¾ Áß½É À§Ä¡(Y¸¸ ¾Æ·¡·Î ³»¸²)
    //   - ±âÁ¸ centerY(-70)¿¡¼­ +100 ³»·Á¼­ »ó´Ü HUD¿Í °ãÄ¡Áö ¾Ê°Ô
    // ¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡
    const float SHIFT_DOWN = 60.f;   // ¡ç 100.f ¿¡¼­ 60.f ·Î º¯°æ
    const float screenEndX = FRAME_CX;
    const float screenEndY = FRAME_CY + SHIFT_DOWN;      // -70 ¡æ +30
    const float phoneEndX = screenEndX - 40.f;          // Æù°ú ½ºÅ©¸° »ó´ë ¿ÀÇÁ¼Â X(ÆùÀÌ ¾à°£ ¿ÞÂÊ)
    const float phoneEndY = screenEndY + 0.f;

    // ¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡
    // ¼Õ À§Ä¡/Å©±â (Æù ¹Ù±ùÀ¸·Î »ìÂ¦ ³ª°¡°Ô ÇØ¼­ È­¸é °¡¸®Áö ¾Êµµ·Ï)
    //  - OUTSIDE_X : °¡ÀåÀÚ¸® ¹ÛÀ¸·Î ³»º¸³»´Â °Å¸®
    //  - HAND_Y    : ¼ÕÀÇ Y (½ºÅ©¸°º¸´Ù »ìÂ¦ ¾Æ·¡·Î)
    // ¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡
    const float HAND_W = 400.f;    // 420 ¡æ 400 (Á¶±Ý ÁÙÀÓ)
    const float HAND_H = 600.f;    // 630 ¡æ 600
    const float HAND_Y = screenEndY + 10.f;

    // Æù ÁÂ¿ì ¿¡Áö(¼¾ÅÍ ¡¾ Æø/2)
    const float leftEdge = phoneEndX - TARGET_PHN_W * 0.5f;
    const float rightEdge = phoneEndX + TARGET_PHN_W * 0.5f;

    // ÇÁ·ÎÁ§Æ® ³×ÀÌ¹Ö: m_pLeftHand == ¿À¸¥ÂÊ ¼Õ, m_pRightHand == ¿ÞÂÊ ¼Õ
    const float rightHandEndX = leftEdge + 60.f;   // ¿ÞÂÊ ¼Õ(º¯¼ö RightHand)Àº ¿ÞÂÊ ¹ÛÀ¸·Î
    const float leftHandEndX = rightEdge + 40.f;   // ¿À¸¥ÂÊ ¼Õ(º¯¼ö LeftHand)Àº ¿À¸¥ÂÊ ¹ÛÀ¸·Î

    // ¾Ö´Ï ±æÀÌ
    const float DUR = 0.60f;
    const float DELAY = 0.00f;

    // ¦¡¦¡¦¡¦¡¦¡ ½ÇÁ¦ Å¥ µî·Ï: (ÇöÀç À§Ä¡/Å©±â) ¡æ (¸ñÇ¥ À§Ä¡/Å©±â)
    // PHONE(º£Á©)
    AddScaleIn(m_pPhone,
        -160.f, 50.f, 600.f, 300.f,       // ÇöÀç(½½¶óÀÌµå ÈÄ) °ª°ú ¸ÂÃß¸é ´õ ÀÚ¿¬½º·¯¿ò
        phoneEndX, phoneEndY, TARGET_PHN_W, TARGET_PHN_H,
        DELAY, DUR);

    // SCREEN(Æù ³»ºÎ)
    if (m_pPhoneScreen) {
        AddScaleIn(m_pPhoneScreen,
            -120.f, 50.f, 410.f, 250.f,
            screenEndX, screenEndY, TARGET_SCR_W, TARGET_SCR_H,
            DELAY, DUR);
    }

    // ¿À¸¥ÂÊ ¼Õ(º¯¼ö m_pLeftHand)
    if (m_pLeftHand) {
        AddScaleIn(m_pLeftHand,
            190.f, 80.f, 350.f, 410.f,
            leftHandEndX, HAND_Y, HAND_W, HAND_H,
            DELAY, DUR);
    }

    // ¿ÞÂÊ ¼Õ(º¯¼ö m_pRightHand)
    if (m_pRightHand) {
        AddScaleIn(m_pRightHand,
            -450.f, 80.f, 350.f, 410.f,
            rightHandEndX, HAND_Y, HAND_W, HAND_H,
            DELAY, DUR);
    }

}


bool CUIManager::PhoneScaleDone() const
{
    auto done = [&](CUI* ui) {
        return std::none_of(m_scaleTasks.begin(), m_scaleTasks.end(),
            [&](const ScaleTask& t) { return t.ui == ui; });
        };
    return done(m_pPhone) && done(m_pPhoneScreen) && done(m_pLeftHand) && done(m_pRightHand);
}

void CUIManager::ChangePhoneScreenAfterPull(const std::wstring& texTag)
{
    m_nextPhoneScreenTexTag = texTag;
    m_changeScreenOnPullFinish = true;
}

void CUIManager::Free()
{
    ClearAllUI();
}
