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

    // ===== 수명주기 =====
    HRESULT Initialize(void* pArg) override;
    _int    Update_GameObject(const _float& dt) override;
    void    Render_GameObject() override;

    // ===== 설정 =====
    void Set_ButtonRect(float cx, float cy, float w, float h) {
        Set_UIPosition(cx, cy, w, h);
        cacheBaseRect(); m_baseCached = true;
    }

    // 단색/텍스처 겸용
    void SetSolidMode(bool on);

    void SetSolidColors(D3DXCOLOR normal, D3DXCOLOR hover, D3DXCOLOR pressed, D3DXCOLOR disabled) {
        m_colNormal = normal; m_colHover = hover; m_colPressed = pressed; m_colDisabled = disabled;
        updateTargetsByState(); updateTextureByState();
    }

    // 상태별 텍스처 태그
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
        case State::Normal:   m_texNormal = tag;  break;
        case State::Hover:    m_texHover = tag;   break;
        case State::Pressed:  m_texPressed = tag; break;
        case State::Disabled: m_texDisabled = tag; break;
        }
        updateTextureByState();
    }

    // 텍스처 모드 상태별 틴트 설정 (이걸 호출한 버튼만 색 적용)
    void SetTextureTints(D3DXCOLOR normal, D3DXCOLOR hover, D3DXCOLOR pressed, D3DXCOLOR disabled) {
        m_texTintNormal = normal;
        m_texTintHover = hover;
        m_texTintPressed = pressed;
        m_texTintDisabled = disabled;
        m_hasCustomTints = true;
        m_targetTint = m_curTint = m_texTintNormal;
    }
    void SetTextureTint(State s, D3DXCOLOR c) {
        m_hasCustomTints = true;
        switch (s) {
        case State::Normal:   m_texTintNormal = c;   break;
        case State::Hover:    m_texTintHover = c;    break;
        case State::Pressed:  m_texTintPressed = c;  break;
        case State::Disabled: m_texTintDisabled = c; break;
        }
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

    // 프레임당 1회 외부 설정하는 마우스 UI좌표 (모든 버튼 공유)
    static void SetFrameMouseUIPos(float x, float y) { s_mouseX = x; s_mouseY = y; s_mouseValid = true; }
    static void InvalidateMouseUIPos() { s_mouseValid = false; }

public:
    static CButtonUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    CGameObject* Clone(void* pArg = nullptr) override;
    void Free() override;

private:
    // 입력/상태
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

    private:
        // 텍스처 모드용 상태별 틴트 (기본=흰색 → 텍스처 원본색 유지)
        D3DXCOLOR  m_texTintNormal = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
        D3DXCOLOR  m_texTintHover = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
        D3DXCOLOR  m_texTintPressed = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
        D3DXCOLOR  m_texTintDisabled = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

        // 이 버튼이 커스텀 틴트를 갖는지?
        bool       m_hasCustomTints = false;

        D3DXCOLOR  m_curTint = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
        D3DXCOLOR  m_targetTint = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

    // 프레임 공유 마우스 좌표
    static inline float s_mouseX = 0.f, s_mouseY = 0.f;
    static inline bool  s_mouseValid = false;
};

