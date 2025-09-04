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
#include "CPlayer.h"
#include "Sound_Manager.h"

// 嶸せ - UI 避檜晦
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
    // 擁棲詭檜暮 蝸塭檜萄 (UI)
    m_slideTasks.erase(
        std::remove_if(m_slideTasks.begin(), m_slideTasks.end(),
            [](const SlideTask& t) { return t.done || t.ui == nullptr; }),
        m_slideTasks.end());

    // 擁棲詭檜暮 觼晦 酈辦晦 蝸塭檜萄
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

    // 顫歜 臢蝶お 衛除 蝶ア
    if (!m_spawnedTimeUI && m_pVictoryText && m_pFloorTimeText) {
        if (m_pVictoryText->IsAppearFinished() && m_pFloorTimeText->IsAppearFinished()) {

            // Ы溯檜橫曖 衛除擊 陛螳褥
            CPlayer* pPlayer = dynamic_cast<CPlayer*>(CObjectManager::GetInstance()->Find_Object(CManagement::GetInstance()->Get_CurrentSceneIdx(),
                TEXT("Player_Layer"), 0));
            if (pPlayer)
            {
                _float fPlayTime = pPlayer->Get_PlayTime();
                int totalSeconds = static_cast<int>(fPlayTime);

                int minutes = totalSeconds / 60;
                int seconds = totalSeconds % 60;
                int mmsec = (totalSeconds - minutes * 60 - seconds) * 100;

                auto To2Digit = [](int num) -> std::wstring {
                    return (num < 10 ? L"0" : L"") + std::to_wstring(num);
                    };

                std::wstring timeStr =
                    To2Digit(minutes) + L":" +
                    To2Digit(seconds) + L":" +
                    To2Digit(mmsec);

                CreateTimeTextUI(timeStr);
            }
            
            else
            {
                CreateTimeTextUI(L"01:12:45");
            }
            m_spawnedTimeUI = true;
        }
    }

    // 式式 с萄ア 渡晦晦
    if (m_phonePullStarted && !m_phonePullFinished && PhoneScaleDone()) {
        m_phonePullFinished = true;

        if (m_pPhoneScreen) {
            const wchar_t* tag = m_nextPhoneScreenTexTag.empty()
                ? L"Com_Texture_PhoneErrorUI"
                : m_nextPhoneScreenTexTag.c_str();

            if (m_changeScreenOnPullFinish) {
                m_pPhoneScreen->ChangeTexture(tag);
                m_pPhoneScreen->Play(true);
                // 1蟾 �� с萄ア 湮晦晦
                m_createPhoneScreenPending = true;
                m_createPhoneScreenTimer = 0.f;
                m_createPhoneScreenDelay = 1.0f;

                m_changeScreenOnPullFinish = false;
                m_nextPhoneScreenTexTag.clear();
            }
        }
    }

    // с萄ア 湮晦晦 顫檜該
    if (m_createPhoneScreenPending) {
        m_createPhoneScreenTimer += dt;
        if (m_createPhoneScreenTimer >= m_createPhoneScreenDelay) {
            m_createPhoneScreenPending = false;
            m_createPhoneScreenTimer = 0.f;
            CreatePhoneScreen();
        }
    }

    // UI 餉薯 顫檜該
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
                std::vector<std::wstring> dialogues = { L"贗葬橫ж樟捱蹂...", L"謠擎 勘 爾罹萄萵啪蹂", L"ч遴擊 網橫蹂!!" };
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

    // с萄ア 渡晦晦 お葬剪
    if (m_phoneSlideActive && !m_phonePullArmed && PhoneSlidesDone()) {
        m_phonePullArmed = true;
        m_phonePullTimer = 0.f;
        m_phoneSlideActive = false;   // 醞犒 寞雖
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
    CSound_Manager::GetInstance()->StopAll();
    CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/mx_stinger_stage_end", 1.f, SOUND_UI, false);
    CSound_Manager::GetInstance()->PlayBGM(L"../Bin/Resource/mx_elevator_v2_fx", 1.f, true);
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
        pBlack->SetHoleRect(-130.f, -70.f, 1080.f, 600.f); // Щ溯歜 掘誠
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

}  // 啪歜 贗葬橫 UI 儅撩

void CUIManager::CreateEffectUI(const std::wstring& str)
{
    if (m_pEffectUI && m_pEffectUI->Get_Dead())
        m_pEffectUI = nullptr;

    if (m_pEffectUI) return; // 醞犒 儅撩 寞雖

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

    if (m_pItemUI) return; // 醞犒 寞雖
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
    // 壁 掖羹 (煎註壁戲煎)
    LPDIRECT3DDEVICE9 pDev = CManagement::GetInstance()->GetCurrentScene()->GetDevice();

    CManagement::GetInstance()->Open_Scene(SCENE_LOADING, CLoading_Scene::Create(pDev, (SCENE)(SCENE_SNIPE)));
    //CManagement::GetInstance()->Open_Scene(SCENE_LOADING, CLoading_Scene::Create(pDev, (SCENE)(sceneIdx + 1)));
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

void CUIManager::ClearPlayerUI()
{
    Destory_PlayerEff_ALL();
    Safe_Release(m_pPlayerEffUI);
    Destory_CureEff();
    Safe_Release(m_pCureEffUI);
}


void CUIManager::Create_PlayerEff(PLAYEREFF _eEffect)
{
    auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();

    // 虜擒 player eff ui 陛 虜菟橫雖雖 彊懊棻賊
    if (!m_pPlayerEffUI)
    {
        m_pPlayerEffUI = dynamic_cast<CUIBase*>(
            CObjectManager::GetInstance()->Clone_GameObject(
                L"Prototype_GameObject_UIRoot", sceneIdx, L"UI_Layer"));
    }

    auto* effect = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"));

    if (!effect) return;

    // 撢た 高 蟾晦��
    _vec4           vSizeOffset = {};
    const wchar_t* proto = L"";
    const wchar_t* childTag = L"";
    _uint           iIdx = 0;
    _float          fSpeed = 0.f;
    _bool           bLoop = false;

    // 高 撢た
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

    // 醞犒 寞橫 : 虜擒 п渡 tag陛 child煎 氈棻賊 return
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
    // Prototype_Component_Texture_Effect_Cure

    auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();

    // 虜擒 player eff ui 陛 虜菟橫雖雖 彊懊棻賊
    if (!m_pCureEffUI)
    {
        m_pCureEffUI = dynamic_cast<CUIBase*>(
            CObjectManager::GetInstance()->Clone_GameObject(
                L"Prototype_GameObject_UIRoot", sceneIdx, L"UI_Layer"));
    }

    // 醞犒 寞橫 : children檜 綠錶氈擊 陽虜 儅撩
    if (!m_pCureEffUI->GetChildren().empty())
        return;

    // 20偃 虜菟橫 // test : 2偃

    for (int i = 0; i < 30; i++)
    {
        auto* effect = dynamic_cast<CImageUI*>(
            CObjectManager::GetInstance()->Clone_GameObject(
                L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"));

        // texture 撢た
        effect->RegisterTexture(L"Com_Texture_PLayerEff", L"Prototype_Component_Texture_Effect_Cure", 0, 0, 1.f, false);
        effect->ChangeTexture(L"Com_Texture_PLayerEff");

        // 嬪纂 撢た
        _float fPosX, fPosY;

        fPosX = (rand() % 100) * 10.f;
        if (i % 2 == 0)
        {
            fPosX *= -1.f;
        }
        fPosY = float(rand() % 301 + WINCY) + 10.f; // wincy ~ wincy + 300 餌檜縑憮 儅撩
        //fPosY = 0.f;
       
        effect->Set_UISizeAndPos(50.f, 50.f, WINCX * 0.5 + fPosX, fPosY);
        
        // move 撢た
        _float fRange = float(rand() % 500) + 200.f; // 100 縑憮 300 餌檜
        effect->Set_UIMoveInfo({ MV_UP,true, fRange , 0.f,true }); // 嬪煎 楠渾и 虜躑 遺霜檜堅  render off
        effect->Set_New_TransInfo(500.f, 0.f);
        effect->Set_IsPosFix(false);

        m_pCureEffUI->Add_Child(effect);
    }

    // y高擎 wincy爾棻 嬴楚煎
    // x高擎 0  ~ wincx
    // 嬪纂 濩嬴 輿堅

    // speed 橾薑
    // mv_up / range 楠渾
    // render off true
    // ui move 瞳辨
}

void CUIManager::Update_CureEff(const _float& fTimeDelta)
{
    if (!m_pCureEffUI ||
        m_pCureEffUI->GetChildren().empty()) // 蕨諼籀葬
        return;

    list<CUIBase*> plistChildren = m_pCureEffUI->GetChildren();

    for (auto pChild : plistChildren)
    {
        CUI* pUI = dynamic_cast<CUI*>(pChild);
        _float y;
        pUI->Move_UI(fTimeDelta);
        //pUI->Update_Position(pUI->GetTransform()->Get_Info(INFO_POS));

        if (pUI->Get_UIMoveInfo().IsRangeEnd())
        {
            pUI->Set_Dead(true);
            m_pCureEffUI->Remove_Child(pUI);
        }
    }
}

void CUIManager::Create_AimUI()
{
    if (m_pAimUI) return; // 醞犒 寞雖
    auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
    m_pAimUI = dynamic_cast<CUIBase*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIRoot", sceneIdx, L"UI_Layer"));
    if (!m_pAimUI) return;

    //if (auto* pRed = dynamic_cast<CBlackGackGround*>(
    //    CObjectManager::GetInstance()->Clone_GameObject(
    //        L"Prototype_GameObject_BlackBackground", sceneIdx, L"UI_Layer"))) {
    //    pRed->Set_UIPosition(0.f, 130.f, 170.f, 50.f);
    //    pRed->SetAlpha(255);
    //    pRed->SetColor(D3DCOLOR_ARGB(255, 220, 80, 80)); // �撣� 碩擎儀
    //    m_pAimUI->Add_Child(pRed);
    //}

    if (auto* pBlack = dynamic_cast<CBlackGackGround*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_BlackBackground", sceneIdx, L"UI_Layer"))) {
        pBlack->Set_UIPosition(-18.f, 125.f, 210.f, 40.f);
        pBlack->SetAlpha(255);
        m_pAimUI->Add_Child(pBlack);
    }

    if (auto* pGreen = dynamic_cast<CBlackGackGround*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_BlackBackground", sceneIdx, L"UI_Layer"))) {
        pGreen->Set_UIPosition(37.f, 125.f, 100.f, 40.f);
        pGreen->SetAlpha(255);
        pGreen->SetColor(D3DCOLOR_ARGB(255, 255, 0, 0)); // 
        //pGreen->EnableColorCycle(true, 0.8f); // ∠ 檜 偌羹虜 儀鼻 濠翕 滲唳
        m_pAimUI->Add_Child(pGreen);
    }


    if (auto* txt1 = dynamic_cast<CTextUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_TextUI", sceneIdx, L"UI_Layer"))) {
        txt1->SetFontTag(L"Font_UI_Regular");
        txt1->SetText(L"[Mouse 2]");
        txt1->Set_UIPosition(-65.f, -115.f, 50.f, 25.f);
        txt1->SetColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
        txt1->SetScale(0.5f);
        txt1->SetCentered(true);
        m_pAimUI->Add_Child(txt1);
    }

    if (auto* txt2 = dynamic_cast<CTextUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_TextUI", sceneIdx, L"UI_Layer"))) {
        txt2->SetFontTag(L"Font_UI_Bold");
        txt2->SetText(L"褻 遽");
        txt2->Set_UIPosition(37.f, -115.f, 50.f, 25.f);
        txt2->SetColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
        txt2->SetScale(0.5f);
        txt2->SetCentered(true);
        m_pAimUI->Add_Child(txt2);
    }
}


void CUIManager::Destory_PlayerEff(PLAYEREFF _eEffect)
{
    if (!m_pPlayerEffUI ||
        m_pPlayerEffUI->GetChildren().empty()) 
        return;

    const wchar_t* childTag = L"";
    CImageUI* pEff = nullptr;
    // 高 撢た
    switch (_eEffect)
    {
    case PLAYEREFF::DASH:
        pEff = dynamic_cast<CImageUI*> (m_pPlayerEffUI->Find_Child_ByTag(L"DashEff"));
        if (pEff == nullptr)
            return;

        break;

    case PLAYEREFF::BLOODR:
        pEff = dynamic_cast<CImageUI*> (m_pPlayerEffUI->Find_Child_ByTag(L"BloodREff"));
        if (pEff == nullptr)
            return;
        if (!pEff->GetTextureCom()->Is_AnimFinished())
            return;

        break;

    case PLAYEREFF::BLOODG:
        pEff = dynamic_cast<CImageUI*> (m_pPlayerEffUI->Find_Child_ByTag(L"BloodGEff"));
        if (pEff ==nullptr)
            return;
        if (!pEff->GetTextureCom()->Is_AnimFinished())
            return;

        break;
    }

    pEff->Set_Dead(true);                   // 偌羹 dead 籀葬
    m_pPlayerEffUI-> Remove_Child(pEff);    // child縑憮 薯剪
}

void CUIManager::Destory_CureEff()
{
    if (!m_pCureEffUI) return;

    m_pCureEffUI->Set_Dead(true);
    m_pCureEffUI = nullptr;
}

void CUIManager::Destory_PlayerEff_ALL()
{
    if (!m_pPlayerEffUI) return;

    m_pPlayerEffUI->Set_Dead(true);
    m_pPlayerEffUI = nullptr;
}

void CUIManager::Destroy_AimUI()
{
    if (!m_pAimUI) return;
    m_pAimUI->Set_Dead(true);
    m_pAimUI = nullptr;
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

    // ⑷營 壁 陛螳螃晦
    int sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();

    // 壁縑 蜃朝 蘋萄 葬蝶お 瓊晦
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

        // ID煎 ShopItemDef 瓊晦
        if (const ShopItemDef* def = FindShopDef(id))
        {
            int poolIdx = (int)(def - &kShopPool[0]); // 檣策蝶 掘ж晦
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

    // - 2) 寡唳�飛�
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

  
    // 式式 1) 蘋萄 幗が(Щ溯歜)
    const float CARD_W = 200.f;
    const float CARD_H = 300.f;
    auto* btn = dynamic_cast<CButtonUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIButton", sceneIdx, L"UI_Layer"));
    if (!btn) return;

    btn->Set_ButtonRect(cx, cy, CARD_W, CARD_H);
    btn->SetSolidMode(false);
    btn->RegisterTexture(L"Com_Btn_Frame", L"Prototype_Component_Texture_PhoneShop_BoardFrameUI", 0, 1, 0.f, false);
    // 賅萇 鼻鷓陛 偽擎 臢蝶籀蒂 噙腎, '儀'擎 鼻鷓滌 ずお煎 薯橫
    btn->SetStateTextures(
        L"Com_Btn_Frame",   // Normal
        L"Com_Btn_Frame",   // Hover
        L"Com_Btn_Frame",   // Pressed
        L"Com_Btn_Frame"    // Disabled
    );

    btn->SetTextureTints(
        D3DXCOLOR(0.f, 0.f, 0.f, 1.f),  // Normal ⊥ 匐薑(鼠褻勒 梱蜊啪)
        D3DXCOLOR(1.f, 1.f, 1.f, 1.f),  // Hover  ⊥ 錳獄儀
        D3DXCOLOR(1.f, 1.f, 1.f, 1.f),  // Pressed⊥ 錳獄儀 (錳ж賊 0.9煎 擒除 橫萍啪)
        D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.f)// Disabled⊥ �蜓�
    );

    // 釭該雖 �膩� 斜渠煎
    btn->SetHoverScale(1.14f);
    btn->SetPressScale(1.04f);
    btn->SetLerpSpeeds(22.f, 14.f);
    attach(btn);
    outCard.btn = btn;
    outCard.id = def.id;



    // 式式 蘋萄 嬴檜夔
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

    //  贗葛衛 蛔濰 衛鑒 檜嘐雖
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
        buyLabel->SetText(L"掘衙");
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
                buyLabel->m_bHovering = true;   // ∠ Hover 衛濛
                CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/sfx_ui_trailer_hover", SOUND_UI, 1.f);
            }
            });

        btn->SetOnHoverExit([buyLabel]() {
            if (buyLabel) {
                buyLabel->m_bHovering = false;  // ∠ Hover 部
                buyLabel->SetColor(D3DXCOLOR(0.22f, 1.f, 0.08f, 1.f)); // 晦獄儀 犒錳
                buyLabel->Set_RenderOn(false);
                buyLabel->Set_Active(false);
            }
            });
    }
    // 式式 贗葛 煎霜
    btn->SetOnClick([this, &outCard]()
        {
            m_bRemoveUI = true;
            m_pTalkUI->NextDialogue();
            CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/sfx_ui_trailer_select", SOUND_UI, 1.f);
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

    // 嬴檜夔虜 寡纂
    if (card.icon) {
        const float iw = w * 0.62f, ih = iw;
        const float iy = cy - h * 0.23f;
        card.icon->Set_UIPosition(cx, iy, iw, ih);
    }
}

void CUIManager::ClearAllUI()
{
    // --- 晦獄 UI ん檣攪 薑葬 ---
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

    // eunbi player effect ui delete
    Destory_PlayerEff_ALL();
    Safe_Release(m_pPlayerEffUI);
    Destory_CureEff();
    Safe_Release(m_pCureEffUI );
    Safe_Release(m_pAimUI);

    // --- 鼻薄 UI ---
    Safe_Release(m_pShopRoot);
    for (auto& card : m_shopCards) {
        Safe_Release(card.btn);
        Safe_Release(card.icon);
        Safe_Release(card.pBack);
    }
    m_shopCards.clear();
    m_shopIndices.clear();
    m_shopOpen = false;

    // --- 鼻鷓 滲熱 蟾晦�� ---
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
    CSound_Manager::GetInstance()->StopAll();
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

    // �飛� 蝶觼萼 儅撩
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

        // 豭槳
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

        // 螃艇槳
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
    // 1) 煎堅 擁棲 莖堅, 寡唳 ぬ割 �側瘓�
    if (m_pPhoneScreen)           m_pPhoneScreen->Set_Active(false);
    if (m_pPhoeScreenBackGround)  m_pPhoeScreenBackGround->Set_Active(true);

    CUIBase* parent = m_pPhoeScreenBackGround ? static_cast<CUIBase*>(m_pPhoeScreenBackGround)
        : m_pEnterUI;
    if (!parent) return;

    auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
    // 2) 豭薹 撮煎 顫檜ぎ
    if (auto* img = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer")))
    {
        img->Set_UIPosition(-380.f, -10.f, 60.f, 390.f);
        img->RegisterTexture(L"Com_Texture_Logo", L"Prototype_Component_Texture_Phone_ScreenTitleUI", 0, 1, 0.f, false);
        img->ChangeTexture(L"Com_Texture_Logo");
        parent->Add_Child(img);
    }

    // 掘衙 顫檜ぎ
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


    // 3) ア Щ溯歜
    if (auto* img2 = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer")))
    {
        img2->Set_UIPosition(-100.f, 0.f, 630.f, 420.f); // �飛� Щ溯歜
        img2->RegisterTexture(L"Com_Texture_Logo", L"Prototype_Component_Texture_Phone_FrameUI", 0, 4, 10.f, true);
        img2->ChangeTexture(L"Com_Texture_Logo");
        img2->Play(true);
        // Щ溯歜擊 譆鼻欽戲煎 螢葬堅 談戲賊:
        parent->Add_ChildFront(img2);
    }
}

void CUIManager::CreateReloadUI()
{
    if (m_pReloadUI) return; // 醞犒 寞雖
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
        pRed->SetColor(D3DCOLOR_ARGB(255, 220, 80, 80)); // �撣� 碩擎儀
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
        pGreen->SetColor(D3DCOLOR_ARGB(255, 0, 255, 0)); // 嫩擎 翱舒儀 (Lime Green)
        pGreen->EnableColorCycle(true, 0.8f); // ∠ 檜 偌羹虜 儀鼻 濠翕 滲唳
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
        txt2->SetText(L"營濰瞪");
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
    // 蝸塭檜萄 ※衛濛脾§ Ы楚斜/顫檜該 葬撢
    m_phoneSlideActive = true;
    m_phonePullArmed = false;
    m_phonePullStarted = false;
    m_phonePullTimer = 0.f;

    // 獄羹 / 槳 / 蝶觼萼 陝陝 嬪煎 蝸塭檜萄 檣
    AddSlideInY(m_pPhone, -160.f, 50.f, 600.f, 300.f, +400.f, 0.f, 1.f);
    if (m_pLeftHand)  AddSlideInY(m_pLeftHand, 190.f, 80.f, 350.f, 410.f, +600.f, 0.f, 1.f);
    if (m_pRightHand) AddSlideInY(m_pRightHand, -450.f, 80.f, 350.f, 410.f, +600.f, 0.f, 1.f);
    if (m_pPhoneScreen)
        AddSlideInY(m_pPhoneScreen, -120.f, 50.f, 410.f, 250.f, +600.f, 0.f, 0.85f);
}

void CUIManager::StartPhonePullAnim()
{
    CSound_Manager::GetInstance()->PlaySoundW(L"sfx_ui_shop_enter", SOUND_UI, 1.0f);
    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
// Щ溯歜 掘誠(center, size)  ≦ CreateClearUI()諦 翕橾п撿 л
//   Black/Hole & Frame : (-130, -70) 醞褕, 1080 x 600 觼晦
// 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    const float FRAME_CX = -130.f;
    const float FRAME_CY = -70.f;
    const float HOLE_W = 1080.f;
    const float HOLE_H = 600.f;

    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    // 蝶觼萼擊 Щ溯歜 掘誠爾棻 髦礎 濛啪 (罹寥 �捏�)
    // 綠徽擎 臢蝶籀(410x250 ? 1.64) 嶸雖
    //   - 渦 濛啪 爾檜啪 ж堅 談戲賊 RATIO蒂 雪醮(蕨: 0.70f)
    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    const float SCREEN_RATIO = 0.74f;                 // 0.80f ⊥ 0.74f 煎 還歜(測 粕 離啪)
    const float TARGET_SCR_W = SCREEN_RATIO * HOLE_W; // ? 799
    const float TARGET_SCR_H = TARGET_SCR_W * (250.f / 410.f); // ? 487

    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    // ア(漆薨) 觼晦 = 蝶觼萼 + ぬ註(謝辦/鼻ж 罹寥)
    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    const float PHONE_PAD_W = 140.f;     // 謝+辦 罹寥 м
    const float PHONE_PAD_H = 70.f;      // 鼻+ж 罹寥 м
    const float TARGET_PHN_W = TARGET_SCR_W + PHONE_PAD_W; // ? 939
    const float TARGET_PHN_H = TARGET_SCR_H + PHONE_PAD_H; // ? 557

    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    // 譆謙 醞褕 嬪纂(Y虜 嬴楚煎 頂葡)
    //   - 晦襄 centerY(-70)縑憮 +100 頂溥憮 鼻欽 HUD諦 啜纂雖 彊啪
    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    const float SHIFT_DOWN = 60.f;   // ∠ 100.f 縑憮 60.f 煎 滲唳
    const float screenEndX = FRAME_CX;
    const float screenEndY = FRAME_CY + SHIFT_DOWN;      // -70 ⊥ +30
    const float phoneEndX = screenEndX - 40.f;          // ア婁 蝶觼萼 鼻渠 螃Щ撢 X(ア檜 擒除 豭薹)
    const float phoneEndY = screenEndY + 0.f;

    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    // 槳 嬪纂/觼晦 (ア 夥梨戲煎 髦礎 釭陛啪 п憮 �飛� 陛葬雖 彊紫煙)
    //  - OUTSIDE_X : 陛濰濠葬 夤戲煎 頂爾頂朝 剪葬
    //  - HAND_Y    : 槳曖 Y (蝶觼萼爾棻 髦礎 嬴楚煎)
    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    const float HAND_W = 400.f;    // 420 ⊥ 400 (褻旎 還歜)
    const float HAND_H = 600.f;    // 630 ⊥ 600
    const float HAND_Y = screenEndY + 10.f;

    // ア 謝辦 縑雖(撫攪 ▽ ァ/2)
    const float leftEdge = phoneEndX - TARGET_PHN_W * 0.5f;
    const float rightEdge = phoneEndX + TARGET_PHN_W * 0.5f;

    // Щ煎薛お 啻檜墅: m_pLeftHand == 螃艇薹 槳, m_pRightHand == 豭薹 槳
    const float rightHandEndX = leftEdge + 60.f;   // 豭薹 槳(滲熱 RightHand)擎 豭薹 夤戲煎
    const float leftHandEndX = rightEdge + 40.f;   // 螃艇薹 槳(滲熱 LeftHand)擎 螃艇薹 夤戲煎

    // 擁棲 望檜
    const float DUR = 0.60f;
    const float DELAY = 0.00f;

    // 式式式式式 褒薯 聽 蛔煙: (⑷營 嬪纂/觼晦) ⊥ (跡ル 嬪纂/觼晦)
    // PHONE(漆薨)
    AddScaleIn(m_pPhone,
        -160.f, 50.f, 600.f, 300.f,       // ⑷營(蝸塭檜萄 ��) 高婁 蜃蹺賊 渦 濠翱蝶楝遺
        phoneEndX, phoneEndY, TARGET_PHN_W, TARGET_PHN_H,
        DELAY, DUR);

    // SCREEN(ア 頂睡)
    if (m_pPhoneScreen) {
        AddScaleIn(m_pPhoneScreen,
            -120.f, 50.f, 410.f, 250.f,
            screenEndX, screenEndY, TARGET_SCR_W, TARGET_SCR_H,
            DELAY, DUR);
    }

    // 螃艇薹 槳(滲熱 m_pLeftHand)
    if (m_pLeftHand) {
        AddScaleIn(m_pLeftHand,
            190.f, 80.f, 350.f, 410.f,
            leftHandEndX, HAND_Y, HAND_W, HAND_H,
            DELAY, DUR);
    }

    // 豭薹 槳(滲熱 m_pRightHand)
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