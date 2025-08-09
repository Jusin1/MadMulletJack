#include "pch.h"
#include "CGuiManager.h"
#include "CGui_Transform.h"
#include "CGui_Button.h"
#include "CGui_InputText.h"

CGui_Transform::CGui_Transform(TransformDataType _eType)
	: CGuiBase(""), m_fPadding(30.f), m_eType(_eType)
{
	switch (m_eType)
	{
	case TransformDataType::ROTATION:
	{
		m_label = "Rotation";
	} break;
	case TransformDataType::POSITION:
	{
		m_label = "Position";
	} break;
	default:
		MSG_BOX("CGui_Transform type error");
		break;
	}
	m_vecButtons.reserve(2);
	m_vecTextInfos.reserve(3);
}

CGui_Transform::~CGui_Transform()
{
	
}

void CGui_Transform::Free()
{
	for (int i = 0; i < m_vecButtons.size(); ++i)
	{
		Safe_Release(m_vecButtons[i]);
	}
	for (int i = 0; i < m_vecTextInfos.size(); ++i)
	{
		Safe_Release(m_vecTextInfos[i]);
	}
}

HRESULT CGui_Transform::Ready_GuiTransform()
{
	
	return S_OK;
}

CGui_Transform *CGui_Transform::Create(TransformDataType _eType)
{
	CGui_Transform *pNew = new CGui_Transform(_eType);
	if (FAILED(pNew->Ready_GuiTransform()))
	{
		MSG_BOX("GuiTransform create failed");
		Safe_Release(pNew);
		return nullptr;
	}

	return pNew;
}

void CGui_Transform::Render()
{
	switch (m_eType)
	{
	case TransformDataType::ROTATION:
		RotationRender();
		break;
	case TransformDataType::POSITION:
		PositionRender();
		break;
	default:
		break;
	}
}

void CGui_Transform::PositionRender()
{
	if (CGameObject *pTarget = CGuiManager::GetInstance()->GetTarget())
	{

	}
	else
	{
		AllReset();
	}
}

void CGui_Transform::RotationRender()
{
	if (CGameObject *pTarget = CGuiManager::GetInstance()->GetTarget())
	{

	}
	else
	{
		AllReset();
	}
}

void CGui_Transform::ApplySetInfo()
{
	if (CGameObject *pTarget = CGuiManager::GetInstance()->GetTarget())
	{

	}
	else
	{
		AllReset();
	}
}

void CGui_Transform::ResetInfo()
{
	if (CGameObject *pTarget = CGuiManager::GetInstance()->GetTarget())
	{

	}
	else
	{
		AllReset();
	}
}

void CGui_Transform::AllReset()
{
	for (int i = 0; i < m_vecTextInfos.size(); ++i)
	{
	}
}
