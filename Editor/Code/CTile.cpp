#include "CVIBuffer_Rect.h"
#include "CRenderer.h"
#include "CEditorPickingManager.h"
#include "CManagement.h"
#include "CGuiManager.h"
#include "CTexture.h"
#include "CTile.h"

CTile::CTile(LPDIRECT3DDEVICE9 pGraphicDevice)
	: Engine::CGameObject(pGraphicDevice), m_pBuffer(nullptr), m_pTexture(nullptr)
{
}

CTile::CTile(const CTile &rhs)
	: Engine::CGameObject(rhs)
{
}

CTile::~CTile()
{
}

void CTile::Free()
{
	CGameObject::Free();
	CEditorPickingManager::GetInstance()->Remove_PickingGroup(this);
}

CTile *CTile::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTile *pNew = new CTile(pGraphicDev);

	if (FAILED(pNew->Ready_GameObject()))
	{
		Safe_Release(pNew);
		MSG_BOX("CTile::Create, Failed");
		return nullptr;
	}

	return pNew;
}

CGameObject *CTile::Clone(void *pArg)
{
	CTile *pInstance = new CTile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CTile::Clone, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CTile::Ready_GameObject()
{
	if (FAILED(CGameObject::Ready_GameObject()))
		return E_FAIL;

	m_vPosition = { 0.f, 0.f, 0.f };

	return S_OK;
}

HRESULT CTile::Initialize(void *pArg)
{
	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Set_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

_int CTile::Update_GameObject(const _float &fTimeDelta)
{
	if (m_bDead)
		return DEAD;

	CEditorPickingManager::GetInstance()->Remove_PickingGroup(this);
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this);

	return NO_EVENT;
}

void CTile::LateUpdate_GameObject(const _float &fTimeDelta)
{
	if (m_bDead)
		return;

	Update_Position(m_pTransformCom->Get_Info(INFO_POS));

	CEditorPickingManager::GetInstance()->Add_PickingGroup(this);
	Engine::CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CTile::Render_GameObject()
{
	if (m_bDead)
		return;

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTransformCom->Apply_WorldMatrix();
	m_pTexture->Set_Texture();
	m_pBuffer->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

_bool CTile::Picking(_vec3 *PickingPoint)
{
	if (!PickingPoint || !GetBuffer())
		return FALSE;

	if (m_pBuffer->Picking(m_pTransformCom, PickingPoint))
	{
		return TRUE;
	}

	return FALSE;
}

void CTile::PickingTrue()
{
	if (CManagement::GetInstance()->Get_CurrentSceneIdx() == SCENE_PREFAB)
	{
		if (!m_pParent)
		{
			CGuiManager::GetInstance()->SetTarget(this);
		}
		else
		{
			if (CGuiManager::GetInstance()->GetTarget() == m_pParent)
				CGuiManager::GetInstance()->SetTarget(this);
			else
				CGuiManager::GetInstance()->SetTarget(m_pParent);
		}
	}
	else
	{
		if (!m_pParent)
		{
			CGuiManager::GetInstance()->SetTarget(this);
		}
		else
		{
			CGuiManager::GetInstance()->SetTarget(m_pParent);
		}
	}
}

void CTile::ExportData(void *pData)
{
	if (MAPOBJECTDATA *p = reinterpret_cast<MAPOBJECTDATA *>(pData))
	{
		// type
		p->eCategory = ObjectCategory::TILE;
		p->iType = GetType();
		p->bChild = (GetParent() != nullptr);
		// texture
		p->texture.OriginComponentName = m_pTexture->GetOriginCompName();

		// transform
		::memcpy(&p->transform.Right, &((*m_pTransformCom->Get_World()).m[0][0]), sizeof(_vec3));
		::memcpy(&p->transform.Up, &((*m_pTransformCom->Get_World()).m[1][0]), sizeof(_vec3));
		::memcpy(&p->transform.Look, &((*m_pTransformCom->Get_World()).m[2][0]), sizeof(_vec3));
		::memcpy(&p->transform.Pos, &((*m_pTransformCom->Get_World()).m[3][0]), sizeof(_vec3));
	}
	else
	{
		MSG_BOX("CTile::ExportData, pData is ¸À°¨");
	}
}

HRESULT CTile::Change_Texture(_uint iSceneIdx, const _tchar *pPrototypeTag, void *pArg)
{
	if (!m_pTexture)
	{
		MSG_BOX("Not good situation at CTile::Change_Texture");
		return E_FAIL;
	}

	auto	iter = find_if(m_mapComponent.begin(), m_mapComponent.end(), CTag_Finder(L"Com_Texture"));
	if (iter == m_mapComponent.end())
	{
		MSG_BOX("CTile::Com_Texture is not matching with components container");
		return E_FAIL;
	}
	Safe_Release(iter->second);
	m_mapComponent.erase(iter);

	return CGameObject::Add_Components(L"Com_Texture", iSceneIdx, pPrototypeTag, (CComponent **)&m_pTexture, pArg);
}

HRESULT CTile::Change_Buffer(_uint iSceneIdx, const _tchar *pPrototypeTag, void *pArg)
{
	if (!m_pBuffer)
	{
		MSG_BOX("Not good situation at CTile::Change_ChangeBuffer");
		return E_FAIL;
	}

	auto	iter = find_if(m_mapComponent.begin(), m_mapComponent.end(), CTag_Finder(L"Com_Buffer"));
	if (iter == m_mapComponent.end())
	{
		MSG_BOX("CTile::Com_Buffer is not matching with components container");
		return E_FAIL;
	}
	Safe_Release(iter->second);
	m_mapComponent.erase(iter);

	return CGameObject::Add_Components(L"Com_Buffer", iSceneIdx, pPrototypeTag, (CComponent **)&m_pBuffer, pArg);
}

HRESULT CTile::Set_Component(void *pArg)
{
	if (pArg)
	{
		if (MAPOBJECTDATA *p = reinterpret_cast<MAPOBJECTDATA *>(pArg))
		{
			SetType(p->iType);

			// VIBuffer
			if (FAILED(Add_Components(L"Com_Buffer", SCENE_STATIC, L"Proto_Component_Buffer_TileDefault", (CComponent **)&m_pBuffer)))
				return E_FAIL;

			// Texture
			if (FAILED(Add_Components(L"Com_Texture", SCENE_STATIC, p->texture.OriginComponentName.c_str(), (CComponent **)&m_pTexture)))
				return E_FAIL;

			if (p->bChild)
			{
				GetTransform()->Set_LocalInfo(INFO::INFO_RIGHT, p->transform.Right);
				GetTransform()->Set_LocalInfo(INFO::INFO_UP, p->transform.Up);
				GetTransform()->Set_LocalInfo(INFO::INFO_LOOK, p->transform.Look);
				GetTransform()->Set_LocalInfo(INFO::INFO_POS, p->transform.Pos);
			}
			else
			{
				GetTransform()->Set_Info(INFO::INFO_RIGHT, p->transform.Right);
				GetTransform()->Set_Info(INFO::INFO_UP, p->transform.Up);
				GetTransform()->Set_Info(INFO::INFO_LOOK, p->transform.Look);
				GetTransform()->Set_Info(INFO::INFO_POS, p->transform.Pos);
			}
		}
		else
		{
			MSG_BOX("CGridPanel::Set_Component, Something Wrong");
			return E_FAIL;
		}
	}
	else
	{
		// VIBuffer Default
		if (FAILED(Add_Components(L"Com_Buffer", SCENE_STATIC, L"Proto_Component_Buffer_TileDefault", (CComponent **)&m_pBuffer)))
			return E_FAIL;

		// Texture Default
		if (FAILED(Add_Components(L"Com_Texture", SCENE_STATIC, L"Proto_GridDefault", (CComponent **)&m_pTexture)))
			return E_FAIL;
	}

	return S_OK;
}
