#pragma once
#include "CDummyBase.h"
class CEndPoint : public CDummyBase
{
private:
	explicit CEndPoint(LPDIRECT3DDEVICE9 pGraphicDevice);
	explicit CEndPoint(const CEndPoint &rhs);
	virtual ~CEndPoint();

	virtual void Free() override;
public:
	static CEndPoint *Create(LPDIRECT3DDEVICE9 pGraphicDevice);
	virtual CGameObject *Clone(void *pArg = nullptr) override;

	virtual HRESULT Ready_GameObject();
	virtual HRESULT Initialize(void *pArg) override;
	virtual _int    Update_GameObject(const _float &fTimeDelta);
	virtual void    LateUpdate_GameObject(const _float &fTimeDelta);
	virtual void    Render_GameObject();
private:
	void			Update_Collider();
	HRESULT			Set_Component();
};

