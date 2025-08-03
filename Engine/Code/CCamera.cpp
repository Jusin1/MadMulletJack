#include "CCamera.h"

// 컴포넌트 키값 초기화
const _tchar* CCamera::m_pTransformTag = L"Com_Transform";


CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CCamera::CCamera(const CCamera& rhs) : CGameObject(rhs)
{
}

CCamera::~CCamera()
{
}

HRESULT CCamera::Ready_GameObject()
{
	return S_OK;
}

// 외부로 전달된 카메라 정보 설정
HRESULT CCamera::Initialize(void* pArg)
{
	// 컴포넌트 생성
	m_pTransform = CTransform::Create(m_pGraphicDev);
	if (m_pTransform == nullptr)
		return E_FAIL;

	// 카메라 설정값 복사
	memcpy(&m_camInfo, pArg, sizeof(CAMINFO));

	m_pTransform->Set_Info(INFO_POS, m_camInfo.vEye); // 위치
	m_pTransform->LookAt(m_camInfo.vAt); // 방향
	m_pTransform->SetTransformInfo(m_camInfo.TransformInfo); // 정보 설정

	// 컴포넌트로 등록
	m_mapComponent.emplace(m_pTransformTag, m_pTransform);
	m_pTransform->Add_Ref(); // 참조 카운트 증가
	return S_OK;
}

_int CCamera::Update_GameObject(const _float& fTimeDelta)
{
	return NO_EVENT;
}

void CCamera::LateUpdate_GameObject(const _float& fTimeDelta)
{

}

HRESULT CCamera::Render()
{
	return S_OK;
}

// View 및 Project행렬 설정
HRESULT CCamera::Apply_ViewPorjection()
{
	_matrix matWorld = *m_pTransform->Get_World();

	_matrix matView = *D3DXMatrixInverse(&matView, nullptr, &matWorld);
	_matrix matProj = *D3DXMatrixPerspectiveFovLH(&matProj, m_camInfo.fFov, m_camInfo.fAspect, m_camInfo.fNear, m_camInfo.fFar);

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

	return S_OK;
}

void CCamera::Free()
{
	CGameObject::Free();
	Safe_Release(m_pTransform);
}
