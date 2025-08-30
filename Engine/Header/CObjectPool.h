#pragma once
#include "CBase.h"
#include "CGameObject.h"


BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CObjectPool : public CBase
{
private:
	explicit CObjectPool(_uint _iReserve);
	virtual ~CObjectPool();
	
	virtual void Free() override;
public:
	static CObjectPool *Create(const wstring &_pPrototypeTag, _uint _iReserve = MAX_POOLING);
	HRESULT Ready_ObjectPool(const wstring &_pPrototypeTag);

	CGameObject *Spawn(void* pArg, std::function<void(CGameObject *)> _callback);
	HRESULT Despawn(CGameObject *_pObject);
	void All_Despawn();
public:
	int Get_ActiveCount() const { return m_iActiveCount; }
	int Get_Capacity() const { return m_Objects.size(); }
	CGameObject *Get_ActiveAt(_int i);
private:
	int m_iActiveCount;
	wstring m_PrototypeTag;
	wstring m_LayerTag;
	vector<CGameObject *> m_Objects;
};

END