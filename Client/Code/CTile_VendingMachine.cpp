#include "pch.h"
#include "Engine_Define.h"
#include "CColiderManager.h"
#include "Clinet_Define.h"
#include "Client_Global.h"
#include "CVIBuffer_Rect.h"
#include "CRenderer.h"
#include "CColider_Sphere.h"
#include "CTexture.h"
#include "CTile_VendingMachine.h"
#include "CGlobal_Info.h"
#include "CPickingManager.h"
#include "CObjectManager.h"
#include "CManagement.h"
#include "CItem.h"

CTile_VendingMachine::CTile_VendingMachine(LPDIRECT3DDEVICE9 pGraphicDevice)
	: CTileBase(pGraphicDevice, TileType::VENDINGMACHINE), m_pColliderSphere(nullptr)
{
}

CTile_VendingMachine::CTile_VendingMachine(const CTile_VendingMachine &rhs)
	: CTileBase(rhs, TileType::VENDINGMACHINE), m_pColliderSphere(nullptr)
{
}

CTile_VendingMachine::~CTile_VendingMachine()
{
}

void CTile_VendingMachine::Free()
{
	CTileBase::Free();
}

CTile_VendingMachine *CTile_VendingMachine::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
	CTile_VendingMachine *pProto = new CTile_VendingMachine(pGraphicDevice);

	if (FAILED(pProto->Ready_GameObject()))
	{
		Safe_Release(pProto);
		MSG_BOX("CTile_VendingMachine::Create, Failed");
		return nullptr;
	}

	return pProto;
}

CGameObject *CTile_VendingMachine::Clone(void *pArg)
{
	CTile_VendingMachine *pClone = new CTile_VendingMachine(*this);

	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CTile_VendingMachine::Clone, Failed");
		Safe_Release(pClone);
	}

	return pClone;
}

HRESULT CTile_VendingMachine::Ready_GameObject()
{
	return CTileBase::Ready_GameObject();
}

HRESULT CTile_VendingMachine::Initialize(void *pArg)
{
	if (FAILED(CTileBase::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Set_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

_int CTile_VendingMachine::Update_GameObject(const _float &fTimeDelta)
{
	if (m_bDead) return DEAD;

	if (!m_bDestroyed)
	{
		CPickingManager::GetInstance()->Remove_PickingGroup(this); // picking 에서 제거
	}
	
	CColiderManager::GetInstance()->Add_CollisionGroup(CColiderManager::COLLISION_TILE_ELECTRIC, this);
	Engine::CGameObject::Update_GameObject(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RENDER_NONALPHA, this);
	return NO_EVENT;
}

void CTile_VendingMachine::LateUpdate_GameObject(const _float &fTimeDelta)
{
	if (m_bDead) return;

	if (!m_bDestroyed)
	{
		CPickingManager::GetInstance()->Add_PickingGroup(this);

		// 테스트용 추후에 몬스터로
		if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_PLAYER, this, CColiderManager::COLLISION_SPHERE, nullptr))
		{
			// player가 kick 하거나 총으로 쏘면
			if (CGlobal_Info::Get_Instance()->Get_PlayerInfo().ePlayerState == KICK)
			{
				// 음료수 생성
				if (FAILED(Create_Drink()))
					return;
				// destory
				m_bDestroyed = true;
			}
		}
	}

	__super::LateUpdate_GameObject(fTimeDelta);
}

_bool CTile_VendingMachine::Picking(_vec3* PickingPoint)
{
	return m_pBuffer->Picking(m_pTransformCom, PickingPoint);
}

void CTile_VendingMachine::PickingTrue()
{
	// 음료수 생성`
	if (!m_bDestroyed)
	{
		if (FAILED(Create_Drink()))
			return;

		m_bDestroyed = true;
	}
}

HRESULT CTile_VendingMachine::Create_Drink()
{
	CItem::ITEMINFO tInfo{};
	
	_vec3 vPos = GetTransform()->Get_Info(INFO_POS); // 시작 위치
	vPos.y += 0.1f; // 살짝 위로

	_vec3 vLook = GetTransform()->Get_Info(INFO_LOOK); // 자판기 보다 살짝 앞으로 보내기 위해
	D3DXVec3Normalize(&vLook, &vLook); // 정규화
	vPos = vPos - vLook * 0.1f;

	tInfo.vStartPos = vPos;
	tInfo.eWeapon = WP_DOPING;
	if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Prototype_GameObject_Item", 
		CManagement::GetInstance()->Get_CurrentSceneIdx(), L"GameLogic_Layer",&tInfo)))
		return E_FAIL;

	return S_OK;
}

void CTile_VendingMachine::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTransformCom->Apply_WorldMatrix();
	m_pTexture->Set_Texture(m_bDestroyed);
	m_pBuffer->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CTile_VendingMachine::Set_Component(void *pArg)
{
	if (FAILED(Add_Components(L"Com_Texture", SCENE_STATIC, L"Proto_VendingMachine", (CComponent **)&m_pTexture)))
		return E_FAIL;

	CColider_Sphere::COLLINFO CollSphereInfo;
	ZeroMemory(&CollSphereInfo, sizeof(CColider_Sphere::COLLINFO));
	CollSphereInfo.fRadius = 0.5f;
	CollSphereInfo.vOffset = _vec3(0.f, 0.f, 0.f);

	// Colider_Sphere
	if (FAILED(Add_Components(L"Com_Collider_Sphere", SCENE_STATIC, L"Proto_Colider_Sphere", (CComponent **)&m_pColliderSphere, &CollSphereInfo)))
		return E_FAIL;

	m_pColliderSphere->Set_Transform(m_pTransformCom);
	m_pColliderSphere->Update_ColliderSphere();
	m_pTransformCom->Set_Info(INFO::INFO_POS, m_pTransformCom->Get_Info(INFO::INFO_POS) + _vec3{ 0.f, 0.25f, 0.f });
	m_pTransformCom->Set_Scale(1.f, 1.5f, 1.f);

	return S_OK;
}
