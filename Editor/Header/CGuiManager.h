#pragma once
#include "CBase.h"

class CGameObject;

class CGuiManager : public CBase
{
	DECLARE_SINGLETON(CGuiManager)
private:
	explicit CGuiManager();
	virtual ~CGuiManager();

	virtual void Free() override;
public:
	const CGameObject *GetTarget() const { return m_pTarget; }
	void SetTarget(CGameObject *_p) { m_pTarget = _p; }
private:
	CGameObject *m_pTarget;
};

