#include "CFont.h"

CFont::CFont(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev), m_pFont(nullptr), m_pSprite(nullptr)
{
	m_pGraphicDev->AddRef();
}

CFont::~CFont()
{
}

HRESULT CFont::Ready_Font(const _tchar* pFontType,
	const _uint& iWidth,
	const _uint& iHeight,
	const _uint& iWeight)
{
	D3DXFONT_DESC fd{};

	fd.CharSet = DEFAULT_CHARSET;     
	fd.OutputPrecision = OUT_TT_PRECIS;
	fd.Quality = ANTIALIASED_QUALITY;
	fd.PitchAndFamily = FF_DONTCARE;


	fd.Height = -static_cast<INT>(iHeight > 0 ? iHeight : 32);

	fd.Width = 0;
	fd.Weight = (iWeight == 0 ? FW_DONTCARE : iWeight);
	fd.Italic = FALSE;


	lstrcpynW(fd.FaceName, pFontType, LF_FACESIZE);

	if (FAILED(D3DXCreateFontIndirect(m_pGraphicDev, &fd, &m_pFont))) {
		MSG_BOX("Font Create Failed");
		return E_FAIL;
	}
	if (FAILED(D3DXCreateSprite(m_pGraphicDev, &m_pSprite))) {
		MSG_BOX("Sprite Create Failed");
		return E_FAIL;
	}


	D3DXFONT_DESC got{};
	if (SUCCEEDED(m_pFont->GetDesc(&got))) {
		wchar_t buf[256];
		swprintf(buf, 256, L"[Font] Face='%s' Height=%d Weight=%d Italic=%d\n",
			got.FaceName, got.Height, got.Weight, got.Italic);
		OutputDebugStringW(buf);
	}
	return S_OK;
}

void CFont::Render_Font(const _tchar* pString, const _vec2* pPos, D3DXCOLOR Color)
{
	RECT rc{ (LONG)pPos->x, (LONG)pPos->y, 4096, 4096 };

	if (EnsureSprite()) {
		m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
		m_pFont->DrawTextW(m_pSprite, pString, -1, &rc, DT_LEFT | DT_TOP | DT_NOCLIP, Color);
		m_pSprite->End();
	}
	else {
		m_pFont->DrawTextW(nullptr, pString, -1, &rc, DT_LEFT | DT_TOP | DT_NOCLIP, Color);
	}
}

bool CFont::Render_Font_Scaled(const wchar_t* text, const _vec2* leftTop, D3DXCOLOR color, float scale)
{
	if (!m_pFont || !m_pSprite || !text || !leftTop || scale <= 0.f) return false;

	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	D3DXMATRIX prev;
	m_pSprite->GetTransform(&prev);

	D3DXMATRIX S, Tfix, M;
	D3DXMatrixScaling(&S, scale, scale, 1.f);
	D3DXMatrixTranslation(&Tfix, leftTop->x * (1.f - scale), leftTop->y * (1.f - scale), 0.f);
	M = S * Tfix;
	m_pSprite->SetTransform(&M);

	RECT rc{ (LONG)leftTop->x, (LONG)leftTop->y, 4096, 4096 };
	m_pFont->DrawTextW(m_pSprite, text, -1, &rc, DT_LEFT | DT_NOCLIP, color);
	m_pSprite->SetTransform(&prev);
	m_pSprite->End();
	return true;
}

bool CFont::EnsureSprite()
{
	if (m_pSprite) return true;
	return SUCCEEDED(D3DXCreateSprite(m_pGraphicDev, &m_pSprite));
}

bool CFont::Render_Font_ScaledCenteredRot(const wchar_t* text, const _vec2* center, D3DXCOLOR color, float scale, float angleDeg)
{
	if (!m_pFont || !text || !center || scale <= 0.f) return false;
	if (!EnsureSprite()) {
		Render_Font(text, center, color);
		return false;
	}

	const float cx = floorf(center->x + 0.5f);
	const float cy = floorf(center->y + 0.5f);

	_vec2 base{ 0.f, 0.f };
	Measure(text, &base, 0.f);

	RECT rc{};
	rc.left = static_cast<LONG>(cx - base.x * 0.5f);
	rc.top = static_cast<LONG>(cy - base.y * 0.5f);
	rc.right = 4096;
	rc.bottom = 4096;

	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	D3DXMATRIX prev, M;
	m_pSprite->GetTransform(&prev);

	D3DXVECTOR2 c(cx, cy);
	D3DXVECTOR2 sc(scale, scale);
	float rad = D3DXToRadian(angleDeg);
	D3DXMatrixTransformation2D(&M, &c, 0.f, &sc, &c, rad, nullptr);
	m_pSprite->SetTransform(&M);

	m_pFont->DrawTextW(m_pSprite, text, -1, &rc, DT_LEFT | DT_NOCLIP, color);

	m_pSprite->SetTransform(&prev);
	m_pSprite->End();
	return true;
}

CFont* CFont::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight)
{
	CFont* pInstance = new CFont(pGraphicDev);

	if (FAILED(pInstance->Ready_Font(pFontType, iWidth, iHeight, iWeight)))
	{
		Safe_Release(pInstance);
		MSG_BOX("폰트 문제");
		return nullptr;
	}

	return pInstance;
}

bool CFont::Measure(const wchar_t* text, _vec2* outSize, float wrapWidth)
{
	if (!m_pFont || !text || !outSize) return false;

	RECT rc{ 0, 0, (wrapWidth > 0.f) ? (LONG)wrapWidth : 0, 0 };
	DWORD fmt = DT_LEFT | DT_TOP | DT_CALCRECT;
	if (wrapWidth > 0.f) fmt |= DT_WORDBREAK;

	m_pFont->DrawTextW(nullptr, text, -1, &rc, fmt, 0);
	outSize->x = (float)(rc.right - rc.left);
	outSize->y = (float)(rc.bottom - rc.top);
	return true;

}

void CFont::Free()
{
	Safe_Release(m_pFont);
	Safe_Release(m_pSprite);

	Safe_Release(m_pGraphicDev);
}
