#include "pch.h"
#include "CGuiBase.h"
#include "CGuiManager.h"
#include "CGui_Button.h"
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
	for (size_t i = 0; i < m_pElements.size(); ++i)
	{
		Safe_Release(m_pElements[i]);
	}
}

HRESULT CGui_Panel::Ready_Panel()
{
	return S_OK;
}

void CGui_Panel::AddElement(_uint _iType, CGuiBase *pElement)
{
	if (_iType < 0 || !pElement)
		return;

	m_pElements[_iType] = pElement;
}

CGuiBase *CGui_Panel::GetElement(_uint _iType)
{
	if (_iType < 0)
		return nullptr;

	return m_pElements[_iType];
}

void CGui_Panel::Render()
{
}
