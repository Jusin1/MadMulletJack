#include "pch.h"
#include "CGui_InputText.h"

CGui_InputText::CGui_InputText(const string &_label)
	:CGuiBase(_label)
{
	m_value.reserve(256);
}

CGui_InputText::~CGui_InputText()
{
}

void CGui_InputText::Free()
{
}

CGui_InputText *CGui_InputText::Create(const string &_label)
{
	return nullptr;
}

void CGui_InputText::Render()
{
	//std::vector<char> buffer{ 256 };
	ImGui::InputText(m_label.c_str(), m_value.data(), m_value.size());
}
