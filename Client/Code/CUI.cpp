#include "pch.h"
#include "CUI.h"

CUI::CUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUIBase(pGraphicDev), m_bAniFinish(false)
{
}

CUI::CUI(const CUI& rhs)
	: CUIBase(rhs), m_bAniFinish(rhs.m_bAniFinish)
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

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	m_pVIBufferCom->Render_Buffer();
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
	Engine::CGameObject::Free();
}

