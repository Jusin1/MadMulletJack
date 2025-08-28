#pragma once
#include "CBase.h"
#include "CGameObject.h"

BEGIN(Engine)

template<typename T>
class ENGINE_DLL CObjectPool : public CBase
{
private:
	explicit CObjectPool(_uint _iReserve = MAX_POOLING);
	virtual ~CObjectPool();
	
	virtual void Free() override;
public:
	CGameObject *Spawn(std::function<void(T *)> _callback = nullptr);
	void Despawn(T *_pObject);
	HRESULT Ready_ObjectPool();
	void Update(const _float _fDeltaTime);
	void Late_Update(const _float &_fDeltaTime);
	void All_Despawn();

private:
	int m_iActiveTopIndex{ -1 };
	vector<CGameObject *> m_Objects;
};


template<typename T>
inline CObjectPool<T>::CObjectPool(_uint _iReserve)
	: m_iActiveTopIndex(-1)
{
	m_Objects.reserve(_iReserve);
}

template<typename T>
inline CObjectPool<T>::~CObjectPool()
{
	Free();
}

template<typename T>
inline void CObjectPool<T>::Free()
{
	for (CGameObject *pElement : m_Objects)
	{
		Safe_Release(pElement);
	}
	m_Objects.clear();
	m_iActiveTopIndex = -1;
}

template<typename T>
inline CGameObject *CObjectPool<T>::Spawn(std::function<void(T *)> _callback)
{
	if (m_iActiveTopIndex + 1 >= (int)m_Objects.size())
	{
		MSG_BOX("CObjectPool<T>::Spawn, Pool is Full");
		return nullptr;
	}

	++m_iActiveTopIndex;
	CGameObject *pGo = m_Objects[m_iActiveTopIndex];
	pGo->Init_Pooling();
	static_cast<T *>(pGo)->Set_ActiveIndex(m_iActiveTopIndex);
	if (_callback) _callback(static_cast<T *>(pGo));
	return pGo;
}

template<typename T>
inline void CObjectPool<T>::Despawn(T *_pObject)
{
	int src = _pObject->Get_ActiveIndex();
	int dst = m_iActiveTopIndex;
	if (src < 0 || src > dst)
	{
		MSG_BOX("CObjectPool<T>::Despawn, Index was wrong");
		return;
	}

	std::swap(m_Objects[src], m_Objects[dst]);
	static_cast<T *>(m_Objects[src])->Set_ActiveIndex(src);
	static_cast<T *>(m_Objects[dst])->Set_ActiveIndex(dst);

	// 비활성화
	m_Objects[dst]->Set_AcitveIndex();
	--m_iActiveTopIndex;
}

template<typename T>
inline HRESULT CObjectPool<T>::Ready_ObjectPool()
{
	// proto 확보
	return S_OK;
}

template<typename T>
inline void CObjectPool<T>::Late_Update(const _float &_fDeltaTime)
{
	for (int i = 0; i <= m_iActiveTopIndex; ++i)
	{
		m_Objects[i]->LateUpdate_GameObject(_fDeltaTime);
	}
}

template<typename T>
inline void CObjectPool<T>::Update(const _float _fDeltaTime)
{
	for (int i = 0; i <= m_iActiveTopIndex; ++i)
	{
		m_Objects[i]->Update_GameObject(_fDeltaTime);
	}
}

template<typename T>
inline void CObjectPool<T>::All_Despawn()
{
	while (m_iActiveTopIndex >= 0)
	{
		Despawn(static_cast<T *>(m_Objects[m_iActiveTopIndex]));
	}
}

END