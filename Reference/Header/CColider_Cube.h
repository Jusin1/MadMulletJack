#pragma once
#include "CComponent.h"
BEGIN(Engine)
class ENGINE_DLL CColider_Cube :
    public CComponent
{
public:
	struct COLLRECTDESC
	{
		_float fRadiusX = 0.5f, fRadiusY = 0.5f, fRadiusZ = 0.5f;
		_float fOffSetX = 0.f, fOffSetY = 0.f, fOffsetZ = 0.f;
	};

public:
	CColider_Cube(LPDIRECT3DDEVICE9 pGraphic_Device);
	CColider_Cube(const CColider_Cube& rhs);
	virtual ~CColider_Cube() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg) override;
	HRESULT Update_ColliderBox(); 
	HRESULT Render_ColliderBox();
	_bool Collision_Check(CColider_Cube* pTarget, _vec3* pOutDistance = nullptr);

public:
	void Set_Transform(class CTransform* pTransform);
	void Set_IsInverse(_bool bIsInverse) { m_bIsInverse = bIsInverse; }
	COLLRECTDESC Get_CollRectDesc() const { return m_StateDesc; }
	void Set_Active(_bool bActive) { m_bActive = bActive; }
	_bool Is_Active() const { return m_bActive; }

protected:
	COLLRECTDESC m_StateDesc;
	_matrix m_matWorld; 
	class CTransform* m_pTransform = nullptr;
	_bool m_bActive = true;

protected:
	_vec3 m_vPoint[8];
	_bool m_bIsInverse = false;

protected:
	LPDIRECT3DVERTEXBUFFER9 m_pVB = nullptr;
	LPDIRECT3DINDEXBUFFER9  m_pIB = nullptr;

	_uint m_iNumVertices = 0;
	_uint m_iStride = 0;
	_ulong m_dwFVF = 0;
	D3DPRIMITIVETYPE m_ePrimitiveType;
	_uint m_iNumPrimitive = 0;
	_uint m_iIndicesByte = 0;
	D3DFORMAT m_eIndexFormat;

public:
	static CColider_Cube* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END
