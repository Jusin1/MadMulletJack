#include "CFontMgr.h"

IMPLEMENT_SINGLETON(CFontMgr)

CFontMgr::CFontMgr()
{
}

CFontMgr::~CFontMgr()
{
	Free();
}

HRESULT CFontMgr::Ready_Font(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFontTag, const _tchar* pFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight)
{
	CFont* pMyFont = Find_Font(pFontTag);

	if (nullptr != pMyFont)
		return E_FAIL;

	pMyFont = CFont::Create(pGraphicDev, pFontType, iWidth, iHeight, iWeight);

	if (nullptr == pMyFont)
		return E_FAIL;

	m_mapFont.insert({ pFontTag, pMyFont });

	return S_OK;
}

void CFontMgr::Render_Font(const _tchar* pFontTag, const _tchar* pString, const _vec2* pPos, D3DXCOLOR Color)
{
	if (!pString || !pPos) return;

	const _tchar* key = (pFontTag && pFontTag[0]) ? pFontTag : L"DefaultFont";
	CFont* pFont = Find_Font(key);
	if (!pFont && lstrcmpW(key, L"DefaultFont") != 0)
		pFont = Find_Font(L"DefaultFont");
	if (!pFont) return;

	pFont->Render_Font(pString, pPos, Color);
}

bool CFontMgr::Measure(const wchar_t* fontTag, const wchar_t* text, _vec2* outSize, float wrapWidth)
{
	if (!text || !outSize) return false;

	const _tchar* key = (fontTag && fontTag[0]) ? fontTag : L"DefaultFont";
	CFont* pFont = Find_Font(key);
	if (!pFont && lstrcmpW(key, L"DefaultFont") != 0) {
		pFont = Find_Font(L"DefaultFont");
	}
	if (!pFont) return false;

	return pFont->Measure(text, outSize, wrapWidth);
}



bool CFontMgr::Render_Font_Scaled(const wchar_t* tag,
	const wchar_t* text,
	const _vec2* leftTop,
	D3DXCOLOR      color,
	float          scale)
{
	if (!text || !leftTop) return false;

	const _tchar* key = (tag && tag[0]) ? tag : L"DefaultFont";
	CFont* pFont = Find_Font(key);
	if (!pFont && lstrcmpW(key, L"DefaultFont") != 0)
		pFont = Find_Font(L"DefaultFont");
	if (!pFont) return false;

	return pFont->Render_Font_Scaled(text, leftTop, color, scale);
}

bool CFontMgr::Render_Font_ScaledCenteredRot(const wchar_t* tag, const wchar_t* text, const _vec2* center, D3DXCOLOR color, float scale, float angleDeg)
{
	if (!text || !center) return false;
	const _tchar* key = (tag && tag[0]) ? tag : L"DefaultFont";
	CFont* pFont = Find_Font(key);
	if (!pFont && lstrcmpW(key, L"DefaultFont") != 0)
		pFont = Find_Font(L"DefaultFont");
	if (!pFont) return false;

	return pFont->Render_Font_ScaledCenteredRot(text, center, color, scale, angleDeg);
}

bool CFontMgr::Measure_Scaled(const wchar_t* tag,
	const wchar_t* text,
	_vec2* out,
	float          scale)
{
	if (!out) return false;

	const _tchar* key = (tag && tag[0]) ? tag : L"DefaultFont";
	CFont* pFont = Find_Font(key);
	if (!pFont && lstrcmpW(key, L"DefaultFont") != 0)
		pFont = Find_Font(L"DefaultFont");
	if (!pFont) return false;

	_vec2 base{ 0.f, 0.f };
	if (!pFont->Measure(text, &base, 0.f)) return false;

	out->x = base.x * scale;
	out->y = base.y * scale;
	return true;
}

CFont* CFontMgr::Find_Font(const _tchar* pFontTag)
{
	auto	iter = find_if(m_mapFont.begin(), m_mapFont.end(), CTag_Finder(pFontTag));

	if (iter == m_mapFont.end())
		return nullptr;

	return iter->second;
}

void CFontMgr::Free()
{
	for_each(m_mapFont.begin(), m_mapFont.end(), CDeleteMap());
	m_mapFont.clear();
}
