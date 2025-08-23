#pragma once
#include "CGui_Panel.h"

// 에디터에서 PrefabLayer에 Child를 저장하지않으면
// 클라이언트에서 인스턴싱 할 때 중복 생성하게 된다.
//

class CGui_PrefabEditorPanel : public CGui_Panel
{
private:
	explicit CGui_PrefabEditorPanel();
	virtual ~CGui_PrefabEditorPanel();

	virtual void Free() override;
public:
	static CGui_PrefabEditorPanel *Create();
	virtual void Render() override;
private:
	virtual HRESULT Ready_Panel() override;

	void WallRender();
	void TileRender();
	void EnvObjRender();
	void MonsterRender();
	void LightRender();
	void PrefabRender();

	CGuiBase *CategoryDropbox_Create();
	CGuiBase *WalltypeDropbox_Create();
	CGuiBase *TiletypeDropbox_Create();
	CGuiBase *EnvObjtypeDropbox_Create();
	CGuiBase *MonstertypeDropbox_Create();
	CGuiBase *LighttypeDropbox_Create();
	CGuiBase *SceneDropbox_Create();
	CGuiBase *PrefabtypeDropbox_Create();
	CGuiBase *CreateButton_Create();
	CGuiBase *GridPanelSizeButtons_Create();
	CGuiBase *SaveDataButton_Create();
	CGuiBase *PositionInputfield_Create();
	CGuiBase *RotationInputfield_Create();
	CGuiBase *WallThumbnail_Create();
	CGuiBase *TileThumbnail_Create();
	CGuiBase *EnvThumbnail_Create();
	CGuiBase *MonsterThumbnail_Create();
	CGuiBase *CreateModeCheckBox_Create();
	CGuiBase *SnapModeCheckBox_Create();

	void CategoryDropbox_Render();
	void WalltypeDropbox_Render();
	void TiletypeDropbox_Render();
	void EnvObjtypeDropbox_Render();
	void MonstertypeDropbox_Render();
	void LighttypeDropbox_Render();
	void SceneDropbox_Render();
	void PrefabtypeDropbox_Render();

	void AllCheckBox_SetFalse();
	void AllThumbnailTexture_SetClear();

	void ChangeType(_uint _iType);

	void ClearScene();
	void SetScene();
public:
		const _tchar *GetSelectedThumbnailTexture();
private:
	_uint m_iPrefabType{0};
	ObjectCategory m_eChildrenObjectCategory{ ObjectCategory::WALL};
	_uint m_iChildrenObjectType{0};
	_uint m_iSceneType{ SCENE_PREFAB };
};

