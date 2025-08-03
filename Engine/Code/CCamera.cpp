#include "CCamera.h"
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

HRESULT CCamera::Initialize(void* pArg)
{
	m_pTransform = CTransform::Create(m_pGraphicDev);
	if (m_pTransform == nullptr)
		return E_FAIL;

	memcpy(&m_camInfo, pArg, sizeof(CAMINFO));

	m_pTransform->Set_Info(INFO_POS, m_camInfo.vEye);
	m_pTransform->LookAt(m_camInfo.vAt);
	m_pTransform->SetTransformInfo(m_camInfo.TransformInfo);

	m_mapComponent.emplace(m_pTransformTag, m_pTransform);

	m_pTransform->Add_Ref();
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
