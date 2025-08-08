#pragma once
#include "CMonster.h"
class CMonster_Suit : public CMonster
{
private:
	explicit CMonster_Suit(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMonster_Suit(const CMonster_Suit& rhs);
	virtual ~CMonster_Suit();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg) override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	virtual HRESULT Texture_Clone() override;
	virtual _bool Picking(_vec3* PickingPoint) override;
	void Set_Collider();

public:
	static CMonster_Suit* Create(LPDIRECT3DDEVICE9 pGrahpicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

