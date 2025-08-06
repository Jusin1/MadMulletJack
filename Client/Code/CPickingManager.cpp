#include "pch.h"
#include "CPickingManager.h"
#include "CTransform.h"

IMPLEMENT_SINGLETON(CPickingManager)

CPickingManager::CPickingManager()
{

}

CPickingManager::~CPickingManager()
{
}

HRESULT CPickingManager::Ready_Picking()
{

	return S_OK;
}

// 모든 픽킹 대상 제거
void CPickingManager::Clear_Picking()
{
	m_PickingList.clear();
}

// 픽킹 대상 추가
void CPickingManager::Add_PickingGroup(CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return;

	m_PickingList.push_back(pGameObject);
}

// 픽킹 대상 제거
void CPickingManager::Remove_PickingGroup(CGameObject* pGameObject)
{
	auto iter = m_PickingList.begin();
	while (iter != m_PickingList.end())
	{
		if (*iter == pGameObject)
			iter = m_PickingList.erase(iter);
		else
			++iter;
	}
}

// 마우스 클릭 시 가장 가까운 오브젝트를 픽킹
_bool CPickingManager::Picking()
{
	if(m_bMouseInUI)
		return false;

	// 마우스 왼쪽 버튼이 눌리지 않으면 픽킹 하지 않음
	if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
		return false;

	vector<CGameObject*> vecPicked;
	vector<_vec3> vecPos;
	_vec3 vPos;

	// 모든 오브젝트 중 레이와 충돌한 것 수집
	for (auto& pGameObject : m_PickingList)
	{
		if (pGameObject->Picking(&vPos))
		{
			vecPicked.push_back(pGameObject);
			vecPos.push_back(vPos);
		}
	}

	// 하나라도 충돌한 게 있다면
	if (!vecPicked.empty())
	{
		_vec3 vecNearPos;
		int NearNum = 0;
		// 가장 가까운 오브젝트 찾기 (z값 기준)
		for (_uint i = 0; i < vecPos.size(); ++i)
		{
			if (vecPos[i].z <= vecNearPos.z || i == 0)
			{
				vecNearPos = vecPos[i];
				NearNum = i;
			}
		}

		m_vPickingPos = vecNearPos;
		vecPicked[NearNum]->PickingTrue(); // 해당 오브젝트에 알림

		return true;
	}

	return false;
}

// 현재 픽킹된 오브젝트 해제
void CPickingManager::Release_PickingObject()
{
	if (nullptr != m_pPickingObject)
	{
		Safe_Release(m_pPickingObject);
	}
}

void CPickingManager::Free()
{
	m_PickingList.clear();
}