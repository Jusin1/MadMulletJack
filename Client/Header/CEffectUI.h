#pragma once
#include "CUI.h"

class CEffectUI :
    public CUI
{
private:
    explicit CEffectUI(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CEffectUI(const CEffectUI& rhs);
    virtual ~CEffectUI();

public:
    HRESULT Ready_GameObject() override;
    HRESULT Initialize(void* pArg) override;
    _int    Update_GameObject(const _float& fTimeDelta) override;
    void    LateUpdate_GameObject(const _float& fTimeDelta) override;
    void    Render_GameObject() override;

public:
    void Show(const wchar_t* text,
        const wchar_t* texTag,
        float seconds,
        float centerX = WINCX * 0.5f,
        float centerY = WINCY * 0.33f,
        float angleDeg = 0.f,
        float bgAlpha = 0.85f,
        const wchar_t* fontTag = L"Font_UI_ROUGH",
        D3DXCOLOR fontColor = D3DXCOLOR(1, 1, 1, 1));


    void ShowBanner(const wchar_t* text,
        float seconds,
        float x, float y,
        float scaleStart = 1.80f,
        float scaleEnd = 1.00f,
        const wchar_t* fontTag = L"Font_UI_ROUGH",
        D3DXCOLOR fontColor = D3DXCOLOR(1, 1, 1, 1),
        float bgAlpha = 0.85f,
        float angleDeg = -10.f);


    void ShowFollowTransform(const wchar_t* text,
        const wchar_t* texTag,
        float seconds,
        CTransform* pAnchorTr,
        float worldYOffset = 0.90f,
        float risePixels = 160.f,
        float followSpeed = 60.f,
        float bgAlpha = 0.85f,
        const wchar_t* fontTag = L"Font_UI_ROUGH",
        D3DXCOLOR fontColor = D3DXCOLOR(1, 1, 1, 1));

    void Hide();


    void SetImageSize(float w, float h) { m_iconW = w; m_iconH = h; }
    void SetImageOffset(float ox, float oy) { m_iconOff.x = ox; m_iconOff.y = oy; }
    void SetBoxSize(float w, float h = 30.f) { m_boxW = w; m_boxH = h; }
    void SetMoveSpeed(float pxPerSec, bool computeLifeBySpeed = false) { m_moveSpeed = pxPerSec; m_useSpeed = computeLifeBySpeed; }
    void SetTargetBounds(float leftX, float rightX) { m_targetLeftX = leftX; m_targetRightX = rightX; }
    void SetBannerExtraWidth(float w) { m_bannerExtraW = w; }
    void SetBannerAngle(float deg) { m_bannerAngleDeg = deg; if (m_linkTextAngleToBanner) m_textAngleDeg = deg; }
    HRESULT Change_Texture(const _tchar* tag);

    void SetNumberEmphasis(const wchar_t* digits, float scale);

    void SetBannerRightText(const wchar_t* s) { m_RightText = s ? s : L""; }
    void SetBannerTextAngle(float deg) { m_textAngleDeg = deg; m_linkTextAngleToBanner = false; }
    void LinkBannerTextAngleToBanner(bool link) { m_linkTextAngleToBanner = link; if (link) m_textAngleDeg = m_bannerAngleDeg; }
    void SetBannerTextOffset(float dx, float dy) { m_textOffX = dx; m_textOffY = dy; }
    void SetBannerDownSpeed(float v) { m_bannerDownSpeed = v; }
    void SetBannerShowIcon(bool on) { m_bannerShowIcon = on; }

    void SetBannerLabelPop(float startScale = 1.35f, float duration = 0.22f)
    {
        m_labelPopStart = (startScale > 0.f ? startScale : 1.f); m_labelPopDur = max(0.01f, duration); m_labelPopEnabled = true;
    }
    void DisableBannerLabelPop() { m_labelPopEnabled = false; }

    void SetBannerRightFixedScale(float s) { m_rightFixedScale = (s > 0.f ? s : 1.f); m_rightUseFixedScale = true; }

    static CEffectUI* GetMain() { return s_pMain; }

private:
    enum MODE { FLOAT_MOVE, BANNER_FIXED, SCREEN_FOLLOW };
    MODE   m_mode = FLOAT_MOVE;

    bool   m_bVisible = false;
    float  m_time = 0.f;
    float  m_totalLife = 0.f;
    float  m_fRemain = 0.f;
    float  m_cx = WINCX * 0.5f, m_cy = WINCY * 0.33f;


    float  m_startX = 0.f, m_startY = 0.f;
    float  m_endX = 0.f, m_endY = 0.f;

    float  m_progress = 0.f;
    float  m_scaleStart = 0.70f, m_scaleEnd = 1.00f;

    int    m_dirX = -1;
    float  m_liftMin = 120.f, m_liftMax = 220.f;
    float  m_targetLeftX = 100.f, m_targetRightX = 1200.f;
    float  m_moveSpeed = 0.f;
    bool   m_useSpeed = false;

    float  m_bgAlpha = 0.85f;

    float  m_boxW = 100.f, m_boxH = 30.f;
    float  m_padL = 10.f, m_gap = 6.f;

    std::wstring m_Text;
    std::wstring m_FontTag;
    D3DXCOLOR    m_FontColor = D3DXCOLOR(1, 1, 1, 1);

    float  m_iconW = 42.f, m_iconH = 42.f;
    _vec2  m_iconOff = { 0.f, 0.f };
    bool   m_hasIcon = false;
    float  m_fadeOutDur = 0.35f;
    float  m_blinkSpeed = 18.f;

    float  m_bannerExtraW = 120.f;
    float  m_bannerAngleDeg = -10.f;

    VIBuffer_Color* m_pBgBufferCom = nullptr;
    std::map<const _tchar*, CTexture*> m_mapTextures;

    static CEffectUI* s_pMain;

    CTransform* m_pAnchorTr = nullptr;
    _vec3       m_lastWorld = { 0,0,0 };
    float       m_worldYOffset = 0.90f;
    float       m_risePixels = 160.f;
    float       m_followSpeed = 60.f;

    float       m_screenStartY = WINCY * 0.33f;


    std::wstring m_emDigits = L"";
    float        m_emScale = 1.35f;

    std::wstring m_RightText;     
    float  m_textAngleDeg = +8.f; 
    float  m_textOffX = 0.f, m_textOffY = 0.f;
    float  m_bannerDownSpeed = 32.f; 
    float  m_bannerLifeAdd = 0.40f;  
    bool   m_bannerShowIcon = true;  

    bool   m_labelPopEnabled = true;
    float  m_labelPopStart = 1.35f; 
    float  m_labelPopDur = 0.22f;

    float  m_rightFixedScale = 1.00f;
    bool   m_rightUseFixedScale = true;

    bool   m_linkTextAngleToBanner = true;

private:
    HRESULT PreloadTexture(const _tchar* mapTag, const _tchar* protoTag);
    void RenderBox(float cx, float cy, float w, float h, float alpha, float angleDeg);
    void RenderIcon(float cx, float cy, float aMul, float scale);
    void RenderText(float leftX, float centerY, float aMul);

    bool ProjectWorldToScreen(const _vec3& world, float& outSX, float& outSY) const;

    static float EaseExpoOut(float t) {
        return (t >= 1.f) ? 1.f : (t <= 0.f) ? 0.f : (1.f - powf(2.f, -10.f * t));
    }

public:
    static CEffectUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    CGameObject* Clone(void* pArg = nullptr) override;
    void        Free() override;
};

