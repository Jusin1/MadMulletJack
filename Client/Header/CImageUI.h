#pragma once
#include "CUI.h"
class CImageUI :
    public CUI
{
public:
    enum class ColorMode {
        TintMultiply,  // Texture * Tint (기본, 가장 무난)
        TintAdd,       // Texture + Tint (색을 더해서 강조)
        TextureOnly,   // Texture만 (틴트 무시)
        TintOnly       // Tint만(단색 사각처럼 사용, 알파 포함)
    };

protected:
    explicit CImageUI(LPDIRECT3DDEVICE9 dev);
    explicit CImageUI(const CImageUI& rhs);
    virtual ~CImageUI();

public: // CGameObject
    HRESULT Ready_GameObject() override;
    HRESULT Initialize(void* pArg) override;
    _int    Update_GameObject(const _float& dt) override;
    void    LateUpdate_GameObject(const _float& dt) override;
    void    Render_GameObject() override;
    void    Free() override;

public:
    static CImageUI* Create(LPDIRECT3DDEVICE9 dev);
    CGameObject* Clone(void* pArg = nullptr) override;

public: 
    HRESULT RegisterTexture(const _tchar* tag, const _tchar* proto,
        int start = 0, int end = 0, float fps = 0.f, bool loop = false);
    HRESULT ChangeTexture(const _tchar* tag);

public: 
    CTexture* GetTextureCom() const { return m_pTextureCom; }
    std::wstring GetCurTag() const { return m_curTag; }

public: 
    void   SetTint(D3DCOLOR tint);
    void   SetTintRGBA(BYTE r, BYTE g, BYTE b, BYTE a = 255); 
    void   SetAlpha(BYTE a);                             
    void   SetAdditive(bool on);
    void   SetUVRepeat(float rx, float ry);
    void   SetColorMode(ColorMode m);     
    
    void Set_IsPosFix(_bool _bPosFix) { m_bPosFix = _bPosFix; }

public: // 애니메이션
    void   Play(bool on);
    void   ResetToFirstFrame();


protected:
    D3DCOLOR      m_tint = D3DCOLOR_ARGB(255, 255, 255, 255);
    bool          m_additive = false;  // 블렌딩(출력) 단계
    float         m_repX = 1.f, m_repY = 1.f;
    bool          m_play = true;
    ColorMode     m_colorMode = ColorMode::TintMultiply;

    std::wstring  m_curTag;

    // 달라진 pos로 그리기 위한 bool
    _bool m_bPosFix;
};

