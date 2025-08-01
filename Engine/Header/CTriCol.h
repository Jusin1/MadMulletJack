#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTriCol :   public CVIBuffer
{
private:
	explicit CTriCol();
	explicit CTriCol(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTriCol(const CTriCol& rhs);
	virtual ~CTriCol();

public:
	virtual HRESULT	Ready_Buffer();
	virtual void	Render_Buffer();

public:
	virtual CComponent* Clone(void* pArg) override;
	static CTriCol* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

END