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

// 충돌 그룹에 오브젝트 등록
HRESULT CColiderManager::Add_CollisionGroup(COLLSION_GROUP _collisionGroup, CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects[_collisionGroup].push_back(pGameObject);
	Add_Ref(); // 참조 카운트 증가(안전하게 보관하기 위해)

	return S_OK;
}

// 충돌 그룹에서 특정 오브젝트 삭제
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

// 충돌 그룹 초기화 및 참조 해제
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

// 특정 그룹과 오브젝트 간의 충돌 검사
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
