#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrian : public CVIBuffer
{
private:
	explicit CVIBuffer_Terrian();
	explicit CVIBuffer_Terrian(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer_Terrian(const CVIBuffer_Terrian& rhs);
	virtual ~CVIBuffer_Terrian();

public:
	const _vec3* Get_VtxPos() { return m_pPos; }

public:
	HRESULT	Ready_Buffer(const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv);
	virtual HRESULT Initialize(void* pArg) override;

private:
	HANDLE			m_hFile;
	BITMAPFILEHEADER		m_fh;
	BITMAPINFOHEADER		m_ih;
	_vec3*					m_pPos;



public:
	virtual CComponent* Clone(void* pArg) override;
	static CVIBuffer_Terrian* Create(LPDIRECT3DDEVICE9 pGraphicDev,
		const _ulong& dwCntX = VTXCNTX,
		const _ulong& dwCntZ = VTXCNTZ,
		const _ulong& dwVtxItv = VTXITV);
	virtual void		Free();
};

END