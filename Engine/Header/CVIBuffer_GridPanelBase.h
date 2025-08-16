#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_GridPanelBase : public CVIBuffer
{
protected:
	explicit CVIBuffer_GridPanelBase(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer_GridPanelBase(const CVIBuffer_GridPanelBase &rhs);
	virtual ~CVIBuffer_GridPanelBase();

	virtual void Free();
	virtual HRESULT Initialize(void *pArg) override;
public:
	virtual _bool Picking(CTransform *pTransform, _vec3 *pOut = nullptr) override;
protected:
	virtual HRESULT	Ready_Buffer(void *pArg);
	virtual HRESULT Set_Buffer();
	virtual _bool IntersectRay(_vec3 *pOut);
protected:
	_ulong m_iColMax{ 0 };
	_ulong m_iRowMax{ 0 };
	_vec3 *m_pVerticesData{ nullptr };
	PANELDATA m_tData;
};

END
