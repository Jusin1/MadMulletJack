#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CCubeTex : public CVIBuffer
{
private:
	explicit CCubeTex();
	explicit CCubeTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCubeTex(const CCubeTex& rhs);
	virtual ~CCubeTex();

public:
	virtual HRESULT	Ready_Buffer();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Render_Buffer();

public:
	virtual CComponent* Clone(void* pArg) override;
	static CCubeTex* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

END