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
    // CGameObject
    virtual HRESULT Ready_GameObject() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual _int    Update_GameObject(const _float& fTimeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void    Render_GameObject() override;

public: 
    void Show(const wchar_t* text,
        const wchar_t* texTag,
        float seconds,
        float centerX = WINCX * 0.5f,
        float centerY = WINCY * 0.33f,
        float /*angleDeg*/ = 0.f,
        float bgAlpha = 0.85f,
        const wchar_t* fontTag = L"DefaultFont",
        D3DXCOLOR fontColor = D3DXCOLOR(1, 1, 1, 1));

    void ShowBanner(const wchar_t* text,
        float seconds,
        float x, float y,
        float scaleStart = 1.80f,             
        float scaleEnd = 1.00f,              
        const wchar_t* fontTag = L"DefaultFont",
        D3DXCOLOR fontColor = D3DXCOLOR(1, 1, 1, 1),
        float bgAlpha = 0.85f,
        float angleDeg = -10.f);

    void Hide();

    // 옵션
    void SetImageSize(float w, float h) { m_iconW = w; m_iconH = h; }
    void SetImageOffset(float ox, float oy) { m_iconOff.x = ox; m_iconOff.y = oy; }
    void SetBoxSize(float w, float h = 30.f) { m_boxW = w; m_boxH = h; }

    // 플로팅 전용
    void SetMoveSpeed(float pxPerSec, bool computeLifeBySpeed = false) { m_moveSpeed = pxPerSec; m_useSpeed = computeLifeBySpeed; }
    void SetTargetBounds(float leftX, float rightX) { m_targetLeftX = leftX; m_targetRightX = rightX; }

    // 배너 전용
    void SetBannerExtraWidth(float w) { m_bannerExtraW = w; }   // 텍스트폭 + 여유폭
    void SetBannerAngle(float deg) { m_bannerAngleDeg = deg; }

    HRESULT Change_Texture(const _tchar* tag);
    static CEffectUI* GetMain() { return s_pMain; }

private:
    enum MODE { FLOAT_MOVE, BANNER_FIXED };
    MODE   m_mode;

    // 상태/타이밍
    bool   m_bVisible;
    float  m_time;
    float  m_totalLife;
    float  m_fRemain;

    // 위치
    float  m_cx, m_cy;
    float  m_startX, m_startY;
    float  m_endX, m_endY;

    // 진행/스케일(글자 전용)
    float  m_progress;
    float  m_scaleStart;  
    float  m_scaleEnd;   

    // 플로팅 이동
    int    m_dirX;
    float  m_liftMin, m_liftMax;
    float  m_targetLeftX, m_targetRightX;
    float  m_moveSpeed;
    bool   m_useSpeed;

    // 스타일
    float  m_bgAlpha;

    // 박스/패딩(고정 크기)
    float  m_boxW, m_boxH;
    float  m_padL, m_gap;

    // 텍스트
    std::wstring m_Text;
    std::wstring m_FontTag;
    D3DXCOLOR    m_FontColor;

    // 아이콘(고정 크기)
    float  m_iconW, m_iconH;
    _vec2  m_iconOff;
    bool   m_hasIcon;

    // 페이드/깜빡임
    float  m_fadeOutDur;
    float  m_blinkSpeed;

    // 배너 
    float  m_bannerExtraW;
    float  m_bannerAngleDeg;   

    VIBuffer_Color* m_pBgBufferCom;
    std::map<const _tchar*, CTexture*> m_mapTextures;

    static CEffectUI* s_pMain;

private:
    HRESULT PreloadTexture(const _tchar* mapTag, const _tchar* protoTag);

    void    RenderBox(float cx, float cy, float w, float h, float alpha, float angleDeg);
    void    RenderIcon(float cx, float cy, float aMul, float scale);
    void    RenderText(float leftX, float centerY, float aMul);

    static float EaseExpoOut(float t) { return (t >= 1.f) ? 1.f : (t <= 0.f) ? 0.f : (1.f - powf(2.f, -10.f * t)); }

public:
    static CEffectUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone(void* pArg = nullptr) override;
    virtual void        Free() override;
};

