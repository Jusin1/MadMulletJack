#include "CPicking.h"
#include "CTransform.h"
#include "CDInputMgr.h"

IMPLEMENT_SINGLETON(CPicking)

CPicking::CPicking()
	: m_pGrahpicDev(nullptr),
	m_hWnd(nullptr),
	m_vRayDir{},
	m_vRayPos{},	
	m_vRayDir_Local{},
	m_vRayPos_Local{}
{
}

CPicking::~CPicking()
{
}

HRESULT CPicking::Initialize(HWND hWnd, LPDIRECT3DDEVICE9 pGrahpicDev)
{
	m_pGrahpicDev = pGrahpicDev;

	m_hWnd = hWnd; 
	m_pGrahpicDev->AddRef();

	return S_OK;
}

void CPicking::Update()
{
	POINT			ptMouse;

	GetCursorPos(&ptMouse);

	ScreenToClient(m_hWnd, &ptMouse);

	_vec3			vMousePos;

	// 투영 마우스 좌표 
	vMousePos.x = _float(ptMouse.x / (WINCX * 0.5f) - 1);
	vMousePos.y = _float(ptMouse.y / (WINCY * -0.5f) + 1);
	vMousePos.z = 0.f;


	// 뷰 스페이스 좌표
	_matrix		ProjMatrixInv;
	m_pGrahpicDev->GetTransform(D3DTS_PROJECTION, &ProjMatrixInv);
	D3DXMatrixInverse(&ProjMatrixInv, nullptr, &ProjMatrixInv);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &ProjMatrixInv);


	// 뷰스페이스 상의 마우스 레이와 레이 위치
	m_vRayPos = _vec3(0.f, 0.f, 0.f);
	m_vRayDir = vMousePos - m_vRayPos;

	// 월드 스페이스 상의 마우스 레이와 레이 위치
	_matrix		ViewMatrixInv;
	m_pGrahpicDev->GetTransform(D3DTS_VIEW, &ViewMatrixInv);
	D3DXMatrixInverse(&ViewMatrixInv, nullptr, &ViewMatrixInv);
	D3DXVec3TransformCoord(&m_vRayPos, &m_vRayPos, &ViewMatrixInv);
	D3DXVec3TransformNormal(&m_vRayDir, &m_vRayDir, &ViewMatrixInv);
	D3DXVec3Normalize(&m_vRayDir, &m_vRayDir);
}

void CPicking::TransformRayToLocalSpace(_matrix WorldMatrixInverse)
{
	D3DXVec3TransformCoord(&m_vRayPos_Local, &m_vRayPos, &WorldMatrixInverse);
	D3DXVec3TransformNormal(&m_vRayDir_Local, &m_vRayDir, &WorldMatrixInverse);

	D3DXVec3Normalize(&m_vRayDir_Local, &m_vRayDir_Local);
}

_bool CPicking::IntersectRayWithTriangleInWorld(_vec3 vPointA, _vec3 vPointB, _vec3 vPointC, _vec3* pOut)
{
	_float		fU, fV, fDist;

	if (TRUE == D3DXIntersectTri(&vPointA, &vPointB, &vPointC, &m_vRayPos, &m_vRayDir, &fU, &fV, &fDist))
	{
		*pOut = m_vRayPos + m_vRayDir * fDist;
		return true;
	}

	return false;
}

_bool CPicking::IntersectRayWithTriangleInLocal(_vec3 vPointA, _vec3 vPointB, _vec3 vPointC, _vec3* pOut)
{
	_float		fU, fV, fDist;

	if (TRUE == D3DXIntersectTri(&vPointA, &vPointB, &vPointC, &m_vRayPos_Local, &m_vRayDir_Local, &fU, &fV, &fDist))
	{
		*pOut = m_vRayPos_Local + m_vRayDir_Local * fDist;

		return true;
	}

	return false;
}

bool CPicking::IntersectRaySphere(const _vec3& ro, const _vec3& rd, const _vec3& c, float r, float* tHit)
{
	_vec3 oc = ro - c;
	float b = D3DXVec3Dot(&oc, &rd);
	float cterm = D3DXVec3Dot(&oc, &oc) - r * r;
	float disc = b * b - cterm;
	if (disc < 0.f) return false;

	float t = -b - sqrtf(max(0.f, disc));
	if (t < 0.f) t = -b + sqrtf(max(0.f, disc));
	if (t < 0.f) return false;

	if (tHit) *tHit = t;
	return true;
}

void CPicking::Free()
{
	Safe_Release(m_pGrahpicDev);
}


