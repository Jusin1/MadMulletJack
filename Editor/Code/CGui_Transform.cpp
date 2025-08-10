#include "pch.h"
#include "CGui_Transform.h"
#include "CGui_Button.h"
#include "CGui_InputFloat.h"
#include "CGameObject.h"
#include "CGuiManager.h"


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
	m_vecInfos.reserve(3);
}

CGui_Transform::~CGui_Transform()
{
	
}

void CGui_Transform::Free()
{
	for (int i = 0; i < m_vecInfos.size(); ++i)
	{
		Safe_Release(m_vecInfos[i]);
	}
}

HRESULT CGui_Transform::Ready_GuiTransform()
{
	CGui_InputFloat *pX = CGui_InputFloat::Create("X");
	CGui_InputFloat *pY = CGui_InputFloat::Create("Y");
	CGui_InputFloat *pZ = CGui_InputFloat::Create("Z");

	m_vecInfos.push_back(pX);
	m_vecInfos.push_back(pY);
	m_vecInfos.push_back(pZ);

	switch (m_eType)
	{
	case TransformDataType::ROTATION:
	{
		RotationInit();
	} break;
	case TransformDataType::POSITION:
	{
		PositionInit();
	} break;
	default:
		MSG_BOX("CGui_Transform type error");
		break;
	}
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
	if (ImGui::CollapsingHeader(m_label.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginTable(m_label.c_str(), (int)m_vecInfos.size(),
			ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV))
		{
			ImGui::TableNextRow();
			for (std::size_t i = 0; i < m_vecInfos.size(); ++i) {
				ImGui::TableSetColumnIndex((int)i);
				m_vecInfos[i]->Render();
			}
			ImGui::EndTable();
		}
	}
}

void CGui_Transform::PositionInit()
{
	m_vecInfos[0]->SetOnEvent(
		[](CGameObject *pGo)->_float
		{
			return pGo->GetTransform()->Get_Info(INFO_POS).x;
		});
	m_vecInfos[1]->SetOnEvent(
		[](CGameObject *pGo)->_float
		{
			return pGo->GetTransform()->Get_Info(INFO_POS).y;
		});
	m_vecInfos[2]->SetOnEvent(
		[](CGameObject *pGo)->_float
		{
			return pGo->GetTransform()->Get_Info(INFO_POS).z;
		});

	m_vecInfos[0]->SetEndEvent(
		[&](_float _f)->void
		{
			if (CGameObject* pGo = m_vecInfos[0]->GetTarget())
			{
				if (CTransform *pTransform = pGo->GetTransform())
				{
					_vec3 pos = pTransform->Get_Info(INFO_POS);
					pos.x = _f;
					pTransform->Set_Info(INFO_POS, pos);
				}
			}			
		});
	m_vecInfos[1]->SetEndEvent(
		[&](_float _f)->void
		{
			if (CGameObject *pGo = m_vecInfos[1]->GetTarget())
			{
				if (CTransform *pTransform = pGo->GetTransform())
				{
					_vec3 pos = pTransform->Get_Info(INFO_POS);
					pos.y = _f;
					pTransform->Set_Info(INFO_POS, pos);
				}
			}
		});
	m_vecInfos[2]->SetEndEvent(
		[&](_float _f)->void
		{
			if (CGameObject *pGo = m_vecInfos[2]->GetTarget())
			{
				if (CTransform *pTransform = pGo->GetTransform())
				{
					_vec3 pos = pTransform->Get_Info(INFO_POS);
					pos.z = _f;
					pTransform->Set_Info(INFO_POS, pos);
				}
			}
		});
}

void CGui_Transform::RotationInit()
{
}
