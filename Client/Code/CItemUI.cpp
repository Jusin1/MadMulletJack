#include "pch.h"
#include "CItemUI.h"

static inline float clamp01(float v) { return v < 0.f ? 0.f : (v > 1.f ? 1.f : v); }

CItemUI::CItemUI(LPDIRECT3DDEVICE9 dev)
    : CImageUI(dev)
    , m_bAppearing(false)
    , m_fAppearTime(0.f)
    , m_fAppearDuration(0.35f)
    , m_startW(0.f), m_startH(0.f)
    , m_targetW(0.f), m_targetH(0.f)
    , m_targetX(0.f), m_targetY(0.f)
    , m_bBlink(false)
    , m_blinkArmed(false)
    , m_blinkPeriod(1.0f)
    , m_blinkOnRatio(0.5f)
    , m_blinkT(0.f)
    , m_blinkAlphaMode(true)
    , m_blinkAlphaOn(255)
    , m_blinkAlphaOff(0)
{
}

CItemUI::CItemUI(const CItemUI& rhs)
    : CImageUI(rhs)
    
    , m_bAppearing(false)
    , m_fAppearTime(0.f)
    , m_fAppearDuration(rhs.m_fAppearDuration)
    , m_startW(0.f), m_startH(0.f)
    , m_targetW(0.f), m_targetH(0.f)
    , m_targetX(0.f), m_targetY(0.f)
    
    , m_bBlink(false)
    , m_blinkArmed(false)
    , m_blinkPeriod(rhs.m_blinkPeriod)
    , m_blinkOnRatio(rhs.m_blinkOnRatio)
    , m_blinkT(0.f)
    , m_blinkAlphaMode(rhs.m_blinkAlphaMode)
    , m_blinkAlphaOn(rhs.m_blinkAlphaOn)
    , m_blinkAlphaOff(rhs.m_blinkAlphaOff)
{
}

CItemUI::~CItemUI() {}

HRESULT CItemUI::Ready_GameObject() { return __super::Ready_GameObject(); }
HRESULT CItemUI::Initialize(void* pArg) { return __super::Initialize(pArg); }
void    CItemUI::LateUpdate_GameObject(const _float& dt) { __super::LateUpdate_GameObject(dt); }
void    CItemUI::Render_GameObject() { __super::Render_GameObject(); }

void CItemUI::PlayAppear(float x, float y, float w, float h, float duration)
{
    m_targetX = x;  m_targetY = y;
    m_targetW = w;  m_targetH = h;

    
    m_startW = w * 20.f;
    m_startH = h * 20.f;
    Set_UIPosition(x, y, m_startW, m_startH);

    
    m_fAppearTime = 0.f;
    m_fAppearDuration = duration > 0.f ? duration : 0.01f;
    m_bAppearing = true;


    Set_RenderOn(true);
    SetAlpha(255);

    if (m_bBlink) {
        m_bBlink = false;
        m_blinkArmed = true;
        m_blinkT = 0.f;
    }
}

_int CItemUI::Update_GameObject(const _float& dt)
{
    bool justFinishedAppear = false;
    if (m_bAppearing)
    {
        m_fAppearTime += dt;
        float t = m_fAppearTime / m_fAppearDuration;
        if (t >= 1.f) { t = 1.f; m_bAppearing = false; justFinishedAppear = true; }

        float inv = 1.f - t;
        float scale = 1.f - (inv * inv * inv);

        float curW = m_startW + (m_targetW - m_startW) * scale;
        float curH = m_startH + (m_targetH - m_startH) * scale;

        Set_UIPosition(m_targetX, m_targetY, curW, curH);
    }

    if (justFinishedAppear && m_blinkArmed)
    {
        m_blinkArmed = false;
        m_bBlink = true;
        m_blinkT = 0.f;

        Set_RenderOn(true);
        if (m_blinkAlphaMode) SetAlpha(m_blinkAlphaOn);
    }

    if (!m_bAppearing && m_bBlink)
    {
        m_blinkT += dt;
        const float period = (m_blinkPeriod > 0.0001f ? m_blinkPeriod : 0.05f);
        float T = fmodf(m_blinkT, period);
        float onWindow = clamp01(m_blinkOnRatio) * period;
        bool on = (T < onWindow);

        if (m_blinkAlphaMode) {
            if (on) {
                Set_RenderOn(true);
                SetAlpha(m_blinkAlphaOn);
            }
            else {
                Set_RenderOn(false);
            }
        }
        else {
            Set_RenderOn(on);
        }
    }

    return __super::Update_GameObject(dt);
}

void CItemUI::StartBlink(float periodSec, float onRatio,
    bool alphaMode, BYTE onAlpha, BYTE offAlpha)
{
    m_blinkPeriod = (periodSec > 0.05f ? periodSec : 0.05f);
    m_blinkOnRatio = onRatio;
    m_blinkAlphaMode = alphaMode;
    m_blinkAlphaOn = onAlpha;
    m_blinkAlphaOff = offAlpha;
    m_blinkT = 0.f;

    if (m_bAppearing) {
        m_blinkArmed = true;
        m_bBlink = false;
        Set_RenderOn(true);
        SetAlpha(255);
    }
    else {
        m_blinkArmed = false;
        m_bBlink = true;
        Set_RenderOn(true);
        if (m_blinkAlphaMode) SetAlpha(m_blinkAlphaOn);
    }
}

void CItemUI::StopBlink()
{
    m_blinkArmed = false;
    m_bBlink = false;
    Set_RenderOn(true);
    SetAlpha(255);
}

CItemUI* CItemUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CItemUI* p = new CItemUI(pGraphicDev);
    if (FAILED(p->Ready_GameObject())) {
        MSG_BOX("CButtonUI Create Failed");
        Safe_Release(p);
    }
    return p;
}

CGameObject* CItemUI::Clone(void* pArg)
{
    CItemUI* p = new CItemUI(*this);
    if (FAILED(p->Initialize(pArg))) {
        MSG_BOX("CButtonUI Clone Failed");
        Safe_Release(p);
    }
    return p;
}

void CItemUI::Free()
{
    CImageUI::Free();
}