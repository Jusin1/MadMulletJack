#include "pch.h"
#include "CGui_Button.h"
#include "CGui_ButtonList.h"

CGui_ButtonList::CGui_ButtonList(const string &_label, const vector<string> &_buttonsLabel, const vector<std::function<void()>> &_buttonsEvent)
	: CGuiBase(_label)
{
	m_vecButtons.reserve(_buttonsEvent.size());
}

CGui_ButtonList::~CGui_ButtonList()
{
}

void CGui_ButtonList::Free()
{
	for (int i = 0; i < m_vecButtons.size(); ++i)
	{
		Safe_Release(m_vecButtons[i]);
	}
}

CGui_ButtonList *CGui_ButtonList::Create(const string &_label, const vector<string> &_buttonsLabel, const vector<std::function<void()>> &_buttonsEvent)
{
	CGui_ButtonList *pNew = new CGui_ButtonList(_label, _buttonsLabel, _buttonsEvent);
	if (FAILED(pNew->Ready_ButtonList(_buttonsLabel, _buttonsEvent)))
	{
		Safe_Release(pNew);
		MSG_BOX("CGui_ButtonList::Create, Failed");
		return nullptr;
	}

	return pNew;
}

_bool CGui_ButtonList::Render(_int _iState)
{
	if (m_vecButtons.empty()) return FALSE;

	if (ImGui::CollapsingHeader(m_label.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginTable(m_label.c_str(), (int)m_vecButtons.size(),
			ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV))
		{
			ImGui::TableNextRow();
			for (std::size_t i = 0; i < m_vecButtons.size(); ++i) {
				ImGui::TableSetColumnIndex((int)i);
				m_vecButtons[i]->Render();
			}
			ImGui::EndTable();
		}
	}

	return FALSE;
}

HRESULT CGui_ButtonList::Ready_ButtonList(const vector<string> &_buttonsLabel, const vector<std::function<void()>> &_buttonsEvent)
{
	if (_buttonsLabel.size() != _buttonsEvent.size())
	{
		MSG_BOX("CGui_ButtonList::Ready_ButtonList, not matched vector size");
		return E_FAIL;
	}

	for (int i = 0; i < _buttonsEvent.size(); ++i)
	{
		CGui_Button *pNewButton = CGui_Button::Create(_buttonsLabel[i], _buttonsEvent[i]);
		if (!pNewButton)
			return E_FAIL;
		m_vecButtons.push_back(pNewButton);
	}

	return S_OK;
}