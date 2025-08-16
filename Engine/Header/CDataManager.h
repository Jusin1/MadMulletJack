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
	void Clear();
private:
	vector<MAPOBJECTDATA> *FindData(const wstring &Layer);
private:
	unordered_map<wstring, vector<MAPOBJECTDATA>> m_LoadedData;
};

END