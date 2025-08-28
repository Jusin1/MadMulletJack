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
	CGameObject *Spawn(std::function<void(CGameObject *)> _callback = nullptr);
	void Despawn(CGameObject *_pObject);
	void Initialize();
	void Late_Update();
	void Update(const float _fDeltaTime);
	void Render(HDC hDC);
public:

private:

};

END