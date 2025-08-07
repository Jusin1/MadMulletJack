#pragma once
#include "CComponent.h"

BEGIN(Engine)
// 사각형 충돌체 컴포넌트
class ENGINE_DLL CColider_Rect : public CComponent
{
public:
	typedef struct ColiderInfo
	{
		_float	fRadiusX = 0.f, fRadiusY = 0.f; // 가소, 세로 반지름
		_float	fOffSetX = 0.f, fOffSetY = 0.f; // 충돌체 오프셋
		_matrix		StateMatrix; // 충돌체의 변환 행렬
	}COLIDERINFO;

public:
	CColider_Rect(LPDIRECT3DDEVICE9 pGraphic_Device); 
	CColider_Rect(const CColider_Rect& rhs);
	virtual ~CColider_Rect() = default; 

public:
	HRESULT Ready_Colider(); // 사각 충돌 범위 설정 및 버텍스 초기화
	HRESULT Initialize(void* pArg); // 외부로부터 정보 받아서 초기화
	HRESULT Update_ColliderBox(_matrix WorldMatrix); // 충돌 위치 행렬 갱신
	HRESULT Render_ColliderBox(); // 와이어프레임 렌더링(디버깅용)
	_bool	Collision_Check(CColider_Rect* pTarget, _vec3* pOutDistance = nullptr); // 충돌 체크

public:
	COLIDERINFO Get_CollRectDesc() { return m_StateDesc; } // 충돌체 반환
	_vec3						m_vPoint[4]; // 4개의 정정 정보
	_bool						m_bIsInverse = false; // 좌우 반전된 상태 여부
<<<<<<< HEAD
=======
	void Set_Active(_bool bEnable) { m_bActive = bEnable; } // 충돌 활성화
	_bool Get_Active() const { return m_bActive; } // 충돌 비활성화
>>>>>>> develop

public:
	void Set_IsInverse(_bool bIsInverse) { m_bIsInverse = bIsInverse; } // 좌우 반전 설정

protected:
	COLIDERINFO		m_StateDesc; //현재 충돌체 상태
<<<<<<< HEAD

protected:
	LPDIRECT3DVERTEXBUFFER9  m_pVB = nullptr;
	_uint						m_iNumVertices = 0;
	_uint						m_iStride = 0; 
	_ulong						m_dwFVF = 0;
=======
	_bool m_bActive = true; // 충돌체 활성화 여부

protected:
	LPDIRECT3DVERTEXBUFFER9  m_pVB;
	_uint						m_iNumVertices;
	_uint						m_iStride;
	_ulong						m_dwFVF;
>>>>>>> develop
	D3DPRIMITIVETYPE			m_ePrimitiveType;
	_uint						m_iNumPrimitive;

protected:
<<<<<<< HEAD
	LPDIRECT3DINDEXBUFFER9		m_pIB = nullptr;
	_uint						m_iIndicesByte = 0; 
=======
	LPDIRECT3DINDEXBUFFER9		m_pIB;
	_uint						m_iIndicesByte;
>>>>>>> develop
	D3DFORMAT					m_eIndexFormat;

public:
	static CColider_Rect* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone(void* pArg);
private:
	virtual void	Free();
};
END

