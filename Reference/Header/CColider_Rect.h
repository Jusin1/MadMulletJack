#pragma once
#include "CComponent.h"

BEGIN(Engine)
class ENGINE_DLL CColider_Rect : public CComponent
{
public:
	typedef struct ColiderInfo
	{
		_float	fRadiusX = 0.f, fRadiusY = 0.f;
		_float	fOffSetX = 0.f, fOffSetY = 0.f;
		_matrix		StateMatrix;
	}COLIDERINFO;

public:
	CColider_Rect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CColider_Rect(const CColider_Rect& rhs);
	virtual ~CColider_Rect() = default;

public:
	HRESULT Ready_Colider();
	HRESULT Initialize(void* pArg);
	HRESULT Update_ColliderBox(_matrix WorldMatrix);
	HRESULT Render_ColliderBox();
	_bool	Collision_Check(CColider_Rect* pTarget, _vec3* pOutDistance = nullptr);

public:
	COLIDERINFO Get_CollRectDesc() { return m_StateDesc; }
	_vec3						m_vPoint[4];
	_bool						m_bIsInverse = false;

public:
	void Set_IsInverse(_bool bIsInverse) { m_bIsInverse = bIsInverse; }

protected:
	COLIDERINFO		m_StateDesc;

protected:
	LPDIRECT3DVERTEXBUFFER9  m_pVB = nullptr;
	_uint						m_iNumVertices = 0;
	_uint						m_iStride = 0; /* 정점의 크기(byte) */
	_ulong						m_dwFVF = 0;
	D3DPRIMITIVETYPE			m_ePrimitiveType;
	_uint						m_iNumPrimitive = 0;

protected:
	LPDIRECT3DINDEXBUFFER9		m_pIB = nullptr;
	_uint						m_iIndicesByte = 0;
	D3DFORMAT					m_eIndexFormat;


public:
	static CColider_Rect* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone(void* pArg);
private:
	virtual void	Free();
};
END

