#pragma once
#include "CComponent.h"
BEGIN(Engine)
class ENGINE_DLL CColider_Sphere : public CComponent
{
public:
	struct COLLINFO
	{
		_float fRadius = 1.f;
		_vec3 vOffset = { 0.f, 0.f, 0.f };
	};

public:
	explicit CColider_Sphere(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CColider_Sphere(const CColider_Sphere& rhs);
	virtual ~CColider_Sphere() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg) override;
	HRESULT Update_ColliderSphere(); 
	HRESULT Render_ColliderSphere();
	_bool	Collision_Check(CColider_Sphere* pTarget, _vec3* pOutDistance = nullptr);

public:
	void Set_Transform(class CTransform* pTransform);
	void Set_IsInverse(_bool bIsInverse) { m_bIsInverse = bIsInverse; }
	void Set_Active(_bool bActive) { m_bActive = bActive; }
	_bool Is_Active() const { return m_bActive; }

	COLLINFO Get_SphereDesc() const { return m_SphereDesc; }

protected:
	COLLINFO m_SphereDesc;
	class CTransform* m_pTransform = nullptr;

	_vec3 m_vCenter = { 0.f, 0.f, 0.f };
	_float m_fRadius = 1.f;
	_float m_fBaseRadius = 1.f;
	_bool m_bIsInverse = false;
	_bool m_bActive = true;

protected:
#ifdef _DEBUG
	LPD3DXMESH m_pSphereMesh = nullptr;
#endif
	LPDIRECT3DDEVICE9 m_pGraphicDev = nullptr;

public:
	static CColider_Sphere* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END;

