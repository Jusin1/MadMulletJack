#pragma once
#include "CBase.h"

namespace Engine
{
	class CGameObject;
	class CVIBuffer_GridPanelBase;
	typedef struct tagPanelEntry PANELENTRY;
}

class CGridPanel;

class CGameDataManager : public CBase
{
	DECLARE_SINGLETON(CGameDataManager)
private:
	explicit CGameDataManager();
	virtual ~CGameDataManager();

	virtual void Free() override;
public:
	void AllClear();
	
public:
	HRESULT Bind_FloorList(list<Engine::CGameObject *> *_pFloorlist);
	HRESULT Bind_SideWallList(list<Engine::CGameObject *> *_pSlideWalllist);
	vector<PANELENTRY> *Get_SortedFloorEntries() { return &m_vecSortedFloorEntries; }
	vector<PANELENTRY> *Get_SortedSlideWallEntries()
	{
		if (m_vecSortedSlideWallEntries.size() <= 0)
			return nullptr;

		return &m_vecSortedSlideWallEntries;
	}
private:
	void Sort_List_ByZValue(const vector<PANELENTRY> &_list);
	void Clear_FloorList();
	void Clear_SliedWallList();
private:
	vector<PANELENTRY> m_vecSortedFloorEntries;
	vector<PANELENTRY> m_vecSortedSlideWallEntries;
};

