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
public:
	const PANELDATA *Get_Data() { return &m_tData; }
	void Set_Data(void *pData) { ::memcpy(&m_tData, pData, sizeof(PANELDATA)); }
private:
	HRESULT	Ready_Buffer(void *pArg);
	HRESULT Ready_HorizonWallBuffer();
	HRESULT Ready_VerticalWallBuffer();
	HRESULT Ready_PlaneBuffer();

private:
	PANELDATA m_tData;
};

END