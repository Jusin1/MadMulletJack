#include "pch.h"
#include "CUIManager.h"
#include "CObjectManager.h"
#include "CHeartUI.h"
#include "CLisaUI.h"

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
		pHeatUI->Set_UIPosition(-300.f, 200.f, 80.f, 80.f);
		m_pEnterUI->Add_Child(pHeatUI);
		CHeartUI* pHeatUI2 = dynamic_cast<CHeartUI*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_HeartUI", SCENE_STAGE, L"UI_Layer"));
		pHeatUI2->Set_UIPosition(300.f, 200.f, 80.f, 80.f);
		m_pEnterUI->Add_Child(pHeatUI2);
		CLisaUI* pLisaUI = dynamic_cast<CLisaUI*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_LisaUI", SCENE_STAGE, L"UI_Layer"));
		pLisaUI->Set_UIPosition(550.f, -200.f, 200.f, 300.f);
		m_pEnterUI->Add_Child(pLisaUI);
	}
}


void CUIManager::Free()
{
	Safe_Release(m_pEnterUI);
}