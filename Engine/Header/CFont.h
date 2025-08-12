#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CFont : public CBase
{
	explicit CFont(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFont();

public:
	HRESULT				Ready_Font(const _tchar* pFontType,
									const _uint& iWidth,
									const _uint& iHeight,
									const _uint& iWeight);

	void				Render_Font(const _tchar* pString, const _vec2* pPos, D3DXCOLOR Color);
	bool Render_Font_Scaled(const wchar_t* text, const _vec2* leftTop, D3DXCOLOR color, float scale);
	bool EnsureSprite(); 
	bool Render_Font_ScaledCenteredRot(const wchar_t* text, const _vec2* center, D3DXCOLOR color, float scale, float angleDeg);

private:
	LPDIRECT3DDEVICE9			m_pGraphicDev;
	LPD3DXSPRITE				m_pSprite;	
	LPD3DXFONT					m_pFont;	

public:
	static CFont* Create(LPDIRECT3DDEVICE9 pGraphicDev,
		const _tchar* pFontType,
		const _uint& iWidth,
		const _uint& iHeight,
		const _uint& iWeight);
	bool Measure(const wchar_t* text, _vec2* outSize, float wrapWidth = 0.f);

private:
	virtual void	Free();
};
END

