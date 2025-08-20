#include "pch.h"
#include "CGridPanel.h"
#include "CTexture.h"
#include "Clinet_Define.h"
#include "Client_Global.h"
#include "CColider_Cube.h"
#include "CComponentMgr.h"
#include "CColiderManager.h"
#include "CTransform.h"
#include "CVIBuffer_GridPanelBase.h"
#include "CRenderer.h"

CGridPanel::CGridPanel(LPDIRECT3DDEVICE9 pGraphicDevice)
	: Engine::CGameObject(pGraphicDevice), m_pBuffer(nullptr)
	, m_pTexture(nullptr), m_eType(WallType::NONE), m_eCategory(ObjectCategory::WALL)
	, m_pColliderCube(nullptr)
{
}

CGridPanel::CGridPanel(const CGridPanel &rhs)
	: Engine::CGameObject(rhs), m_pBuffer(nullptr), m_pTexture(nullptr), m_eType(WallType::NONE), m_eCategory(ObjectCategory::WALL)
	, m_pColliderCube(nullptr)
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

	// 캐싱용 월드행렬 역행렬
	D3DXMatrixInverse(&m_matInverse, nullptr, GetTransform()->Get_World());

	return S_OK;
}

_int CGridPanel::Update_GameObject(const _float &fTimeDelta)
{
	if (m_bDead)
		return DEAD;

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(RENDER_NONALPHA, this);

	Update_CollisionGroup();
	return NO_EVENT;
}

void CGridPanel::LateUpdate_GameObject(const _float &fTimeDelta)
{
	if (m_bDead)
		return;

	Engine::CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CGridPanel::Render_GameObject()
{
	if (m_bDead)
		return;

	m_pTransformCom->Apply_WorldMatrix();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTexture->Set_Texture();
	m_pBuffer->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

#ifdef _DEBUG
	if (g_ColiderRender && m_pColliderCube)
		m_pColliderCube->Render_ColliderBox();
#endif
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

			// Texture
			if (FAILED(Add_Components(L"Com_Texture", SCENE_STATIC, p->texture.OriginComponentName.c_str(), (CComponent **)&m_pTexture)))
				return E_FAIL;

			// Transform
			GetTransform()->Set_Info(INFO::INFO_RIGHT, p->transform.Right);
			GetTransform()->Set_Info(INFO::INFO_UP, p->transform.Up);
			GetTransform()->Set_Info(INFO::INFO_LOOK, p->transform.Look);
			GetTransform()->Set_Info(INFO::INFO_POS, p->transform.Pos);
			m_pTransformCom->Apply_WorldMatrix();

			// VIBuffer
			switch (GetType())
			{
			case WallType::WALL_HOR:
			{
				if (FAILED(Add_Components(L"Com_Buffer", SCENE_STATIC, L"Proto_Buffer_GridPanel_Horizon", (CComponent **)&m_pBuffer, &(p->panelBuffer))))
					return E_FAIL;
			} break;
			case WallType::WALL_VER:
			case WallType::WALL_SLIDE:
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

			_float fWidth{ 0.f };
			_float fHeight{ 0.f };
			Engine::CColider_Cube::COLLRECTDESC tDesc;
			tDesc.bMapObject = true;
			// Collider
			switch (GetType())
			{
			case WallType::WALL_HOR:
			{
				fWidth = (m_pBuffer->GetData()->dwCountX - 1) * m_pBuffer->GetData()->dwInterval;
				fHeight = (m_pBuffer->GetData()->dwCountY - 1) * m_pBuffer->GetData()->dwInterval;
				tDesc.fRadiusX = fWidth * 0.5f;
				tDesc.fRadiusY = fHeight * 0.5f;
				tDesc.fOffSetX = fWidth * 0.5f;
				tDesc.fOffSetY = fHeight * 0.5f;
				tDesc.fOffsetZ = 0.5f;

				if (FAILED(Add_Components(L"Com_Collider", SCENE_STATIC, L"Proto_Colider_Cube", (CComponent **)&m_pColliderCube, &tDesc)))
					return E_FAIL;
			} break;
			case WallType::WALL_VER:
			{
				fWidth = (m_pBuffer->GetData()->dwCountZ - 1) * m_pBuffer->GetData()->dwInterval;
				fHeight = (m_pBuffer->GetData()->dwCountY - 1) * m_pBuffer->GetData()->dwInterval;
				tDesc.fRadiusZ = fWidth * 0.5f;
				tDesc.fRadiusY = fHeight * 0.5f;
				tDesc.fOffSetX = -0.5f;
				tDesc.fOffSetY = fHeight * 0.5f;
				tDesc.fOffsetZ = fWidth * 0.5f;

				if (FAILED(Add_Components(L"Com_Collider", SCENE_STATIC, L"Proto_Colider_Cube", (CComponent **)&m_pColliderCube, &tDesc)))
					return E_FAIL;
			} break;
			case WallType::CEILING:
			{
				fWidth = (m_pBuffer->GetData()->dwCountX - 1) * m_pBuffer->GetData()->dwInterval;
				fHeight = (m_pBuffer->GetData()->dwCountZ - 1) * m_pBuffer->GetData()->dwInterval;
				tDesc.fRadiusX = fWidth * 0.5f;
				tDesc.fRadiusZ = fHeight * 0.5f;
				tDesc.fOffSetX = fWidth * 0.5f ;
				tDesc.fOffSetY = -0.5f;
				tDesc.fOffsetZ = fHeight * 0.5f;

				if (FAILED(Add_Components(L"Com_Collider", SCENE_STATIC, L"Proto_Colider_Cube", (CComponent **)&m_pColliderCube, &tDesc)))
					return E_FAIL;
			} break;
			}

			if (m_pColliderCube)
			{
				m_pColliderCube->Set_Transform(m_pTransformCom);
				m_pColliderCube->Initialize_Matrix_WithDescription();
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
		MSG_BOX("CGridPanel::Set_Component, No data");
		return E_FAIL;
	}

	return S_OK;
}

void CGridPanel::Update_CollisionGroup()
{
	switch (GetType())
	{
	case WallType::WALL_HOR:
	case WallType::WALL_VER:
	{
		CColiderManager::GetInstance()->Add_CollisionGroup(CColiderManager::COLLISION_WALL, this);
	} break;
	case WallType::CEILING:
	{
		CColiderManager::GetInstance()->Add_CollisionGroup(CColiderManager::COLLISION_CEILING, this);
	} break;
	}
}

void CGridPanel::Set_Collider()
{
	if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_MONSTER, this, CColiderManager::COLLISION_SPHERE, nullptr))
	{
		_vec3 vPosition = m_pTransformCom->Get_Info(INFO_POS);

		// Dash_attack 중일때 몬스터와 충돌하면 
		if (m_tPlayerInfo.ePlayerState == DASH_ATTACK)
		{
			// wap2에 따라 state 변경
			switch (m_tPlayerInfo.eWeapon2)
			{
			case WP_KICK:
				m_tPlayerInfo.ePlayerState = KICK;
				//m_pTransformCom->Move_PosDown(0.5);
				break;

			case WP_KNIFE:
			case WP_BOOK:
				m_tPlayerInfo.ePlayerState = ATTACK_INSTANT;
				break;
			}
		}

		// Dash attack이 아니면 hit
		/*else
		{
			m_tPlayerInfo.ePlayerState = HIT;
		}*/

	}
}
