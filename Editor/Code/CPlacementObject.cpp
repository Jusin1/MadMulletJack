#include "CVIBuffer_Cube_Color.h"
#include "CRenderer.h"
#include "CGuiManager.h"
#include "Engine_Define.h"
#include "CGridPanel.h"
#include "CEditorPickingManager.h"
#include "CDummyPlacementObject.h"
#include "CVIBuffer_GridPanel_Editor.h"
#include "Editor_Define.h"
#include "CObjectManager.h"
#include "CPlacementObject.h"

CPlacementObject::CPlacementObject(LPDIRECT3DDEVICE9 pGraphicDevice)
	: CGameObject(pGraphicDevice), m_pBuffer(nullptr), m_eCategory(ObjectCategory::NONE), m_iType(0)
{
}

CPlacementObject::CPlacementObject(const CPlacementObject &rhs)
	: CGameObject(rhs)
{
}

CPlacementObject::~CPlacementObject()
{
}

void CPlacementObject::Free()
{
	CGameObject::Free();
	CEditorPickingManager::GetInstance()->Remove_PickingGroup(this);
}

CPlacementObject *CPlacementObject::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlacementObject *pNew = new CPlacementObject(pGraphicDev);

	if (FAILED(pNew->Ready_GameObject()))
	{
		Safe_Release(pNew);
		MSG_BOX("PlacementObject Create Failed");
		return nullptr;
	}

	return pNew;
}

CGameObject *CPlacementObject::Clone(void *pArg)
{
	CPlacementObject *pInstance = new CPlacementObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("PlacementObject Clone Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CPlacementObject::Ready_GameObject()
{
	if (FAILED(CGameObject::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlacementObject::Initialize(void *pArg)
{
	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Set_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

_int CPlacementObject::Update_GameObject(const _float &fTimeDelta)
{
	if (m_bDead)
		return DEAD;

	CEditorPickingManager::GetInstance()->Remove_PickingGroup(this);
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this);

	return NO_EVENT;
}

void CPlacementObject::LateUpdate_GameObject(const _float &fTimeDelta)
{
	if (m_bDead)
		return;

	Update_Position(m_pTransformCom->Get_Info(INFO_POS));

	CEditorPickingManager::GetInstance()->Add_PickingGroup(this);
	Engine::CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CPlacementObject::Render_GameObject()
{
	if (m_bDead)
		return;

	m_pGraphicDev->SetTexture(0, nullptr);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTransformCom->Apply_WorldMatrix();
	m_pBuffer->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

_bool CPlacementObject::Picking(_vec3 *PickingPoint)
{
	if (!PickingPoint || !GetBuffer())
		return FALSE;

	if (m_pBuffer->Picking(m_pTransformCom, PickingPoint))
	{
		return TRUE;
	}

	return FALSE;
}

void CPlacementObject::PickingTrue()
{
	CGuiManager::GetInstance()->SetTarget(this);
}

void CPlacementObject::ExportData(void *pData)
{
}

HRESULT CPlacementObject::Set_Component(void *pArg)
{
	if (pArg)
	{
		if (MAPOBJECTDATA *p = reinterpret_cast<MAPOBJECTDATA *>(pArg))
		{
			::memcpy(p, pArg, sizeof(MAPOBJECTDATA));

			GetTransform()->Set_Info(INFO::INFO_RIGHT, p->transform.Right);
			GetTransform()->Set_Info(INFO::INFO_UP, p->transform.Up);
			GetTransform()->Set_Info(INFO::INFO_LOOK, p->transform.Look);
			GetTransform()->Set_Info(INFO::INFO_POS, p->transform.Pos);
			SetCategory(p->eCategory);
			SetType(p->iType);
			
			// VIBuffer
			if (FAILED(Add_Components(L"Com_Buffer", SCENE_STATIC, L"Proto_Component_Buffer_CubeColor", (CComponent **)&m_pBuffer, &p->dwColor)))
				return E_FAIL;
		}
		else
		{
			MSG_BOX("CPlacementObject::Set_Component, Something Wrong");
			return E_FAIL;
		}
	}
	else
	{
		// VIBuffer Default
		if (FAILED(Add_Components(L"Com_Buffer", SCENE_STATIC, L"Proto_Component_Buffer_CubeColor", (CComponent **)&m_pBuffer)))
			return E_FAIL;
	}
	return S_OK;
}
