#include "pch.h"
#include "CPickingManager.h"
#include "CTransform.h"
#include "CObjectManager.h"
#include "CCamera.h"
#include "CGlobal_Info.h"

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
    if (CGlobal_Info::Get_Instance()->Get_PlayerInfo().ePlayerState != ATTACK &&
        CGlobal_Info::Get_Instance()->Get_PlayerInfo().ePlayerState != ATTACK_ZOOM)
    {
        return false;
    }

    if (m_bMouseInUI) return false;
    if (!(GetAsyncKeyState(VK_LBUTTON) & 0x0001)) return false;

    vector<CGameObject*> vecPicked;
    vector<_vec3> vecPos;

    for (auto it = m_PickingList.begin(); it != m_PickingList.end(); )
    {
        CGameObject* obj = *it;

        // 1) 기본 null/죽음/비활성화 검사
        if (!obj || obj->Get_Dead() || !obj->Is_Active()) {
            it = m_PickingList.erase(it);
            continue;
        }

        // 2) 트랜스폼 같은 핵심 컴포넌트 없는 경우 바로 스킵
        if (!obj->GetTransform()) {
            ++it;
            continue;
        }

        // 3) 안전하게 Picking() 호출
        _vec3 hitW(0, 0, 0);
        _bool bPicked = false;
        try {
            bPicked = obj->Picking(&hitW);
        }
        catch (...) {
            // Picking 중 예외 터져도 리스트만 유지
            bPicked = false;
        }

        if (bPicked) {
            vecPicked.push_back(obj);
            vecPos.push_back(hitW);
        }

        ++it;
    }
    if (vecPicked.empty()) return false;
    const _matrix& view = CCamera::GetView();

    int best = -1;
    float bestZ = FLT_MAX;
    for (int i = 0; i < (int)vecPos.size(); ++i)
    {
        _vec3 vEye;
        D3DXVec3TransformCoord(&vEye, &vecPos[i], &view);
        if (vEye.z > 0.f && vEye.z < bestZ) { bestZ = vEye.z; best = i; }
    }
    if (best < 0) return false;

    m_vPickingPos = vecPos[best];
    m_pPickingObject = vecPicked[best];
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