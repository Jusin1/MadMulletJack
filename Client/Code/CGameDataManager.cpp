#include "pch.h"
#include "Engine_Define.h"
#include "CGridPanel.h"
#include "CVIBuffer_GridPanelBase.h"
#include "CGameDataManager.h"

IMPLEMENT_SINGLETON(CGameDataManager)

CGameDataManager::CGameDataManager()
{
	m_vecSortedFloorEntries.reserve(15);
	m_fZStarts.reserve(15);
}

CGameDataManager::~CGameDataManager()
{
	Free();
}

void CGameDataManager::Free()
{
	AllClear();
}

HRESULT CGameDataManager::Bind_FloorList(list<Engine::CGameObject *> *_pFloorlist)
{
	if (!_pFloorlist)
	{
		MSG_BOX("CGameDataManager::Bind_FloorList, loorlist is nullptr");
		return E_FAIL;
	}
		

	for (list<Engine::CGameObject *>::iterator itr = _pFloorlist->begin();
		itr != _pFloorlist->end();
		++itr)
	{
		if (!(*itr))
			continue;

		CGridPanel *pFloor= static_cast<CGridPanel *>(*itr);
		CTransform *pTransform = pFloor->GetTransform();
		CVIBuffer_GridPanelBase *pBuffer = pFloor->GetBuffer();

		if (!pTransform || !pBuffer)
		{
			MSG_BOX("CGameDataManager::Bind_FloorList, Transform and Buffer was invalid");
			return E_FAIL;
		}

		PANELENTRY tEntry;
		tEntry.eType = pFloor->GetType();
		tEntry.fY = pTransform->Get_Info(INFO_POS).y;
		tEntry.fMin_X = pTransform->Get_Info(INFO_POS).x;
		tEntry.fMin_Z = pTransform->Get_Info(INFO_POS).z;
		tEntry.fMax_X = pTransform->Get_Info(INFO_POS).x + ((pBuffer->GetData()->dwCountX - 1) * pBuffer->GetData()->dwInterval);
		tEntry.fMax_Z = pTransform->Get_Info(INFO_POS).z + ((pBuffer->GetData()->dwCountZ - 1) * pBuffer->GetData()->dwInterval);
		tEntry.fHalfX = 0.5f * tEntry.fMax_X;
		tEntry.fHalfZ = 0.5f * tEntry.fMax_Z;
		tEntry.fLocalCenter = _vec3{ tEntry.fHalfX, 0.f, tEntry.fHalfZ };
		tEntry.fInverseItv = 1.f / pBuffer->GetData()->dwInterval;
		tEntry.pBuffer = pBuffer;
		tEntry.matWorld = *pTransform->Get_World();
		::D3DXMatrixInverse(&tEntry.matWorldInv, nullptr, pTransform->Get_World());

		m_vecSortedFloorEntries.push_back(tEntry);
	}

	Sort_FloorList();

	return S_OK;
}

void CGameDataManager::AllClear()
{
	Clear_FloorList();
}

void CGameDataManager::Sort_FloorList()
{
	if (m_vecSortedFloorEntries.size() <= 1)
		return;

	std::sort(m_vecSortedFloorEntries.begin(), m_vecSortedFloorEntries.end(),
		[](const PANELENTRY &a, const PANELENTRY &b)->bool
		{
			if (a.fMin_Z != b.fMin_Z)
				return a.fMin_Z < b.fMin_Z;
			
			// Z값이 동일할 경우를 대비
			return a.fMin_X != b.fMin_X;
		});
}

void CGameDataManager::Clear_FloorList()
{
	if (m_vecSortedFloorEntries.size() > 0)
	{
		m_vecSortedFloorEntries.clear();
	}
}