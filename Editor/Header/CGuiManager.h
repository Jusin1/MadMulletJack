#pragma once
#include "CBase.h"
#include "Engine_Define.h"

namespace Engine
{
	class CGameObject;
}

class CGuiManager : public CBase
{
	DECLARE_SINGLETON(CGuiManager)
private:
	explicit CGuiManager();
	virtual ~CGuiManager();

	virtual void Free() override;
public:
	const Engine::CGameObject *GetTarget() const { return m_pTarget; }
	void SetTarget(Engine::CGameObject *_p) { m_pTarget = _p; }
private:
	Engine::CGameObject *m_pTarget;
};

