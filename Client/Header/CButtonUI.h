#pragma once
#include "CImageUI.h"
class CTextUI;
class CButtonUI :
    public CImageUI
{
public:
    enum class State { Normal = 0, Hover, Pressed, Disabled };

public:
    explicit CButtonUI(LPDIRECT3DDEVICE9 gd) : CImageUI(gd) {}
    CButtonUI(const CButtonUI& rhs) : CImageUI(rhs) {}
    virtual ~CButtonUI() {}


    HRESULT Initialize(void* pArg) override;
    _int    Update_GameObject(const _float& dt) override;
    void    Render_GameObject() override;


    void Set_ButtonRect(float cx, float cy, float w, float h) {
        Set_UIPosition(cx, cy, w, h);
        cacheBaseRect(); m_baseCached = true;
    }


    void SetSolidMode(bool on) {
        m_useSolid = on;
        if (m_useSolid) {
            m_colNormal = D3DXCOLOR(0.f, 0.f, 0.f, 0.85f);
            m_colHover = D3DXCOLOR(1.f, 1.f, 1.f, 0.90f);
            m_colPressed = D3DXCOLOR(1.f, 1.f, 1.f, 0.95f);
            m_colDisabled = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.70f);
        }
        else {
            m_colNormal = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
            m_colHover = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
            m_colPressed = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
            m_colDisabled = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.f);
        }
        updateTargetsByState(); 
        updateTextureByState(); 
    }

    void SetSolidColors(D3DXCOLOR normal, D3DXCOLOR hover, D3DXCOLOR pressed, D3DXCOLOR disabled) {
        m_colNormal = normal; m_colHover = hover; m_colPressed = pressed; m_colDisabled = disabled;
        updateTargetsByState(); updateTextureByState();
    }


    void SetStateTextures(const wchar_t* normal,
        const wchar_t* hover = L"",
        const wchar_t* pressed = L"",
        const wchar_t* disabled = L"")
    {
        m_texNormal = normal ? normal : L"";
        m_texHover = hover ? hover : L"";
        m_texPressed = pressed ? pressed : L"";
        m_texDisabled = disabled ? disabled : L"";
        updateTextureByState(); 
    }

    void SetStateTextureTag(State s, const std::wstring& tag) {
        switch (s) {
        case State::Normal:   m_texNormal = tag; break;
        case State::Hover:    m_texHover = tag; break;
        case State::Pressed:  m_texPressed = tag; break;
        case State::Disabled: m_texDisabled = tag; break;
        }
        updateTextureByState();
    }

    void SetInteractable(bool on) { m_interactable = on; updateTargetsByState(); updateTextureByState(); }
    bool IsInteractable() const { return m_interactable; }

    void SetHoverScale(float s) { m_hoverScale = s; }
    void SetPressScale(float s) { m_pressScale = s; }
    void SetLerpSpeeds(float scaleSpeed, float colorSpeed) { m_scaleLerpSpeed = scaleSpeed; m_colorLerpSpeed = colorSpeed; }

    void SetOnClick(std::function<void()> cb) { m_onClick = std::move(cb); }
    void SetOnHoverEnter(std::function<void()> cb) { m_onHoverEnter = std::move(cb); }
    void SetOnHoverExit(std::function<void()> cb) { m_onHoverExit = std::move(cb); }
    void SetOnHoverStay(std::function<void()> cb) { m_onHoverStay = std::move(cb); }

public:
    static CButtonUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    CGameObject* Clone(void* pArg = nullptr) override;
    void Free() override;

private:
    void updateInput();
    bool hitTest(float mx, float my) const;
    void setState(State s);
    void updateTargetsByState();
    void updateTextureByState(); 

    void cacheBaseRect() { Get_UIPosition(m_baseX, m_baseY); Get_UISize(m_baseW, m_baseH); }
    void getMousePosUI(float& x, float& y) const;

private:
    State m_state = State::Normal;
    bool  m_interactable = true;
    bool  m_hovered = false;
    bool  m_pressed = false;

    bool  m_baseCached = false;
    float m_baseX = 0.f, m_baseY = 0.f, m_baseW = 0.f, m_baseH = 0.f;

    float m_curScale = 1.f, m_targetScale = 1.f;
    float m_hoverScale = 1.06f, m_pressScale = 1.02f;
    float m_scaleLerpSpeed = 12.f;


    bool       m_useSolid = false; 
    D3DXCOLOR  m_colNormal = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
    D3DXCOLOR  m_colHover = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
    D3DXCOLOR  m_colPressed = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
    D3DXCOLOR  m_colDisabled = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.f);
    D3DXCOLOR  m_curColor = m_colNormal;
    D3DXCOLOR  m_targetColor = m_colNormal;
    float      m_colorLerpSpeed = 10.f;

    std::wstring m_texNormal, m_texHover, m_texPressed, m_texDisabled;
    std::wstring m_curTexTag; 

    std::function<void()> m_onClick;
    std::function<void()> m_onHoverEnter;
    std::function<void()> m_onHoverExit;
    std::function<void()> m_onHoverStay;
};

