#include "CEditorCamera.h"
#include "Engine_Define.h"
#include "CDInputMgr.h"
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
	m_vPosition = m_pTransformCom->Get_Info(INFO_POS);

	/*_long iWheel = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Z);
	if (iWheel != 0)
	{
		m_lMouseWheel += iWheel * 0.05f;
		m_pTransformCom->Move_Forward(fTimeDelta * m_lMouseWheel * 0.01f);
	}*/

	if(KEY_BUTTON_HOLD(DIK_Q))
		m_pTransformCom->Rotation(_vec3(0.f, 1.f, 0.f), -fTimeDelta);
	if (KEY_BUTTON_HOLD(DIK_E))
		m_pTransformCom->Rotation(_vec3(0.f, 1.f, 0.f), fTimeDelta);

	if (KEY_BUTTON_HOLD(DIK_R))
		m_pTransformCom->Rotation(m_pTransformCom->Get_Info(INFO_RIGHT), -fTimeDelta);
	if (KEY_BUTTON_HOLD(DIK_F))
		m_pTransformCom->Rotation(m_pTransformCom->Get_Info(INFO_RIGHT), fTimeDelta);

	if (KEY_BUTTON_HOLD(DIK_W))
		m_pTransformCom->Move_Forward(fTimeDelta, m_vPosition.y);
	if (KEY_BUTTON_HOLD(DIK_S))
		m_pTransformCom->Move_Backward(fTimeDelta, m_vPosition.y);
	if (KEY_BUTTON_HOLD(DIK_A))
		m_pTransformCom->Move_Left(fTimeDelta, m_vPosition.y);
	if (KEY_BUTTON_HOLD(DIK_D))
		m_pTransformCom->Move_Right(fTimeDelta, m_vPosition.y);


	if (KEY_BUTTON_HOLD(DIK_SPACE))
	{
		_vec3 vPos = m_pTransformCom->Get_Info(INFO_POS);
		vPos.y += fTimeDelta * m_pTransformCom->GetTransformInfo().fSpeed;
		m_pTransformCom->Set_Info(INFO_POS, vPos);
	}
	if (KEY_BUTTON_HOLD(DIK_LCONTROL))
	{
		_vec3 vPos = m_pTransformCom->Get_Info(INFO_POS);
		vPos.y -= fTimeDelta * m_pTransformCom->GetTransformInfo().fSpeed;
		m_pTransformCom->Set_Info(INFO_POS, vPos);
	}
}
