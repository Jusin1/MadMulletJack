#include "Engine_Define.h"
#include "CObjectManager.h"
#include "CMapFactory.h"

IMPLEMENT_SINGLETON(CMapFactory)

CMapFactory::CMapFactory()
{
}

CMapFactory::~CMapFactory()
{
}

void CMapFactory::Free()
{
}

void CMapFactory::Register(ObjectCategory _eCategory, _uint _iType, CreateFunction _func)
{
	if (_eCategory < ObjectCategory::WALL || _eCategory >= ObjectCategory::NONE || _iType < 0 || !_func)
	{
		MSG_BOX("CMapFactory::Register, Category or type is invalid");
		return;
	}

	_uint iCategory = static_cast<_uint>(_eCategory);
	m_Register[iCategory].insert(std::map<_uint, CreateFunction>::value_type(_iType, _func));
}

HRESULT CMapFactory::Create(ObjectCategory _eCategory, _uint _iType, void *_pData)
{
	if (_eCategory < ObjectCategory::WALL || _eCategory >= ObjectCategory::NONE || _iType < 0)
	{
		MSG_BOX("CMapFactory::Create, Category or type is invalid");
		return E_FAIL;
	}

	_uint iCategory = static_cast<_uint>(_eCategory);
	auto itr = m_Register[iCategory].find(_iType);
	if (itr == m_Register[iCategory].end())
	{
		MSG_BOX("CMapFactory::Create, std::function is invalid");
		return E_FAIL;
	}

	return (*itr).second(_pData);
}
