#include "pch.h"
#include "CGui_Dropbox.h"
#include "CGuiManager.h"
#include "CGameObject.h"
#include "CGui_ButtonList.h"
#include "CGui_Transform.h"
#include "CGui_Checkbox.h"
#include "CGui_Thumbnail.h"
#include "CVIBuffer_GridPanel_Editor.h"
#include "CObjectManager.h"
#include "CGridPanel.h"
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
	m_pElements[static_cast<_uint>(MapEditorGuiType::CREATEMODE_CHECKBOX)] = CreateModeCheckBox_Create();
	m_pElements[static_cast<_uint>(MapEditorGuiType::SNAPMODE_CHECKBOX)] = SnapModeCheckBox_Create();
	m_pElements[static_cast<_uint>(MapEditorGuiType::CREATE_BUTTONS)] = CreateButton_Create();
	m_pElements[static_cast<_uint>(MapEditorGuiType::PANEL_SIZE_BUTTONS)] = GridPanelSizeButtons_Create();
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
	if (m_pElements[static_cast<_uint>(MapEditorGuiType::WALL_TYPE_DROPBOX)]->Render())
	{
		auto DropboxElement = dynamic_cast<CGui_Dropbox<WallType> *>
			(m_pElements[static_cast<_uint>(MapEditorGuiType::WALL_TYPE_DROPBOX)]);

		if (DropboxElement)
		{
			AllThumbnailTexture_SetClear();
			CGuiManager::GetInstance()->SetTarget(nullptr);

			DropboxElement->Confirm();
			CGuiManager::GetInstance()->SetObjectType(static_cast<_uint>(DropboxElement->GetConfirmedState()));
			m_iObjectType = CGuiManager::GetInstance()->GetInstance()->GetObjectType();
		}
	}
	m_pElements[static_cast<_uint>(MapEditorGuiType::CREATE_BUTTONS)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::PANEL_SIZE_BUTTONS)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::POSITION)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::WALL_THUMBNAIL)]->Render(m_iObjectType);
}

void CGui_MapEditorPanel::TileRender()
{
	if (m_pElements[static_cast<_uint>(MapEditorGuiType::TILE_TYPE_DROPBOX)]->Render())
	{
		auto DropboxElement = dynamic_cast<CGui_Dropbox<TileType> *>
			(m_pElements[static_cast<_uint>(MapEditorGuiType::TILE_TYPE_DROPBOX)]);

		if (DropboxElement)
		{
			AllCheckBox_SetFalse();
			AllCheckBox_SetFalse();
			AllThumbnailTexture_SetClear();
			CGuiManager::GetInstance()->SetTarget(nullptr);

			DropboxElement->Confirm();
			CGuiManager::GetInstance()->SetObjectType(static_cast<_uint>(DropboxElement->GetConfirmedState()));
			m_iObjectType = CGuiManager::GetInstance()->GetInstance()->GetObjectType();
		}
	}
	m_pElements[static_cast<_uint>(MapEditorGuiType::CREATEMODE_CHECKBOX)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::SNAPMODE_CHECKBOX)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::CREATE_BUTTONS)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::POSITION)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::TILE_THUMBNAIL)]->Render(m_iObjectType);
}

void CGui_MapEditorPanel::EnvObjRender()
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
	m_pElements[static_cast<_uint>(MapEditorGuiType::CREATEMODE_CHECKBOX)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::SNAPMODE_CHECKBOX)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::ENV_THUMBNAIL)]->Render(m_iObjectType);
}

void CGui_MapEditorPanel::MonsterRender()
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
	m_pElements[static_cast<_uint>(MapEditorGuiType::CREATEMODE_CHECKBOX)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::SNAPMODE_CHECKBOX)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::MONSTER_THUMBNAIL)]->Render(m_iObjectType);
}

void CGui_MapEditorPanel::LightRender()
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

	return CGui_Dropbox<TileType>::Create("TileType", TileType::DECO, Names);
}

CGuiBase *CGui_MapEditorPanel::EnvObjtypeDropbox_Create()
{
	vector<string> Names{ g_EnvTypeCount };
	Names[0] = "Display";
	Names[1] = "Bottle";
	Names[2] = "VendingMachine";
	Names[3] = "SpawnPoint";
	Names[4] = "EndPoint";

	return CGui_Dropbox<EnvType>::Create("EnvType", EnvType::DISPLAY, Names);
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
			CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_DefaultPanel", SCENE_DEV, L"Wall_Layer", &defaultData);
			CGuiManager::GetInstance()->SetTarget(CObjectManager::GetInstance()->Get_ObjectList(SCENE_DEV, L"Wall_Layer")->back());
		} break;
		case ObjectCategory::TILE:
		{
			MAPOBJECTDATA defaultData;
			defaultData.eCategory = ObjectCategory::TILE;
			defaultData.iType = m_iObjectType;
			defaultData.texture.OriginComponentName = GetSelectedThumbnailTexture();
			CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_DefaultTile", SCENE_DEV, L"Tile_Layer", &defaultData);
		} break;
		case ObjectCategory::ENV_OBJ:
		{
			MSG_BOX("CGui_MapEditorPanel::CreateButton_Create(), write function");
		} break;
		case ObjectCategory::MONSTER:
		{
			CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_DefaultPlacementObject", SCENE_DEV, L"Monster_Layer");
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

CGuiBase *CGui_MapEditorPanel::WallThumbnail_Create()
{
	CGui_Thumbnail *pThumbnail = CGui_Thumbnail::Create("Textures", g_WallTypeCount);

	AddThumbnail("Default", L"Proto_GridDefault", pThumbnail, WallType::WALL_HOR);
	AddThumbnail("Trigger", L"Proto_GridTrigger", pThumbnail, WallType::WALL_VER);
	AddThumbnail("NoDraw", L"Proto_GridNoDraw", pThumbnail, WallType::INCLINE);
	AddThumbnail("Collider", L"Proto_GridCollider", pThumbnail, WallType::FLOOR);
	AddThumbnail("Floor_1", L"Proto_Floor_1", pThumbnail, WallType::FLOOR);
	AddThumbnail("Floor_2", L"Proto_Floor_2", pThumbnail, WallType::CEILING);

	return pThumbnail;
}

CGuiBase *CGui_MapEditorPanel::TileThumbnail_Create()
{
	CGui_Thumbnail *pThumbnail = CGui_Thumbnail::Create("Textures", g_TileTypeCount);

	// Floor
	AddThumbnail("Floor_1", L"Proto_Floor_1", pThumbnail, TileType::DECO);
	AddThumbnail("Floor_2", L"Proto_Floor_2", pThumbnail, TileType::DECO);
	AddThumbnail("Floor_3", L"Proto_Floor_3", pThumbnail, TileType::DECO);
	AddThumbnail("Floor_4", L"Proto_Floor_4", pThumbnail, TileType::DECO);
	AddThumbnail("Floor_5", L"Proto_Floor_5", pThumbnail, TileType::DECO);
	AddThumbnail("Floor_6", L"Proto_Floor_6", pThumbnail, TileType::DECO);
	AddThumbnail("Floor_7", L"Proto_Floor_7", pThumbnail, TileType::DECO);

	// RUG
	AddThumbnail("Rug_1", L"Proto_Rug_1", pThumbnail, TileType::DECO);
	AddThumbnail("Rug_2", L"Proto_Rug_2", pThumbnail, TileType::DECO);
	AddThumbnail("Rug_3", L"Proto_Rug_3", pThumbnail, TileType::DECO);
	AddThumbnail("Rug_4", L"Proto_Rug_4", pThumbnail, TileType::DECO);

	// ACID
	AddThumbnail("Acid_Floor_1", L"Proto_Acid_Floor_1", pThumbnail, TileType::ACID);
	AddThumbnail("Acid_Floor_2", L"Proto_Acid_Floor_2", pThumbnail, TileType::DECO);
	AddThumbnail("Acid_Wall_1", L"Proto_Acid_Wall_1", pThumbnail, TileType::DECO);
	AddThumbnail("Acid_Wall_2", L"Proto_Acid_Wall_2", pThumbnail, TileType::DECO);

	// Electric
	AddThumbnail("Electirc_Wall", L"Proto_Electric_Wall", pThumbnail, TileType::ELECTRIC);

	return pThumbnail;
}

CGuiBase *CGui_MapEditorPanel::EnvThumbnail_Create()
{
	CGui_Thumbnail *pThumbnail = CGui_Thumbnail::Create("Textures", g_EnvTypeCount);

	AddThumbnail("Acid_Display", L"Proto_Acid_Env", pThumbnail, EnvType::DISPLAY);

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
