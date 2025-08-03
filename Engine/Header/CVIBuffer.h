#pragma once
#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer :  public CComponent
{
protected:
	explicit CVIBuffer();
	explicit CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer();

public:
	virtual HRESULT			Ready_Buffer();
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Render_Buffer();


protected:
	LPDIRECT3DVERTEXBUFFER9		m_pVB;
	_ulong						m_dwVtxSize;
	_ulong						m_dwVtxCnt;
	_ulong						m_dwTriCnt;
	_ulong						m_dwFVF;

	LPDIRECT3DINDEXBUFFER9		m_pIB;
	_ulong						m_dwIdxSize;
	D3DFORMAT					m_IdxFmt;

protected:
	HRESULT Ready_Vertex_Buffer();
	HRESULT Ready_Index_Buffer();

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void	Free() override;

};

END
