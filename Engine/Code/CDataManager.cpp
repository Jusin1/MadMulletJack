#include "Engine_Define.h"
#include "CFileManager.h"
#include "CDataManager.h"

IMPLEMENT_SINGLETON(CDataManager)

CDataManager::CDataManager()
{
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
}

vector<MAPOBJECTDATA> *CDataManager::FindData(const wstring &Layer)
{
	unordered_map<wstring, vector<MAPOBJECTDATA>>::iterator itr = m_LoadedData.find(Layer);
	if (itr == m_LoadedData.end())
		return nullptr;

	return &(itr->second);
}
