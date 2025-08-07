#include "pch.h"
#include "CGameObject.h"
#include "CGui_Panel.h"
#include "CGuiManager.h"

IMPLEMENT_SINGLETON(CGuiManager)

CGuiManager::CGuiManager()
	: m_pTarget(nullptr)
{
}

CGuiManager::~CGuiManager()
{
}

void CGuiManager::Free()
{
	for_each(m_pPanels.begin(), m_pPanels.end(), [](std::pair<const string, CGui_Panel *> &pair)->void {
		Safe_Release(pair.second);
		});
	m_pPanels.clear();
}

void CGuiManager::Render()
{
	for_each(m_pPanels.begin(), m_pPanels.end(), [](std::pair<const string, CGui_Panel *> &pair)
		->void{ pair.second->Render(); });
}

void CGuiManager::AddPanel(CGui_Panel *_p)
{
	if (!_p)
		return;

	m_pPanels.insert(map<const string, CGui_Panel *>::value_type(_p->GetTitle(), _p));
}

CGui_Panel *CGuiManager::GetPanel(const string &_keyName)
{
	map<string, CGui_Panel *>::iterator itr = m_pPanels.find(_keyName);
	if (itr == m_pPanels.end())
		return nullptr;

	return itr->second;
}
