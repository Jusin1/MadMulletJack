#include "pch.h"
#include "CUI.h"

CUI::CUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUIBase(pGraphicDev)
{
	ZeroMemory(&m_tMoveInfo, sizeof(UIMoveInfo));
}

CUI::CUI(const CUI& rhs)
	: CUIBase(rhs), m_tMoveInfo(rhs.m_tMoveInfo)
{
}

CUI::~CUI()
{

}

HRESULT CUI::Ready_GameObject()
{
	return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Set_Component()))
		return E_FAIL;
	D3DXMatrixOrthoLH(&m_ProjMatrix, WINCX, WINCY, 0.f, 1.f);

	m_tMoveInfo = {MV_END, false, 0.f, 0.f};

	return S_OK;
}


_int CUI::Update_GameObject(const _float& fTimeDelta)
{
	__super::Update_GameObject(fTimeDelta);
	return NO_EVENT;
}

void CUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);

}

void CUI::Render_GameObject()
{
	m_pTransformCom->Apply_WorldMatrix();

	_matrix ViewMatrix;
	D3DXMatrixIdentity(&ViewMatrix);

	// 1. UI 렌더링 시작 전 Z-buffer 끄기
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	m_pVIBufferCom->Render_Buffer();

	// 2. UI 렌더링 끝난 후 Z-buffer 원래 상태로 되돌리기
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	
}



HRESULT CUI::Set_Component()
{
	if (FAILED(__super::Set_Component()))
		return E_FAIL;

	// VIBUFFER
	if (FAILED(Add_Components(L"Com_VIBuffer", SCENE_STATIC, L"Proto_Rect_Buffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

void CUI::Set_UISizeAndPos(_float _fSizeX, _float _fSizeY, _float _fX, _float _fY)
{
	m_fSizeX = _fSizeX;
	m_fSizeY = _fSizeY;

	m_fX = _fX;
	m_fY = _fY;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_Info(INFO_POS, _vec3(m_fX - WINCX * 0.5f, -m_fY + WINCY * 0.5f, 0.f));
}

void CUI::Set_UISize(_float _fSizeX, _float _fSizeY)
{
	m_fSizeX = _fSizeX;
	m_fSizeY = _fSizeY;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
}

void CUI::Move_UI(const _float& fTimeDelta)
{
	switch (m_tMoveInfo.eUIMove) {
	case MV_RIGHT:
		m_pTransformCom ->Move_PosRight(fTimeDelta, m_tMoveInfo.fRange, m_tMoveInfo.bStop, m_tMoveInfo.fSumRange);
		break;

	case MV_LEFT:
		m_pTransformCom->Move_PosLeft(fTimeDelta, m_tMoveInfo.fRange, m_tMoveInfo.bStop, m_tMoveInfo.fSumRange);
		break;

	case MV_RL: // range 만큼 좌우로 움직임
		m_pTransformCom->Move_RL(fTimeDelta, m_tMoveInfo.fRange, m_tMoveInfo.bStop, m_tMoveInfo.fSumRange);
		break;

	case MV_UP: // y기준으로 위 아래로 움직임
		m_pTransformCom->Move_YUp(fTimeDelta, m_tMoveInfo.fRange, m_tMoveInfo.bStop, m_tMoveInfo.fSumRange);
		break;

	case MV_DOWN: 
		m_pTransformCom->Move_YDown(fTimeDelta, m_tMoveInfo.fRange, m_tMoveInfo.bStop, m_tMoveInfo.fSumRange);
		break;

	case MV_ROTATIONZ: // z축 기준으로 회전
		m_pTransformCom->Rotation({0.f,0.f,1.f}, fTimeDelta);
		m_fRotSum += m_pTransformCom->GetTransformInfo().fRotationSpeed * fTimeDelta;
		break;

	case MV_UpDown:
		m_pTransformCom->Move_YUpDown(fTimeDelta, m_tMoveInfo.fRange, m_tMoveInfo.bStop, m_tMoveInfo.fSumRange);
		break;
	}
}

CUI* CUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CUI* pUI = new CUI(pGraphicDev);

	if (FAILED(pUI->Ready_GameObject()))
	{
		Safe_Release(pUI);
		MSG_BOX("pUI Create Failed");
		return nullptr;
	}

	return pUI;
}

CGameObject* CUI::Clone(void* pArg)
{
	CUI* pInstance = new CUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("pUI Clone Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI::Free()
{
	__super::Free();
}

