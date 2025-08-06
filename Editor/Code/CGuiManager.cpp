#include "pch.h"
#include "CGameObject.h"
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
