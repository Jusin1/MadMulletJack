#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_GridPanel_Editor : public CVIBuffer
{
private:
	explicit CVIBuffer_GridPanel_Editor();
	explicit CVIBuffer_GridPanel_Editor(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer_GridPanel_Editor(const CVIBuffer_GridPanel_Editor &rhs);
	virtual ~CVIBuffer_GridPanel_Editor();

	virtual void		Free();
	virtual HRESULT Initialize(void *pArg) override;
public:
	virtual CComponent *Clone(void *pArg) override;
	static CVIBuffer_GridPanel_Editor *Create(LPDIRECT3DDEVICE9 pGraphicDev, void *pArg = nullptr);
	virtual _bool Picking(class CTransform *pTransform, _vec3 *pOut = nullptr) override;
public:
	const PANELDATA *Get_Data() { return &m_tData; }
	void Set_Data(void *pData) { ::memcpy(&m_tData, pData, sizeof(PANELDATA)); }

	void Increase_RowBuffer();
	void Increase_ColBuffer();
	void Increase_Interval();
	void Decrease_RowBuffer();
	void Decrease_ColBuffer();
	void Decrease_Interval();
private:
	HRESULT	Ready_Buffer(void *pArg);
	HRESULT Ready_HorizonWallBuffer();
	HRESULT Ready_VerticalWallBuffer();
	HRESULT Ready_PlaneBuffer();
	HRESULT Create_VertexBuffer();
	HRESULT Create_IndexBuffer();
	_bool IntersectRayWithPlane(_vec3 *pOut);
	_bool IntersectRayWithPlaneForEditor(_vec3 *pOut);

	HRESULT Set_Buffer(_ulong iRowMax, _ulong iColMax);

	void Update_BufferInfo();
	void Update_BufferInfo_Horizon();
	void Update_BufferInfo_Vertical();
	void Update_BufferInfo_Plane();

	void Update_Buffer(_ulong iRowMax, _ulong iColMax);
private:
	PANELDATA m_tData;
	_vec3 *m_pVerticesData;
};

END