#pragma once
#include "CBase.h"

BEGIN(Engine)

class ENGINE_DLL CDataManager : public CBase
{
	DECLARE_SINGLETON(CDataManager)
private:
	explicit CDataManager();
	virtual ~CDataManager();

	virtual void Free() override;
public:
	void AddData(const wstring &Layer, const MAPOBJECTDATA &objData);
	void AddPrefabData(PrefabType _e, const PREFABDATA &tData);
	void Clear();
	vector<MAPOBJECTDATA> *FindData(const wstring &Layer);

	vector<PREFABDATA> *GetPrefabDataList();
	const PREFABDATA &GetPrefabData(PrefabType _e);
private:
	_bool IsEmpty_PrefabData() { return m_PrefabData.size() <= 0; }
private:
	unordered_map<wstring, vector<MAPOBJECTDATA>> m_LoadedData;
	vector<PREFABDATA> m_PrefabData;
};

END