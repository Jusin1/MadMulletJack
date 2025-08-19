#pragma once
#include "CVIBuffer.h"



BEGIN(Engine)

class CVIBuffer_GridPanelBase;

typedef struct tagPanelEntry
{
	WallType eType;
	_float fMin_X;
	_float fMax_X;
	_float fMin_Z;
	_float fMax_Z;
	_float fY;
	_float fInverseItv;
	_float fHalfX;
	_float fHalfZ;
	_vec3 fLocalCenter;
	CVIBuffer_GridPanelBase *pBuffer;
	_matrix matWorldInv;
	_matrix matWorld;
} PANELENTRY;

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
	_ulong GetColMax() const { return m_iColMax; }
	_ulong GetRowMax() const { return m_iRowMax; }
	PANELDATA *GetData() { return &m_tData; }
	_vec3 *GetVerticesData() { return m_pVerticesData; }
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
