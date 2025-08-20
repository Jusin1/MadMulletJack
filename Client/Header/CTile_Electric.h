#pragma once
#include "CTileBase.h"

namespace Engine
{
	class CColider_Sphere;
}

class CTile_Electric : public CTileBase
{
protected:
	explicit CTile_Electric(LPDIRECT3DDEVICE9 pGraphicDevice);
	explicit CTile_Electric(const CTile_Electric &rhs);
	virtual ~CTile_Electric();

	virtual void Free();
public:
	static CTile_Electric *Create(LPDIRECT3DDEVICE9 pGraphicDevice);
	virtual HRESULT Ready_GameObject() override;
	virtual HRESULT Initialize(void *pArg) override;
	virtual _int Update_GameObject(const _float &fTimeDelta) override;
	virtual void LateUpdate_GameObject(const _float &fTimeDelta) override;
	virtual void Render_GameObject() override;

	Engine::CColider_Sphere *GetCollider() { return m_pColliderSphere; }
	bool IsActivated() const { return m_bActivated; }
private:
	virtual CGameObject *Clone(void *pArg = nullptr) override;
	HRESULT			Set_Component(void *pArg);
private:
	bool m_bActivated{ false };
	Engine::CColider_Sphere *m_pColliderSphere;
};

