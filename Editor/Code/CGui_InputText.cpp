#include "pch.h"
#include "CGui_InputText.h"

CGui_InputText::CGui_InputText(const string &_label)
	:CGuiBase(_label)
{
	cBuffer[0] = '\0';
}

CGui_InputText::~CGui_InputText()
{
}

void CGui_InputText::Free()
{
}

void CGui_InputText::Render()
{
	ImGui::InputText(m_label.c_str(), cBuffer, sizeof(cBuffer));
}
