#pragma once
#include "CGuiBase.h"

typedef struct GuiThumbnailTexture
{
	std::string name = "";
	const _tchar *comp_name = nullptr;
	IDirect3DBaseTexture9 *pTexture = nullptr;
	ImTextureID imguiID = 0;
}THUMBDATA;

class CGui_Thumbnail : public CGuiBase
{
private:
	explicit CGui_Thumbnail(const string &_label);
	virtual ~CGui_Thumbnail();

	virtual void Free() override;
public:
	static CGui_Thumbnail *Create(const string &_label);
	virtual void Render() override;

	void Add_Thumbnail(const string &_name, const _tchar *_comp_name, IDirect3DBaseTexture9 *_pTexture);
	void Change_Texture();
private:
	int m_iSelectedIndex;
	int m_iCol;
	float m_fThumbnailSize;
	float m_fPadding;
	vector<GuiThumbnailTexture> m_vecThumbnails;
};