#pragma once
#include "CVIBuffer_GridPanelBase.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_GridPanel_Horizon : public CVIBuffer_GridPanelBase
{
private:
	explicit CVIBuffer_GridPanel_Horizon(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer_GridPanel_Horizon(const CVIBuffer_GridPanel_Horizon &rhs);
	virtual ~CVIBuffer_GridPanel_Horizon();

	virtual void Free();
	virtual HRESULT Initialize(void *pArg) override;
public:
	static CVIBuffer_GridPanel_Horizon *Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent *Clone(void *pArg) override;
	virtual _bool Picking(CTransform *pTransform, _vec3 *pOut = nullptr) override;
private:
	HRESULT	Ready_Buffer(void *pArg);
	HRESULT Set_Buffer();
	_bool IntersectRay(_vec3 *pOut);
};

END

