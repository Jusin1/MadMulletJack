#pragma once
#include "CBase.h"

namespace Engine
{
	class CGameObject;
}

// m_pPrefabList에서 들고있기때문에 절대 ObjectList가 먼저 해제되면 Dangling 위험!
class CPrefabRecycleSystem : public CBase
{
	DECLARE_SINGLETON(CPrefabRecycleSystem);
private:
	explicit CPrefabRecycleSystem();
	virtual ~CPrefabRecycleSystem();

	virtual void Free();
public:
	void Update();
	HRESULT Set_Player(CGameObject *p) { m_pPlayer = p; return S_OK; }
	HRESULT Set_PrefabList(list<CGameObject *> *p);
private:
	void Sorting();
	void RandomSpawnMonster(const _vec3 &vPos);
private:
	CGameObject *m_pPlayer{ nullptr };
	list<CGameObject *> m_pPrefabList;
	size_t m_PrefabCount{ 0 };
	_float m_fDistance{ 0.f };
};

