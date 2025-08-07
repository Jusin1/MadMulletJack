#pragma once
#include "CBase.h"
#include "Engine_Define.h"

namespace Engine
{
	class CGameObject;
}

class CGui_Panel;

class CGuiManager : public CBase
{
	DECLARE_SINGLETON(CGuiManager)
private:
	explicit CGuiManager();
	virtual ~CGuiManager();

	virtual void Free() override;
public:
	void Render();
	Engine::CGameObject *GetTarget() const { return m_pTarget; }
	void SetTarget(Engine::CGameObject *_p) { m_pTarget = _p; }
	
	map<string, CGui_Panel *> *GetPanelMapList() { return &m_pPanels; }
	void AddPanel(CGui_Panel *_p);
	CGui_Panel *GetPanel(const string &_keyName);
private:
	Engine::CGameObject *m_pTarget;
	map<string, CGui_Panel *> m_pPanels;
};

