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
	void AddInstancedPrefabData(const PREFABDATA &tData) { m_InstancePrefabData.push_back(tData); }
	void Clear();
	vector<MAPOBJECTDATA> *FindData(const wstring &Layer);

	vector<PREFABDATA> *GetPrefabDataList();
	const PREFABDATA &GetPrefabData(PrefabType _e);

	vector<PREFABDATA> *GetInstancedPrefabDataList()
	{
		if (IsEmpty_InstancedPrefabData()) return nullptr;
		return &m_InstancePrefabData;
	};
private:
	_bool IsEmpty_PrefabData() { return m_PrefabData.size() <= 0; }
	_bool IsEmpty_InstancedPrefabData()
	{ return m_InstancePrefabData.size() <= 0; }
private:
	unordered_map<wstring, vector<MAPOBJECTDATA>> m_LoadedData;
	vector<PREFABDATA> m_InstancePrefabData;
	vector<PREFABDATA> m_PrefabData;
};

END