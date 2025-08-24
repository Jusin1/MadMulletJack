#include "CCullingManager.h"

IMPLEMENT_SINGLETON(CCullingManager)

CCullingManager::CCullingManager() : m_pGrahpicDev(nullptr)
{

}

HRESULT CCullingManager::Ready_Culling(LPDIRECT3DDEVICE9 pGrahpicDev)
{
	m_pGrahpicDev = pGrahpicDev;
	m_pGrahpicDev->AddRef();

	m_WorldPoint[0] = _vec3(-1.f, 1.f, 0.f);
	m_WorldPoint[1] = _vec3(1.f, 1.f, 0.f);
	m_WorldPoint[2] = _vec3(1.f, -1.f, 0.f);
	m_WorldPoint[3] = _vec3(-1.f, -1.f, 0.f);
	m_WorldPoint[4] = _vec3(-1.f, 1.f, 1.f);
	m_WorldPoint[5] = _vec3(1.f, 1.f, 1.f);
	m_WorldPoint[6] = _vec3(1.f, -1.f, 1.f);
	m_WorldPoint[7] = _vec3(-1.f, -1.f, 1.f);

	return S_OK;
}

HRESULT CCullingManager::Update_Culling()
{
	m_WorldPoint[0] = _vec3(-1.f, 1.f, 0.f);
	m_WorldPoint[1] = _vec3(1.f, 1.f, 0.f);
	m_WorldPoint[2] = _vec3(1.f, -1.f, 0.f);
	m_WorldPoint[3] = _vec3(-1.f, -1.f, 0.f);
	m_WorldPoint[4] = _vec3(-1.f, 1.f, 1.f);
	m_WorldPoint[5] = _vec3(1.f, 1.f, 1.f);
	m_WorldPoint[6] = _vec3(1.f, -1.f, 1.f);
	m_WorldPoint[7] = _vec3(-1.f, -1.f, 1.f);

	_matrix ProjMatrixInv;
	m_pGrahpicDev->GetTransform(D3DTS_PROJECTION, &ProjMatrixInv);
	D3DXMatrixInverse(&ProjMatrixInv, nullptr, &ProjMatrixInv);

	for (int i = 0; i < 8; ++i)
		D3DXVec3TransformCoord(&m_WorldPoint[i], &m_WorldPoint[i], &ProjMatrixInv);

	_matrix		ViewMatrixInv;
	m_pGrahpicDev->GetTransform(D3DTS_VIEW, &ViewMatrixInv);
	D3DXMatrixInverse(&ViewMatrixInv, nullptr, &ViewMatrixInv);

	for (int i = 0; i < 8; ++i)
		D3DXVec3TransformCoord(&m_WorldPoint[i], &m_WorldPoint[i], &ViewMatrixInv);

	return S_OK;
}

_bool CCullingManager::Is_In_Frustum(_vec3 pGameObjectPos, _float fRadius)
{
	D3DXPlaneFromPoints(&m_FrustumPlane[0], &m_WorldPoint[0], &m_WorldPoint[1], &m_WorldPoint[2]);
	D3DXPlaneFromPoints(&m_FrustumPlane[1], &m_WorldPoint[1], &m_WorldPoint[5], &m_WorldPoint[6]);
	D3DXPlaneFromPoints(&m_FrustumPlane[2], &m_WorldPoint[4], &m_WorldPoint[5], &m_WorldPoint[1]);
	D3DXPlaneFromPoints(&m_FrustumPlane[3], &m_WorldPoint[4], &m_WorldPoint[0], &m_WorldPoint[3]);
	D3DXPlaneFromPoints(&m_FrustumPlane[4], &m_WorldPoint[5], &m_WorldPoint[4], &m_WorldPoint[7]);
	D3DXPlaneFromPoints(&m_FrustumPlane[5], &m_WorldPoint[3], &m_WorldPoint[2], &m_WorldPoint[6]);

	for (int i = 0; i < 6; ++i)
	{
		_float distance = m_FrustumPlane[i].a * pGameObjectPos.x + m_FrustumPlane[i].b * pGameObjectPos.y + m_FrustumPlane[i].c * pGameObjectPos.z + m_FrustumPlane[i].d;
		if (distance > fRadius + 1)
			return false;
	}

	return true;
}

void CCullingManager::Free()
{
	Safe_Release(m_pGrahpicDev);
}
