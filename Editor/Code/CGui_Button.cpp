#include "pch.h"
#include "CGui_Button.h"

CGui_Button::CGui_Button(const string &_label, std::function<void()> _func)
	: CGuiBase(_label), m_onClick(_func)
{
}

CGui_Button::~CGui_Button()
{
}

void CGui_Button::Free()
{
}

CGui_Button *CGui_Button::Create(const string &_label, std::function<void()> _func)
{
	return new CGui_Button(_label, _func);
}

void CGui_Button::Render()
{
	if (ImGui::Button(m_label.c_str()))
	{
		if (m_onClick)
			m_onClick();
	}
}
