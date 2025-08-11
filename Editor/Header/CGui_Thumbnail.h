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
	explicit CGui_Thumbnail(const string &_label, _uint _reserve);
	virtual ~CGui_Thumbnail();

	virtual void Free() override;
public:
	static CGui_Thumbnail *Create(const string &_label, _uint _reserve);
	virtual _bool Render(_int _iState = -1) override;

	void Add_Thumbnail(_uint _iType, const string &_name, const _tchar *_comp_name, IDirect3DBaseTexture9 *_pTexture);
	void Change_Texture(_uint _iType);

private:
	int m_iSelectedIndex;
	int m_iCol;
	float m_fThumbnailSize;
	float m_fAreaHeight;
	float m_fPadding;
	vector<vector<GuiThumbnailTexture>> m_vecThumbnails;
};