#include "CCalculator.h"
#include "CTransform.h"

CCalculator::CCalculator(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
	
}

CCalculator::~CCalculator()
{
}

HRESULT CCalculator::Ready_Calculator()
{
	return S_OK;
}

_float CCalculator::Compute_HeightOnTerrain(const _vec3* pPos, 
	const _vec3* pTerrainVtxPos, const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv)
{
	_ulong	dwIndex = _ulong(pPos->z / dwVtxItv) * dwCntX + _ulong(pPos->x / dwVtxItv);

	_float	fWidth  = (pPos->x - pTerrainVtxPos[dwIndex + dwCntX].x) / dwVtxItv;
	_float	fHeight = ( pTerrainVtxPos[dwIndex + dwCntX].z - pPos->z) / dwVtxItv;

	D3DXPLANE		Plane;

	// 우 상단 
	if (fWidth > fHeight)
	{
		D3DXPlaneFromPoints(&Plane, 
			&pTerrainVtxPos[dwIndex + dwCntX],
			&pTerrainVtxPos[dwIndex + dwCntX + 1],
			&pTerrainVtxPos[dwIndex + 1]);
	}
	// 좌 하단
	else
	{
		D3DXPlaneFromPoints(&Plane,
			&pTerrainVtxPos[dwIndex + dwCntX],
			&pTerrainVtxPos[dwIndex + 1],
			&pTerrainVtxPos[dwIndex]);
	}

	return (-Plane.a * pPos->x - Plane.c * pPos->z - Plane.d) / Plane.b;
}

_vec3 CCalculator::Picking_OnTerrain(HWND hWnd, 
									const CTerrainTex* pTerrainBufferCom, 
									const CTransform* pTerrainTransformCom)
{
	POINT	ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	// 뷰포트 -> 투영
	_vec3 vMousePos;

	D3DVIEWPORT9		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));

	m_pGraphicDev->GetViewport(&ViewPort);

	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	// 투영 -> 뷰스페이스
	D3DXMATRIX	matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, 0, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	// 뷰 스페이스 -> 월드

	D3DXMATRIX	matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0, &matView);

	_vec3	vRayPos{ 0.f, 0.f, 0.f };		// 뷰 스페이스
	_vec3	vRayDir = vMousePos - vRayPos;

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	// 월드 -> 로컬
	_matrix	matWorld;
	pTerrainTransformCom->Get_World(&matWorld);
	D3DXMatrixInverse(&matWorld, 0, &matWorld);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);


	return _vec3();
}

CCalculator* CCalculator::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCalculator* pCalculator = new CCalculator(pGraphicDev);

	if (FAILED(pCalculator->Ready_Calculator()))
	{
		Safe_Release(pCalculator);
		MSG_BOX("pCalculator Create Failed");
		return nullptr;
	}

	return pCalculator;
}

CComponent* CCalculator::Clone(void* pArg)
{
	return new CCalculator(*this);
}

void CCalculator::Free()
{
	CComponent::Free();
}
