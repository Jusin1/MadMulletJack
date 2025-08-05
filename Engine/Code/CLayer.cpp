#include "CLayer.h"

CLayer::CLayer()
{
}

CLayer::~CLayer()
{
}

HRESULT CLayer::Ready_Layer()
{
	return S_OK;
}



HRESULT CLayer::Add_GameObject(CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_objList.push_back(pGameObject);

	return S_OK;
}



void CLayer::Update_Layer(const _float& fTimeDelta)
{
	for (auto& pGameObject : m_objList)
	{
		if (nullptr != pGameObject)
		{
			int iEvent = pGameObject->Update_GameObject(fTimeDelta);
			if (iEvent == DEAD)
			{
				Safe_Release(pGameObject);
			}
		}
	}
}

void CLayer::LateUpdate_Layer(const _float& fTimeDelta)
{
	for (auto& pGameObject : m_objList)
	{
		if (nullptr != pGameObject)
			pGameObject->LateUpdate_GameObject(fTimeDelta);
	}
}

CGameObject* CLayer::Get_Object(_uint iIndex)
{
	if (m_objList.size() <= iIndex)
		return nullptr;

	auto	iter = m_objList.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	return *iter;
}

CComponent* CLayer::Get_Component(const _tchar* pComponentTag, _uint iIndex)
{
	auto	iter = m_objList.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Find_Component(pComponentTag);
}


CLayer* CLayer::Create()
{
	CLayer* pLayer = new CLayer;

	if (FAILED(pLayer->Ready_Layer()))
	{
		MSG_BOX("Layer Create Failed");
		Safe_Release(pLayer);
		return nullptr;
	}

	return pLayer;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_objList)
		Safe_Release(pGameObject);

	m_objList.clear();
}
