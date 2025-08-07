#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL VIBuffer_Color : public CVIBuffer
{
private:
	explicit VIBuffer_Color(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit VIBuffer_Color(const VIBuffer_Color& rhs);
	virtual ~VIBuffer_Color();

public:
	virtual HRESULT	Ready_Buffer();
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual CComponent* Clone(void* pArg) override;
	static  VIBuffer_Color* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free();
};

END
