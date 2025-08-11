#include "pch.h"
#include "CGuiBase.h"
CGuiBase::CGuiBase(const string &_label)
	: m_label(_label)
{
}

CGuiBase::~CGuiBase()
{
	
}

void CGuiBase::Free()
{
	
}

_bool CGuiBase::Render(_int _iState)
{
	return FALSE;
}
