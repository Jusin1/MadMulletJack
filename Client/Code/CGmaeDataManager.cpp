#include "Engine_Define.h"
#include "CGridPanel.h"
#include "CGmaeDataManager.h"

IMPLEMENT_SINGLETON(CGmaeDataManager)

CGmaeDataManager::CGmaeDataManager()
{
	m_vecFloors_Sorted.reserve(15);
}

CGmaeDataManager::~CGmaeDataManager()
{
	Free();
}

void CGmaeDataManager::Free()
{
	AllClear();
}

HRESULT CGmaeDataManager::Bind_FloorList(list<Engine::CGameObject *> *_pFloorlist)
{
	if (!_pFloorlist)
	{
		MSG_BOX("CGmaeDataManager::Bind_FloorList, loorlist is nullptr");
		return E_FAIL;
	}
		

	for (list<Engine::CGameObject *>::iterator itr = _pFloorlist->begin();
		itr != _pFloorlist->end();
		++itr)
	{
		if (!(*itr))
			continue;

		(*itr)->Add_Ref();
		m_vecFloors_Sorted.push_back(static_cast<CGridPanel *>((*itr)));
	}

	Sort_FloorList();

	return S_OK;
}

void CGmaeDataManager::AllClear()
{
	Clear_FloorList();
}

void CGmaeDataManager::Sort_FloorList()
{
	if (m_vecFloors_Sorted.size() <= 1)
		return;

	std::sort(m_vecFloors_Sorted.begin(), m_vecFloors_Sorted.end(),
		[](CGridPanel *pA, CGridPanel *pB)->bool
		{
			_vec3 pA_pos = pA->GetTransform()->Get_Info(INFO_POS);
			_vec3 pB_pos = pB->GetTransform()->Get_Info(INFO_POS);

			if (pA_pos.z != pB_pos.z)
				return pA_pos.z < pB_pos.z;

			return pA_pos.x < pB_pos.x;
		});
}

void CGmaeDataManager::Clear_FloorList()
{
	if (m_vecFloors_Sorted.size() > 0)
	{
		for_each(m_vecFloors_Sorted.begin(), m_vecFloors_Sorted.end(),
			[](CGridPanel *pElement)->void
			{
				Safe_Release(pElement);
			});
		m_vecFloors_Sorted.clear();
	}
}