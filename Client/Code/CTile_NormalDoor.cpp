#include "pch.h"
#include "Client_Global.h"
#include "CColiderManager.h"
#include "CObjectManager.h"
#include "CTile_Deco.h"
#include "CColider_Sphere.h"
#include "CMapFactory.h"
#include "CTile_NormalDoor.h"
#include "CPickingManager.h"
#include "CVIBuffer_Rect.h"
#include "Sound_Manager.h"

CTile_NormalDoor::CTile_NormalDoor(LPDIRECT3DDEVICE9 pGraphicDevice)
	: CTileBase(pGraphicDevice, TileType::NORMALDOOR)
	, m_pColiderSphere(nullptr)
	, m_pColiderSphereOpen(nullptr) // 추가
{
	m_pDoors.reserve(2);
}

CTile_NormalDoor::CTile_NormalDoor(const CTile_NormalDoor &rhs)
	: CTileBase(rhs, TileType::NORMALDOOR), m_pColiderSphere(nullptr)
{
	m_pDoors.reserve(2);
}

CTile_NormalDoor::~CTile_NormalDoor()
{
}

void CTile_NormalDoor::Free()
{
	CTileBase::Free();
}

CGameObject *CTile_NormalDoor::Clone(void *pArg)
{
	CTile_NormalDoor *pClone = new CTile_NormalDoor(*this);

	if (FAILED(pClone->Initialize(pArg)))
	{
		Safe_Release(pClone);
		MSG_BOX("CTile_NormalDoor::Clone, Failed");
		return nullptr;
	}

	return pClone;
}

CTile_NormalDoor *CTile_NormalDoor::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
	CTile_NormalDoor *pProto = new CTile_NormalDoor(pGraphicDevice);

	if (FAILED(pProto->Ready_GameObject()))
	{
		Safe_Release(pProto);
		MSG_BOX("CTile_NormalDoor::Create, Failed");
		return nullptr;
	}

	return pProto;
}

HRESULT CTile_NormalDoor::Ready_GameObject()
{
	return CTileBase::Ready_GameObject();
}

HRESULT CTile_NormalDoor::Initialize(void *pArg)
{
	if (FAILED(CTileBase::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Set_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

_int CTile_NormalDoor::Update_GameObject(const _float &fTimeDelta)
{
	if (m_bDead) return DEAD;

	if (m_fAngle < m_fTargetAngle) {
		m_fAngle += fTimeDelta * 500.f;
		if (m_fAngle - m_fTargetAngle > 0.f) m_fAngle = m_fTargetAngle;
		PivotRotate();
	}

	CPickingManager::GetInstance()->Remove_PickingGroup(this); // picking 추가

	if (!m_bOpend)
	{
		CColiderManager::GetInstance()->Add_CollisionGroup(CColiderManager::COLLISION_DOOR, this);
	}
	
	return __super::Update_GameObject(fTimeDelta);
}

void CTile_NormalDoor::LateUpdate_GameObject(const _float &fTimeDelta)
{
	if (m_bDead) return;

	if (!m_bOpend)
	{
		CPickingManager::GetInstance()->Add_PickingGroup(this); // 픽킹 그룹에 추가

		if (CColiderManager::GetInstance()->CollisionGroup(
			CColiderManager::COLLISION_PLAYER, this,
			CColiderManager::COLLISION_SPHERE, nullptr))
		{
			if (m_fTargetAngle < 84.99f) m_fTargetAngle = 84.99f;
			m_bOpend = true;

			if (m_pColiderSphere) {
				m_pColiderSphere->Set_Active(false);
				m_pColiderSphere->Update_ColliderSphere();
			}
			if (m_pColiderSphereOpen) {
				m_pColiderSphereOpen->Set_Active(true);
				m_pColiderSphereOpen->Update_ColliderSphere();
			}
		}
	}

	__super::LateUpdate_GameObject(fTimeDelta);
}

void CTile_NormalDoor::Render_GameObject()
{
#ifdef _DEBUG
	if (g_ColiderRender) {
		if (m_pColiderSphere)     m_pColiderSphere->Render_ColliderSphere();
		if (m_pColiderSphereOpen) m_pColiderSphereOpen->Render_ColliderSphere();
	}
#endif
}

HRESULT CTile_NormalDoor::Set_Component(void *pArg)
{
	if (pArg)
	{
		if (MAPOBJECTDATA *pData = reinterpret_cast<MAPOBJECTDATA *>(pArg))
		{
			// 왼쪽
			wstring originName = pData->texture.OriginComponentName;
			pData->texture.OriginComponentName = originName + L"_1";
			_matrix vLeftTransformData;
			::D3DXMatrixIdentity(&vLeftTransformData);
			vLeftTransformData._41 -= 0.25f;
			vLeftTransformData *= (*m_pTransformCom->Get_World());
			::memcpy(&pData->transform.Right[0], vLeftTransformData.m[0], sizeof(_vec3));
			::memcpy(&pData->transform.Up[0], vLeftTransformData.m[1], sizeof(_vec3));
			::memcpy(&pData->transform.Look[0], vLeftTransformData.m[2], sizeof(_vec3));
			::memcpy(&pData->transform.Pos[0], vLeftTransformData.m[3], sizeof(_vec3));
			if (FAILED(CObjectManager::GetInstance()->Add_GameObject(
				L"Prototype_GameObject_DecoTile",
				CMapFactory::GetInstance()->GetTargetSceneIndex(),
				L"Tile_Layer",
				pData)))
				return E_FAIL;
			
			m_pDoors.push_back(static_cast<CTile_Deco*>(CObjectManager::GetInstance()->Get_ObjectList(CMapFactory::GetInstance()->GetTargetSceneIndex(), L"Tile_Layer")->back()));

			// 오른쪽
			pData->texture.OriginComponentName = originName + L"_2";
			::D3DXMatrixIdentity(&vLeftTransformData);
			vLeftTransformData._41 += 0.25f;
			vLeftTransformData *= (*m_pTransformCom->Get_World());
			::memcpy(&pData->transform.Right[0], vLeftTransformData.m[0], sizeof(_vec3));
			::memcpy(&pData->transform.Up[0], vLeftTransformData.m[1], sizeof(_vec3));
			::memcpy(&pData->transform.Look[0], vLeftTransformData.m[2], sizeof(_vec3));
			::memcpy(&pData->transform.Pos[0], vLeftTransformData.m[3], sizeof(_vec3));
			if (FAILED(CObjectManager::GetInstance()->Add_GameObject(
				L"Prototype_GameObject_DecoTile",
				CMapFactory::GetInstance()->GetTargetSceneIndex(),
				L"Tile_Layer",
				pData)))
				return E_FAIL;

			m_pDoors.push_back(static_cast<CTile_Deco *>(CObjectManager::GetInstance()->Get_ObjectList(CMapFactory::GetInstance()->GetTargetSceneIndex(), L"Tile_Layer")->back()));
			m_pDoors[0]->GetTransform()->Set_Scale(1.f, 2.f, 1.f);
			m_matInitDoors[0] = *m_pDoors[0]->GetTransform()->Get_World();
			m_pDoors[1]->GetTransform()->Set_Scale(1.f, 2.f, 1.f);
			m_matInitDoors[1] = *m_pDoors[1]->GetTransform()->Get_World();

			// 닫혀있는 문 충돌체
			CColider_Sphere::COLLINFO CollSphereInfo;
			ZeroMemory(&CollSphereInfo, sizeof(CColider_Sphere::COLLINFO));
			CollSphereInfo.fRadius = 0.5f;
			CollSphereInfo.vOffset = _vec3(0.f, 0.f, 0.f);    // 중심 오프셋 없음

			// Colider_Sphere
			if (FAILED(Add_Components(L"Com_Collider_Sphere", SCENE_STATIC, L"Proto_Colider_Sphere", (CComponent **)&m_pColiderSphere, &CollSphereInfo)))
				return E_FAIL;
			m_pColiderSphere->Set_Transform(m_pTransformCom);
			m_pColiderSphere->Update_ColliderSphere();

			// 열려있는 문 충돌체
			CColider_Sphere::COLLINFO openDesc{};
			openDesc.fRadius = 0.3f;              
			openDesc.vOffset = _vec3(0.f, 0.f, 1.5f);
			if (FAILED(Add_Components(
				L"Com_Collider_Sphere_Open", SCENE_STATIC, L"Proto_Colider_Sphere",
				(CComponent**)&m_pColiderSphereOpen, &openDesc)))
				return E_FAIL;
			m_pColiderSphereOpen->Set_Transform(m_pTransformCom);
			m_pColiderSphereOpen->Set_Active(false);   
			m_pColiderSphereOpen->Update_ColliderSphere();
		}
		else
		{
			MSG_BOX("CTileBase::Set_Component, Something wrong");
			return E_FAIL;
		}
	}
	else
	{
		MSG_BOX("CTileBase::Set_Component, No Data");
		return E_FAIL;
	}

	return S_OK;
}

void CTile_NormalDoor::PivotRotate()
{
	auto lambda_rotation =
	[&](CTransform *pDoorTransform, const _matrix &matWorld, _vec3 vDoorPivotLocal, _float fAngle)->void
	{
		_matrix matLocalPivot_negative, matScale, matRotation, matLocalPivot, matWorldPos, matWorldRot;

		// 월드 분해
		_vec3 vWorldPos;
		_vec3 vWorldScale;
		D3DXQUATERNION qQuaternion;
		::D3DXMatrixDecompose(&vWorldScale, &qQuaternion, &vWorldPos, &matWorld);
		::D3DXMatrixRotationQuaternion(&matWorldRot, &qQuaternion);

		::D3DXMatrixTranslation(&matLocalPivot_negative, -vDoorPivotLocal.x, -vDoorPivotLocal.y, -vDoorPivotLocal.z);
		::D3DXMatrixScaling(&matScale, vWorldScale.x, vWorldScale.y, vWorldScale.z);
		::D3DXMatrixRotationY(&matRotation, D3DXToRadian(fAngle));
		::D3DXMatrixTranslation(&matLocalPivot, vDoorPivotLocal.x, vDoorPivotLocal.y, vDoorPivotLocal.z);
		::D3DXMatrixTranslation(&matWorldPos, vWorldPos.x, vWorldPos.y, vWorldPos.z);

		

		// 현재 Tile의 Local 중심 좌표는 Cetner (0.5, 0.5, 0)
		// matLocalPivot_negative => 문의 회전축을 로컬 원점(0,0,0)으로 옮김
		// matScale => 원점 기준 스케일
		// matRotation => 원점 기준 Y축 회전
		// matLocalPivot => 원래 중점으로 돌림 ( 뺀만큼 더하기 )
		// matWorldRot = > 월드 회전 적용
		// matWorldPos = > 월드위치로 옮김
		_matrix matResult = matLocalPivot_negative * matScale * matRotation * matLocalPivot * matWorldRot * matWorldPos;

		_vec3 vRight, vUp, vLook, vPos;
		::memcpy(&vRight, &matResult.m[0][0], sizeof(_vec3));
		::memcpy(&vUp, &matResult.m[1][0], sizeof(_vec3));
		::memcpy(&vLook, &matResult.m[2][0], sizeof(_vec3));
		::memcpy(&vPos, &matResult.m[3][0], sizeof(_vec3));

		pDoorTransform->Set_Info(INFO_RIGHT, vRight);
		pDoorTransform->Set_Info(INFO_UP, vUp);
		pDoorTransform->Set_Info(INFO_LOOK, vLook);
		pDoorTransform->Set_Info(INFO_POS, vPos);
	};
	
	lambda_rotation(m_pDoors[0]->GetTransform(), m_matInitDoors[0], _vec3{-0.5f, 0.f, 0.f}, m_fAngle * -1.f);
	lambda_rotation(m_pDoors[1]->GetTransform(), m_matInitDoors[1], _vec3{ 0.5f, 0.f, 0.f }, m_fAngle);
}

_bool CTile_NormalDoor::Picking(_vec3* PickingPoint)
{
	return m_pBuffer->Picking(m_pTransformCom, PickingPoint);
}

void CTile_NormalDoor::PickingTrue()
{
	//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Sounds\eunbi\player\kick\door.open-001.wav"
	CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/eunbi/player/kick/door.open-001.wav", SOUND_OBJECT, 2.3f, false);
	if (m_fTargetAngle < 84.99f) m_fTargetAngle = 84.99f;
	m_bOpend = true;
}
