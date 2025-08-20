#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CColider_Cube;
}

class CDummyBase : public CGameObject
{
protected:
	explicit CDummyBase(LPDIRECT3DDEVICE9 pGraphicDevice, EnvType _e);
	explicit CDummyBase(const CDummyBase &rhs, EnvType _e);
	virtual ~CDummyBase();

	virtual void Free();
public:
	virtual HRESULT Ready_GameObject() override;
	virtual HRESULT Initialize(void *pArg) override;
	virtual _int Update_GameObject(const _float &fTimeDelta) override;
	virtual void LateUpdate_GameObject(const _float &fTimeDelta) override;
	virtual void Render_GameObject() override;

	Engine::CColider_Cube*GetCollider() { m_pCollider; }
	EnvType GetType() const { return m_eType; }
	RENDERID GetRenderId() const { return m_eRenderID; }
	ObjectCategory GetCategory() const { return m_eCategory; }
private:
	virtual CGameObject *Clone(void *pArg = nullptr) override;
	HRESULT			Set_Component(void *pArg);
protected:
	ObjectCategory m_eCategory;
	EnvType m_eType;
	RENDERID m_eRenderID;
	Engine::CColider_Cube *m_pCollider;
};

