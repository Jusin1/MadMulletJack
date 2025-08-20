#pragma once
#include "CTileBase.h"

namespace Engine
{
	class CColider_Sphere;
}

class CTile_VendingMachine : public CTileBase
{
protected:
	explicit CTile_VendingMachine(LPDIRECT3DDEVICE9 pGraphicDevice);
	explicit CTile_VendingMachine(const CTile_VendingMachine &rhs);
	virtual ~CTile_VendingMachine();

	virtual void Free();
public:
	static CTile_VendingMachine *Create(LPDIRECT3DDEVICE9 pGraphicDevice);
	virtual HRESULT Ready_GameObject() override;
	virtual HRESULT Initialize(void *pArg) override;
	virtual _int Update_GameObject(const _float &fTimeDelta) override;
	virtual void LateUpdate_GameObject(const _float &fTimeDelta) override;
	virtual void Render_GameObject() override;

	Engine::CColider_Sphere *GetCollider() { return m_pColliderSphere; }
	bool IsDestroyed() const { return m_bDestroyed; }
private:
	virtual CGameObject *Clone(void *pArg = nullptr) override;
	HRESULT			Set_Component(void *pArg);
private:
	bool m_bDestroyed{ false };
	Engine::CColider_Sphere *m_pColliderSphere;
};

