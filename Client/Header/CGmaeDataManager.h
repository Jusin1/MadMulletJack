#pragma once
#include "CBase.h"

namespace Engine
{
	class CGameObject;
}

class CGridPanel;

class CGmaeDataManager : public CBase
{
	DECLARE_SINGLETON(CGmaeDataManager)
private:
	explicit CGmaeDataManager();
	virtual ~CGmaeDataManager();

	virtual void Free() override;
public:
	void AllClear();
	
public:
	HRESULT Bind_FloorList(list<Engine::CGameObject *> *_pFloorlist);
	vector<CGridPanel *> *Get_SortedFloorList() { return &m_vecFloors_Sorted; }
private:
	void Sort_FloorList();
	void Clear_FloorList();
private:
	map<CGridPanel *, PANELENTRY> m_mapFloors_Sorted;
};

