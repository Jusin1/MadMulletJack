#include "pch.h"
#include "CButtonUI.h"
#include "CDInputMgr.h"

HRESULT CButtonUI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg))) return E_FAIL;

    m_baseCached = false;
    SetColorMode(CImageUI::ColorMode::TintMultiply);
    SetAdditive(false);

    if (!m_useSolid) {
        SetTintRGBA(255, 255, 255, 255); 
    }

    m_curScale = m_targetScale = 1.f;
    m_curColor = m_targetColor = m_colNormal;

    updateTextureByState();

    return S_OK;
}

_int CButtonUI::Update_GameObject(const _float& dt)
{
    if (!Is_Active() || Get_Dead()) return NO_EVENT;

    if (!m_baseCached) {
        float w, h; Get_UISize(w, h);
        if (w > 0.5f && h > 0.5f) { cacheBaseRect(); m_baseCached = true; }
    }

    updateInput();

    // 스케일 보간
    const float ks = 1.f - expf(-m_scaleLerpSpeed * dt);
    m_curScale += (m_targetScale - m_curScale) * ks;

    if (m_baseCached) {
        const float w = m_baseW * m_curScale;
        const float h = m_baseH * m_curScale;
        Set_UIPosition(m_baseX, m_baseY, w, h);
    }


    const float kc = 1.f - expf(-m_colorLerpSpeed * dt);
    m_curColor.r += (m_targetColor.r - m_curColor.r) * kc;
    m_curColor.g += (m_targetColor.g - m_curColor.g) * kc;
    m_curColor.b += (m_targetColor.b - m_curColor.b) * kc;
    m_curColor.a += (m_targetColor.a - m_curColor.a) * kc;

    if (!m_useSolid) {
        const _ubyte r = (_ubyte)std::clamp(m_curColor.r * 255.f, 0.f, 255.f);
        const _ubyte g = (_ubyte)std::clamp(m_curColor.g * 255.f, 0.f, 255.f);
        const _ubyte b = (_ubyte)std::clamp(m_curColor.b * 255.f, 0.f, 255.f);
        const _ubyte a = (_ubyte)std::clamp(m_curColor.a * 255.f, 0.f, 255.f);
        SetTintRGBA(r, g, b, a);
    }

    return __super::Update_GameObject(dt);
}

void CButtonUI::Render_GameObject()
{
    if (!Is_Active() || Get_Dead()) return;

    if (m_useSolid) {
        LPDIRECT3DDEVICE9 dev = m_pGraphicDev;
        LPDIRECT3DSTATEBLOCK9 sb = nullptr;
        if (SUCCEEDED(dev->CreateStateBlock(D3DSBT_ALL, &sb))) sb->Capture();

        dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        dev->SetTexture(0, nullptr);
        dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
        dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);

        const _ubyte r = (_ubyte)std::clamp(m_curColor.r * 255.f, 0.f, 255.f);
        const _ubyte g = (_ubyte)std::clamp(m_curColor.g * 255.f, 0.f, 255.f);
        const _ubyte b = (_ubyte)std::clamp(m_curColor.b * 255.f, 0.f, 255.f);
        const _ubyte a = (_ubyte)std::clamp(m_curColor.a * 255.f, 0.f, 255.f);
        dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(a, r, g, b));

        CUI::Render_GameObject();

        if (sb) { sb->Apply(); sb->Release(); }
    }
    else {
        CImageUI::Render_GameObject();
    }

    for (auto* ch : GetChildren())
        if (ch && ch->Is_Active() && ch->Get_RenderOn())
            ch->Render_GameObject();
}


void CButtonUI::getMousePosUI(float& x, float& y) const
{
    HWND hWnd = GetActiveWindow();
    POINT pt{ 0,0 };
    ::GetCursorPos(&pt);
    if (hWnd) ::ScreenToClient(hWnd, &pt);
    x = static_cast<float>(pt.x) - WINCX * 0.5f;
    y = static_cast<float>(pt.y) - WINCY * 0.5f;
}

void CButtonUI::updateInput()
{
    if (!m_interactable) {
        if (m_hovered) { m_hovered = false; if (m_onHoverExit) m_onHoverExit(); }
        m_pressed = false;
        setState(State::Disabled);
        return;
    }

    float mx = 0.f, my = 0.f;
    getMousePosUI(mx, my);
    const bool inside = hitTest(mx, my);

    if (inside) {
        if (!m_hovered) { m_hovered = true; if (m_onHoverEnter) m_onHoverEnter(); }
        if (m_onHoverStay) m_onHoverStay();
    }
    else {
        if (m_hovered) { m_hovered = false; if (m_onHoverExit) m_onHoverExit(); }
    }

    const bool down = CDInputMgr::GetInstance()->GetMouseButtonDown((uint8_t)MOUSEKEYSTATE::DIM_LB);
    const bool up = CDInputMgr::GetInstance()->GetMouseButtonUp((uint8_t)MOUSEKEYSTATE::DIM_LB);

    if (inside && down) m_pressed = true;

    if (up) {
        if (m_pressed && inside && m_onClick) m_onClick();
        m_pressed = false;
    }

    State next = State::Normal;
    if (m_pressed && inside) next = State::Pressed;
    else if (inside)         next = State::Hover;

    if (next != m_state) setState(next);
}

bool CButtonUI::hitTest(float mx, float my) const
{
    float cx, cy; Get_UIPosition(cx, cy);
    float w, h;   Get_UISize(w, h);
    return (fabsf(mx - cx) <= w * 0.5f) && (fabsf(my - cy) <= h * 0.5f);
}

void CButtonUI::setState(State s)
{
    if (m_state == s) return;
    m_state = s;
    updateTargetsByState();
    updateTextureByState();
}

void CButtonUI::updateTargetsByState()
{
    switch (m_state)
    {
    case State::Disabled:
        m_targetScale = 1.f;         m_targetColor = m_colDisabled; break;
    case State::Pressed:
        m_targetScale = m_pressScale; m_targetColor = m_useSolid ? m_colPressed : D3DXCOLOR(1, 1, 1, 1); break;
    case State::Hover:
        m_targetScale = m_hoverScale; m_targetColor = m_useSolid ? m_colHover : D3DXCOLOR(1, 1, 1, 1); break;
    default: // Normal
        m_targetScale = 1.f;          m_targetColor = m_useSolid ? m_colNormal : D3DXCOLOR(1, 1, 1, 1); break;
    }
}

void CButtonUI::updateTextureByState()
{
    if (m_useSolid) return;

    const std::wstring* wanted = nullptr;
    switch (m_state) {
    case State::Disabled: wanted = &m_texDisabled; break;
    case State::Pressed:  wanted = &m_texPressed;  break;
    case State::Hover:    wanted = &m_texHover;    break;
    default:              wanted = &m_texNormal;   break;
    }
    if (!wanted || wanted->empty()) return;       
    if (m_curTexTag == *wanted) return;              

    ChangeTexture(wanted->c_str());
    m_curTexTag = *wanted;
}

CButtonUI* CButtonUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CButtonUI* p = new CButtonUI(pGraphicDev);
    if (FAILED(p->Ready_GameObject())) {
        MSG_BOX("CButtonUI Create Failed");
        Safe_Release(p);
    }
    return p;
}

CGameObject* CButtonUI::Clone(void* pArg)
{
    CButtonUI* p = new CButtonUI(*this);
    if (FAILED(p->Initialize(pArg))) {
        MSG_BOX("CButtonUI Clone Failed");
        Safe_Release(p);
    }
    return p;
}

void CButtonUI::Free()
{
    CImageUI::Free();
}