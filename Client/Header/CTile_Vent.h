#pragma once
#include "CTileBase.h"

namespace Engine
{
	class CColider_Sphere;
}

class CTile_Deco;

class CTile_Vent : public CTileBase
{
protected:
	explicit CTile_Vent(LPDIRECT3DDEVICE9 pGraphicDevice);
	explicit CTile_Vent(const CTile_Vent &rhs);
	virtual ~CTile_Vent();

	virtual void Free();
public:
	static CTile_Vent *Create(LPDIRECT3DDEVICE9 pGraphicDevice);
	virtual HRESULT Ready_GameObject() override;
	virtual HRESULT Initialize(void *pArg) override;
	virtual _int Update_GameObject(const _float &fTimeDelta) override;
	virtual void LateUpdate_GameObject(const _float &fTimeDelta) override;
	virtual void Render_GameObject() override;

	const CVIBuffer_Rect *GetBuffer() const { m_pBuffer; }
	const CTexture *GetTexture() const { m_pTexture; }
	TileType GetType() const { return m_eType; }
	RENDERID GetRenderId() const { return m_eRenderID; }

	void RotateProp(const _float &fTimeDelta);

	Engine::CColider_Sphere *GetCollider() { return m_pColliderSphere; }
	bool IsKilled() const { return m_bKilled; }
private:
	virtual CGameObject *Clone(void *pArg = nullptr) override;
	HRESULT			Set_Component(void *pArg);
private:
	bool m_bKilled{ false };
	Engine::CColider_Sphere *m_pColliderSphere;
	CTile_Deco *m_pProp;
	_vec3 m_vLook{};
};

