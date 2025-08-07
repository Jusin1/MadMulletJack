#include "CColider_Rect.h"

CColider_Rect::CColider_Rect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
	, m_StateDesc{}
	, m_bActive(true)
	, m_pVB(nullptr)
	, m_iNumVertices(0)
	, m_iStride(0)
	, m_dwFVF(0)
	, m_ePrimitiveType(D3DPT_TRIANGLELIST)
	, m_iNumPrimitive(0)
	, m_pIB(nullptr)
	, m_iIndicesByte(0)
	, m_eIndexFormat(D3DFMT_INDEX16)
{
}

CColider_Rect::CColider_Rect(const CColider_Rect& rhs)
	: CComponent(rhs)
	, m_StateDesc(rhs.m_StateDesc)
	, m_bActive(rhs.m_bActive)
	, m_pVB(rhs.m_pVB)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_iStride(rhs.m_iStride)
	, m_dwFVF(rhs.m_dwFVF)
	, m_ePrimitiveType(rhs.m_ePrimitiveType)
	, m_iNumPrimitive(rhs.m_iNumPrimitive)
	, m_pIB(rhs.m_pIB)
	, m_iIndicesByte(rhs.m_iIndicesByte)
	, m_eIndexFormat(rhs.m_eIndexFormat)
{
	memcpy(m_vPoint, rhs.m_vPoint, sizeof(_vec3) * 4);

	if (m_pVB) m_pVB->AddRef();
	if (m_pIB) m_pIB->AddRef();
}



// 기본 설정 크기(1x1)
HRESULT CColider_Rect::Ready_Colider()
{
	D3DXMatrixIdentity(&m_StateDesc.StateMatrix);

	m_iNumVertices = 4;
	m_iStride = sizeof(VTXTEX);
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1;
	m_ePrimitiveType = D3DPT_TRIANGLELIST;
	m_iNumPrimitive = 2;

	if (FAILED(m_pGraphicDev->CreateVertexBuffer(m_iNumVertices * m_iStride, 0, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, 0)))
		return E_FAIL;

	VTXTEX* pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPosition = m_vPoint[0] = _vec3(-0.5f, 0.5f, 0.f);// + InitPos;
	pVertices[0].vTexUV = _vec2(0.0f, 0.f);

	pVertices[1].vPosition = m_vPoint[1] = _vec3(0.5f, 0.5f, 0.f);// +InitPos;
	pVertices[1].vTexUV = _vec2(1.f, 0.f);

	pVertices[2].vPosition = m_vPoint[2] = _vec3(0.5f, -0.5f, 0.f);// +InitPos;
	pVertices[2].vTexUV = _vec2(1.f, 1.f);

	pVertices[3].vPosition = m_vPoint[3] = _vec3(-0.5f, -0.5f, 0.f);// +InitPos;
	pVertices[3].vTexUV = _vec2(0.f, 1.f);
	m_pVB->Unlock();

	m_iIndicesByte = sizeof(INDEX16);
	m_eIndexFormat = D3DFMT_INDEX16;

	if (FAILED(m_pGraphicDev->CreateIndexBuffer(m_iNumPrimitive * m_iIndicesByte, 0, m_eIndexFormat, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;

	INDEX16* pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	m_pIB->Unlock();

	return S_OK;
}

// 외부 설정 받아서 충돌체 생성
HRESULT CColider_Rect::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		m_bIsInverse = false; 
		memcpy(&m_StateDesc, pArg, sizeof(COLIDERINFO));

		m_iNumVertices = 4;
		m_iStride = sizeof(VTXTEX);
		m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1;
		m_ePrimitiveType = D3DPT_TRIANGLELIST;
		m_iNumPrimitive = 2;
		if (FAILED(m_pGraphicDev->CreateVertexBuffer(m_iNumVertices * m_iStride, 0, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, 0)))
			return E_FAIL;

		VTXTEX* pVertices = nullptr;

		m_pVB->Lock(0, 0, (void**)&pVertices, 0);

		pVertices[0].vPosition = m_vPoint[0] = _vec3(-m_StateDesc.fRadiusX, m_StateDesc.fRadiusY, 0.f);// + InitPos;
		pVertices[0].vTexUV = _vec2(0.0f, 0.f);

		pVertices[1].vPosition = m_vPoint[1] = _vec3(m_StateDesc.fRadiusX, m_StateDesc.fRadiusY, 0.f);// +InitPos;
		pVertices[1].vTexUV = _vec2(1.f, 0.f);

		pVertices[2].vPosition = m_vPoint[2] = _vec3(m_StateDesc.fRadiusX, -m_StateDesc.fRadiusY, 0.f);// +InitPos;
		pVertices[2].vTexUV = _vec2(1.f, 1.f);

		pVertices[3].vPosition = m_vPoint[3] = _vec3(-m_StateDesc.fRadiusX, -m_StateDesc.fRadiusY, 0.f);// +InitPos;
		pVertices[3].vTexUV = _vec2(0.f, 1.f);
		m_pVB->Unlock();


		m_iIndicesByte = sizeof(INDEX16);
		m_eIndexFormat = D3DFMT_INDEX16;

		if (FAILED(m_pGraphicDev->CreateIndexBuffer(m_iNumPrimitive * m_iIndicesByte, 0, m_eIndexFormat, D3DPOOL_MANAGED, &m_pIB, nullptr)))
			return E_FAIL;


		INDEX16* pIndices = nullptr;

		m_pIB->Lock(0, 0, (void**)&pIndices, 0);

		pIndices[0]._0 = 0;
		pIndices[0]._1 = 1;
		pIndices[0]._2 = 2;

		pIndices[1]._0 = 0;
		pIndices[1]._1 = 2;
		pIndices[1]._2 = 3;

		m_pIB->Unlock();
	}


	return S_OK;
}

// 월드행렬 반영해서 정점 위치를 갱신
HRESULT CColider_Rect::Update_ColliderBox(_matrix WorldMatrix)
{

	m_vPoint[0] = _vec3(-m_StateDesc.fRadiusX, m_StateDesc.fRadiusY, 0.f);

	m_vPoint[1] = _vec3(m_StateDesc.fRadiusX, m_StateDesc.fRadiusY, 0.f);

	m_vPoint[2] = _vec3(m_StateDesc.fRadiusX, -m_StateDesc.fRadiusY, 0.f);

	m_vPoint[3] = _vec3(-m_StateDesc.fRadiusX, -m_StateDesc.fRadiusY, 0.f);

	m_StateDesc.StateMatrix = WorldMatrix;
	_vec3 vecOffsetPos = *(_vec3*)&(m_StateDesc.StateMatrix.m[3][0]);
	vecOffsetPos.x += m_StateDesc.fOffSetX;
	vecOffsetPos.y += m_StateDesc.fOffSetY;

	m_StateDesc.StateMatrix.m[3][0] = vecOffsetPos.x;
	m_StateDesc.StateMatrix.m[3][1] = vecOffsetPos.y;


	for (size_t i = 0; i < 4; ++i)
	{
		D3DXVec3TransformCoord(&m_vPoint[i], &m_vPoint[i], &m_StateDesc.StateMatrix);
	}

	return S_OK;
}

// 충돌체 그리기(디버깅용 와이어프레임)
HRESULT CColider_Rect::Render_ColliderBox()
{
	// 재질 적용
	D3DMATERIAL9 mtrl = {};
	mtrl.Diffuse = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);  // 초록색
	mtrl.Ambient = mtrl.Diffuse;
	m_pGraphicDev->SetMaterial(&mtrl);

	// 와이어프레임 모드
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, m_iStride);
	m_pGraphicDev->SetFVF(m_dwFVF);
	m_pGraphicDev->SetIndices(m_pIB);

	m_pGraphicDev->DrawIndexedPrimitive(m_ePrimitiveType, 0, 0, m_iNumVertices, 0, m_iNumPrimitive);

	// 원래대로 
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	return S_OK;
}

// AABB충돌 검사
_bool CColider_Rect::Collision_Check(CColider_Rect* pTarget, _vec3* pOutDistance)
{
	CColider_Rect* pOther = pTarget;
	if (nullptr == pOther)
		return false;

	_vec3		vSourMin, vSourMax, vSourCenter;
	_vec3		vDestMin, vDestMax, vDestCenter;
	_vec3		vDistance = _vec3(0, 0, 0);

	if (m_bIsInverse)
	{
		vSourMin = m_vPoint[2];
		vSourMax = m_vPoint[0];
	}
	else
	{
		vSourMin = m_vPoint[3];
		vSourMax = m_vPoint[1];
	}

	vSourCenter = (vSourMax + vSourMin) * 0.5f;

	// Flip Min and Max if pOther is Scaled by -1 (X-Axis)
	if (pTarget->m_bIsInverse)
	{
		vDestMin = pOther->m_vPoint[2];
		vDestMax = pOther->m_vPoint[0];
	}
	else
	{
		vDestMin = pOther->m_vPoint[3];
		vDestMax = pOther->m_vPoint[1];
	}

	vDestCenter = (vDestMax + vDestMin) * 0.5f;



	if (min(vSourMax.x, vDestMax.x) < max(vSourMin.x, vDestMin.x))
		return false;
	else
	{
		if (vSourCenter.x > vDestCenter.x)
		{
			vDistance.x = -(min(vSourMax.x, vDestMax.x) - max(vSourMin.x, vDestMin.x));
		}
		else
			vDistance.x = (min(vSourMax.x, vDestMax.x) - max(vSourMin.x, vDestMin.x));
	}


	if (min(vSourMax.z, vDestMax.z) < max(vSourMin.z, vDestMin.z))
		return false;
	else
	{
		if (vSourCenter.z > vDestCenter.z)
		{
			vDistance.z = -(min(vSourMax.z, vDestMax.z) - max(vSourMin.z, vDestMin.z));
		}
		else
			vDistance.z = min(vSourMax.z, vDestMax.z) - max(vSourMin.z, vDestMin.z);
	}


	if (pOutDistance != nullptr)
		*pOutDistance = vDistance;

	return true;
}


CColider_Rect* CColider_Rect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CColider_Rect* pColider_Rect = new CColider_Rect(pGraphic_Device);

	if (FAILED(pColider_Rect->Ready_Colider()))
	{
		Safe_Release(pColider_Rect);
		MSG_BOX("pColider Create Failed");
		return nullptr;
	}
	return pColider_Rect;
}

CComponent* CColider_Rect::Clone(void* pArg)
{
	CColider_Rect* pClone = new CColider_Rect(*this);

	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CColider_Rect Clone Initialize Failed");
		Safe_Release(pClone);
		return nullptr;
	}

	return pClone;
}

void CColider_Rect::Free()
{
	__super::Free();

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}