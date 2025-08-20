#pragma once
#include "CTileBase.h"
class CTile_Glass : public CTileBase
{
protected:
	explicit CTile_Glass(LPDIRECT3DDEVICE9 pGraphicDevice);
	explicit CTile_Glass(const CTile_Glass &rhs);
	virtual ~CTile_Glass();

	virtual void Free();
	virtual CGameObject *Clone(void *pArg = nullptr) override;
public:
	static CTile_Glass *Create(LPDIRECT3DDEVICE9 pGraphicDevice);
	virtual HRESULT Ready_GameObject() override;
	virtual HRESULT Initialize(void *pArg) override;
	virtual _int Update_GameObject(const _float &fTimeDelta) override;
	virtual void LateUpdate_GameObject(const _float &fTimeDelta) override;
	virtual void Render_GameObject() override;

	virtual _bool Picking(_vec3 *PickingPoint) override;
	virtual void PickingTrue() override;
private:
	HRESULT			Set_Component(void *pArg);
};

