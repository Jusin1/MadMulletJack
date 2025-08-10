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
    if (m_bMouseInUI) return false;

    if (!(GetAsyncKeyState(VK_LBUTTON) & 0x0001))
        return false;

    vector<CGameObject*> vecPicked;
    vector<_vec3> vecPos;

    for (auto it = m_PickingList.begin(); it != m_PickingList.end(); )
    {
        CGameObject* obj = *it;

        if (!obj || obj->Get_Dead() || !obj->Is_Active()) {
            it = m_PickingList.erase(it);
            continue;
        }

        _vec3 hit;
        if (obj->Picking(&hit)) { 
            vecPicked.push_back(obj);
            vecPos.push_back(hit);
        }
        ++it;
    }

    if (vecPicked.empty()) return false;

    // 가장 가까운(z가 작은) 점 선택
    size_t nearIdx = 0;
    float minZ = vecPos[0].z;
    for (size_t i = 1; i < vecPos.size(); ++i) {
        if (vecPos[i].z < minZ) { minZ = vecPos[i].z; nearIdx = i; }
    }

    m_vPickingPos = vecPos[nearIdx];
    m_pPickingObject = vecPicked[nearIdx];
    m_pPickingObject->PickingTrue();
    m_pPickingObject->HitAt(m_vPickingPos);
    return true;
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