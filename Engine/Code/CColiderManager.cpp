#include "CColiderManager.h"
#include "CComponentMgr.h"

IMPLEMENT_SINGLETON(CColiderManager)

CColiderManager::CColiderManager()
{

}

CColiderManager::~CColiderManager()
{
	Free();
}

HRESULT CColiderManager::Add_CollisionGroup(COLLSION_GROUP _collisionGroup, CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects[_collisionGroup].push_back(pGameObject);
	Add_Ref();

	return S_OK;
}

void CColiderManager::Remove_CollisionGroup(COLLSION_GROUP _collisionGroup, CGameObject* pGameObject)
{
	auto iter = m_GameObjects[_collisionGroup].begin();
	while (iter != m_GameObjects[_collisionGroup].end())
	{
		if (*iter == pGameObject)
			iter = m_GameObjects[_collisionGroup].erase(iter);
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

_bool  CColiderManager::CollisionGroup(COLLSION_GROUP _collisionGroup, class CGameObject* pGameObject, COLLISION_TYPE _collisionType, _vec3* pOutDistance)
{
	CComponent* Target = nullptr;
	CComponent* DamageOwner = nullptr;

	for (auto& iter : m_GameObjects[_collisionGroup])
	{
		if (nullptr != iter)
		{
			switch (_collisionType)
			{
			case Engine::CColiderManager::COLLISION_RECT:
				DamageOwner = (CColider_Rect*)pGameObject->Find_Component(L"Com_Colider_Rect");
				Target = (CColider_Rect*)iter->Find_Component(L"Com_Collider_Rect");
				if (Target == nullptr)
					continue;
				if (true == (dynamic_cast<CColider_Rect*>(DamageOwner)->Collision_Check((CColider_Rect*)Target, pOutDistance)))
					return true;
				break;
			default:
				break;
			}

		}
	}

	return false;
}

void CColiderManager::Free()
{
	for (_uint i = 0; i < COLLISION_END; ++i)
		m_GameObjects[i].clear();
}
