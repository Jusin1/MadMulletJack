#include "pch.h"
#include "CUIManager.h"
#include "CObjectManager.h"
#include "CHeartUI.h"

IMPLEMENT_SINGLETON(CUIManager)

CUIManager::CUIManager()
{

}

CUIManager::~CUIManager()
{
}

void CUIManager::CreateEnterUI()
{
	m_pEnterUI = dynamic_cast<CUIBase*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UIRoot", SCENE_STAGE, L"UI_Layer"));
	if (m_pEnterUI)
	{
		CHeartUI* pHeatUI = dynamic_cast<CHeartUI*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_HeartUI", SCENE_STAGE, L"UI_Layer"));
		pHeatUI->Set_UIPosition(WINCX * 0.5f - 300.f, WINCY * 0.5f + 250.f, 50.f, 50.f);
		m_pEnterUI->Add_Child(pHeatUI);
		pHeatUI = dynamic_cast<CHeartUI*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_HeartUI", SCENE_STAGE, L"UI_Layer"));
		pHeatUI->Set_UIPosition(WINCX * 0.5F - 800.f, WINCY * 0.5f + 250.f, 50.f, 50.f);
		m_pEnterUI->Add_Child(pHeatUI);
	}
}


void CUIManager::Free()
{
	Safe_Release(m_pEnterUI);
}