#include "pch.h"
#include "CGui_Dropbox.h"
#include "CGuiManager.h"
#include "CGameObject.h"
#include "CGui_Button.h"
#include "CGui_ButtonList.h"
#include "CGui_Transform.h"
#include "CGui_Checkbox.h"
#include "CGui_Thumbnail.h"
#include "CVIBuffer_GridPanel_Editor.h"
#include "CGraphicDev.h"
#include "CEditorLoadingScene.h"
#include "CObjectManager.h"
#include "CGridPanel.h"
#include "CManagement.h"
#include "CGui_MapEditorPanel.h"

CGui_MapEditorPanel::CGui_MapEditorPanel()
	: CGui_Panel("Insepctor"), m_eCategory(ObjectCategory::WALL), m_iObjectType(0)
{
	m_pElements = std::vector<CGuiBase *>{ g_MapEditorGuiTypeCount };
}

CGui_MapEditorPanel::~CGui_MapEditorPanel()
{
}

void CGui_MapEditorPanel::Free()
{
	CGui_Panel::Free();
}

CGui_MapEditorPanel *CGui_MapEditorPanel::Create()
{
	CGui_MapEditorPanel *pNew = new CGui_MapEditorPanel();
	if (FAILED(pNew->Ready_Panel()))
	{
		MSG_BOX("CGui_MapEditorPanel::Create, Failed");
		Safe_Release(pNew);
		return nullptr;
	}

	return pNew;
}

void CGui_MapEditorPanel::Render()
{
	ImGui::Begin(m_title.c_str());

	CategoryDropbox_Render();

	switch (m_eCategory)
	{
	case ObjectCategory::WALL:
	{
		WallRender();
	} break;
	case ObjectCategory::TILE:
	{
		TileRender();
	} break;
	case ObjectCategory::ENV_OBJ:
	{
		EnvObjRender();
	} break;
	case ObjectCategory::MONSTER:
	{
		MonsterRender();
	} break;
	case ObjectCategory::LIGHT:
	{
		LightRender();
	} break;
	}

	ImGui::End();
}

HRESULT CGui_MapEditorPanel::Ready_Panel()
{
	m_pElements[static_cast<_uint>(MapEditorGuiType::CATEGORY_DROPBOX)] = CategoryDropbox_Create();
	m_pElements[static_cast<_uint>(MapEditorGuiType::WALL_TYPE_DROPBOX)] = WalltypeDropbox_Create();
	m_pElements[static_cast<_uint>(MapEditorGuiType::TILE_TYPE_DROPBOX)] = TiletypeDropbox_Create();
	m_pElements[static_cast<_uint>(MapEditorGuiType::ENVOBJ_TYPE_DROPBOX)] = EnvObjtypeDropbox_Create();
	m_pElements[static_cast<_uint>(MapEditorGuiType::MONSTER_TYPE_DROPBOX)] = MonstertypeDropbox_Create();
	m_pElements[static_cast<_uint>(MapEditorGuiType::LIGHT_TYPE_DROPBOX)] = LighttypeDropbox_Create();
	m_pElements[static_cast<_uint>(MapEditorGuiType::SCENE_TYPE_DROPBOX)] = SceneDropbox_Create();
	m_pElements[static_cast<_uint>(MapEditorGuiType::CREATEMODE_CHECKBOX)] = CreateModeCheckBox_Create();
	m_pElements[static_cast<_uint>(MapEditorGuiType::SNAPMODE_CHECKBOX)] = SnapModeCheckBox_Create();
	m_pElements[static_cast<_uint>(MapEditorGuiType::CREATE_BUTTONS)] = CreateButton_Create();
	m_pElements[static_cast<_uint>(MapEditorGuiType::PANEL_SIZE_BUTTONS)] = GridPanelSizeButtons_Create();
	m_pElements[static_cast<_uint>(MapEditorGuiType::SAVE_DATA_BUTTON)] = SaveDataButton_Create();
	m_pElements[static_cast<_uint>(MapEditorGuiType::POSITION)] = PositionInputfield_Create();
	m_pElements[static_cast<_uint>(MapEditorGuiType::ROTATION)] = RotationInputfield_Create();
	m_pElements[static_cast<_uint>(MapEditorGuiType::WALL_THUMBNAIL)] = WallThumbnail_Create();
	m_pElements[static_cast<_uint>(MapEditorGuiType::TILE_THUMBNAIL)] = TileThumbnail_Create();
	m_pElements[static_cast<_uint>(MapEditorGuiType::ENV_THUMBNAIL)] = EnvThumbnail_Create();
	m_pElements[static_cast<_uint>(MapEditorGuiType::MONSTER_THUMBNAIL)] = MonsterThumbnail_Create();

	return S_OK;
}

void CGui_MapEditorPanel::WallRender()
{
	WalltypeDropbox_Render();

	m_pElements[static_cast<_uint>(MapEditorGuiType::CREATE_BUTTONS)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::PANEL_SIZE_BUTTONS)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::POSITION)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::WALL_THUMBNAIL)]->Render(m_iObjectType);

	SceneDropbox_Render();
	m_pElements[static_cast<_uint>(MapEditorGuiType::SAVE_DATA_BUTTON)]->Render(m_iObjectType);
}

void CGui_MapEditorPanel::TileRender()
{
	TiletypeDropbox_Render();
	m_pElements[static_cast<_uint>(MapEditorGuiType::CREATE_BUTTONS)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::CREATEMODE_CHECKBOX)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::SNAPMODE_CHECKBOX)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::POSITION)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::TILE_THUMBNAIL)]->Render(m_iObjectType);

	SceneDropbox_Render();
	m_pElements[static_cast<_uint>(MapEditorGuiType::SAVE_DATA_BUTTON)]->Render(m_iObjectType);
}

void CGui_MapEditorPanel::EnvObjRender()
{
	EnvObjtypeDropbox_Render();
	m_pElements[static_cast<_uint>(MapEditorGuiType::CREATE_BUTTONS)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::CREATEMODE_CHECKBOX)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::SNAPMODE_CHECKBOX)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::POSITION)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::ENV_THUMBNAIL)]->Render(m_iObjectType);

	SceneDropbox_Render();
	m_pElements[static_cast<_uint>(MapEditorGuiType::SAVE_DATA_BUTTON)]->Render(m_iObjectType);
}

void CGui_MapEditorPanel::MonsterRender()
{
	MonstertypeDropbox_Render();
	m_pElements[static_cast<_uint>(MapEditorGuiType::CREATE_BUTTONS)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::CREATEMODE_CHECKBOX)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::SNAPMODE_CHECKBOX)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::POSITION)]->Render(m_iObjectType);
	
	SceneDropbox_Render();
	m_pElements[static_cast<_uint>(MapEditorGuiType::SAVE_DATA_BUTTON)]->Render(m_iObjectType);
}

void CGui_MapEditorPanel::LightRender()
{
	LighttypeDropbox_Render();



	SceneDropbox_Render();
	m_pElements[static_cast<_uint>(MapEditorGuiType::SAVE_DATA_BUTTON)]->Render(m_iObjectType);
}

CGuiBase *CGui_MapEditorPanel::CategoryDropbox_Create()
{
	vector<string> Names{g_ObjectCategoryCount};
	Names[0] = "Wall";
	Names[1] = "Tile";
	Names[2] = "Env";
	Names[3] = "Monster";
	Names[4] = "Light";

	return CGui_Dropbox<ObjectCategory>::Create("Category", ObjectCategory::WALL, Names);
}

CGuiBase *CGui_MapEditorPanel::WalltypeDropbox_Create()
{
	vector<string> Names{ g_WallTypeCount };
	Names[0] = "Wall_Hor";
	Names[1] = "Wall_Ver";
	Names[2] = "Incline";
	Names[3] = "Floor";
	Names[4] = "Ceiling";
	Names[5] = "SideDash";

	return CGui_Dropbox<WallType>::Create("WallType", WallType::WALL_HOR, Names);
}

CGuiBase *CGui_MapEditorPanel::TiletypeDropbox_Create()
{
	vector<string> Names{ g_TileTypeCount };
	Names[0] = "Deco";
	Names[1] = "Glass";
	Names[2] = "Acid";
	Names[3] = "Electric";
	Names[4] = "Vent";
	Names[5] = "NormalDoor";
	Names[6] = "OpeningDoor";
	Names[7] = "Display";
	Names[8] = "Bottle";
	Names[9] = "VendingMachine";

	return CGui_Dropbox<TileType>::Create("TileType", TileType::DECO, Names);
}

CGuiBase *CGui_MapEditorPanel::EnvObjtypeDropbox_Create()
{
	vector<string> Names{ g_EnvTypeCount };
	Names[0] = "SpawnPoint";
	Names[1] = "EndPoint";

	return CGui_Dropbox<EnvType>::Create("EnvType", EnvType::SPAWNPOINT, Names);
}

CGuiBase *CGui_MapEditorPanel::MonstertypeDropbox_Create()
{
	vector<string> Names{ g_MonsterTypeCount };
	Names[0] = "Suit";

	return CGui_Dropbox<MonsterType>::Create("MonsterType", MonsterType::SUIT, Names);
}

CGuiBase *CGui_MapEditorPanel::LighttypeDropbox_Create()
{
	vector<string> Names{ g_LightTypeCount };
	Names[0] = "NONE";

	return CGui_Dropbox<LightType>::Create("LightType", LightType::tmp, Names);
}

CGuiBase *CGui_MapEditorPanel::SceneDropbox_Create()
{
	vector<string> Names{ g_MapEditorSceneTypeCount };
	Names[0] = "DEV";
	Names[1] = "TUTORIAL";
	Names[2] = "STAGE_1";
	Names[3] = "STAGE_2";
	Names[4] = "STAGE_3";
	Names[5] = "SNIPE";
	Names[6] = "ROOFTOP";
	Names[7] = "ROAD";

	return CGui_Dropbox<MapEditorSceneType>::Create("SceneType", MapEditorSceneType::DEV, Names);
}

CGuiBase *CGui_MapEditorPanel::CreateButton_Create()
{
	vector<string> _labels{ 2 };
	vector<std::function<void()>> _funcs{ 2 };

	_labels[0] = "Create";
	_funcs[0] =
		[this]()->void {
		ObjectCategory eCategory = CGuiManager::GetInstance()->GetCategory();
		switch (eCategory)
		{
		case ObjectCategory::WALL:
		{
			// TODO : type에 따른 생성
			MAPOBJECTDATA defaultData;
			defaultData.eCategory = ObjectCategory::WALL;
			defaultData.iType = m_iObjectType;
			defaultData.panelBuffer.eType = static_cast<WallType>(m_iObjectType);
			defaultData.texture.OriginComponentName = GetSelectedThumbnailTexture();
			_uint iCurSceneID = CManagement::GetInstance()->Get_CurrentSceneIdx();
			CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_DefaultPanel", iCurSceneID, L"Wall_Layer", &defaultData);
			CGuiManager::GetInstance()->SetTarget(CObjectManager::GetInstance()->Get_ObjectList(iCurSceneID, L"Wall_Layer")->back());
		} break;
		case ObjectCategory::TILE:
		{
			MSG_BOX("CGui_MapEditorPanel::CreateButton_Create(), write function");
		} break;
		case ObjectCategory::ENV_OBJ:
		{
			MSG_BOX("CGui_MapEditorPanel::CreateButton_Create(), write function");
		} break;
		case ObjectCategory::MONSTER:
		{
			MSG_BOX("CGui_MapEditorPanel::CreateButton_Create(), write function");
		} break;
		case ObjectCategory::LIGHT:
		{
			MSG_BOX("CGui_MapEditorPanel::CreateButton_Create(), write function");
		} break;
		}
	};

	_labels[1] = "Delete";
	_funcs[1] =
		[]()->void {
		if (CGameObject *pGo = CGuiManager::GetInstance()->GetTarget())
		{
			pGo->Set_Dead(TRUE);
			CGuiManager::GetInstance()->SetTarget(nullptr);
		}
	};

	return CGui_ButtonList::Create("Wall Create", _labels, _funcs);
}

CGuiBase *CGui_MapEditorPanel::GridPanelSizeButtons_Create()
{
	vector<string> _labels{ 6 };
	vector<std::function<void()>> _funcs{ 6 };

	_labels[0] = "++Row";
	_funcs[0] =
		[]()->void {
		if (CGameObject *pGo = CGuiManager::GetInstance()->GetTarget())
		{
			static_cast<CGridPanel *>(pGo)->GetBuffer()->Increase_RowBuffer();
		}
	};

	_labels[1] = "--Row";
	_funcs[1] =
		[]()->void {
		if (CGameObject *pGo = CGuiManager::GetInstance()->GetTarget())
		{
			static_cast<CGridPanel *>(pGo)->GetBuffer()->Decrease_RowBuffer();
		}
	};

	_labels[2] = "++Col";
	_funcs[2] =
		[]()->void {
		if (CGameObject *pGo = CGuiManager::GetInstance()->GetTarget())
		{
			static_cast<CGridPanel *>(pGo)->GetBuffer()->Increase_ColBuffer();
		}
	};

	_labels[3] = "--Col";
	_funcs[3] =
		[]()->void {
		if (CGameObject *pGo = CGuiManager::GetInstance()->GetTarget())
		{
			static_cast<CGridPanel *>(pGo)->GetBuffer()->Decrease_ColBuffer();
		}
	};

	_labels[4] = "++Interval";
	_funcs[4] =
		[]()->void {
		if (CGameObject *pGo = CGuiManager::GetInstance()->GetTarget())
		{
			static_cast<CGridPanel *>(pGo)->GetBuffer()->Increase_Interval();
		}
	};

	_labels[5] = "--Interval";
	_funcs[5] =
		[]()->void {
		if (CGameObject *pGo = CGuiManager::GetInstance()->GetTarget())
		{
			static_cast<CGridPanel *>(pGo)->GetBuffer()->Decrease_Interval();
		}
	};

	return CGui_ButtonList::Create("Grid Size", _labels, _funcs);
}

CGuiBase *CGui_MapEditorPanel::SaveDataButton_Create()
{
	std::function func =
		[]()->void {
		CManagement::GetInstance()->SaveData();
	};

	return CGui_Button::Create("SaveJson", func);
}

CGuiBase *CGui_MapEditorPanel::PositionInputfield_Create()
{
	return CGui_Transform::Create(TransformDataType::POSITION);;
}

CGuiBase *CGui_MapEditorPanel::RotationInputfield_Create()
{
	return nullptr;
}

#define AddThumbnail(ThumnailName, CompName, _pThumbnail, eType)	\
CGuiManager::GetInstance()->AddThumbnail(ThumnailName, CompName, _pThumbnail, static_cast<_uint>(eType))


#pragma region WallThumbnail
CGuiBase *CGui_MapEditorPanel::WallThumbnail_Create()
{
	CGui_Thumbnail *pThumbnail = CGui_Thumbnail::Create("Textures", g_WallTypeCount);

	//==================
	// WALL
	//==================
	// HOR
	AddThumbnail("Acid_Wall_1", L"Proto_Acid_Wall_1", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Acid_Wall_2", L"Proto_Acid_Wall_2", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Concrete_Wall", L"Proto_Concrete_Wall", pThumbnail, WallType::WALL_HOR);

	AddThumbnail("Corner_Wall_1", L"Proto_Corner_Wall_1", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Corner_Wall_2", L"Proto_Corner_Wall_2", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Corner_Wall_3", L"Proto_Corner_Wall_3", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Corner_Wall_4", L"Proto_Corner_Wall_4", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Corner_Wall_5_1", L"Proto_Corner_Wall_5_1", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Corner_Wall_5_2", L"Proto_Corner_Wall_5_2", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Corner_Wall_6", L"Proto_Corner_Wall_6", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Corner_Wall_7", L"Proto_Corner_Wall_7", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Corner_Wall_8", L"Proto_Corner_Wall_8", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Corner_Wall_Boss", L"Proto_Corner_Wall_Boss", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_1A", L"Proto_Wall_1A", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_1B", L"Proto_Wall_1B", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_1C", L"Proto_Wall_1C", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_2A", L"Proto_Wall_2A", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_2B", L"Proto_Wall_2B", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_2C", L"Proto_Wall_2C", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_3A_1", L"Proto_Wall_3A_1", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_3A_2", L"Proto_Wall_3A_2", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_3B_1", L"Proto_Wall_3B_1", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_3B_2", L"Proto_Wall_3B_2", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_3C_1", L"Proto_Wall_3C_1", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_3C_2", L"Proto_Wall_3C_2", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_4A", L"Proto_Wall_4A", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_4B", L"Proto_Wall_4B", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_4C_1", L"Proto_Wall_4C_1", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_4C_2", L"Proto_Wall_4C_2", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_5A", L"Proto_Wall_5A", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_5B", L"Proto_Wall_5B", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_5C", L"Proto_Wall_5C", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_6A", L"Proto_Wall_6A", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_6B", L"Proto_Wall_6B", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_6C", L"Proto_Wall_6C", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_7A", L"Proto_Wall_7A", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_7B", L"Proto_Wall_7B", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_7C", L"Proto_Wall_7C", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_8A", L"Proto_Wall_8A", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_8B", L"Proto_Wall_8B", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_8C", L"Proto_Wall_8C", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_9A", L"Proto_Wall_9A", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_9B", L"Proto_Wall_9B", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_9C", L"Proto_Wall_9C", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_10A", L"Proto_Wall_10A", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_10B", L"Proto_Wall_10B", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_10C", L"Proto_Wall_10C", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_Deco_1", L"Proto_Wall_Deco_1", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_Deco_2", L"Proto_Wall_Deco_2", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_Deco_3", L"Proto_Wall_Deco_3", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_Deco_4", L"Proto_Wall_Deco_4", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_Deco_5", L"Proto_Wall_Deco_5", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_Deco_6", L"Proto_Wall_Deco_6", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_Deco_7", L"Proto_Wall_Deco_7", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_Deco_8", L"Proto_Wall_Deco_8", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_Hole_1", L"Proto_Wall_Hole_1", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_Hole_2", L"Proto_Wall_Hole_2", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_Hole_3", L"Proto_Wall_Hole_3", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_Hole_4", L"Proto_Wall_Hole_4", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_Border", L"Proto_Wall_Border", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_Boss_1", L"Proto_Wall_Boss_1", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_Boss_2", L"Proto_Wall_Boss_2", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_Boss_3", L"Proto_Wall_Boss_3", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Wall_Elevator", L"Proto_Wall_Elevator", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Fence", L"Proto_Fence", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("JumpBorder", L"Proto_JumpBorder", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Windows", L"Proto_Windows", pThumbnail, WallType::WALL_HOR);

	// VER
	AddThumbnail("Acid_Wall_1", L"Proto_Acid_Wall_1", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Acid_Wall_2", L"Proto_Acid_Wall_2", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Concrete_Wall", L"Proto_Concrete_Wall", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Corner_Wall_1", L"Proto_Corner_Wall_1", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Corner_Wall_2", L"Proto_Corner_Wall_2", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Corner_Wall_3", L"Proto_Corner_Wall_3", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Corner_Wall_4", L"Proto_Corner_Wall_4", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Corner_Wall_5_1", L"Proto_Corner_Wall_5_1", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Corner_Wall_5_2", L"Proto_Corner_Wall_5_2", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Corner_Wall_6", L"Proto_Corner_Wall_6", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Corner_Wall_7", L"Proto_Corner_Wall_7", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Corner_Wall_8", L"Proto_Corner_Wall_8", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Corner_Wall_Boss", L"Proto_Corner_Wall_Boss", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_1A", L"Proto_Wall_1A", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_1B", L"Proto_Wall_1B", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_1C", L"Proto_Wall_1C", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_2A", L"Proto_Wall_2A", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_2B", L"Proto_Wall_2B", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_2C", L"Proto_Wall_2C", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_3A_1", L"Proto_Wall_3A_1", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_3A_2", L"Proto_Wall_3A_2", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_3B_1", L"Proto_Wall_3B_1", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_3B_2", L"Proto_Wall_3B_2", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_3C_1", L"Proto_Wall_3C_1", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_3C_2", L"Proto_Wall_3C_2", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_4A", L"Proto_Wall_4A", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_4B", L"Proto_Wall_4B", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_4C_1", L"Proto_Wall_4C_1", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_4C_2", L"Proto_Wall_4C_2", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_5A", L"Proto_Wall_5A", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_5B", L"Proto_Wall_5B", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_5C", L"Proto_Wall_5C", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_6A", L"Proto_Wall_6A", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_6B", L"Proto_Wall_6B", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_6C", L"Proto_Wall_6C", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_7A", L"Proto_Wall_7A", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_7B", L"Proto_Wall_7B", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_7C", L"Proto_Wall_7C", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_8A", L"Proto_Wall_8A", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_8B", L"Proto_Wall_8B", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_8C", L"Proto_Wall_8C", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_9A", L"Proto_Wall_9A", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_9B", L"Proto_Wall_9B", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_9C", L"Proto_Wall_9C", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_10A", L"Proto_Wall_10A", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_10B", L"Proto_Wall_10B", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_10C", L"Proto_Wall_10C", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_Deco_1", L"Proto_Wall_Deco_1", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_Deco_2", L"Proto_Wall_Deco_2", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_Deco_3", L"Proto_Wall_Deco_3", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_Deco_4", L"Proto_Wall_Deco_4", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_Deco_5", L"Proto_Wall_Deco_5", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_Deco_6", L"Proto_Wall_Deco_6", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_Deco_7", L"Proto_Wall_Deco_7", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_Deco_8", L"Proto_Wall_Deco_8", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_Hole_1", L"Proto_Wall_Hole_1", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_Hole_2", L"Proto_Wall_Hole_2", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_Hole_3", L"Proto_Wall_Hole_3", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_Hole_4", L"Proto_Wall_Hole_4", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_Border", L"Proto_Wall_Border", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Wall_Elevator", L"Proto_Wall_Elevator", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Fence", L"Proto_Fence", pThumbnail, WallType::WALL_VER);
	AddThumbnail("JumpBorder", L"Proto_JumpBorder", pThumbnail, WallType::WALL_VER);
	AddThumbnail("Windows", L"Proto_Windows", pThumbnail, WallType::WALL_VER);

	//==================
	// FLOOR
	//==================
	AddThumbnail("Floor_1", L"Proto_Floor_1", pThumbnail, WallType::FLOOR);
	AddThumbnail("Floor_2", L"Proto_Floor_2", pThumbnail, WallType::FLOOR);
	AddThumbnail("Floor_3", L"Proto_Floor_3", pThumbnail, WallType::FLOOR);
	AddThumbnail("Floor_4", L"Proto_Floor_4", pThumbnail, WallType::FLOOR);
	AddThumbnail("Floor_5", L"Proto_Floor_5", pThumbnail, WallType::FLOOR);
	AddThumbnail("Floor_6", L"Proto_Floor_6", pThumbnail, WallType::FLOOR);
	AddThumbnail("Floor_7", L"Proto_Floor_7", pThumbnail, WallType::FLOOR);
	AddThumbnail("Acid_Floor_1", L"Proto_Acid_Floor_1", pThumbnail, WallType::FLOOR);
	AddThumbnail("Acid_Floor_2", L"Proto_Acid_Floor_2", pThumbnail, WallType::FLOOR);
	AddThumbnail("Floor_Elevator", L"Proto_Floor_Elevator", pThumbnail, WallType::FLOOR);

	//==================
	// INCLINE
	//==================
	AddThumbnail("Steira", L"Proto_Steira", pThumbnail, WallType::INCLINE);

	//==================
	// CEILING
	//==================
	AddThumbnail("Floor_1", L"Proto_Floor_1", pThumbnail, WallType::CEILING);
	AddThumbnail("Floor_2", L"Proto_Floor_2", pThumbnail, WallType::CEILING);
	AddThumbnail("Floor_3", L"Proto_Floor_3", pThumbnail, WallType::CEILING);
	AddThumbnail("Floor_4", L"Proto_Floor_4", pThumbnail, WallType::CEILING);
	AddThumbnail("Floor_5", L"Proto_Floor_5", pThumbnail, WallType::CEILING);
	AddThumbnail("Floor_6", L"Proto_Floor_6", pThumbnail, WallType::CEILING);
	AddThumbnail("Floor_7", L"Proto_Floor_7", pThumbnail, WallType::CEILING);
	AddThumbnail("Acid_Floor_1", L"Proto_Acid_Floor_1", pThumbnail, WallType::CEILING);
	AddThumbnail("Acid_Floor_2", L"Proto_Acid_Floor_2", pThumbnail, WallType::CEILING);
	AddThumbnail("Ceiling_Elevator", L"Proto_Ceiling_Elevator", pThumbnail, WallType::CEILING);

	//==================
	// All Category
	//==================
	for (int i = 0; i < g_WallTypeCount - 1; ++i)
	{
		WallType eType = static_cast<WallType>(i);
		AddThumbnail("Default", L"Proto_GridDefault", pThumbnail, eType);
		AddThumbnail("Trigger", L"Proto_GridTrigger", pThumbnail, eType);
		AddThumbnail("Collider", L"Proto_GridCollider", pThumbnail, eType);
		AddThumbnail("NoDraw", L"Proto_GridNoDraw", pThumbnail, eType);
	}

	//==================
	// WALL_SLIDE
	//==================
	AddThumbnail("Wall_Slidedash", L"Proto_Wall_Slidedash", pThumbnail, WallType::WALL_SLIDE);

	return pThumbnail;
}
#pragma endregion

CGuiBase *CGui_MapEditorPanel::TileThumbnail_Create()
{
	CGui_Thumbnail *pThumbnail = CGui_Thumbnail::Create("Textures", g_TileTypeCount);

	//==================
	// Deco
	//==================
	AddThumbnail("Floor_1", L"Proto_Floor_1", pThumbnail, TileType::DECO);
	AddThumbnail("BIO_1", L"Proto_BIO_1", pThumbnail, TileType::DECO);
	AddThumbnail("BIO_2", L"Proto_BIO_2", pThumbnail, TileType::DECO);
	AddThumbnail("Rug_1", L"Proto_Rug_1", pThumbnail, TileType::DECO);
	AddThumbnail("Rug_2", L"Proto_Rug_2", pThumbnail, TileType::DECO);
	AddThumbnail("Rug_3", L"Proto_Rug_3", pThumbnail, TileType::DECO);
	AddThumbnail("Rug_4", L"Proto_Rug_4", pThumbnail, TileType::DECO);
	AddThumbnail("Katana", L"Proto_Wall_Katana", pThumbnail, TileType::DECO);
	AddThumbnail("Transparent_1", L"Proto_Wall_Transparent_1", pThumbnail, TileType::DECO);
	AddThumbnail("Transparent_2", L"Proto_Wall_Transparent_2", pThumbnail, TileType::DECO);
	AddThumbnail("Transparent_3", L"Proto_Wall_Transparent_3", pThumbnail, TileType::DECO);
	AddThumbnail("Transparent_4", L"Proto_Wall_Transparent_4", pThumbnail, TileType::DECO);
	AddThumbnail("Default", L"Proto_GridDefault", pThumbnail, TileType::DECO);
	AddThumbnail("Trigger", L"Proto_GridTrigger", pThumbnail, TileType::DECO);
	AddThumbnail("Collider", L"Proto_GridCollider", pThumbnail, TileType::DECO);
	AddThumbnail("Acid_Wall_1", L"Proto_Acid_Wall_1", pThumbnail, TileType::DECO);
	AddThumbnail("Acid_Wall_2", L"Proto_Acid_Wall_2", pThumbnail, TileType::DECO);
	AddThumbnail("Concrete_Wall", L"Proto_Concrete_Wall", pThumbnail, TileType::DECO);
	AddThumbnail("Corner_Wall_1", L"Proto_Corner_Wall_1", pThumbnail, TileType::DECO);
	AddThumbnail("Corner_Wall_2", L"Proto_Corner_Wall_2", pThumbnail, TileType::DECO);
	AddThumbnail("Corner_Wall_3", L"Proto_Corner_Wall_3", pThumbnail, TileType::DECO);
	AddThumbnail("Corner_Wall_4", L"Proto_Corner_Wall_4", pThumbnail, TileType::DECO);
	AddThumbnail("Corner_Wall_5_1", L"Proto_Corner_Wall_5_1", pThumbnail, TileType::DECO);
	AddThumbnail("Corner_Wall_5_2", L"Proto_Corner_Wall_5_2", pThumbnail, TileType::DECO);
	AddThumbnail("Corner_Wall_6", L"Proto_Corner_Wall_6", pThumbnail, TileType::DECO);
	AddThumbnail("Corner_Wall_7", L"Proto_Corner_Wall_7", pThumbnail, TileType::DECO);
	AddThumbnail("Corner_Wall_8", L"Proto_Corner_Wall_8", pThumbnail, TileType::DECO);
	AddThumbnail("Corner_Wall_Boss", L"Proto_Corner_Wall_Boss", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_1A", L"Proto_Wall_1A", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_1B", L"Proto_Wall_1B", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_1C", L"Proto_Wall_1C", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_2A", L"Proto_Wall_2A", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_2B", L"Proto_Wall_2B", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_2C", L"Proto_Wall_2C", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_3A_1", L"Proto_Wall_3A_1", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_3A_2", L"Proto_Wall_3A_2", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_3B_1", L"Proto_Wall_3B_1", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_3B_2", L"Proto_Wall_3B_2", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_3C_1", L"Proto_Wall_3C_1", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_3C_2", L"Proto_Wall_3C_2", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_4A", L"Proto_Wall_4A", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_4B", L"Proto_Wall_4B", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_4C_1", L"Proto_Wall_4C_1", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_4C_2", L"Proto_Wall_4C_2", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_5A", L"Proto_Wall_5A", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_5B", L"Proto_Wall_5B", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_5C", L"Proto_Wall_5C", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_6A", L"Proto_Wall_6A", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_6B", L"Proto_Wall_6B", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_6C", L"Proto_Wall_6C", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_7A", L"Proto_Wall_7A", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_7B", L"Proto_Wall_7B", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_7C", L"Proto_Wall_7C", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_8A", L"Proto_Wall_8A", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_8B", L"Proto_Wall_8B", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_8C", L"Proto_Wall_8C", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_9A", L"Proto_Wall_9A", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_9B", L"Proto_Wall_9B", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_9C", L"Proto_Wall_9C", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_10A", L"Proto_Wall_10A", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_10B", L"Proto_Wall_10B", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_10C", L"Proto_Wall_10C", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_Deco_1", L"Proto_Wall_Deco_1", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_Deco_2", L"Proto_Wall_Deco_2", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_Deco_3", L"Proto_Wall_Deco_3", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_Deco_4", L"Proto_Wall_Deco_4", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_Deco_5", L"Proto_Wall_Deco_5", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_Deco_6", L"Proto_Wall_Deco_6", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_Deco_7", L"Proto_Wall_Deco_7", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_Deco_8", L"Proto_Wall_Deco_8", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_Hole_1", L"Proto_Wall_Hole_1", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_Hole_2", L"Proto_Wall_Hole_2", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_Hole_3", L"Proto_Wall_Hole_3", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_Hole_4", L"Proto_Wall_Hole_4", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_Border", L"Proto_Wall_Border", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_Boss_1", L"Proto_Wall_Boss_1", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_Boss_2", L"Proto_Wall_Boss_2", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_Boss_3", L"Proto_Wall_Boss_3", pThumbnail, TileType::DECO);
	AddThumbnail("Wall_Elevator", L"Proto_Wall_Elevator", pThumbnail, TileType::DECO);
	AddThumbnail("Fence", L"Proto_Fence", pThumbnail, TileType::DECO);
	AddThumbnail("JumpBorder", L"Proto_JumpBorder", pThumbnail, TileType::DECO);
	AddThumbnail("Windows", L"Proto_Windows", pThumbnail, TileType::DECO);
	AddThumbnail("NoDraw", L"Proto_GridNoDraw", pThumbnail, TileType::DECO);
	AddThumbnail("Cable_1", L"Proto_CABLES_1", pThumbnail, TileType::DECO);
	AddThumbnail("Cable_2", L"Proto_CABLES_2", pThumbnail, TileType::DECO);

	// Acid
	AddThumbnail("Acid_Floor_1", L"Proto_Acid_Floor_1", pThumbnail, TileType::ACID);
	AddThumbnail("Acid_Floor_2", L"Proto_Acid_Floor_2", pThumbnail, TileType::ACID);

	// Glass
	AddThumbnail("Glass", L"Proto_Glass", pThumbnail, TileType::GLASS);

	// Electric
	AddThumbnail("Electirc", L"Proto_Electric_Wall", pThumbnail, TileType::ELECTRIC);

	// Vent
	AddThumbnail("Vent", L"Proto_Vent", pThumbnail, TileType::VENT);
	
	// NormalDoor
	AddThumbnail("Door_1", L"Proto_NormalDoor_1", pThumbnail, TileType::NORMALDOOR);
	AddThumbnail("Door_2", L"Proto_NormalDoor_2", pThumbnail, TileType::NORMALDOOR);
	AddThumbnail("Door_3", L"Proto_NormalDoor_3", pThumbnail, TileType::NORMALDOOR);

	// OpeningDoor
	AddThumbnail("OpeningDoor", L"Proto_OpeningDoor", pThumbnail, TileType::OPENINGDOOR);

	//==================
	// Display
	//==================
	AddThumbnail("Band", L"Proto_Bandit_Outdoor", pThumbnail, TileType::DISPLAY);
	AddThumbnail("Beach", L"Proto_Beach_Outdoor", pThumbnail, TileType::DISPLAY);
	AddThumbnail("Beer", L"Proto_Beer_Outdoor", pThumbnail, TileType::DISPLAY);
	AddThumbnail("Fuck", L"Proto_Fuck_Outdoor", pThumbnail, TileType::DISPLAY);
	AddThumbnail("Kimono", L"Proto_Kimono_Outdoor", pThumbnail, TileType::DISPLAY);
	AddThumbnail("SaveHer", L"Proto_SaveHer_Outdoor", pThumbnail, TileType::DISPLAY);
	AddThumbnail("Shoes", L"Proto_Shoes_Outdoor", pThumbnail, TileType::DISPLAY);

	//==================
	// Bottle
	//==================
	AddThumbnail("Bottle_1", L"Proto_Bottle_1", pThumbnail, TileType::BOTTLE);
	AddThumbnail("Bottle_2", L"Proto_Bottle_2", pThumbnail, TileType::BOTTLE);
	AddThumbnail("Bottle_3", L"Proto_Bottle_3", pThumbnail, TileType::BOTTLE);
	AddThumbnail("Bottle_4", L"Proto_Bottle_4", pThumbnail, TileType::BOTTLE);
	AddThumbnail("Bottle_5", L"Proto_Bottle_5", pThumbnail, TileType::BOTTLE);
	AddThumbnail("Bottle_6", L"Proto_Bottle_6", pThumbnail, TileType::BOTTLE);
	AddThumbnail("Bottle_7", L"Proto_Bottle_7", pThumbnail, TileType::BOTTLE);

	// VandingMachine
	AddThumbnail("VendingMachine", L"Proto_VendingMachine", pThumbnail, TileType::VENDINGMACHINE);

	return pThumbnail;
}

CGuiBase *CGui_MapEditorPanel::EnvThumbnail_Create()
{
	CGui_Thumbnail *pThumbnail = CGui_Thumbnail::Create("Textures", g_EnvTypeCount);

	////==================
	//// Signs
	////==================
	//AddThumbnail("Signs_1", L"Proto_Signs_1", pThumbnail, EnvType::SIGNS);
	//AddThumbnail("Signs_2", L"Proto_Signs_2", pThumbnail, EnvType::SIGNS);
	//AddThumbnail("Signs_3", L"Proto_Signs_3", pThumbnail, EnvType::SIGNS);
	//AddThumbnail("Signs_4", L"Proto_Signs_4", pThumbnail, EnvType::SIGNS);
	//AddThumbnail("Signs_5", L"Proto_Signs_5", pThumbnail, EnvType::SIGNS);
	//AddThumbnail("Signs_6", L"Proto_Signs_6", pThumbnail, EnvType::SIGNS);
	//AddThumbnail("Signs_7", L"Proto_Signs_7", pThumbnail, EnvType::SIGNS);

	return pThumbnail;
}

CGuiBase *CGui_MapEditorPanel::MonsterThumbnail_Create()
{
	CGui_Thumbnail *pThumbnail = CGui_Thumbnail::Create("Textures", g_MonsterTypeCount);

	return pThumbnail;
}

CGuiBase *CGui_MapEditorPanel::CreateModeCheckBox_Create()
{
	CGui_Checkbox *pCheckbox = CGui_Checkbox::Create("CreateMode",
		// TrueEvent
		[this]()->void {
			CGuiManager::GetInstance()->SetCreateMode(TRUE, m_eCategory);
			CGuiManager::GetInstance()->SetTarget(nullptr);
		},
		// FalseEvent
		[this]()->void {
			CGuiManager::GetInstance()->SetCreateMode(FALSE, m_eCategory);
			CGuiManager::GetInstance()->SetTarget(nullptr);
		});
	return pCheckbox;
}

CGuiBase *CGui_MapEditorPanel::SnapModeCheckBox_Create()
{
	CGui_Checkbox *pCheckbox = CGui_Checkbox::Create("SnapMode",
		// TrueEvent
		[this]()->void {
			CGuiManager::GetInstance()->SetSnap(TRUE);
		},
		// FalseEvent
			[this]()->void {
			CGuiManager::GetInstance()->SetSnap(FALSE);
		});
	return pCheckbox;
}

void CGui_MapEditorPanel::CategoryDropbox_Render()
{
	if (m_pElements[static_cast<_uint>(MapEditorGuiType::CATEGORY_DROPBOX)]->Render())
	{
		auto DropboxElement = dynamic_cast<CGui_Dropbox<ObjectCategory> *>
			(m_pElements[static_cast<_uint>(MapEditorGuiType::CATEGORY_DROPBOX)]);

		if (DropboxElement)
		{
			AllCheckBox_SetFalse();
			AllThumbnailTexture_SetClear();
			CGuiManager::GetInstance()->SetTarget(nullptr);

			DropboxElement->Confirm();
			CGuiManager::GetInstance()->SetCategory(DropboxElement->GetConfirmedState());
			m_eCategory = CGuiManager::GetInstance()->GetCategory();
			m_iObjectType = 0;
			CGuiManager::GetInstance()->SetObjectType(0);
		}
	}
}

void CGui_MapEditorPanel::WalltypeDropbox_Render()
{
	if (m_pElements[static_cast<_uint>(MapEditorGuiType::WALL_TYPE_DROPBOX)]->Render())
	{
		auto DropboxElement = dynamic_cast<CGui_Dropbox<WallType> *>
			(m_pElements[static_cast<_uint>(MapEditorGuiType::WALL_TYPE_DROPBOX)]);

		if (DropboxElement)
		{
			AllCheckBox_SetFalse();
			AllThumbnailTexture_SetClear();
			CGuiManager::GetInstance()->SetTarget(nullptr);

			DropboxElement->Confirm();
			CGuiManager::GetInstance()->SetObjectType(static_cast<_uint>(DropboxElement->GetConfirmedState()));
			m_iObjectType = CGuiManager::GetInstance()->GetInstance()->GetObjectType();
		}
	}
}

void CGui_MapEditorPanel::TiletypeDropbox_Render()
{
	if (m_pElements[static_cast<_uint>(MapEditorGuiType::TILE_TYPE_DROPBOX)]->Render())
	{
		auto DropboxElement = dynamic_cast<CGui_Dropbox<TileType> *>
			(m_pElements[static_cast<_uint>(MapEditorGuiType::TILE_TYPE_DROPBOX)]);

		if (DropboxElement)
		{
			AllCheckBox_SetFalse();
			AllThumbnailTexture_SetClear();
			CGuiManager::GetInstance()->SetTarget(nullptr);

			DropboxElement->Confirm();
			CGuiManager::GetInstance()->SetObjectType(static_cast<_uint>(DropboxElement->GetConfirmedState()));
			m_iObjectType = CGuiManager::GetInstance()->GetInstance()->GetObjectType();
		}
	}
}

void CGui_MapEditorPanel::EnvObjtypeDropbox_Render()
{
	if (m_pElements[static_cast<_uint>(MapEditorGuiType::ENVOBJ_TYPE_DROPBOX)]->Render())
	{
		auto DropboxElement = dynamic_cast<CGui_Dropbox<EnvType> *>
			(m_pElements[static_cast<_uint>(MapEditorGuiType::ENVOBJ_TYPE_DROPBOX)]);

		if (DropboxElement)
		{
			AllCheckBox_SetFalse();
			AllThumbnailTexture_SetClear();
			CGuiManager::GetInstance()->SetTarget(nullptr);

			DropboxElement->Confirm();
			CGuiManager::GetInstance()->SetObjectType(static_cast<_uint>(DropboxElement->GetConfirmedState()));
			m_iObjectType = CGuiManager::GetInstance()->GetInstance()->GetObjectType();
		}
	}
}

void CGui_MapEditorPanel::MonstertypeDropbox_Render()
{
	if (m_pElements[static_cast<_uint>(MapEditorGuiType::MONSTER_TYPE_DROPBOX)]->Render())
	{
		auto DropboxElement = dynamic_cast<CGui_Dropbox<MonsterType> *>
			(m_pElements[static_cast<_uint>(MapEditorGuiType::MONSTER_TYPE_DROPBOX)]);

		if (DropboxElement)
		{
			AllCheckBox_SetFalse();
			AllThumbnailTexture_SetClear();
			CGuiManager::GetInstance()->SetTarget(nullptr);

			DropboxElement->Confirm();
			CGuiManager::GetInstance()->SetObjectType(static_cast<_uint>(DropboxElement->GetConfirmedState()));
			m_iObjectType = CGuiManager::GetInstance()->GetInstance()->GetObjectType();
		}
	}
}

void CGui_MapEditorPanel::LighttypeDropbox_Render()
{
	if (m_pElements[static_cast<_uint>(MapEditorGuiType::LIGHT_TYPE_DROPBOX)]->Render())
	{
		auto DropboxElement = dynamic_cast<CGui_Dropbox<LightType> *>
			(m_pElements[static_cast<_uint>(MapEditorGuiType::LIGHT_TYPE_DROPBOX)]);

		if (DropboxElement)
		{
			AllCheckBox_SetFalse();
			AllThumbnailTexture_SetClear();
			CGuiManager::GetInstance()->SetTarget(nullptr);

			DropboxElement->Confirm();
			CGuiManager::GetInstance()->SetObjectType(static_cast<_uint>(DropboxElement->GetConfirmedState()));
			m_iObjectType = CGuiManager::GetInstance()->GetInstance()->GetObjectType();
		}
	}
}

void CGui_MapEditorPanel::SceneDropbox_Render()
{
	if (m_pElements[static_cast<_uint>(MapEditorGuiType::SCENE_TYPE_DROPBOX)]->Render())
	{
		auto DropboxElement = dynamic_cast<CGui_Dropbox<MapEditorSceneType> *>
			(m_pElements[static_cast<_uint>(MapEditorGuiType::SCENE_TYPE_DROPBOX)]);

		if (DropboxElement)
		{
			AllCheckBox_SetFalse();
			AllThumbnailTexture_SetClear();
			CGuiManager::GetInstance()->SetTarget(nullptr);
			DropboxElement->Confirm();
			m_iSceneType = static_cast<_uint>(DropboxElement->GetConfirmedState());
			CManagement::GetInstance()->Open_Scene(SCENE_LOADING, CEditorLoadingScene::Create(CGraphicDev::GetInstance()->Get_GraphicDev(), (SCENE)(m_iSceneType + 3)));
		}
	}
}

void CGui_MapEditorPanel::AllCheckBox_SetFalse()
{
	static_cast<CGui_Checkbox *>(m_pElements[static_cast<_uint>(MapEditorGuiType::CREATEMODE_CHECKBOX)])->OnFalse();
	static_cast<CGui_Checkbox *>(m_pElements[static_cast<_uint>(MapEditorGuiType::SNAPMODE_CHECKBOX)])->OnFalse();
}

void CGui_MapEditorPanel::AllThumbnailTexture_SetClear()
{
	static_cast<CGui_Thumbnail *>(m_pElements[static_cast<_uint>(MapEditorGuiType::WALL_THUMBNAIL)])->Set_Clear();
	static_cast<CGui_Thumbnail *>(m_pElements[static_cast<_uint>(MapEditorGuiType::TILE_THUMBNAIL)])->Set_Clear();
	static_cast<CGui_Thumbnail *>(m_pElements[static_cast<_uint>(MapEditorGuiType::ENV_THUMBNAIL)])->Set_Clear();
	static_cast<CGui_Thumbnail *>(m_pElements[static_cast<_uint>(MapEditorGuiType::MONSTER_THUMBNAIL)])->Set_Clear();
}

const _tchar *CGui_MapEditorPanel::GetSelectedThumbnailTexture()
{
	switch (m_eCategory)
	{
	case Engine::ObjectCategory::WALL:
		return static_cast<CGui_Thumbnail *>(m_pElements[static_cast<_uint>(MapEditorGuiType::WALL_THUMBNAIL)])->GetSelectedCompName(m_iObjectType);
	case Engine::ObjectCategory::TILE:
		return static_cast<CGui_Thumbnail *>(m_pElements[static_cast<_uint>(MapEditorGuiType::TILE_THUMBNAIL)])->GetSelectedCompName(m_iObjectType);
	case Engine::ObjectCategory::ENV_OBJ:
		return static_cast<CGui_Thumbnail *>(m_pElements[static_cast<_uint>(MapEditorGuiType::ENV_THUMBNAIL)])->GetSelectedCompName(m_iObjectType);
	case Engine::ObjectCategory::MONSTER:
		return static_cast<CGui_Thumbnail *>(m_pElements[static_cast<_uint>(MapEditorGuiType::MONSTER_THUMBNAIL)])->GetSelectedCompName(m_iObjectType);
	}
}