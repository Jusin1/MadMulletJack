#include "pch.h"
#include "Engine_Define.h"
#include "Client_Global.h"
#include "CPrefab.h"
#include "CMonster_Dron.h"
#include "CObjectManager.h"
#include "CGameObject.h"
#include "CPrefabRecycleSystem.h"

IMPLEMENT_SINGLETON(CPrefabRecycleSystem)

CPrefabRecycleSystem::CPrefabRecycleSystem()
{
}

CPrefabRecycleSystem::~CPrefabRecycleSystem()
{
	Free();
}

void CPrefabRecycleSystem::Free()
{
}

void CPrefabRecycleSystem::Update()
{
	const _float fStandard_Z = m_pPrefabList.front()->GetTransform()->Get_Info(INFO::INFO_POS).z;
	const _float fPlayerPosition_Z = m_pPlayer->Get_Position().z;

	if (fPlayerPosition_Z - fStandard_Z >= m_fDistance)
		Sorting();
}

HRESULT CPrefabRecycleSystem::Set_PrefabList(list<CGameObject *> *p)
{
	if (!p)
	{
		MSG_BOX("CPrefabRecycleSystem::Set_PrefabList, list is invalid");
		return E_FAIL;
	}
	
	m_pPrefabList = *p;
	m_PrefabCount = m_pPrefabList.size();

	if (m_PrefabCount <= 0 || m_PrefabCount > 100)
	{
		MSG_BOX("CPrefabRecycleSystem::Set_PrefabList, PrefabCount is invalid");
		return E_FAIL;
	}

	list<CGameObject *>::iterator itr{ m_pPrefabList.begin() };

	CGameObject *pFirst{ *itr };
	CGameObject *pSecond{ *(++itr) };

	if (!pFirst || !pSecond)
	{
		MSG_BOX("CPrefabRecycleSystem::Set_PrefabList, Cant Bind Object");
		return E_FAIL;
	}

	m_fDistance = std::fabs(pFirst->GetTransform()->Get_Info(INFO::INFO_POS).z - pSecond->GetTransform()->Get_Info(INFO::INFO_POS).z);

	RandomSpawnMonster(pSecond->GetTransform()->Get_Info(INFO::INFO_POS));
	return S_OK;
}

void CPrefabRecycleSystem::Sorting()
{
	CGameObject *pWasFront = m_pPrefabList.front();
	CGameObject *pBack = m_pPrefabList.back();
	m_pPrefabList.pop_front();
	_vec3 vNewPosition = pBack->GetTransform()->Get_Info(INFO::INFO_POS);
	vNewPosition.z += m_fDistance;
	pWasFront->GetTransform()->Set_Info(INFO::INFO_POS, vNewPosition);
	static_cast<CPrefab *>(pWasFront)->Set_ChildrensMatrix();
	m_pPrefabList.push_back(pWasFront);

	RandomSpawnMonster(vNewPosition);
}

void CPrefabRecycleSystem::RandomSpawnMonster(const _vec3 &vPos)
{
	for (int i = 0; i < 9; ++i)
	{
		const _float fRand1 = Rand_Float(-5.f, 5.f);
		const _float iRand = Rand_Int(-10, 15);
		const _float iRand2 = Rand_Int(1, 3);
		const _vec3 SpawnPosition = vPos + _vec3{fRand1 + 12.f, (_float)iRand2, (_float)iRand};
		CGameObject *pGo = CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_Monster_Drone", SCENE_CAR, L"Monster_Layer");
		pGo->GetTransform()->Set_Info(INFO::INFO_POS, SpawnPosition);
		CMonster_Dron *pDron = static_cast<CMonster_Dron *>(pGo);
		pDron->Set_DetectRadius(36.f);
		pDron->Set_LoseRadius(64.f);
	}
}
