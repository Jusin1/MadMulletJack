#pragma once
#include "CBase.h"


BEGIN(Engine)

class CGameObject;
class CObjectPool;

class ENGINE_DLL CObjectPoolManager : public CBase
{
	DECLARE_SINGLETON(CObjectPoolManager)
private:
	explicit CObjectPoolManager();
	virtual ~CObjectPoolManager();

	virtual void Free() override;
public:
	void Update(const _float _fDeltaTime);
	void Late_Update(const _float _fDeltaTime);
	void All_Despawn();
public:
	CGameObject *Spawn(PoolType _ePoolType, void *pArg = nullptr, std::function<void(CGameObject *)> _callback = nullptr);
	HRESULT Ready_Pools();
private:
	std::array<CObjectPool *, g_PoolTypeCount> m_arrayPools{ nullptr };
};

END