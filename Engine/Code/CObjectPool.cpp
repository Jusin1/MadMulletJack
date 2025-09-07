#include "Engine_Define.h"
#include "CManagement.h"
#include "CObjectManager.h"
#include "CObjectPool.h"


CObjectPool::CObjectPool(_uint _iReserve)
	: m_iActiveCount(0), m_PrototypeTag(L""), m_LayerTag(L"")
{
	m_Objects.reserve(_iReserve);
}

CObjectPool::~CObjectPool()
{
	Free();
}


void CObjectPool::Free()
{
	for (CGameObject *pElement : m_Objects)
	{
		Safe_Release(pElement);
	}
	m_Objects.clear();
	m_iActiveCount = 0;
}


CObjectPool *CObjectPool::Create(const wstring &_pPrototypeTag, _uint _iReserve)
{
	CObjectPool *pInstance = new CObjectPool(_iReserve);
	if (FAILED(pInstance->Ready_ObjectPool(_pPrototypeTag)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}


CGameObject *CObjectPool::Spawn(void *pArg, std::function<void(CGameObject *)> _callback)
{
	if (m_iActiveCount >= m_Objects.size())
	{
		return nullptr;
	}
	CGameObject *pGo = m_Objects[m_iActiveCount];
	if (pArg) pGo->Spawn_Pooling(pArg);
	else pGo->Spawn_Pooling();
	pGo->Set_ActiveIndex(m_iActiveCount);
	if (_callback) _callback(pGo);
	++m_iActiveCount;
	return pGo;
}


HRESULT CObjectPool::Despawn(CGameObject *_pObject)
{
	if (!_pObject)
	{
		return E_FAIL;
	}
	int iNeed_Despawned_Index = _pObject->Get_ActiveIndex();
	if (iNeed_Despawned_Index < 0 || iNeed_Despawned_Index >= m_iActiveCount)
	{
		return E_FAIL;
	}

	int iLastActiveIndex = m_iActiveCount - 1;
	if (iNeed_Despawned_Index != iLastActiveIndex)
	{
		std::swap(m_Objects[iNeed_Despawned_Index], m_Objects[iLastActiveIndex]);

		const int &Swaped_Despawned_Index = iNeed_Despawned_Index;
		const int &Swaped_iLastActiveIndex = iLastActiveIndex;

		m_Objects[Swaped_Despawned_Index]->Set_ActiveIndex(Swaped_Despawned_Index);
		m_Objects[Swaped_iLastActiveIndex]->Set_ActiveIndex(Swaped_iLastActiveIndex);
	}

	if (FAILED(_pObject->Despawn_Pooling()))
	{
		return E_FAIL;
	}

	--m_iActiveCount;

	return S_OK;
}


HRESULT CObjectPool::Ready_ObjectPool(const wstring &_pPrototypeTag)
{
	CGameObject *pPrototype = CObjectManager::GetInstance()->Find_Prototype(_pPrototypeTag.c_str());
	if (!pPrototype)
	{
		return E_FAIL;
	}
	for (int i = 0; i < m_Objects.capacity(); ++i)
	{
		CGameObject *pGo = pPrototype->Clone();
		if (!pGo)
		{
			return E_FAIL;
		}
		m_Objects.push_back(pGo);
	}

	m_PrototypeTag = _pPrototypeTag;

	return S_OK;
}

void CObjectPool::All_Despawn()
{
	if (m_iActiveCount <= 0)
		return;

	for (int i = m_iActiveCount - 1; i >= 0; --i)
	{
		Despawn(m_Objects[i]);
	}
}

CGameObject *CObjectPool::Get_ActiveAt(_int i)
{
	if (i >= m_iActiveCount)
	{
		MSG_BOX("CObjectPool::Get_ActiveAt, Failed wrong index");
		return nullptr;
	}

	return m_Objects[i];
}
