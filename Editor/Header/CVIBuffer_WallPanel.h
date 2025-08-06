#pragma once
#include "CVIBuffer.h"
class CVIBuffer_WallPanel : public CVIBuffer
{
private:
	explicit CVIBuffer_WallPanel();
	explicit CVIBuffer_WallPanel(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer_WallPanel(const CVIBuffer_WallPanel &rhs);
	virtual ~CVIBuffer_WallPanel();

	virtual void Free() override;
public:
	HRESULT	Ready_Buffer(const _ulong &dwCntX, const _ulong &dwCntZ, const _ulong &dwVtxItv);
	virtual HRESULT Initialize(void *pArg) override;
};