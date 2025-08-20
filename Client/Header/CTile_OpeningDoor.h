#pragma once
#include "CTileBase.h"

class CTile_Deco;

class CTile_OpeningDoor : public CTileBase
{
private:
	explicit CTile_OpeningDoor(LPDIRECT3DDEVICE9 pGraphicDevice);
	explicit CTile_OpeningDoor(const CTile_OpeningDoor &rhs);
	virtual ~CTile_OpeningDoor();

	virtual void Free() override;
	virtual CGameObject *Clone(void *pArg = nullptr) override;
public:
	static CTile_OpeningDoor *Create(LPDIRECT3DDEVICE9 pGraphicDevice);
	virtual HRESULT Ready_GameObject() override;
	virtual HRESULT Initialize(void *pArg) override;
	virtual _int Update_GameObject(const _float &fTimeDelta) override;
	virtual void LateUpdate_GameObject(const _float &fTimeDelta) override;
	virtual void Render_GameObject() override;
private:
	HRESULT Set_Component(void *pArg);
private:

};

