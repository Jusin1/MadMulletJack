#include "pch.h"
#include "CUI.h"

CUI::CUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CUI::CUI(const CUI& rhs)
	: CGameObject(rhs)
{
}

CUI::~CUI()
{

}

HRESULT CUI::Ready_GameObject()
{
	if (FAILED(CGameObject::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	D3DXMatrixOrthoLH(&m_ProjMatrix, WINCX, WINCY, 0.f, 1.f);

	m_fSizeX = 50.0f;
	m_fSizeY = 50.0f;

	// Á¤Áß¾Ó
	m_fX = WINCX * 0.5f;
	m_fY = WINCY * 0.5f;

	if (FAILED(SetComponent()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_Info(INFO_POS, _vec3(0.f, 0.f, 0.f)); // Á¤Áß¾Ó ÁÂÇ¥

	return S_OK;
}


_int CUI::Update_GameObject(const _float& fTimeDelta)
{

	return NO_EVENT;
}

void CUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(RENDER_UI, this);
}

void CUI::Render_GameObject()
{
	m_pTransformCom->Apply_WorldMatrix();

	_matrix ViewMatrix;
	D3DXMatrixIdentity(&ViewMatrix);

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	m_pTextureCom->Set_Texture(0);

	m_pVIBufferCom->Render_Buffer();

}

HRESULT CUI::SetComponent()
{
	// Renderer
	if (FAILED(Add_Components(L"Com_Renderer", SCENE_STATIC, L"Proto_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// Texture
	if (FAILED(Add_Components(L"Com_Texture", SCENE_STAGE, L"Prototype_Component_Texture_UITest", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// VIBUFFER
	if (FAILED(Add_Components(L"Com_VIBuffer", SCENE_STATIC, L"Proto_Rect_Buffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// Transform
	CTransform::TRANSFORMINFO		TransformInfo;
	ZeroMemory(&TransformInfo, sizeof(CTransform::TRANSFORMINFO));

	TransformInfo.fSpeed = 5.f;
	TransformInfo.fRotationSpeed = D3DXToRadian(90.0f);

	if (FAILED(Add_Components(L"Com_Transform", SCENE_STATIC, L"Proto_Transform", (CComponent**)&m_pTransformCom, &TransformInfo)))
		return E_FAIL;

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

