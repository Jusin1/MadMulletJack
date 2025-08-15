#pragma once
#include "CGui_Panel.h"

class CGui_MapEditorPanel : public CGui_Panel
{
private:
	explicit CGui_MapEditorPanel();
	virtual ~CGui_MapEditorPanel();

	virtual void Free() override;
public:
	static CGui_MapEditorPanel *Create();
	virtual void Render() override;

private:
	virtual HRESULT Ready_Panel() override;

	void WallRender();
	void TileRender();
	void EnvObjRender();
	void MonsterRender();
	void LightRender();

	CGuiBase *CategoryDropbox_Create();
	CGuiBase *WalltypeDropbox_Create();
	CGuiBase *TiletypeDropbox_Create();
	CGuiBase *EnvObjtypeDropbox_Create();
	CGuiBase *MonstertypeDropbox_Create();
	CGuiBase *LighttypeDropbox_Create();
	CGuiBase *CreateButton_Create();
	CGuiBase *GridPanelSizeButtons_Create();
	CGuiBase *PositionInputfield_Create();
	CGuiBase *RotationInputfield_Create();
	CGuiBase *WallThumbnail_Create();
	CGuiBase *TileThumbnail_Create();
	CGuiBase *EnvThumbnail_Create();
	CGuiBase *MonsterThumbnail_Create();
	CGuiBase *CreateModeCheckBox_Create();
	CGuiBase *SnapModeCheckBox_Create();

	void AllCheckBox_SetFalse();
private:
	MapEditorObjectCategory m_eCategory;
	_uint m_iObjectType;
};

