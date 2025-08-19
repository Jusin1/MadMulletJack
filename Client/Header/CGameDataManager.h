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
	vector<PANELENTRY> *Get_SortedFloorEntries() { return &m_vecSortedFloorEntries; }
private:
	void Sort_FloorList();
	void Clear_FloorList();
private:
	vector<PANELENTRY> m_vecSortedFloorEntries;
	// 점프 했을때 찾는 용도
	vector<_float> m_fZStarts;
};

