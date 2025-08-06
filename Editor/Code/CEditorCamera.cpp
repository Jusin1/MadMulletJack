#include "CEditorCamera.h"
#include "CDInputMgr.h"

CEditorCamera::CEditorCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev), m_fSpeed(0.f), m_bFix(false), m_bCheck(false)
{
}

CEditorCamera::CEditorCamera(const CEditorCamera &rhs)
	: Engine::CCamera(rhs)
{
}

CEditorCamera::~CEditorCamera()
{
}

void CEditorCamera::Free()
{
	Engine::CCamera::Free();
}

HRESULT CEditorCamera::Ready_GameObject()
{
	if (FAILED(Engine::CCamera::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEditorCamera::Initialize(void *pArg)
{
	if (FAILED(Engine::CCamera::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_int CEditorCamera::Update_GameObject(const _float &fTimeDelta)
{
	Engine::CCamera::Update_GameObject(fTimeDelta);

	DefaultCamera(fTimeDelta);

	if (FAILED(Engine::CCamera::Apply_ViewPorjection()))
		return NO_EVENT;

	return NO_EVENT;
}

void CEditorCamera::LateUpdate_GameObject(const _float &fTimeDelta)
{
	Engine::CCamera::LateUpdate_GameObject(fTimeDelta);
}

CEditorCamera *CEditorCamera::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
	CEditorCamera *pEditorCamera = new CEditorCamera(pGraphicDevice);
	if (FAILED(pEditorCamera->Ready_GameObject()))
	{
		MSG_BOX("EditorCamera Create Failed");
		Safe_Release(pEditorCamera);
		return nullptr;
	}

	return pEditorCamera;
}

CCamera *CEditorCamera::Clone(void *pArg)
{	
	CEditorCamera *pInstance = new CEditorCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("EditorCamera Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEditorCamera::DefaultCamera(const _float &fTimeDelta)
{
	m_vPosition = m_pTransform->Get_Info(INFO_POS);

	/*_long iWheel = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Z);
	if (iWheel != 0)
	{
		m_lMouseWheel += iWheel * 0.05f;
		m_pTransform->Move_Forward(fTimeDelta * m_lMouseWheel * 0.01f);
	}*/

	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_Q) & 0x80)
		m_pTransform->Rotation(_vec3(0.f, 1.f, 0.f), fTimeDelta);
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_E) & 0x80)
		m_pTransform->Rotation(_vec3(0.f, 1.f, 0.f), -fTimeDelta);

	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_W) & 0x80)
		m_pTransform->Move_Forward(fTimeDelta, m_vPosition.y);
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_S) & 0x80)
		m_pTransform->Move_Backward(fTimeDelta, m_vPosition.y);
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_A) & 0x80)
		m_pTransform->Move_Left(fTimeDelta, m_vPosition.y);
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_D) & 0x80)
		m_pTransform->Move_Right(fTimeDelta, m_vPosition.y);


	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		_vec3 vPos = m_pTransform->Get_Info(INFO_POS);
		vPos.y += fTimeDelta * m_pTransform->GetTransformInfo().fSpeed;
		m_pTransform->Set_Info(INFO_POS, vPos);
	}
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_LCONTROL) & 0x80)
	{
		_vec3 vPos = m_pTransform->Get_Info(INFO_POS);
		vPos.y -= fTimeDelta * m_pTransform->GetTransformInfo().fSpeed;
		m_pTransform->Set_Info(INFO_POS, vPos);
	}
}
