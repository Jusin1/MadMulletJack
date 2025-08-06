#include "pch.h"
#include "CGui_Thumbnail.h"

CGui_Thumbnail::CGui_Thumbnail(const string &_label)
	: CGuiBase(_label), m_iSelectedIndex(-1), m_iCol(4), m_fThumbnailSize(64.f)
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
	int iCount{ 0 };
	for (size_t i = 0; i < m_vecThumbnails.size(); ++i)
	{
		ImGui::PushID(static_cast<int>(i));
		ImGui::BeginGroup();
		if (ImGui::ImageButton(m_vecThumbnails[i].name.c_str(), m_vecThumbnails[i].imguiID, ImVec2(m_fThumbnailSize, m_fThumbnailSize)))
		{
			m_iSelectedIndex = static_cast<int>(i);
		}
		ImGui::TextWrapped("%s", m_vecThumbnails[i].name.c_str());
		ImGui::EndGroup();
		ImGui::PopID();

		if (++iCount % m_iCol != 0)
			ImGui::SameLine();
	}
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