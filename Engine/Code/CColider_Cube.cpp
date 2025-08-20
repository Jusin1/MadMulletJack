#include "CColider_Cube.h"
#include "CTransform.h"

CColider_Cube::CColider_Cube(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
	, m_StateDesc{}
	, m_matWorld{}
	, m_pTransform(nullptr)
	, m_bActive(true)
	, m_vPoint{}
	, m_bIsInverse(false)
	, m_pVB(nullptr)
	, m_pIB(nullptr)
	, m_iNumVertices(0)
	, m_iStride(0)
	, m_dwFVF(0)
	, m_ePrimitiveType(D3DPT_TRIANGLELIST)
	, m_iNumPrimitive(0)
	, m_iIndicesByte(0)
	, m_eIndexFormat(D3DFMT_INDEX16)
{
}

CColider_Cube::CColider_Cube(const CColider_Cube& rhs)
	: CComponent(rhs)
	, m_StateDesc(rhs.m_StateDesc)
	, m_matWorld(rhs.m_matWorld)
	, m_pTransform(rhs.m_pTransform)
	, m_bActive(rhs.m_bActive)
	, m_bIsInverse(rhs.m_bIsInverse)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_iStride(rhs.m_iStride)
	, m_dwFVF(rhs.m_dwFVF)
	, m_ePrimitiveType(rhs.m_ePrimitiveType)
	, m_iNumPrimitive(rhs.m_iNumPrimitive)
	, m_iIndicesByte(rhs.m_iIndicesByte)
	, m_eIndexFormat(rhs.m_eIndexFormat)
{
	memcpy(m_vPoint, rhs.m_vPoint, sizeof(_vec3) * 8);

	if (m_pVB) m_pVB->AddRef();
	if (m_pIB) m_pIB->AddRef();
}


HRESULT CColider_Cube::Initialize_Prototype()
{
	m_iNumVertices = 8;
	m_iStride = sizeof(VTXCOL);
	m_dwFVF = FVF_COL;
	m_ePrimitiveType = D3DPT_TRIANGLELIST;
	m_iNumPrimitive = 12;

	if (FAILED(m_pGraphicDev->CreateVertexBuffer(m_iNumVertices * m_iStride, 0, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, 0)))
		return E_FAIL;

	VTXCOL* pVertices = nullptr;
	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	_vec3 points[8] = {
		{-0.5f,  0.5f, -0.5f}, {0.5f,  0.5f, -0.5f},
		{0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f},
		{-0.5f,  0.5f,  0.5f}, {0.5f,  0.5f,  0.5f},
		{0.5f, -0.5f,  0.5f}, {-0.5f, -0.5f,  0.5f}
	};

	for (int i = 0; i < 8; ++i) {
		m_vPoint[i] = pVertices[i].vPosition = points[i];
		pVertices[i].dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
	}

	m_pVB->Unlock();

	m_iIndicesByte = sizeof(INDEX16);
	m_eIndexFormat = D3DFMT_INDEX16;

	if (FAILED(m_pGraphicDev->CreateIndexBuffer(m_iNumPrimitive * m_iIndicesByte, 0, m_eIndexFormat, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;

	INDEX16* pIndices = nullptr;
	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	INDEX16 indices[12] = {
		{1, 5, 6}, {1, 6, 2}, {4, 0, 3}, {4, 3, 7},
		{4, 5, 1}, {4, 1, 0}, {3, 2, 6}, {3, 6, 7},
		{5, 4, 7}, {5, 7, 6}, {0, 1, 2}, {0, 2, 3}
	};
	memcpy(pIndices, indices, sizeof(indices));

	m_pIB->Unlock();

	D3DXMatrixIdentity(&m_matWorld);

	return S_OK;
}

HRESULT CColider_Cube::Initialize(void* pArg)
{
	if (FAILED(Initialize_Prototype()))
		return E_FAIL;

	if (pArg != nullptr)
		memcpy(&m_StateDesc, pArg, sizeof(COLLRECTDESC));

	if (!m_StateDesc.bMapObject)
		m_matWorld = *m_pTransform->Get_World();

	return S_OK;
}

void CColider_Cube::Set_Transform(CTransform* pTransform)
{
	m_pTransform = pTransform;
}

void CColider_Cube::Initialize_Matrix_WithDescription()
{
	// S * R * T * world 곱으로 적용
	_matrix matScale;
	_matrix matRotation;
	_matrix matTransition;

	::D3DXMatrixScaling(&matScale, m_StateDesc.fRadiusX * 2.f, m_StateDesc.fRadiusY * 2.f, m_StateDesc.fRadiusZ * 2.f);
	::D3DXMatrixIdentity(&matRotation);
	::D3DXMatrixTranslation(&matTransition, m_StateDesc.fOffSetX, m_StateDesc.fOffSetY, m_StateDesc.fOffsetZ);

	m_matWorld = matScale * matRotation * matTransition * (*m_pTransform->Get_World());
}

HRESULT CColider_Cube::Update_ColliderBox()
{
	if (m_pTransform == nullptr)
		return E_FAIL;

	m_matWorld = *m_pTransform->Get_World();

	// 오프셋 적용
	_vec3 vOffset = *(_vec3*)&m_matWorld.m[3][0];
	vOffset += _vec3(m_StateDesc.fOffSetX, m_StateDesc.fOffSetY, m_StateDesc.fOffsetZ);
	m_matWorld._41 = vOffset.x;
	m_matWorld._42 = vOffset.y;
	m_matWorld._43 = vOffset.z;

	return S_OK;
}

HRESULT CColider_Cube::Render_ColliderBox()
{
	if (!m_bActive)
		return S_OK; 
	DWORD oldFillMode = 0;
	m_pGraphicDev->GetRenderState(D3DRS_FILLMODE, &oldFillMode);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld);
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, m_iStride);
	m_pGraphicDev->SetFVF(m_dwFVF);
	m_pGraphicDev->SetIndices(m_pIB);

	m_pGraphicDev->DrawIndexedPrimitive(m_ePrimitiveType, 0, 0, m_iNumVertices, 0, m_iNumPrimitive);

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, oldFillMode);

	return S_OK;
}

_bool CColider_Cube::Collision_Check(CColider_Cube* pTarget, _vec3* pOutDistance)
{
	if (nullptr == pTarget)
		return false;

	auto GetTransformedMinMax = [&](const _vec3* pPoints, const _matrix& matWorld, _vec3& outMin, _vec3& outMax) {
		_vec3 vTransformed[8];
		for (int i = 0; i < 8; ++i)
			D3DXVec3TransformCoord(&vTransformed[i], &pPoints[i], &matWorld);

		outMin = outMax = vTransformed[0];
		for (int i = 1; i < 8; ++i) {
			outMin.x = min(outMin.x, vTransformed[i].x);
			outMin.y = min(outMin.y, vTransformed[i].y);
			outMin.z = min(outMin.z, vTransformed[i].z);
			outMax.x = max(outMax.x, vTransformed[i].x);
			outMax.y = max(outMax.y, vTransformed[i].y);
			outMax.z = max(outMax.z, vTransformed[i].z);
		}
		};

	_vec3 vSourMin, vSourMax;
	_vec3 vDestMin, vDestMax;

	GetTransformedMinMax(m_vPoint, m_matWorld, vSourMin, vSourMax);
	GetTransformedMinMax(pTarget->m_vPoint, pTarget->m_matWorld, vDestMin, vDestMax);

	if (vSourMax.x < vDestMin.x || vSourMin.x > vDestMax.x) return false;
	if (vSourMax.y < vDestMin.y || vSourMin.y > vDestMax.y) return false;
	if (vSourMax.z < vDestMin.z || vSourMin.z > vDestMax.z) return false;

	if (pOutDistance)
	{
		_vec3 vCenter1 = (vSourMax + vSourMin) * 0.5f;
		_vec3 vCenter2 = (vDestMax + vDestMin) * 0.5f;

		_vec3 vDist;
		vDist.x = (vCenter1.x > vCenter2.x ? -1 : 1) * (min(vSourMax.x, vDestMax.x) - max(vSourMin.x, vDestMin.x));
		vDist.y = (vCenter1.y > vCenter2.y ? -1 : 1) * (min(vSourMax.y, vDestMax.y) - max(vSourMin.y, vDestMin.y));
		vDist.z = (vCenter1.z > vCenter2.z ? -1 : 1) * (min(vSourMax.z, vDestMax.z) - max(vSourMin.z, vDestMin.z));

		*pOutDistance = vDist;
	}

	return true;
}

CColider_Cube* CColider_Cube::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CColider_Cube* pInstance = new CColider_Cube(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX("CColider_Cube Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CColider_Cube::Clone(void* pArg)
{
	CColider_Cube* pInstance = new CColider_Cube(*this);
	if (FAILED(pInstance->Initialize(pArg))) {
		MSG_BOX("CColider_Cube Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CColider_Cube::Free()
{
	__super::Free();
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}

