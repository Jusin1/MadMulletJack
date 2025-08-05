#pragma once
#include "CComponent.h"
BEGIN(Engine)
class ENGINE_DLL CColider_Sphere : public CComponent
{
public:
	typedef struct tagSphereDesc
	{
		_float fRadius = 1.f;
		_vec3 vOffset = { 0.f, 0.f, 0.f };
		_matrix StateMatrix;
	}COLLINFO;

public:
	explicit CColider_Sphere(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CColider_Sphere(const CColider_Sphere& rhs);
	virtual ~CColider_Sphere() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg) override;
	HRESULT Update_ColliderSphere(const _matrix& WorldMatrix);
	HRESULT Render_ColliderSphere();
	_bool	Collision_Check(CColider_Sphere* pTarget, _vec3* pOutDistance = nullptr);

public:
	COLLINFO Get_SphereDesc() { return m_SphereDesc; }
	void Set_Position(class CTransform* pTransform) {}
	void Set_IsInverse(_bool bIsInverse) { m_bIsInverse = bIsInverse; }

protected:
	COLLINFO m_SphereDesc;
	_vec3 m_vCenter = { 0.f, 0.f, 0.f };
	_float m_fRadius = 1.f;
	_vec3 m_vOffset = { 0.f, 0.f, 0.f };
	_float m_fBaseRadius = 1.f;
	_bool m_bIsInverse = false;

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

