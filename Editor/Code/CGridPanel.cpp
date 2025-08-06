#include "CGridPanel.h"
#include "CTexture.h"
#include "CComponentMgr.h"
#include "CTransform.h"
#include "CVIBuffer_GridPanel.h"
#include "CRenderer.h"

CGridPanel::CGridPanel(LPDIRECT3DDEVICE9 pGraphicDevice)
	: Engine::CGameObject(pGraphicDevice), m_pBuffer(nullptr), m_pRenderer(nullptr)
	, m_pTexture(nullptr), m_pTransform(nullptr)
{
}

CGridPanel::CGridPanel(const CGridPanel &rhs)
	: Engine::CGameObject(rhs)
{
}

CGridPanel::~CGridPanel()
{
}

void CGridPanel::Free()
{
	__super::Free();
}

CGridPanel *CGridPanel::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CGridPanel *gridPanel = new CGridPanel(pGraphicDev);

	if (FAILED(gridPanel->Ready_GameObject()))
	{
		Safe_Release(gridPanel);
		MSG_BOX("GridPanel Create Failed");
		return nullptr;
	}

	return gridPanel;
}

CGameObject *CGridPanel::Clone(void *pArg)
{
	CGridPanel *pInstance = new CGridPanel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("GridPanel Clone Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CGridPanel::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	m_vPosition = { 0.f, 0.f, 0.f };

	return S_OK;
}

HRESULT CGridPanel::Initialize(void *pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Set_Component()))
		return E_FAIL;

	return S_OK;
}

_int CGridPanel::Update_GameObject(const _float &fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	m_pRenderer->Add_RenderGroup(RENDER_ALPHA, this);
	
	return 0;
}

void CGridPanel::LateUpdate_GameObject(const _float &fTimeDelta)
{
	Update_Position(m_pTransform->Get_Info(INFO_POS));

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CGridPanel::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTransform->Apply_WorldMatrix();
	m_pTexture->Set_Texture();
	m_pBuffer->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CGridPanel::Change_Texture(_uint iSceneIdx, const _tchar *pPrototypeTag, void *pArg)
{
	if (!m_pTexture)
	{
		MSG_BOX("Not good situation at CGridPanel::Change_Texture");
		return E_FAIL;
	}
	
	auto	iter = find_if(m_mapComponent.begin(), m_mapComponent.end(), CTag_Finder(L"Com_Texture"));
	if (iter == m_mapComponent.end())
	{
		MSG_BOX("Com_Texture is not matching with components container");
		return E_FAIL;
	}
	Safe_Release(iter->second);
	m_mapComponent.erase(iter);

	return CGameObject::Add_Components(L"Com_Texture", iSceneIdx, pPrototypeTag, (CComponent**)&m_pTexture, pArg);
}

HRESULT CGridPanel::Change_Buffer(_uint iSceneIdx, const _tchar *pPrototypeTag, void *pArg)
{
	if (!m_pBuffer)
	{
		MSG_BOX("Not good situation at CGridPanel::Change_ChangeBuffer");
		return E_FAIL;
	}

	auto	iter = find_if(m_mapComponent.begin(), m_mapComponent.end(), CTag_Finder(L"Com_Buffer"));
	if (iter == m_mapComponent.end())
	{
		MSG_BOX("Com_Buffer is not matching with components container");
		return E_FAIL;
	}
	Safe_Release(iter->second);
	m_mapComponent.erase(iter);

	return CGameObject::Add_Components(L"Com_Buffer", iSceneIdx, pPrototypeTag, (CComponent **)&m_pBuffer, pArg);
}

HRESULT CGridPanel::Set_Component()
{
	auto *test = CComponentMgr::GetInstance();

	// Render
	if (FAILED(Add_Components(L"Com_Renderer", SCENE_STATIC, L"Proto_Renderer", (CComponent **)&m_pRenderer)))
		return E_FAIL;

	// VIBuffer
	if (FAILED(Add_Components(L"Com_Buffer", SCENE_LOADING, L"Proto_Component_Buffer_PanelDefault", (CComponent **)&m_pBuffer)))
		return E_FAIL;

	// Texture
	if (FAILED(Add_Components(L"Com_Texture", SCENE_STATIC, L"Proto_Component_Texture_PanelDefault", (CComponent **)&m_pTexture)))
		return E_FAIL;

	CTransform::TRANSFORMINFO TransformInfo;
	::ZeroMemory(&TransformInfo, sizeof(CTransform::TRANSFORMINFO));
	TransformInfo.vStartPos = _vec3(0.f, 0.f, 0.f);

	// Transform
	if (FAILED(Add_Components(L"Com_Transform", SCENE_STATIC, L"Proto_Transform", (CComponent **)&m_pTransform, &TransformInfo)))
		return E_FAIL;

	return S_OK;
}
