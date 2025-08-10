#include "pch.h"
#include "Editor_Define.h"
#include "CGuiManager.h"
#include "CGridPanel.h"
#include "CGui_Thumbnail.h"

CGui_Thumbnail::CGui_Thumbnail(const string &_label)
	: CGuiBase(_label), m_iSelectedIndex(-1), m_iCol(3), m_fThumbnailSize(128.f)
	, m_fPadding(5.f)
{
	m_vecThumbnails.reserve(10);
}

CGui_Thumbnail::~CGui_Thumbnail()
{
}

void CGui_Thumbnail::Free()
{
}

CGui_Thumbnail *CGui_Thumbnail::Create(const string &_label)
{
	return new CGui_Thumbnail(_label);
}

void CGui_Thumbnail::Render()
{

	ImGui::BeginTable("ThumbnailTable", m_iCol,
		ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoBordersInBody);

	int iCount = 0;
	for (size_t i = 0; i < m_vecThumbnails.size(); ++i)
	{
		if (iCount % m_iCol == 0)
			ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(iCount % m_iCol);

		ImGui::PushID(static_cast<int>(i));
		ImGui::BeginGroup();

		float cellWidth = m_fThumbnailSize;
		float availWidth = ImGui::GetContentRegionAvail().x;
		float offsetX = (availWidth - cellWidth) * 0.5f;
		if (offsetX > 0) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);

		if (ImGui::ImageButton(
			m_vecThumbnails[i].name.c_str(),
			m_vecThumbnails[i].imguiID,
			ImVec2(m_fThumbnailSize, m_fThumbnailSize)))
		{
			m_iSelectedIndex = static_cast<int>(i);
			Change_Texture();
		}

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
		ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + m_fThumbnailSize);
		ImGui::TextWrapped("%s", m_vecThumbnails[i].name.c_str());
		ImGui::PopTextWrapPos();

		ImGui::EndGroup();
		ImGui::PopID();

		iCount++;
	}

	ImGui::EndTable();
}

void CGui_Thumbnail::Add_Thumbnail(const string &_name, const _tchar *_comp_name, IDirect3DBaseTexture9 *_pTexture)
{
	GuiThumbnailTexture newData;
	newData.name = _name;
	newData.comp_name = _comp_name;
	newData.pTexture = _pTexture;
	newData.imguiID = reinterpret_cast<ImTextureID>(_pTexture);

	m_vecThumbnails.push_back(newData);
}

void CGui_Thumbnail::Change_Texture()
{
	if (m_iSelectedIndex >= 0)
	{
		if (CGridPanel *pTargetPanel = static_cast<CGridPanel *>(CGuiManager::GetInstance()->GetTarget()))
		{
			pTargetPanel->Change_Texture(SCENE_STATIC, m_vecThumbnails[m_iSelectedIndex].comp_name);
		}

		m_iSelectedIndex = -1;
	}
}
