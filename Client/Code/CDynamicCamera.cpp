#include "pch.h"
#include "CDynamicCamera.h"
#include "CDInputMgr.h"

CDynamicCamera::CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev)
{
}

CDynamicCamera::CDynamicCamera(const CDynamicCamera& rhs) : CCamera(rhs)
{

}


CDynamicCamera::~CDynamicCamera()
{
}



HRESULT CDynamicCamera::Ready_GameObject()
{
	if (FAILED(CCamera::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDynamicCamera::Initialize(void* pArg)
{
	if (FAILED(CCamera::Initialize(pArg)))
		return E_FAIL;
	return S_OK;
}

_int CDynamicCamera::Update_GameObject(const _float& fTimeDelta)
{
	CCamera::Update_GameObject(fTimeDelta);

	DefaultCamera(fTimeDelta);

	if (FAILED(Apply_ViewPorjection()))
		return NO_EVENT;
	return NO_EVENT;
}

void CDynamicCamera::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::CCamera::LateUpdate_GameObject(fTimeDelta);
}

void CDynamicCamera::DefaultCamera(_float fTimeDelta)
{
	// 위치 갱신
	m_vPosition = m_pTransform->Get_Info(INFO_POS);

	// 마우스 휠 줌
	_long iWheel = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Z);
	if (iWheel != 0)
	{
		m_lMouseWheel += iWheel * 0.05f;
		m_pTransform->Move_Forward(fTimeDelta * m_lMouseWheel * 0.01f);
	}

	// Yaw 회전 (Q/E)
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_Q) & 0x80)
		m_pTransform->Rotation(_vec3(0.f, 1.f, 0.f), fTimeDelta);
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_E) & 0x80)
		m_pTransform->Rotation(_vec3(0.f, 1.f, 0.f), -fTimeDelta);

	// 이동
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_W) & 0x80)
		m_pTransform->Move_Forward(fTimeDelta, m_vPosition.y);
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_S) & 0x80)
		m_pTransform->Move_Backward(fTimeDelta, m_vPosition.y);
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_A) & 0x80)
		m_pTransform->Move_Left(fTimeDelta, m_vPosition.y);
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_D) & 0x80)
		m_pTransform->Move_Right(fTimeDelta, m_vPosition.y);


	//  상하 이동 (월드 Y축 기준) 직접 처리
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_R) & 0x80)
	{
		_vec3 vPos = m_pTransform->Get_Info(INFO_POS);
		vPos.y += fTimeDelta * m_pTransform->GetTransformInfo().fSpeed;
		m_pTransform->Set_Info(INFO_POS, vPos);
	}
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_F) & 0x80)
	{
		_vec3 vPos = m_pTransform->Get_Info(INFO_POS);
		vPos.y -= fTimeDelta * m_pTransform->GetTransformInfo().fSpeed;
		m_pTransform->Set_Info(INFO_POS, vPos);
	}
}


CDynamicCamera* CDynamicCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CDynamicCamera* pCamera = new CDynamicCamera(pGraphicDev);

	if (FAILED(pCamera->Ready_GameObject()))
	{
		Safe_Release(pCamera);
		MSG_BOX("DynamicCamera Create Failed");
		return nullptr;
	}
	return pCamera;
}

CCamera* CDynamicCamera::Clone(void* pArg)
{
	CDynamicCamera* pInstance = new CDynamicCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("DynamicCamera Clone Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDynamicCamera::Free()
{
	Engine::CCamera::Free();
}
