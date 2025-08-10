#include "pch.h"
#include "CGui_InputFloat.h"
#include "CGameObject.h"

CGui_InputFloat::CGui_InputFloat(string _label, std::function<_float(CGameObject *)> onEvent, std::function<void(_float)> endEvnet)
	: CGuiBase(_label), m_fValue(0.f), m_OnEvent(onEvent), m_EndEvent(endEvnet), m_pTarget(nullptr)
{
}

CGui_InputFloat::~CGui_InputFloat()
{
}

void CGui_InputFloat::Free()
{
}

CGui_InputFloat *CGui_InputFloat::Create(string _label, std::function<_float(CGameObject *)> onEvent, std::function<void(_float)> endEvnet)
{
	return new CGui_InputFloat(_label, onEvent, endEvnet);
}

void CGui_InputFloat::Render()
{
	if (CGameObject *pGo = CGuiManager::GetInstance()->GetTarget())
	{
		if (m_pTarget != pGo)
		{
			m_pTarget = pGo;
			if (m_pTarget && m_OnEvent && !ImGui::IsItemActive())
				m_fValue = m_OnEvent(m_pTarget);
		}
	}
	else
	{
		if(m_pTarget)
			m_pTarget = nullptr;
	}

	if (m_pTarget)
	{
		if (m_OnEvent)
		{
			m_fValue = m_OnEvent(m_pTarget);
		}

		if (ImGui::InputFloat(m_label.c_str(), &m_fValue))
		{
			if(m_EndEvent)
				m_EndEvent(m_fValue);
		}
	}
	else
	{
		Reset();
	}
}

void CGui_InputFloat::Reset()
{
	m_fValue = 0.f;
}
