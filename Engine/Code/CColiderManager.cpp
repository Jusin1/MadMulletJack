#include "CColiderManager.h"
#include "CComponentMgr.h"

IMPLEMENT_SINGLETON(CColiderManager)

CColiderManager::CColiderManager()
{
}

CColiderManager::~CColiderManager()
{
}

HRESULT CColiderManager::Add_CollisionGroup(COLLISION_GROUP eCollisionGroup, CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects[eCollisionGroup].push_back(pGameObject);
	pGameObject->Add_Ref();

	return S_OK;
}

void CColiderManager::Remove_CollisionGroup(COLLISION_GROUP eCollisionGroup, CGameObject* pGameObject)
{
	auto iter = m_GameObjects[eCollisionGroup].begin();
	while (iter != m_GameObjects[eCollisionGroup].end())
	{
		if (*iter == pGameObject)
			iter = m_GameObjects[eCollisionGroup].erase(iter);
		else
			++iter;
	}
}

HRESULT CColiderManager::Clear_Colider_Group()
{
	for (_uint i = 0; i < COLLISION_END; ++i)
	{
		for (auto& pGameObject : m_GameObjects[i])
		{
			if (nullptr != pGameObject)
			{
				Safe_Release(pGameObject);
			}
		}
		m_GameObjects[i].clear();
	}

	return S_OK;
}

_bool  CColiderManager::CollisionGroup(COLLISION_GROUP eGroup, class CGameObject* pGameObject, COLLISION_TYPE eCollisionType, _vec3* pOutDistance)
{
	CComponent* Target = nullptr;
	CComponent* DamageOwner = nullptr;

	for (auto& iter : m_GameObjects[eGroup])
	{
			switch (eCollisionType)
			{
			case Engine::CColiderManager::COLLISION_RECT:
				DamageOwner = (CColider_Rect*)pGameObject->Find_Component(TEXT("Com_Collider_Rect"));
				Target = (CColider_Rect*)iter->Find_Component(TEXT("Com_Collider_Rect"));
				if (Target == nullptr)
					continue;
				if (true == (dynamic_cast<CColider_Rect*>(DamageOwner)->Collision_Check((CColider_Rect*)Target, pOutDistance)))
					return true;
				break;
			case Engine::CColiderManager::COLLISION_CUBE:
				DamageOwner = (CColider_Cube*)pGameObject->Find_Component(TEXT("Com_Collider_Cube"));
				Target = (CColider_Cube*)iter->Find_Component(TEXT("Com_Collider_Cube"));
				if (Target == nullptr)
					continue;
				if (true == ((dynamic_cast<CColider_Cube*>(DamageOwner)->Is_Active()) &&
					(dynamic_cast<CColider_Cube*>(Target)->Is_Active()) &&
					(dynamic_cast<CColider_Cube*>(DamageOwner)->Collision_Check((CColider_Cube*)Target, pOutDistance))))
					return true;
				break;
			case Engine::CColiderManager::COLLISION_SPHERE:
				DamageOwner = (CColider_Sphere*)pGameObject->Find_Component(TEXT("Com_Collider_Sphere"));
				Target = (CColider_Sphere*)iter->Find_Component(TEXT("Com_Collider_Sphere"));
				if (Target == nullptr)
					continue;
				if (true == ((dynamic_cast<CColider_Sphere*>(DamageOwner)->Is_Active()) &&
					(dynamic_cast<CColider_Sphere*>(Target)->Is_Active()) &&
					(dynamic_cast<CColider_Sphere*>(DamageOwner)->Collision_Check((CColider_Sphere*)Target, pOutDistance))))
					return true;
				break;
			case Engine::CColiderManager::COLLISION_SPHERE_CUBE:
			{
				auto* pSphere = dynamic_cast<CColider_Sphere*>(pGameObject->Find_Component(TEXT("Com_Collider_Sphere")));
				auto* pCube = dynamic_cast<CColider_Cube*>(iter->Find_Component(TEXT("Com_Collider_Cube")));
				if (!pSphere || !pCube) continue;

				if (pSphere->Is_Active() && pCube->Is_Active() &&
					pSphere->Collision_Check(pCube, pOutDistance))
					return true;
			}
			break;
			case Engine::CColiderManager::COLLISION_CUBE_SPHERE:
			{
				auto* pCube = dynamic_cast<CColider_Cube*>(pGameObject->Find_Component(TEXT("Com_Collider_Cube")));
				auto* pSphere = dynamic_cast<CColider_Sphere*>(iter->Find_Component(TEXT("Com_Collider_Sphere")));
				if (!pCube || !pSphere) continue;

				if (pSphere->Is_Active() && pCube->Is_Active() &&
					pSphere->Collision_Check(pCube, pOutDistance))
					return true;
			}
			break;
			default:
				break;
			}	
	}
	return false;
}

_bool CColiderManager::CollisionGroupSphereTag(COLLISION_GROUP eGroup, CGameObject* pOwner, const wchar_t* ownerSphereTag, const wchar_t* targetSphereTag, _vec3* pOutDistance)
{
	if (!pOwner || !ownerSphereTag || !targetSphereTag) return false;

	auto* pOwnerSphere = dynamic_cast<CColider_Sphere*>(
		pOwner->Find_Component(ownerSphereTag));
	if (!pOwnerSphere || !pOwnerSphere->Is_Active()) return false;

	for (auto& iter : m_GameObjects[eGroup])
	{
		if (!iter) continue;

		auto* pTargetSphere = dynamic_cast<CColider_Sphere*>(
			iter->Find_Component(targetSphereTag));
		if (!pTargetSphere || !pTargetSphere->Is_Active()) continue;

		if (pOwnerSphere->Collision_Check(pTargetSphere, pOutDistance))
			return true;
	}
	return false;
}

_bool CColiderManager::CollisionGroupSphereTagWho(COLLISION_GROUP eGroup, CGameObject* pGameObject, const _tchar* mySphereTag, const _tchar* targetSphereTag, _vec3* pOutDistance, CGameObject*& pWho)
{
	pWho = nullptr;

	auto* pMine = dynamic_cast<CColider_Sphere*>(pGameObject->Find_Component(mySphereTag));
	if (!pMine || !pMine->Is_Active()) return false;

	for (auto& iter : m_GameObjects[eGroup]) {
		auto* pOther = dynamic_cast<CColider_Sphere*>(iter->Find_Component(targetSphereTag));
		if (!pOther || !pOther->Is_Active()) continue;

		if (pMine->Collision_Check(pOther, pOutDistance)) {
			pWho = iter; // ← 충돌한 문 오브젝트
			return true;
		}
	}
	return false;
}

_bool CColiderManager::Collision_Check_Group_Multi(COLLISION_GROUP eGroup, vector<class CGameObject*>& vecDamagedObj, CGameObject* pDamageCauser, COLLISION_TYPE eCollisionType)
{
	CComponent* Target = nullptr;
	CComponent* DamageOwner = nullptr;

	for (auto& iter : m_GameObjects[eGroup])
	{
			switch (eCollisionType)
			{
			case Engine::CColiderManager::COLLISION_RECT:
				DamageOwner = (CColider_Rect*)pDamageCauser->Find_Component(TEXT("Com_Collider_Rect"));
				Target = (CColider_Rect*)iter->Find_Component(TEXT("Com_Collider_Rect"));
				if (Target == nullptr)
					continue;

				if (true == (dynamic_cast<CColider_Rect*>(DamageOwner)->Collision_Check((CColider_Rect*)Target)))
					vecDamagedObj.push_back(iter);
				break;
			case Engine::CColiderManager::COLLISION_CUBE:
				DamageOwner = (CColider_Cube*)pDamageCauser->Find_Component(TEXT("Com_Collider_Cube"));
				Target = (CColider_Cube*)iter->Find_Component(TEXT("Com_Collider_Cube"));
				if (Target == nullptr)
					continue;

				if (true == (dynamic_cast<CColider_Cube*>(DamageOwner)->Collision_Check((CColider_Cube*)Target)))
					vecDamagedObj.push_back(iter);
				break;
			case Engine::CColiderManager::COLLISION_SPHERE:
				DamageOwner = (CColider_Sphere*)pDamageCauser->Find_Component(TEXT("Com_Collider_Sphere"));
				Target = (CColider_Sphere*)iter->Find_Component(TEXT("Com_Collider_Sphere"));
				if (Target == nullptr)
					continue;
				if (true == (dynamic_cast<CColider_Sphere*>(DamageOwner)->Collision_Check((CColider_Sphere*)Target)))
					vecDamagedObj.push_back(iter);
				break;
			default:
				break;
			}
	}

	if (vecDamagedObj.size() > 0)
	{
		return true;
	}
	else {
		return false;
	}
}

_bool CColiderManager::CollisionGroupWho(COLLISION_GROUP eGroup, CGameObject* pGameObject, COLLISION_TYPE eCollisionType, _vec3* pOutDistance, CGameObject*& pWho)
{
	CComponent* Target = nullptr;
	CComponent* DamageOwner = nullptr;

	for (auto& iter : m_GameObjects[eGroup])
	{
		switch (eCollisionType)
		{
		case Engine::CColiderManager::COLLISION_RECT:
			DamageOwner = (CColider_Rect*)pGameObject->Find_Component(TEXT("Com_Collider_Rect"));
			Target = (CColider_Rect*)iter->Find_Component(TEXT("Com_Collider_Rect"));
			if (Target == nullptr)
				continue;
			if (true == (dynamic_cast<CColider_Rect*>(DamageOwner)->Collision_Check((CColider_Rect*)Target, pOutDistance)))
				return true;
			break;
		case Engine::CColiderManager::COLLISION_CUBE:
			DamageOwner = (CColider_Cube*)pGameObject->Find_Component(TEXT("Com_Collider_Cube"));
			Target = (CColider_Cube*)iter->Find_Component(TEXT("Com_Collider_Cube"));
			if (Target == nullptr)
				continue;
			if (true == ((dynamic_cast<CColider_Cube*>(DamageOwner)->Is_Active()) &&
				(dynamic_cast<CColider_Cube*>(Target)->Is_Active()) &&
				(dynamic_cast<CColider_Cube*>(DamageOwner)->Collision_Check((CColider_Cube*)Target, pOutDistance))))
				return true;
			break;
		case Engine::CColiderManager::COLLISION_SPHERE:
			DamageOwner = (CColider_Sphere*)pGameObject->Find_Component(TEXT("Com_Collider_Sphere"));
			Target = (CColider_Sphere*)iter->Find_Component(TEXT("Com_Collider_Sphere"));
			if (Target == nullptr)
				continue;
			if (true == ((dynamic_cast<CColider_Sphere*>(DamageOwner)->Is_Active()) &&
				(dynamic_cast<CColider_Sphere*>(Target)->Is_Active()) &&
				(dynamic_cast<CColider_Sphere*>(DamageOwner)->Collision_Check((CColider_Sphere*)Target, pOutDistance))))
			{
				pWho = iter;
				return true;
			}
			break;
		default:
			break;
		}
	}
	return false;
}

_bool CColiderManager::CollisionGroupPush(COLLISION_GROUP eGroup, CGameObject* pGameObject, COLLISION_TYPE eCollisionType, _float fOffset)
{
	CComponent* Target = nullptr;
	CComponent* DamageOwner = nullptr;
	_vec3 vDistance;

	for (auto& iter : m_GameObjects[eGroup])
	{
		switch (eCollisionType)
		{
		case Engine::CColiderManager::COLLISION_RECT:
			DamageOwner = (CColider_Rect*)pGameObject->Find_Component(TEXT("Com_Collider_Rect"));
			Target = (CColider_Rect*)iter->Find_Component(TEXT("Com_Collider_Rect"));
			if (Target == nullptr)
				continue;
			if (true == (dynamic_cast<CColider_Rect*>(DamageOwner)->Collision_Check((CColider_Rect*)Target, &vDistance)))
			{
				pGameObject->GetTransform()->Set_Info(INFO_POS, pGameObject->GetTransform()->Get_Info(INFO_POS) += (vDistance* fOffset));
				return true;
			}
				
			break;
		case Engine::CColiderManager::COLLISION_CUBE:
			DamageOwner = (CColider_Cube*)pGameObject->Find_Component(TEXT("Com_Collider_Cube"));
			Target = (CColider_Cube*)iter->Find_Component(TEXT("Com_Collider_Cube"));
			if (Target == nullptr)
				continue;
			if (true == ((dynamic_cast<CColider_Cube*>(DamageOwner)->Is_Active()) &&
				(dynamic_cast<CColider_Cube*>(Target)->Is_Active()) &&
				(dynamic_cast<CColider_Cube*>(DamageOwner)->Collision_Check((CColider_Cube*)Target, &vDistance))))
			{
				_vec3 vPos = pGameObject->GetTransform()->Get_Info(INFO_POS);
				pGameObject->GetTransform()->Set_Info(INFO_POS, vPos += (vDistance * fOffset));
				return true;
			}
			break;
		case Engine::CColiderManager::COLLISION_SPHERE:
			DamageOwner = (CColider_Sphere*)pGameObject->Find_Component(TEXT("Com_Collider_Sphere"));
			Target = (CColider_Sphere*)iter->Find_Component(TEXT("Com_Collider_Sphere"));
			if (Target == nullptr)
				continue;
			if (true == ((dynamic_cast<CColider_Sphere*>(DamageOwner)->Is_Active()) &&
				(dynamic_cast<CColider_Sphere*>(Target)->Is_Active()) &&
				(dynamic_cast<CColider_Sphere*>(DamageOwner)->Collision_Check((CColider_Sphere*)Target, &vDistance))))
			{
				_vec3 vPos = pGameObject->GetTransform()->Get_Info(INFO_POS);
				pGameObject->GetTransform()->Set_Info(INFO_POS, vPos += (vDistance * fOffset));
				return true;
			}
			break;
		case Engine::CColiderManager::COLLISION_SPHERE_CUBE:
		{
			auto* pSphere = dynamic_cast<CColider_Sphere*>(pGameObject->Find_Component(TEXT("Com_Collider_Sphere")));
			auto* pCube = dynamic_cast<CColider_Cube*>(iter->Find_Component(TEXT("Com_Collider_Cube")));
			if (!pSphere || !pCube) continue;

			if (pSphere->Is_Active() && pCube->Is_Active() &&
				pSphere->Collision_Check(pCube, &vDistance))
			{
				_vec3 vPos = pGameObject->GetTransform()->Get_Info(INFO_POS);
				pGameObject->GetTransform()->Set_Info(INFO_POS, vPos += (vDistance * fOffset));
				return true;
			}
		}
		break;
		case Engine::CColiderManager::COLLISION_CUBE_SPHERE:
		{
			auto* pCube = dynamic_cast<CColider_Cube*>(pGameObject->Find_Component(TEXT("Com_Collider_Cube")));
			auto* pSphere = dynamic_cast<CColider_Sphere*>(iter->Find_Component(TEXT("Com_Collider_Sphere")));
			if (!pCube || !pSphere) continue;

			if (pSphere->Is_Active() && pCube->Is_Active() &&
				pSphere->Collision_Check(pCube, &vDistance))
			{
				_vec3 vPos = pGameObject->GetTransform()->Get_Info(INFO_POS);
				pGameObject->GetTransform()->Set_Info(INFO_POS, vPos += (vDistance * fOffset));
				return true;
			}
		}
		break;
		default:
			break;
		}
	}
	return false;
}


void CColiderManager::Free()
{
	for (_uint i = 0; i < COLLISION_END; ++i)
		m_GameObjects[i].clear();
}
