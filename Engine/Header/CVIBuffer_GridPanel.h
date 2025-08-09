#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_GridPanel : public CVIBuffer
{
private:
	explicit CVIBuffer_GridPanel();
	explicit CVIBuffer_GridPanel(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer_GridPanel(const CVIBuffer_GridPanel &rhs);
	virtual ~CVIBuffer_GridPanel();

	virtual void		Free();
	virtual HRESULT Initialize(void *pArg) override;
public:
	virtual CComponent *Clone(void *pArg) override;
	static CVIBuffer_GridPanel *Create(LPDIRECT3DDEVICE9 pGraphicDev, void *pArg = nullptr);
	virtual _bool Picking(class CTransform *pTransform, _vec3 *pOut = nullptr) override;
public:
	const PANELDATA *Get_Data() { return &m_tData; }
	void Set_Data(void *pData) { ::memcpy(&m_tData, pData, sizeof(PANELDATA)); }

	void Increase_RowBuffer();
	void Increase_ColBuffer();
	void Decrease_RowBuffer();
	void Decrease_ColBuffer();
private:
	HRESULT	Ready_Buffer(void *pArg);
	HRESULT Ready_HorizonWallBuffer();
	HRESULT Ready_VerticalWallBuffer();
	HRESULT Ready_PlaneBuffer();
	HRESULT Create_VertexBuffer();
	HRESULT Create_IndexBuffer();
	_bool IntersectRayWithPlane(_vec3 *pOut);

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