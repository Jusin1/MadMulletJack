#include "pch.h"
#include "CGridPanel.h"
#include "CTexture.h"
#include "Clinet_Define.h"
#include "CComponentMgr.h"
#include "CTransform.h"
#include "CVIBuffer_GridPanelBase.h"
#include "CRenderer.h"

CGridPanel::CGridPanel(LPDIRECT3DDEVICE9 pGraphicDevice)
	: Engine::CGameObject(pGraphicDevice), m_pBuffer(nullptr)
	, m_pTexture(nullptr), m_eType(WallType::NONE)
{
}

CGridPanel::CGridPanel(const CGridPanel &rhs)
	: Engine::CGameObject(rhs), m_pBuffer(nullptr), m_pTexture(nullptr), m_eType(WallType::NONE)
{
}

CGridPanel::~CGridPanel()
{
}

void CGridPanel::Free()
{
	CGameObject::Free();
}

CGridPanel *CGridPanel::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CGridPanel *gridPanel = new CGridPanel(pGraphicDev);

	if (FAILED(gridPanel->Ready_GameObject()))
	{
		Safe_Release(gridPanel);
		MSG_BOX("CGridPanel::Create, Failed");
		return nullptr;
	}

	return gridPanel;
}

CGameObject *CGridPanel::Clone(void *pArg)
{
	CGridPanel *pInstance = new CGridPanel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CGridPanel::Clone, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CGridPanel::Ready_GameObject()
{
	if (FAILED(CGameObject::Ready_GameObject()))
		return E_FAIL;

	m_vPosition = { 0.f, 0.f, 0.f };

	return S_OK;
}

HRESULT CGridPanel::Initialize(void *pArg)
{
	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Set_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

_int CGridPanel::Update_GameObject(const _float &fTimeDelta)
{
	if (m_bDead)
		return DEAD;

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this);

	return NO_EVENT;
}

void CGridPanel::LateUpdate_GameObject(const _float &fTimeDelta)
{
	if (m_bDead)
		return;

	Update_Position(m_pTransformCom->Get_Info(INFO_POS));

	Engine::CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CGridPanel::Render_GameObject()
{
	if (m_bDead)
		return;

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

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
}

HRESULT CGridPanel::Set_Component(void *pArg)
{
	if (pArg)
	{
		if (MAPOBJECTDATA *p = reinterpret_cast<MAPOBJECTDATA *>(pArg))
		{
			// Type
			SetType(static_cast<WallType>(p->iType));

			// VIBuffer
			switch (GetType())
			{
			case WallType::WALL_HOR:
			{
				if (FAILED(Add_Components(L"Com_Buffer", SCENE_STATIC, L"Proto_Buffer_GridPanel_Horizon", (CComponent **)&m_pBuffer, &(p->panelBuffer))))
					return E_FAIL;
			} break;
			case WallType::WALL_VER:
			{
				if (FAILED(Add_Components(L"Com_Buffer", SCENE_STATIC, L"Proto_Buffer_GridPanel_Vertical", (CComponent **)&m_pBuffer, &(p->panelBuffer))))
					return E_FAIL;
			} break;
			case WallType::INCLINE:
			case WallType::FLOOR:
			case WallType::CEILING:
			{
				if (FAILED(Add_Components(L"Com_Buffer", SCENE_STATIC, L"Proto_Buffer_GridPanel_Normal", (CComponent **)&m_pBuffer, &(p->panelBuffer))))
					return E_FAIL;
			} break;
			}

			// Texture
			if (FAILED(Add_Components(L"Com_Texture", SCENE_STATIC, p->texture.OriginComponentName.c_str(), (CComponent **)&m_pTexture)))
				return E_FAIL;

			// Transform
			GetTransform()->Set_Info(INFO::INFO_RIGHT, p->transform.Right);
			GetTransform()->Set_Info(INFO::INFO_UP, p->transform.Up);
			GetTransform()->Set_Info(INFO::INFO_LOOK, p->transform.Look);
			GetTransform()->Set_Info(INFO::INFO_POS, p->transform.Pos);

			m_pTransformCom->Apply_WorldMatrix();
		}
		else
		{
			MSG_BOX("CGridPanel::Set_Component, Something Wrong");
			return E_FAIL;
		}
	}
	else
	{
		MSG_BOX("CGridPanel::Set_Component, No data");
		return E_FAIL;
	}

	return S_OK;
}