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

void CPickingManager::Clear_Picking()
{
	m_PickingList.clear();
}

void CPickingManager::Add_PickingGroup(CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return;

	m_PickingList.push_back(pGameObject);
}

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

	for (auto& pGameObject : m_PickingList)
	{
		if (pGameObject->Picking(&vPos))
		{
			vecPicked.push_back(pGameObject);
			vecPos.push_back(vPos);
		}
	}

	if (!vecPicked.empty())
	{
		_vec3 vecNearPos;
		int NearNum = 0;
		for (_uint i = 0; i < vecPos.size(); ++i)
		{
			if (vecPos[i].z <= vecNearPos.z || i == 0)
			{
				vecNearPos = vecPos[i];
				NearNum = i;
			}
		}

		m_vPickingPos = vecNearPos;
		vecPicked[NearNum]->PickingTrue();

		return true;
	}

	return false;
}

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