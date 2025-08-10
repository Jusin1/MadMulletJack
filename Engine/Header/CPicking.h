#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
class ENGINE_DLL CPicking :
    public CBase
{
    DECLARE_SINGLETON(CPicking)

private:
	explicit CPicking();
	virtual ~CPicking();

public:
	HRESULT Initialize(HWND hWnd, LPDIRECT3DDEVICE9 pGrahpicDev);
	void Update(); // Ray 갱신
	void TransformRayToLocalSpace(_matrix WorldMatrixInverse); // 월드 -> 로컬
	_bool IntersectRayWithTriangleInWorld(_vec3 vPointA, _vec3 vPointB, _vec3 vPointC, _vec3* pOut); // 월드 레이 충돌 검사
	_bool IntersectRayWithTriangleInLocal(_vec3 vPointA, _vec3 vPointB, _vec3 vPointC, _vec3* pOut); // 로컬 레이 충돌 검사

public:
	const _vec3& GetRayPosWorld() const { return m_vRayPos; }
	const _vec3& GetRayDirWorld() const { return m_vRayDir; }

private:
	LPDIRECT3DDEVICE9		m_pGrahpicDev;
	HWND					m_hWnd;
	_vec3					m_vRayDir;	// 레이 방향벡터 (월드) 
	_vec3					m_vRayPos;  // 레이 시작점 (월드)
	_vec3					m_vRayDir_Local;  //  레이 방향벡터(로컬)
	_vec3					m_vRayPos_Local; // 레이 방향벡터(로컬)
public:
	virtual void Free() override;
};
END

