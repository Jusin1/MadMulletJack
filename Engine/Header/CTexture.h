#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CTexture : public CComponent
{
public:
	typedef struct tagTextureInfo
	{
		int m_iStart = 0;
		int m_iCurrentTex = 0;
		int m_iEndTex = 0;
		_float m_fSpeed = 1;
	}TEXINFO;
protected:
	explicit CTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture();

public:
	HRESULT	Ready_Texture(TEXTUREID eType, const _tchar* pPath, const _uint& iCnt);
	HRESULT Initialize(void* pArg)override;
	void	Set_Texture(const _uint& iIndex = 0);
	IDirect3DBaseTexture9 *Get_Texture(const _uint &iIndex = 0) { return m_vecTexture[iIndex]; }
public:
	void MoveFrame(const _tchar* timeTag);
	void Set_Frame(int iStart, int iEnd, int iSpeed);
	void Set_Zero_Frame() { m_TextureInfo.m_iCurrentTex = 0; }
	TEXINFO& Get_Frame() { return m_TextureInfo; }

private:
	_uint								m_iNumTextures = 0;
	vector<IDirect3DBaseTexture9*>		m_vecTexture;
	TEXINFO								m_TextureInfo;
	_float m_fTimeAcc = 0.0f;
	

public:
	virtual CComponent* Clone(void* pArg);
	static CTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev,
		TEXTUREID eType,
		const _tchar* pPath, 
		const _uint& iCnt = 1);

private:
	virtual void	Free();

};

END