#include "Engine_Define.h"
#include "CVIBuffer_GridPanelBase.h"
#include "CGameObject.h"
#include "CTransform.h"
#include "CGrounding.h"


CGrounding::CGrounding()
{
}

CGrounding::CGrounding(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
}

CGrounding::CGrounding(const CGrounding &rhs)
	: CComponent(rhs)
{
}

CGrounding::~CGrounding()
{
}

void CGrounding::Free()
{
	__super::Free();
}

CGrounding *CGrounding::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CGrounding *pInstance = new CGrounding(pGraphicDev);
	if (FAILED(pInstance->Ready_Component()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CGrounding::Create, Failed");
		return nullptr;
	}
	return pInstance;
}

CComponent *CGrounding::Clone(void *pArg)
{
	CGrounding *pInstance = new CGrounding(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CGrounding::Clone, Failed");
		return nullptr;
	}
	return pInstance;
}

HRESULT CGrounding::Ready_Component()
{
	return S_OK;
}

HRESULT CGrounding::Initialize(void *pArg)
{
	return S_OK;
}

_bool CGrounding::GetHeight(const vector<PANELENTRY> *pPanelEntries, _float fX, _float fZ, _float *fOutY)
{
	if (pPanelEntries->empty())
		return false;

	auto lambda_Inside =
	[&](int _i)->bool
	{
		return (*pPanelEntries)[_i].eType == WallType::FLOOR
			? IsInside((*pPanelEntries)[_i], fX, fZ)
			: IsInside_Slope((*pPanelEntries)[_i], fX, fZ);
	};

	auto lambda_GetHeight = [&](int _i)->bool
	{
		if ((*pPanelEntries)[_i].eType == WallType::FLOOR)
		{
			*fOutY = (*pPanelEntries)[_i].fY;
			m_iCurrentIndex = _i;
			return true;
		}
		else
		{
			_float fY = Y_OnPanelPlane((*pPanelEntries)[_i], fX, fZ);
			_vec3 src{ fX, fY, fZ };
			::D3DXVec3TransformCoord(&src, &src, &(*pPanelEntries)[_i].matWorldInv);
			src.y = Compute_Height((*pPanelEntries)[_i], src.x, src.z);
			::D3DXVec3TransformCoord(&src, &src, &(*pPanelEntries)[_i].matWorld);
			*fOutY = src.y;
			m_iCurrentIndex = _i;
			return true;
		}
	};

	for (int i = 0; i < 3; ++i)
	{
		if (lambda_Inside(m_iCurrentIndex))
			return lambda_GetHeight(m_iCurrentIndex);

		if (m_iCurrentIndex + i < (*pPanelEntries).size() && lambda_Inside(m_iCurrentIndex + i))
			return lambda_GetHeight(m_iCurrentIndex + i);

		if (((m_iCurrentIndex - i) >= 0) && lambda_Inside(m_iCurrentIndex - i))
			return lambda_GetHeight(m_iCurrentIndex - i);
	}

	return false;
}

_bool CGrounding::Initialize_CurrentIndex(const vector<PANELENTRY> *pPanelEntries, _float fX, _float fZ, _float *fOutY)
{
	if (pPanelEntries->empty())
		return false;

	for (int i = 0; i < pPanelEntries->size(); ++i)
	{
		if ((*pPanelEntries)[i].eType == WallType::FLOOR)
		{
			if (IsInside((*pPanelEntries)[i], fX, fZ))
			{
				m_iCurrentIndex = i;
				return true;
			}
		}
		else if((*pPanelEntries)[i].eType == WallType::INCLINE)
		{
			if (IsInside_Slope((*pPanelEntries)[i], fX, fZ))
			{
				m_iCurrentIndex = i;
				return true;
			}
		}
		else
		{
			MSG_BOX("CGrounding::Initialize_CurrentIndex, Wrongtype");
			return false;
		}
	}

	return false;
}

_bool CGrounding::IsInside(const PANELENTRY &tPanelEntry, _float fX, _float fZ)
{
	_float fMinX = tPanelEntry.fMin_X;
	_float fMaxX = tPanelEntry.fMax_X;
	_float fMinZ = tPanelEntry.fMin_Z;
	_float fMaxZ = tPanelEntry.fMax_Z;

	// Floor의 크기 내에 있는지
	return ((fX >= fMinX - m_fEpsilon) && (fX < fMaxX - m_fEpsilon) &&
			(fZ >= fMinZ - m_fEpsilon) && (fZ < fMaxZ - m_fEpsilon));
}

_bool CGrounding::IsInside_Slope(const PANELENTRY &tPanelEntry, _float fX, _float fZ)
{
	auto pBuffer = tPanelEntry.pBuffer;
	_float fLocalMaxX = (pBuffer->GetData()->dwCountX - 1) * pBuffer->GetData()->dwInterval;
	_float fLocalMaxZ = (pBuffer->GetData()->dwCountZ - 1) * pBuffer->GetData()->dwInterval;

	_float fY = Y_OnPanelPlane(tPanelEntry, fX, fZ);
	_vec3 srcLocal{ fX, fY, fZ };
	::D3DXVec3TransformCoord(&srcLocal, &srcLocal, &tPanelEntry.matWorldInv);

	return ((srcLocal.x >= -m_fEpsilon) && (srcLocal.x < fLocalMaxX + m_fEpsilon)) &&
			((srcLocal.z >= -m_fEpsilon) && (srcLocal.z < fLocalMaxZ + m_fEpsilon));
}

_float CGrounding::Compute_Height(const PANELENTRY &tPanelEntry, _float fX, _float fZ)
{
	_ulong dwCntX = tPanelEntry.pBuffer->GetData()->dwCountX;
	_ulong dwCntZ = tPanelEntry.pBuffer->GetData()->dwCountZ;
	_vec3 *pTerrainVtxPos = tPanelEntry.pBuffer->GetVerticesData();

	int indexX = (int)floorf(fX * tPanelEntry.fInverseItv);
	int indexZ = (int)floorf(fZ * tPanelEntry.fInverseItv);

	// 배열 접근 오버런 방지
	indexX = (std::max)(0, (std::min)(indexX, (int)dwCntX - 2));
	indexZ = (std::max)(0, (std::min)(indexZ, (int)dwCntZ - 2));

	_ulong	dwIndex = indexZ * dwCntX + indexX;

	// _float	fWidth = (fX - pTerrainVtxPos[dwIndex].x) * tPanelEntry.fInverseItv;
	// _float	fHeight = (fZ - pTerrainVtxPos[dwIndex].z) * tPanelEntry.fInverseItv;
	//D3DXPLANE		Plane;
	//// 우 상단 
	//if (fWidth + fHeight <= 1.f)
	//{
	//	D3DXPlaneFromPoints(&Plane,
	//		&pTerrainVtxPos[dwIndex],
	//		&pTerrainVtxPos[dwIndex + 1],
	//		&pTerrainVtxPos[dwIndex + dwCntX]);
	//}
	//// 좌 하단
	//else
	//{
	//	D3DXPlaneFromPoints(&Plane,
	//		&pTerrainVtxPos[dwIndex + dwCntX + 1],
	//		&pTerrainVtxPos[dwIndex + dwCntX],
	//		&pTerrainVtxPos[dwIndex + 1]);
	//}

	//return (-Plane.a * fX - Plane.c * fZ - Plane.d) / Plane.b;

	const _vec3 &vLeftBottom = pTerrainVtxPos[dwIndex];
	const _vec3 &vRightBottom = pTerrainVtxPos[dwIndex + 1];
	const _vec3 &vLeftTop = pTerrainVtxPos[dwIndex + dwCntX];
	const _vec3 &vRightTop = pTerrainVtxPos[dwIndex + dwCntX + 1];


	const _float fWidth = (fX - vLeftBottom.x) * tPanelEntry.fInverseItv;
	const _float fHeight = (fZ - vLeftBottom.z) * tPanelEntry.fInverseItv;

	if (fWidth + fHeight <= 1.f) {
		// 왼쪽 아래 삼각형
		return vLeftBottom.y + (vRightBottom.y - vLeftBottom.y) * fWidth + (vLeftTop.y - vLeftBottom.y) * fHeight;
	}
	else {
		// 오른쪽 위 삼각형
		const _float fWidth2 = 1.f - fWidth;
		const _float fHeight2 = 1.f - fHeight;
		return vRightTop.y + (vRightBottom.y - vRightTop.y) * fHeight2 + (vLeftTop.y - vRightTop.y) * fWidth2;
	}
}

_float CGrounding::Y_OnPanelPlane(const PANELENTRY &tPanelEntry, _float fX, _float fZ)
{
	const D3DXPLANE &pl = tPanelEntry.planeWorld;
	return (-pl.a * fX - pl.c * fZ - pl.d) / pl.b;
}
