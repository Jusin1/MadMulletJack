#include "Engine_Define.h"
#include "CFileManager.h"
#include "CDataManager.h"

IMPLEMENT_SINGLETON(CDataManager)

CDataManager::CDataManager()
{
	m_PrefabData = vector<PREFABDATA>{ g_PrefabTypeCount };
	m_PrefabData[0].eType = PrefabType::SIGN_PILLAR;
	m_PrefabData[1].eType = PrefabType::ROAD;
}

CDataManager::~CDataManager()
{
	Free();
}

void CDataManager::Free()
{
	Clear();
}

void CDataManager::AddData(const wstring &Layer, const MAPOBJECTDATA &objData)
{
	vector<MAPOBJECTDATA> *result{nullptr};
	if (!(result = FindData(Layer)))
	{
		vector<MAPOBJECTDATA> vecValue;
		vecValue.reserve(50);
		vecValue.push_back(objData);
		m_LoadedData.insert(unordered_map<wstring, vector<MAPOBJECTDATA>>::value_type(Layer, vecValue));
	}
	else
	{
		result->push_back(objData);
	}
}

void CDataManager::AddPrefabData(PrefabType _e, const PREFABDATA &tData)
{
	if (_e < PrefabType::SIGN_PILLAR || _e >= PrefabType::NONE)
	{
		MSG_BOX("CDataManager::AddPrefabData, PrefabType is Invalid");
		return;
	}

	if (m_PrefabData[static_cast<_uint>(_e)].eType != _e)
	{
		MSG_BOX("CDataManager::AddPrefabData, PrefabType isnt Matched");
		return;
	}

	m_PrefabData[static_cast<_uint>(_e)] = tData;
}

void CDataManager::Clear()
{
	if (!m_LoadedData.empty())
	{
		for_each(m_LoadedData.begin(), m_LoadedData.end(),
			[](auto &pair) {
				pair.second.clear();
		});

		m_LoadedData.clear();
	}

	// size를 유지하며 비우기
	for (int i = 0; i < g_PrefabTypeCount; ++i)
	{
		m_PrefabData[i] = {};
	}
}

vector<MAPOBJECTDATA> *CDataManager::FindData(const wstring &Layer)
{
	unordered_map<wstring, vector<MAPOBJECTDATA>>::iterator itr = m_LoadedData.find(Layer);
	if (itr == m_LoadedData.end())
		return nullptr;

	return &(itr->second);
}

vector<PREFABDATA> *CDataManager::GetPrefabDataList()
{
	if (IsEmpty_PrefabData())
	{
		MSG_BOX("CDataManager::GetPrefabData, PrefabData is empty");
		return nullptr;
	}

	return &m_PrefabData;
}

const PREFABDATA &CDataManager::GetPrefabData(PrefabType _e)
{
	if (IsEmpty_PrefabData())
	{
		MSG_BOX("CDataManager::GetPrefabData, PrefabData is empty");
		return {};
	}

	return m_PrefabData[static_cast<_uint>(_e)];
}
