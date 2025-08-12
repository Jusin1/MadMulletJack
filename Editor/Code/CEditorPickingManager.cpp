#include "pch.h"
#include "CGameObject.h"
#include "CGuiManager.h"
#include "CEditorPickingManager.h"

IMPLEMENT_SINGLETON(CEditorPickingManager)

CEditorPickingManager::CEditorPickingManager()
{
	m_PickingList.reserve(20);
}

CEditorPickingManager::~CEditorPickingManager()
{
}

void CEditorPickingManager::Free()
{
	Clear_Picking();
}

HRESULT CEditorPickingManager::Ready_Picking()
{
	return S_OK;
}

void CEditorPickingManager::Clear_Picking()
{
	m_PickingList.clear();
}

void CEditorPickingManager::Add_PickingGroup(CGameObject *pGameObject)
{
	if (!pGameObject)
		return;

	m_PickingList.insert(pGameObject);
}

void CEditorPickingManager::Remove_PickingGroup(CGameObject *pGameObject)
{
	unordered_set<CGameObject*>::iterator itr = m_PickingList.find(pGameObject);
	if (itr != m_PickingList.end())
		m_PickingList.erase(itr);
}

_bool CEditorPickingManager::Picking()
{
	if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
		return FALSE;
	POINT p; GetCursorPos(&p);
	RECT rc; GetClientRect(g_hWnd, &rc);
	ScreenToClient(g_hWnd, &p);
	if (!PtInRect(&rc, p))
		return FALSE;

	vector<CGameObject *> vecPicked;
	vector<_vec3> vecPos;
	_vec3 vPos;

	for (auto &pGameObject : m_PickingList)
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

		return TRUE;
	}

	CGuiManager::GetInstance()->SetTarget(nullptr);
	return FALSE;
}

_bool CEditorPickingManager::Picking_ForDummy()
{
	POINT p; GetCursorPos(&p);
	RECT rc; GetClientRect(g_hWnd, &rc);
	ScreenToClient(g_hWnd, &p);
	if (!PtInRect(&rc, p))
		return FALSE;

	vector<CGameObject *> vecPicked;
	vector<_vec3> vecPos;
	_vec3 vPos;

	for (auto &pGameObject : m_PickingList)
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
		CGameObject *pPickedGo{nullptr};
		int NearNum = 0;

		for (_uint i = 0; i < vecPos.size(); ++i)
		{
			if (vecPos[i].z <= vecNearPos.z || i == 0)
			{
				vecNearPos = vecPos[i];
				pPickedGo = vecPicked[i];
				NearNum = i;
			}
		}

		m_pPickedOBjectForDummy = pPickedGo;
		m_vDummyPickingPos = vecNearPos;
	}
	else
	{
		m_pPickedOBjectForDummy = nullptr;
		m_vDummyPickingPos = {0.f, 0.f, 0.f};
	}
	return FALSE;
}