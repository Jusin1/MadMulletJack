#include "pch.h"
#include "Engine_Define.h"
#include "CGuiBase.h"
#include "CGui_Panel.h"

CGui_Panel::CGui_Panel(const string &_title)
	: m_title(_title)
{
}

CGui_Panel::~CGui_Panel()
{
}

void CGui_Panel::Free()
{
	for_each(m_pElements.begin(), m_pElements.end(), [](std::pair<const string, CGuiBase *> &pair)->void {
		Safe_Release(pair.second);
		});
	m_pElements.clear();
}

CGui_Panel *CGui_Panel::Create(const string &_title)
{
	return new CGui_Panel(_title);
}

void CGui_Panel::AddElement(CGuiBase *pElement)
{
	if (!pElement)
		return;

	m_pElements.insert(map<const string, CGuiBase *>::value_type(pElement->GetLabel(), pElement));
}

void CGui_Panel::Render()
{
	ImGui::Begin(m_title.c_str());

	for_each(m_pElements.begin(), m_pElements.end(), [](std::pair<const string, CGuiBase *> &pair)->void {
		pair.second->Render();
		});

	ImGui::End();
}

CGuiBase *CGui_Panel::GetElement(const string &_keyName)
{
	map<string, CGuiBase *>::iterator itr = m_pElements.find(_keyName);
	if (itr == m_pElements.end())
		return nullptr;

	return itr->second;
}
