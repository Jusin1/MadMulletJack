#include "pch.h"
#include "Engine_Define.h"
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
}
