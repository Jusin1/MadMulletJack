#pragma once

#include "CBase.h"
#include "Engine_Define.h"
#include "CFont.h"

BEGIN(Engine)

class ENGINE_DLL CFontMgr : public CBase
{
	DECLARE_SINGLETON(CFontMgr)
	struct PrivateFontFile {
		std::wstring path; 
	};
	struct PrivateFontMem {
		HANDLE handle = nullptr;        
		std::vector<BYTE> buffer;    
		DWORD numFaces = 0;        
	};

private:
	explicit CFontMgr();
	virtual ~CFontMgr();

public:
	HRESULT				Ready_Font(LPDIRECT3DDEVICE9 pGraphicDev,
		const _tchar* pFontTag,
		const _tchar* pFontType,
		const _uint& iWidth,
		const _uint& iHeight,
		const _uint& iWeight);

	void				Render_Font(const _tchar* pFontTag,
		const _tchar* pString,
		const _vec2* pPos,
		D3DXCOLOR Color);

	bool Measure(const wchar_t* fontTag, const wchar_t* text, _vec2* outSize, float wrapWidth = 0.f);
	bool Render_Font_Scaled(const wchar_t* tag, const wchar_t* text, const _vec2* leftTop, D3DXCOLOR color, float scale);
	bool Render_Font_ScaledCenteredRot(const wchar_t* tag, const wchar_t* text, const _vec2* center, D3DXCOLOR color, float scale, float angleDeg);
	bool Measure_Scaled(const wchar_t* tag, const wchar_t* text, _vec2* out, float scale);

public:
	// 파일에서 폰트 등록
	bool RegisterPrivateFontFromFile(const wchar_t* ttfPath);

	// 리소스에서 폰트 등록
	bool RegisterPrivateFontFromResource(HINSTANCE hInst, LPCWSTR resId, LPCWSTR resType);

private:
	CFont* Find_Font(const _tchar* pFontTag);

private:
	map<const _tchar*, CFont*>			m_mapFont;
	vector<PrivateFontFile> m_fontFiles;
	vector<PrivateFontMem>  m_fontMems;

private:
	virtual void	Free();
};

END