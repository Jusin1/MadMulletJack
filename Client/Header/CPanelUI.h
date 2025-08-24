#pragma once
#include "CUI.h"
class CPanelUI :
    public CUI
{
private:
    explicit CPanelUI(LPDIRECT3DDEVICE9 dev);
    explicit CPanelUI(const CPanelUI& rhs);
    virtual ~CPanelUI();

public:
    // CGameObject
    HRESULT Ready_GameObject() override;
    HRESULT Initialize(void* pArg) override;
    _int    Update_GameObject(const _float& dt) override;
    void    LateUpdate_GameObject(const _float& dt) override;
    void    Render_GameObject() override;

public: 
    void SetStyle(const _tchar* baseTag, D3DCOLOR baseTint, bool baseAdditive = false);
    void SetOverlay(const _tchar* overlayTag, D3DCOLOR overlayTint,
        bool overlayAdditive = true, float overlayScale = 1.f);

    void SetPanelSize(float w, float h);
    void SetPanelPos(float x, float y);

    void SetUVRepeat(float baseRepeatX, float baseRepeatY);
    void SetOverlayUVRepeat(float ovRepeatX, float ovRepeatY);

    void EnableStroke(float thicknessPx, D3DCOLOR color);

    // ÇÁ¸®¼Â
    void UseGreenFramePreset(float x, float y, float w, float h,
        float borderOrThickness = 24.f, bool useStroke = true);

    void UseRedFramePreset(float x, float y, float w, float h,
        float borderOrThickness = 24.f, bool useStroke = true);

    // È­¸é ÇÈ¼¿ ½º³À
    void SetPixelSnap(bool b) { m_pixelSnap = b; }

private:
    HRESULT Texture_Clone();

    // ·»´õ
    void RenderLayer(const _tchar* tag, D3DCOLOR tint, bool additive,
        float scaleMul, float repX, float repY);

    void RenderStrokeOnly();
    void DrawSolid(float cx, float cy, float w, float h, D3DCOLOR c);

private:
    map<const _tchar*, CTexture*> m_mapTextures;

    const _tchar* m_BaseTag;
    const _tchar* m_OverlayTag;
    D3DCOLOR m_BaseTint;
    D3DCOLOR m_OverlayTint;
    bool m_BaseAdd;
    bool m_OverlayAdd;
    float m_OverlayScale;
    float m_BaseRepeatX, m_BaseRepeatY;
    float m_OvRepeatX, m_OvRepeatY;

    bool m_useStroke;
    float m_strokePx;
    D3DCOLOR m_strokeCol;
    bool m_pixelSnap;

public:
    static CPanelUI* Create(LPDIRECT3DDEVICE9 dev);
    CGameObject* Clone(void* pArg = nullptr) override;
    void Free() override;
};

