#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CTexture : public CComponent
{
protected:
	explicit CTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture();

public:
	HRESULT	Ready_Texture(const _tchar* pPath, TEXTUREID eType, const _uint& iCnt);
	void	Set_Texture(const _uint& iIndex = 0);

private:
	vector<IDirect3DBaseTexture9*>		m_vecTexture;

public:
	virtual CComponent* Clone();
	static CTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev,
		const _tchar* pPath, 
		TEXTUREID eType,
		const _uint& iCnt = 1);

private:
	virtual void	Free();

};

END