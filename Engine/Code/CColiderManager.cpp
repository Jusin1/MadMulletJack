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
			default:
				break;
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


void CColiderManager::Free()
{
	for (_uint i = 0; i < COLLISION_END; ++i)
		m_GameObjects[i].clear();
}
