#include "pch.h"
#include "CTutorialUI.h"
#include "CTutorialTracker.h"
#include "CObjectManager.h"
#include "CTextUI.h"
#include "CImageUI.h"

static inline float clamp01(float x) { return x < 0.f ? 0.f : (x > 1.f ? 1.f : x); }
static inline float easeOutCubic(float t) { t = clamp01(t); float u = 1.f - t; return 1.f - u * u * u; }

CTutorialUI::CTutorialUI(LPDIRECT3DDEVICE9 gd)
    : CUI(gd)
    , m_textMain(nullptr)
    , m_textEmph(nullptr)
    , m_icon(nullptr)
    , m_curStep(-1)
    , m_targetMainX(0.f), m_targetMainY(0.f)
    , m_targetEmphX(0.f), m_targetEmphY(0.f)
    , m_targetIconX(0.f), m_targetIconY(0.f)
    , m_iconW(700.f), m_iconH(150.f)
    , m_emphW(100.f), m_emphH(40.f)
    , m_mainW(100.f), m_mainH(40.f)
    , m_sliding(false)
    , m_slideT(1.f)
    , m_slideDuration(0.45f)
    , m_slideStartOffset(600.f)
    , m_autoHide(false)
    , m_autoHideTimer(0.f)
    , m_autoHideDelay(2.f)
{
}

CTutorialUI::CTutorialUI(const CTutorialUI& rhs)
    : CUI(rhs)
    , m_textMain(nullptr)  
    , m_textEmph(nullptr)
    , m_icon(nullptr)
    , m_curStep(rhs.m_curStep)
    , m_targetMainX(rhs.m_targetMainX), m_targetMainY(rhs.m_targetMainY)
    , m_targetEmphX(rhs.m_targetEmphX), m_targetEmphY(rhs.m_targetEmphY)
    , m_targetIconX(rhs.m_targetIconX), m_targetIconY(rhs.m_targetIconY)
    , m_iconW(rhs.m_iconW), m_iconH(rhs.m_iconH)
    , m_emphW(rhs.m_emphW), m_emphH(rhs.m_emphH)
    , m_mainW(rhs.m_mainW), m_mainH(rhs.m_mainH)
    , m_sliding(false)
    , m_slideT(1.f)
    , m_slideDuration(rhs.m_slideDuration)
    , m_slideStartOffset(rhs.m_slideStartOffset)
    , m_autoHide(false)
    , m_autoHideTimer(0.f)
    , m_autoHideDelay(rhs.m_autoHideDelay)
{
}

HRESULT CTutorialUI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg))) return E_FAIL;

    m_fSizeX = 700.f; m_fSizeY = 60.f;
    m_fX = 500.f;     m_fY = -160.f;
    m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_Info(INFO_POS, _vec3(m_fX, -m_fY, 0.f));
    m_bRenderOn = true; Set_Active(true);

    m_targetIconX = 500.f;          m_targetIconY = m_fY - 90.f;
    m_targetEmphX = 380.f;          m_targetEmphY = m_fY + 430.f;
    m_targetMainX = 540.f;          m_targetMainY = m_fY + 430.f;

    m_icon = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", SCENE_TUTORIAL, L"UI_Layer"));
    if (m_icon) {
        m_icon->Set_UIPosition(m_targetIconX, m_targetIconY, m_iconW, m_iconH);
        m_icon->RegisterTexture(L"Com_Tut_Move", L"Prototype_Component_Texture_Tut_Move");
        m_icon->RegisterTexture(L"Com_Tut_Fire", L"Prototype_Component_Texture_Tut_Shot");
        m_icon->RegisterTexture(L"Com_Tut_Jump", L"Prototype_Component_Texture_Tut_Jump");
        m_icon->RegisterTexture(L"Com_Tut_Dash", L"Prototype_Component_Texture_Tut_Dash");
        m_icon->RegisterTexture(L"Com_Tut_Door", L"Prototype_Component_Texture_Tut_Door");
        m_icon->RegisterTexture(L"Com_Tut_Finish", L"Prototype_Component_Texture_Tut_Finish");
        m_icon->RegisterTexture(L"Com_Tut_Soda", L"Prototype_Component_Texture_Tut_Soda");
        Add_Child(m_icon);
    }

    m_textMain = dynamic_cast<CTextUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_TextUI", SCENE_TUTORIAL, L"UI_Layer"));
    if (m_textMain) {
        m_textMain->SetFontTag(L"Font_UI_Bold");
        m_textMain->SetCentered(true);
        m_textMain->SetScale(0.6f);
        m_textMain->Set_UIPosition(m_targetMainX, m_targetMainY, m_mainW, m_mainH);
        Add_Child(m_textMain);
    }

    m_textEmph = dynamic_cast<CTextUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_TextUI", SCENE_TUTORIAL, L"UI_Layer"));
    if (m_textEmph) {
        m_textEmph->SetFontTag(L"Font_UI_Bold");
        m_textEmph->SetCentered(true);
        m_textEmph->SetScale(0.6f);
        m_textEmph->SetColor(D3DXCOLOR(1.f, 0.9f, 0.25f, 1.f));
        m_textEmph->Set_UIPosition(m_targetEmphX, m_targetEmphY, m_emphW, m_emphH);
        Add_Child(m_textEmph);
    }

    // 첫 표시
    const int first = CTutorialTracker::Get().FirstPendingIndex();
    m_curStep = first;
    if (first < 0) {
        m_bRenderOn = false; Set_Active(false);
    }
    else {
        applyLayoutForStep(first);   
        applyStepText(first);      
        applyStepImage(first);   
        beginSlideIn();            
    }
    return S_OK;
}

_int CTutorialUI::Update_GameObject(const _float& dt)
{
    __super::Update_GameObject(dt);

    const int pending = CTutorialTracker::Get().FirstPendingIndex();
    if (pending < 0) {
        if (m_bRenderOn) { m_bRenderOn = false; Set_Active(false); }
        return NO_EVENT;
    }

    if (pending != m_curStep) {
        m_curStep = pending;
        applyLayoutForStep(m_curStep);
        applyStepText(m_curStep);
        applyStepImage(m_curStep);
        beginSlideIn();
    }

    updateSlide(dt);

    if (m_autoHide) {
        m_autoHideTimer += dt;
        if (m_autoHideTimer >= m_autoHideDelay) {
            Set_ActiveRecursive(false);
            m_autoHide = false;       
        }
    }

    return NO_EVENT;
}

void CTutorialUI::Render_GameObject()
{
    for (auto* ch : GetChildren())
        if (ch && ch->Is_Active() && ch->Get_RenderOn())
            ch->Render_GameObject();
}

void CTutorialUI::applyStepText(int s)
{
    const wchar_t* mainMsg = L"";
    const wchar_t* emphMsg = L"";

    switch (static_cast<TStep>(s)) {
    case TStep::Move:   emphMsg = L"[WASD]";         mainMsg = L"를 사용해 이동합니다.";                 break;
    case TStep::Fire:   emphMsg = L"[마우스좌클릭]"; mainMsg = L"          을 사용해 발사합니다.";       break;
    case TStep::Jump:   emphMsg = L"[Space]";        mainMsg = L"를 사용해 점프합니다.";                 break;
    case TStep::Dash:   emphMsg = L"[Shift]";        mainMsg = L"를 사용해 대시합니다.";                 break;
    case TStep::Door:   emphMsg = L"노란색 문";      mainMsg = L"을 따라가세요";                         break;
    case TStep::Finish: emphMsg = L"아이템";         mainMsg = L"을 획득하여 적을 처형하세요.";          break;
    case TStep::Soda:   emphMsg = L"자판기";         mainMsg = L"를 이용하여 체력을 회복하세요.";         break;
    default: break;
    }

    if (m_textMain) m_textMain->SetText(mainMsg);
    if (m_textEmph) m_textEmph->SetText(emphMsg);
}

/* ====== 단계별 아이콘 텍스처 ====== */
void CTutorialUI::applyStepImage(int s)
{
    if (!m_icon) return;

    const wchar_t* tag = L"";
    switch (static_cast<TStep>(s)) {
    case TStep::Move:   tag = L"Com_Tut_Move";   break;
    case TStep::Fire:   tag = L"Com_Tut_Fire";   break;
    case TStep::Jump:   tag = L"Com_Tut_Jump";   break;
    case TStep::Dash:   tag = L"Com_Tut_Dash";   break;
    case TStep::Door:   tag = L"Com_Tut_Door";   break;
    case TStep::Finish: tag = L"Com_Tut_Finish"; break;
    case TStep::Soda:   tag = L"Com_Tut_Soda";   break;
    default: return;
    }
    m_icon->ChangeTexture(tag);
    m_icon->Play(false);
}

/* ====== 단계별 레이아웃(크기/위치) ======
   - 기본값 유지, Finish / Soda만 별도 */
void CTutorialUI::applyLayoutForStep(int s)
{
    // 기본 레이아웃
    m_iconW = 700.f;  m_iconH = 150.f;
    m_emphW = 100.f;  m_emphH = 40.f;
    m_mainW = 100.f;  m_mainH = 40.f;

    m_targetIconX = 500.f;        m_targetIconY = m_fY - 90.f;
    m_targetEmphX = 380.f;        m_targetEmphY = m_fY + 430.f;
    m_targetMainX = 540.f;        m_targetMainY = m_fY + 430.f;

    // 자동숨김 초기화(기본 off)
    m_autoHide = false;
    m_autoHideTimer = 0.f;

    switch (static_cast<TStep>(s)) {
    case TStep::Finish:
        // 큰 배너 느낌
        m_iconW = 600.f;  m_iconH = 300.f;
        m_targetIconX = 0.f;       m_targetIconY = m_fY - 100.f;

        m_emphW = 120.f; m_mainW = 220.f;
        m_targetEmphX = -120.f;    m_targetEmphY = m_fY + 320.f;
        m_targetMainX = 60.f;     m_targetMainY = m_fY + 320.f;

        m_autoHide = true;    
        break;

    case TStep::Soda:
        // 더 크게
        m_iconW = 600.f;  m_iconH = 300.f;
        m_targetIconX = 0.f;       m_targetIconY = m_fY - 100.f;
        Set_ActiveRecursive(true);
        m_emphW = 120.f; m_mainW = 220.f;
        m_targetEmphX = -120.f;    m_targetEmphY = m_fY + 320.f;
        m_targetMainX = 60.f;     m_targetMainY = m_fY + 320.f;

        m_autoHide = true;     
        break;

    default:
        break;
    }

    // 즉시 적용
    if (m_icon)     m_icon->Set_UIPosition(m_targetIconX, m_targetIconY, m_iconW, m_iconH);
    if (m_textEmph) m_textEmph->Set_UIPosition(m_targetEmphX, m_targetEmphY, m_emphW, m_emphH);
    if (m_textMain) m_textMain->Set_UIPosition(m_targetMainX, m_targetMainY, m_mainW, m_mainH);
}

/* ====== 슬라이드 ====== */
void CTutorialUI::beginSlideIn()
{
    m_sliding = true;
    m_slideT = 0.f;

    const float off = m_slideStartOffset;
    if (m_icon)     m_icon->Set_UIPosition(m_targetIconX + off, m_targetIconY, m_iconW, m_iconH);
    if (m_textEmph) m_textEmph->Set_UIPosition(m_targetEmphX + off, m_targetEmphY, m_emphW, m_emphH);
    if (m_textMain) m_textMain->Set_UIPosition(m_targetMainX + off, m_targetMainY, m_mainW, m_mainH);

    // 자동숨김 타이머 초기화(표시 시작 시점)
    m_autoHideTimer = 0.f;
}

void CTutorialUI::updateSlide(const _float& dt)
{
    if (m_sliding) {
        m_slideT += dt / m_slideDuration;
        if (m_slideT >= 1.f) { m_slideT = 1.f; m_sliding = false; }
    }
    const float t = easeOutCubic(m_slideT);
    const float off = (1.f - t) * m_slideStartOffset;

    if (m_icon)     m_icon->Set_UIPosition(m_targetIconX + off, m_targetIconY, m_iconW, m_iconH);
    if (m_textEmph) m_textEmph->Set_UIPosition(m_targetEmphX + off, m_targetEmphY, m_emphW, m_emphH);
    if (m_textMain) m_textMain->Set_UIPosition(m_targetMainX + off, m_targetMainY, m_mainW, m_mainH);
}

/* factory */
CTutorialUI* CTutorialUI::Create(LPDIRECT3DDEVICE9 gd)
{
    auto* p = new CTutorialUI(gd);
    if (FAILED(p->Ready_GameObject())) { MSG_BOX("CTutorialUI Create Failed"); Safe_Release(p); }
    return p;
}
CGameObject* CTutorialUI::Clone(void* pArg)
{
    auto* p = new CTutorialUI(*this);
    if (FAILED(p->Initialize(pArg))) { MSG_BOX("CTutorialUI Clone Failed"); Safe_Release(p); }
    return p;
}