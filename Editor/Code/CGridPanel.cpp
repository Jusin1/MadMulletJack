#include "CGridPanel.h"
#include "CTexture.h"
#include "CComponentMgr.h"
#include "Editor_Define.h"
#include "CGuiManager.h"
#include "CEditorPickingManager.h"
#include "CTransform.h"
#include "CVIBuffer_GridPanel.h"
#include "CRenderer.h"

CGridPanel::CGridPanel(LPDIRECT3DDEVICE9 pGraphicDevice)
	: Engine::CGameObject(pGraphicDevice), m_pBuffer(nullptr)
	, m_pTexture(nullptr)
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
	CEditorPickingManager::GetInstance()->Remove_PickingGroup(this);
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
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Set_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

_int CGridPanel::Update_GameObject(const _float &fTimeDelta)
{
	if (m_bDead)
		return DEAD;

	CEditorPickingManager::GetInstance()->Remove_PickingGroup(this);
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this);
	
	return NO_EVENT;
}

void CGridPanel::LateUpdate_GameObject(const _float &fTimeDelta)
{
	if (m_bDead)
		return;

	Update_Position(m_pTransformCom->Get_Info(INFO_POS));

	CEditorPickingManager::GetInstance()->Add_PickingGroup(this);
	Engine::CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CGridPanel::Render_GameObject()
{
	if (m_bDead)
		return;

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTransformCom->Apply_WorldMatrix();
	m_pTexture->Set_Texture();
	m_pBuffer->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

_bool CGridPanel::Picking(_vec3 *PickingPoint)
{
	if (!PickingPoint || !GetBuffer())
		return FALSE;

	if (m_pBuffer->Picking(m_pTransformCom, PickingPoint))
	{
		return TRUE;
	}

	return FALSE;
}

void CGridPanel::PickingTrue()
{
	CGuiManager::GetInstance()->SetTarget(this);
}

void CGridPanel::ExportData(void *pData)
{
	if (MAPOBJECTDATA *p = reinterpret_cast<MAPOBJECTDATA *>(pData))
	{
		p->ObjType = OBJ_END;
		p->panelBuffer.dwCountX = GetBuffer()->Get_Data()->dwCountX;
		p->panelBuffer.dwCountY = GetBuffer()->Get_Data()->dwCountY;
		p->panelBuffer.dwCountZ = GetBuffer()->Get_Data()->dwCountZ;
		p->panelBuffer.dwInterval = GetBuffer()->Get_Data()->dwInterval;
		p->panelBuffer.eType = GetBuffer()->Get_Data()->eType;
		p->texture.OriginComponentName = GetTexture()->GetOriginCompName();
		_vec3 cpy = GetTransform()->Get_Info(INFO_RIGHT);
		::memcpy(&p->transform.Right, &cpy, sizeof(_vec3));
		cpy = GetTransform()->Get_Info(INFO_UP);
		::memcpy(&p->transform.Up, &cpy, sizeof(_vec3));
		cpy = GetTransform()->Get_Info(INFO_LOOK);
		::memcpy(&p->transform.Look, &cpy, sizeof(_vec3));
		cpy = GetTransform()->Get_Info(INFO_POS);
		::memcpy(&p->transform.Pos, &cpy, sizeof(_vec3));
	}
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

HRESULT CGridPanel::Set_Component(void *pArg)
{
	if (pArg)
	{
		MAPOBJECTDATA tdata;
		::memcpy(&tdata, pArg, sizeof(MAPOBJECTDATA));
	}

	// VIBuffer
	if (FAILED(Add_Components(L"Com_Buffer", SCENE_STATIC, L"Proto_Component_Buffer_PanelDefault", (CComponent **)&m_pBuffer)))
		return E_FAIL;

	// Texture
	if (FAILED(Add_Components(L"Com_Texture", SCENE_STATIC, L"Proto_PanelDefault", (CComponent **)&m_pTexture)))
		return E_FAIL;

	CTransform::TRANSFORMINFO TransformInfo;
	::ZeroMemory(&TransformInfo, sizeof(CTransform::TRANSFORMINFO));
	TransformInfo.vStartPos = _vec3(0.f, 0.f, 0.f);

	return S_OK;
}