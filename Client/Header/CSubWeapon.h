#pragma once
#include "CWeapon.h"
class CSubWeapon : public CWeapon
{
protected:
	explicit CSubWeapon(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSubWeapon(const CSubWeapon& rhs);
	virtual ~CSubWeapon();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg)override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	virtual void Free() override;
};

