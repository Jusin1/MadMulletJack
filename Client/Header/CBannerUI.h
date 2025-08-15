#pragma once
#include "CUI.h"
class CBannerUI :
    public CUI
{
public:
	explicit CBannerUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBannerUI(const CBannerUI& rhs);
	~CBannerUI() override;

public:
	HRESULT Ready_GameObject() override;
	HRESULT Initialize(void* pArg) override;
	_int	Update_GameObject(const _float& fTimeDelta) override;
	void	LateUpdate_GameObject(const _float& fTimeDelta) override;
	void	Render_GameObject() override;

public: // 배치 및 패딩
	void SetBannerRect(_float x, _float y, _float w, _float h);
	void SetPadding(_float l, _float r, _float t, _float b);

public: // 텍스쳐 설정
	void SetText(const std::wstring& s);
	void SetTextColor(D3DCOLOR c);
	void SetAccentColor(D3DCOLOR c);
	void SetFontHeight(_int px);
	void SetTextColorCycle(_bool enable, _float degPerSec = 90.f);
	void SetTextOffset(_float dx, _float dy);

public:
	// 화살표
	void SetArrowSizePx(_float px);
	void SetArrowOffset(_float dx, _float dy);

public: // 평행사변형 그리기
	enum class StripeAnchor { Top, Middle, Bottom };
	void SetStripeBarAnchor(StripeAnchor a);
	void SetStripeYOffsetPx(_float dy);

	void SetStripeSpeed(_float pxPerSec);
	void SetStripeCount(_int count);
	void SetStripeFillRatio(_float r);
	void SetStripeAngleDeg(_float d);
	void SetStripeBarHeightPx(_float px);
	void SetStripeTileAspect(_float wPerH);
	void SetStripeTileWidthPx(_float px);

public:
	static CBannerUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	CGameObject* Clone(void* pArg = nullptr) override;
	void Free() override;

private:
	HRESULT Texture_Clone();
	void EnsureFont();
	void RenderArrow();
	void RenderStripeBar();
	void RenderText();
	static D3DCOLOR HSVtoARGB(_float h, _float s, _float v, BYTE a);

private: // 텍스쳐 및 폰트
	CTexture* m_pTexArrow;
	CTexture* m_pTexStripe;
	ID3DXFont* m_pFont;

private: // 레이아웃
	_float m_x, m_y, m_w, m_h;
	_float m_padL, m_padR, m_padT, m_padB;

private: // 화갈표
	_float m_arrowSizePx;
	_float m_arrowOffX, m_arrowOffY;

private: // 텍스트
	std::wstring m_text;
	D3DCOLOR m_textColor;
	D3DCOLOR m_accentColor;
	_int m_fontHeight;
	_bool m_colorCycle;
	_float m_hueDeg, m_hueSpeed;
	_float m_textOffX, m_textOffY;

private: // 평행사변형
	StripeAnchor m_stripeAnchor;
	_float m_stripeOffY;
	_float m_scrollPx, m_speedPx;
	_int m_stripes;
	_float m_fillRatio;
	_float m_angleDeg;
	_float m_barHeightPx, m_barHeightRate;
	_float m_tileWPerH, m_tileWidthPx;
};

