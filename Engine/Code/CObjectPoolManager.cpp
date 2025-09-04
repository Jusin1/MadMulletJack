#include "Engine_Define.h"
#include "CEffect_Pixel.h"
#include "CObjectPool.h"
#include "CObjectPoolManager.h"

IMPLEMENT_SINGLETON(CObjectPoolManager)

CObjectPoolManager::CObjectPoolManager()
{
}

CObjectPoolManager::~CObjectPoolManager()
{
	Free();
}

void CObjectPoolManager::Free()
{
	All_Clear();
}

HRESULT CObjectPoolManager::Ready_Pools()
{
	All_Clear();

	if(!m_arrayPools[static_cast<_uint>(PoolType::EFFECT_PIXEL)])
		m_arrayPools[static_cast<_uint>(PoolType::EFFECT_PIXEL)] = CObjectPool::Create(L"Proto_PixelEffect");
	if (!m_arrayPools[static_cast<_uint>(PoolType::EFFECT_WORLD)])
		m_arrayPools[static_cast<_uint>(PoolType::EFFECT_WORLD)] = CObjectPool::Create(L"Proto_EffectWorld", 200);
	if (!m_arrayPools[static_cast<_uint>(PoolType::EFFECT_PIXEL_SPRITE)])
		m_arrayPools[static_cast<_uint>(PoolType::EFFECT_PIXEL_SPRITE)] = CObjectPool::Create(L"Proto_PixelEffect_Sprite");
	if (!m_arrayPools[static_cast<_uint>(PoolType::BULLET)])
		m_arrayPools[static_cast<_uint>(PoolType::BULLET)] = CObjectPool::Create(L"Prototype_GameObject_Bullet", 200);
	if (!m_arrayPools[static_cast<_uint>(PoolType::MISSILE)])
		m_arrayPools[static_cast<_uint>(PoolType::MISSILE)] = CObjectPool::Create(L"Prototype_GameObject_Missile", 50);
	if (!m_arrayPools[static_cast<_uint>(PoolType::WARNING_CIRCLE)])
		m_arrayPools[static_cast<_uint>(PoolType::WARNING_CIRCLE)] = CObjectPool::Create(L"Prototype_GameObject_WarningCircle", 30);

	return S_OK;
}

void CObjectPoolManager::Update(const _float _fDeltaTime)
{
	for (CObjectPool *pElement : m_arrayPools)
	{
		if (pElement)
		{
			for (int i = pElement->Get_ActiveCount() - 1; i >= 0; --i)
			{
				CGameObject *pGo = pElement->Get_ActiveAt(i);
				int iEvent = pGo->Update_GameObject(_fDeltaTime);
				if (iEvent == DEAD)
				{
					pElement->Despawn(pGo);
				}
			}
		}
	}
}

void CObjectPoolManager::Late_Update(const _float _fDeltaTime)
{
	for (CObjectPool *pElement : m_arrayPools)
	{
		if (pElement)
		{
			for (int i = pElement->Get_ActiveCount() - 1; i >= 0; --i)
			{
				CGameObject *pGo = pElement->Get_ActiveAt(i);
				pGo->LateUpdate_GameObject(_fDeltaTime);
			}
		}
	}
}

void CObjectPoolManager::All_Despawn()
{
	for (CObjectPool *pElement : m_arrayPools)
	{
		if(pElement)
			pElement->All_Despawn();
	}
}

void CObjectPoolManager::All_Clear()
{
	for (CObjectPool *pElement : m_arrayPools)
	{
		Safe_Release(pElement);
	}
	m_arrayPools.fill(nullptr);
}

CGameObject *CObjectPoolManager::Spawn(PoolType _ePoolType, void *pArg, std::function<void(CGameObject *)> _callback)
{
	if (_ePoolType < PoolType::EFFECT_PIXEL || _ePoolType >= PoolType::NONE)
	{
		MSG_BOX("CObjectPoolManager::Spawn, PoolType is invalid");
		return nullptr;
	}

	return m_arrayPools[static_cast<_uint>(_ePoolType)]->Spawn(pArg, _callback);
}
