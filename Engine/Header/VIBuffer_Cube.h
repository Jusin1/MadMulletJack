#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL VIBuffer_Cube : public CVIBuffer
{
private:
	explicit VIBuffer_Cube(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit VIBuffer_Cube(const VIBuffer_Cube& rhs);
	virtual ~VIBuffer_Cube();

public:
	virtual HRESULT	Ready_Buffer();
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual CComponent* Clone(void* pArg) override;
	static VIBuffer_Cube* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free();
};

END