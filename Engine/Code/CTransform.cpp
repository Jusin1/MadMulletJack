#include "CTransform.h"

CTransform::CTransform() 
{ 
	ZeroMemory(&m_TransformInfo, sizeof(m_TransformInfo));
	D3DXMatrixIdentity(&m_matWorld);
}

CTransform::CTransform(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
	ZeroMemory(&m_TransformInfo, sizeof(m_TransformInfo));
	D3DXMatrixIdentity(&m_matWorld);
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs), m_matWorld(rhs.m_matWorld), m_TransformInfo(rhs.m_TransformInfo)
	,m_fDir(rhs.m_fDir)
{
}

CTransform::~CTransform()
{
}

HRESULT CTransform::Ready_Transform()
{
	D3DXMatrixIdentity(&m_matWorld);

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		memcpy(&m_TransformInfo, pArg, sizeof(TRANSFORMINFO));
		Set_Info(INFO_POS, m_TransformInfo.vStartPos);
	}
	return S_OK;
}

_vec3 CTransform::Get_Scale()
{
	_vec3		vRight = Get_Info(INFO_RIGHT);
	_vec3		vUp = Get_Info(INFO_UP);
	_vec3		vLook = Get_Info(INFO_LOOK);

	return _vec3(D3DXVec3Length(&vRight), D3DXVec3Length(&vUp), D3DXVec3Length(&vLook));
}

void CTransform::Set_Scale(_float x, _float y, _float z)
{
	_vec3		vRight = Get_Info(INFO_RIGHT);
	_vec3		vUp = Get_Info(INFO_UP);
	_vec3		vLook = Get_Info(INFO_LOOK);

	Set_Info(INFO_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * x);
	Set_Info(INFO_UP, *D3DXVec3Normalize(&vUp, &vUp) * y);
	Set_Info(INFO_LOOK, *D3DXVec3Normalize(&vLook, &vLook) * z);
}

void CTransform::Move_Forward(_float fTimeDelta, _float fHeight)
{
	_vec3 vPos = Get_Info(INFO_POS);
	_vec3 vLook = Get_Info(INFO_LOOK);

	vPos += *D3DXVec3Normalize(&vLook, &vLook) * fTimeDelta * m_TransformInfo.fSpeed;
	vPos.y = fHeight;

	Set_Info(INFO_POS, vPos);
}

void CTransform::Move_Backward(_float fTimeDelta, _float fHeight)
{
	_vec3 vPos = Get_Info(INFO_POS);
	_vec3 vLook = Get_Info(INFO_LOOK);

	vPos -= *D3DXVec3Normalize(&vLook, &vLook) * fTimeDelta * m_TransformInfo.fSpeed;
	vPos.y = fHeight;

	Set_Info(INFO_POS, vPos);
}

void CTransform::Move_Right(_float fTimeDelta, _float fHeight)
{
	_vec3 vPos = Get_Info(INFO_POS);
	_vec3 vRight = Get_Info(INFO_RIGHT);

	vPos += *D3DXVec3Normalize(&vRight, &vRight) * fTimeDelta * m_TransformInfo.fSpeed;
	vPos.y = fHeight;

	Set_Info(INFO_POS, vPos);
}

void CTransform::Move_Left(_float fTimeDelta, _float fHeight)
{
	_vec3 vPos = Get_Info(INFO_POS);
	_vec3 vLeft = Get_Info(INFO_RIGHT);

	vPos -= *D3DXVec3Normalize(&vLeft, &vLeft) * fTimeDelta * m_TransformInfo.fSpeed;
	vPos.y = fHeight;

	Set_Info(INFO_POS, vPos);
}

void CTransform::Move_PosUp(_float fTimeDelta)
{
	_vec3 vPos = Get_Info(INFO_POS);
	vPos.z += fTimeDelta * m_TransformInfo.fSpeed;

	Set_Info(INFO_POS, vPos);
}

void CTransform::Move_PosDown(_float fTimeDelta)
{
	_vec3 vPos = Get_Info(INFO_POS);
	vPos.z -= fTimeDelta * m_TransformInfo.fSpeed;

	Set_Info(INFO_POS, vPos);
}

void CTransform::Move_PosLeft(_float fTimeDelta)
{
	_vec3 vPos = Get_Info(INFO_POS);
	vPos.x -= fTimeDelta * m_TransformInfo.fSpeed;

	Set_Info(INFO_POS, vPos);
}

void CTransform::Move_PosRight(_float fTimeDelta)
{
	_vec3 vPos = Get_Info(INFO_POS);
	vPos.x += fTimeDelta * m_TransformInfo.fSpeed;

	Set_Info(INFO_POS, vPos);
}

void CTransform::Move_PosTarget(_float fTimeDelta, _vec3 TargetPos, _vec3 distance)
{
	_vec3 vPos = Get_Info(INFO_POS);
	_vec3 vNewPos = { TargetPos.x + distance.x, TargetPos.y + distance.y, TargetPos.z + distance.z };
	_vec3 vDir = vNewPos - vPos;

	D3DXVec3Normalize(&vDir, &vDir);

	vPos += vDir * fTimeDelta * m_TransformInfo.fSpeed;

	Set_Info(INFO_POS,vPos);
}

void CTransform::Move_PosDir(_float fTimeDelta, _vec3 _vDir)
{
	_vec3 vPos = Get_Info(INFO_POS);

	_vec3 vDir = _vDir;

	D3DXVec3Normalize(&vDir, &vDir);

	vPos += vDir * fTimeDelta * m_TransformInfo.fSpeed;

	Set_Info(INFO_POS, vPos);
}

void CTransform::LookAt(_vec3 TargetPos)
{
	_vec3 vPos = Get_Info(INFO_POS);
	_vec3 vLook = TargetPos - vPos;
	_vec3 vRight;

	_vec3 vWorldUp = _vec3(0.f, 1.f, 0.f);  
	D3DXVec3Cross(&vRight, &vWorldUp, &vLook);

	_vec3 vUp;
	D3DXVec3Cross(&vUp, &vLook, &vRight);

	_vec3		vScale = Get_Scale();

	Set_Info(INFO_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * vScale.x);
	Set_Info(INFO_UP, *D3DXVec3Normalize(&vUp, &vUp) * vScale.y);
	Set_Info(INFO_LOOK, *D3DXVec3Normalize(&vLook, &vLook) * vScale.z);
}

void CTransform::Rotation(_vec3 vAxis, _float fTimeDelta)
{
	_vec3 vRight = Get_Info(INFO_RIGHT);
	_vec3 vUp = Get_Info(INFO_UP);
	_vec3 vLook = Get_Info(INFO_LOOK);

	_matrix RotateMatrix;
	D3DXMatrixRotationAxis(&RotateMatrix, &vAxis, m_TransformInfo.fRotationSpeed * fTimeDelta);

	D3DXVec3TransformNormal(&vRight, &vRight, &RotateMatrix);
	D3DXVec3TransformNormal(&vUp, &vUp, &RotateMatrix);
	D3DXVec3TransformNormal(&vLook, &vLook, &RotateMatrix);

	Set_Info(INFO_RIGHT, vRight);
	Set_Info(INFO_UP, vUp);
	Set_Info(INFO_LOOK, vLook);
}

void CTransform::Move_YUp(_float fTimeDelta)
{
	_vec3 vPos = Get_Info(INFO_POS);
	vPos += _vec3({0.f,1.f,0.f}) *fTimeDelta* m_TransformInfo.fSpeed;

	Set_Info(INFO_POS, vPos);
}

void CTransform::Move_YDown(_float fTimeDelta)
{
	_vec3 vPos = Get_Info(INFO_POS);
	vPos -= _vec3({ 0.f,1.f,0.f }) * fTimeDelta * m_TransformInfo.fSpeed;

	Set_Info(INFO_POS, vPos);
}

void CTransform::Move_RL(_float fTimeDelta, _float fRange)
{
	_vec3 vPos = Get_Info(INFO_POS);

	// 방향에 맞춰 이동
	vPos += m_fDir * _vec3({ 1.f,0.f,0.f }) * m_TransformInfo.fSpeed * fTimeDelta;

	// 범위 체크 후 반전
	if (vPos.x > m_TransformInfo.vStartPos.x + fRange)
	{
		vPos.x = m_TransformInfo.vStartPos.x + fRange;
		m_fDir = -1.f;
	}
	else if (vPos.x < m_TransformInfo.vStartPos.x - fRange)
	{
		vPos.x = m_TransformInfo.vStartPos.x - fRange;
		m_fDir = 1.f;
	}

	Set_Info(INFO_POS, vPos);
}

void CTransform::Move_YUpDown(_float fTimeDelta, _float fRange)
{
	_vec3 vPos = Get_Info(INFO_POS);

	// 방향에 맞춰 이동
	vPos += m_fDir * _vec3({ 0.f,1.f,0.f }) * m_TransformInfo.fSpeed * fTimeDelta;

	// 범위 체크 후 반전
	if (vPos.y > m_TransformInfo.vStartPos.y + fRange)
	{
		vPos.y = m_TransformInfo.vStartPos.y + fRange;
		m_fDir = -1.f;
	}
	else if (vPos.y < m_TransformInfo.vStartPos.y - fRange)
	{
		vPos.y = m_TransformInfo.vStartPos.y - fRange;
		m_fDir = 1.f;
	}

	Set_Info(INFO_POS, vPos);
}

void CTransform::ChaseTarget(_vec3 TargetPos, _vec3 distance)
{
	_vec3 vPos = Get_Info(INFO_POS);
	_vec3 vNewPos = { TargetPos.x + distance.x, TargetPos.y + distance.y, TargetPos.z + distance.z };

	Set_Info(INFO_POS, vNewPos);
}

HRESULT CTransform::Apply_WorldMatrix()
{
	if (m_pGraphicDev == nullptr)
		return E_FAIL;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld);
	return S_OK;
}


CTransform* CTransform::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTransform* pTransform = new CTransform(pGraphicDev);

	if (FAILED(pTransform->Ready_Transform()))
	{
		Safe_Release(pTransform);
		MSG_BOX("Transform Create Failed");
		return nullptr;
	}

	return pTransform;
}

CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Transform Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CTransform::Free()
{
	CComponent::Free();
}
