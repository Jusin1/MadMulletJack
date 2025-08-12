#include "CGui_Checkbox.h"

CGui_Checkbox::CGui_Checkbox(const string &_label, std::function<void()> _trueFunc, std::function<void()> _falseFunc)
	:CGuiBase(_label), m_bValue(FALSE), m_trueEvent(_trueFunc), m_falseEvent(_falseFunc)
{
}

CGui_Checkbox::~CGui_Checkbox()
{
}

void CGui_Checkbox::Free()
{
}

CGui_Checkbox *CGui_Checkbox::Create(const string &_label, std::function<void()> _trueFunc, std::function<void()> _falseFunc)
{
	return new CGui_Checkbox(_label, _trueFunc, _falseFunc);
}

_bool CGui_Checkbox::Render(_int _iState)
{
	ImGui::SameLine();
	if (ImGui::Checkbox(m_label.c_str(), &m_bValue))
	{
		if (m_bValue)
		{
			EDITOR_CONSOLE("CreateMode, true");
			m_trueEvent();
		}
		else
		{
			EDITOR_CONSOLE("CreateMode, false");
			m_falseEvent();
		}
	}

	return FALSE;
}
