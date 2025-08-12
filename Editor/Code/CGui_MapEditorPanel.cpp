#include "pch.h"
#include "CGui_Dropbox.h"
#include "CGuiManager.h"
#include "CGameObject.h"
#include "CGui_ButtonList.h"
#include "CGui_Transform.h"
#include "CGui_Checkbox.h"
#include "CGui_Thumbnail.h"
#include "CVIBuffer_GridPanel.h"
#include "CObjectManager.h"
#include "CGridPanel.h"
#include "CGui_MapEditorPanel.h"

CGui_MapEditorPanel::CGui_MapEditorPanel()
	: CGui_Panel("Insepctor"), m_eCategory(MapEditorObjectCategory::WALL), m_iObjectType(0)
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
		auto DropboxElement = dynamic_cast<CGui_Dropbox<MapEditorObjectCategory> *>
			(m_pElements[static_cast<_uint>(MapEditorGuiType::CATEGORY_DROPBOX)]);

		if (DropboxElement)
		{
			static_cast<CGui_Checkbox *>(m_pElements[static_cast<_uint>(MapEditorGuiType::CREATEMODE_CHECKBOX)])->OnFalse();
			DropboxElement->Confirm();
			CGuiManager::GetInstance()->SetCategory(DropboxElement->GetConfirmedState());
			m_eCategory = CGuiManager::GetInstance()->GetCategory();
			m_iObjectType = 0;
		}
	}

	switch (m_eCategory)
	{
	case MapEditorObjectCategory::WALL:
	{
		WallRender();
	} break;
	case MapEditorObjectCategory::TILE:
	{
		TileRender();
	} break;
	case MapEditorObjectCategory::ENV_OBJ:
	{
		EnvObjRender();
	} break;
	case MapEditorObjectCategory::MONSTER:
	{
		MonsterRender();
	} break;
	case MapEditorObjectCategory::LIGHT:
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
		auto DropboxElement = dynamic_cast<CGui_Dropbox<MapEditorWallType> *>
			(m_pElements[static_cast<_uint>(MapEditorGuiType::WALL_TYPE_DROPBOX)]);

		if (DropboxElement)
		{
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
		auto DropboxElement = dynamic_cast<CGui_Dropbox<MapEditorTileType> *>
			(m_pElements[static_cast<_uint>(MapEditorGuiType::TILE_TYPE_DROPBOX)]);

		if (DropboxElement)
		{
			DropboxElement->Confirm();
			CGuiManager::GetInstance()->SetObjectType(static_cast<_uint>(DropboxElement->GetConfirmedState()));
			m_iObjectType = CGuiManager::GetInstance()->GetInstance()->GetObjectType();
		}
	}
	m_pElements[static_cast<_uint>(MapEditorGuiType::CREATEMODE_CHECKBOX)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::CREATE_BUTTONS)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::POSITION)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::TILE_THUMBNAIL)]->Render(m_iObjectType);
}

void CGui_MapEditorPanel::EnvObjRender()
{
	if (m_pElements[static_cast<_uint>(MapEditorGuiType::ENVOBJ_TYPE_DROPBOX)]->Render())
	{
		auto DropboxElement = dynamic_cast<CGui_Dropbox<MapEditorEnvObjectType> *>
			(m_pElements[static_cast<_uint>(MapEditorGuiType::ENVOBJ_TYPE_DROPBOX)]);

		if (DropboxElement)
		{
			DropboxElement->Confirm();
			CGuiManager::GetInstance()->SetObjectType(static_cast<_uint>(DropboxElement->GetConfirmedState()));
			m_iObjectType = CGuiManager::GetInstance()->GetInstance()->GetObjectType();
		}
	}
	m_pElements[static_cast<_uint>(MapEditorGuiType::CREATEMODE_CHECKBOX)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::ENV_THUMBNAIL)]->Render(m_iObjectType);
}

void CGui_MapEditorPanel::MonsterRender()
{
	if (m_pElements[static_cast<_uint>(MapEditorGuiType::MONSTER_TYPE_DROPBOX)]->Render())
	{
		auto DropboxElement = dynamic_cast<CGui_Dropbox<MapEditorMonsterType> *>
			(m_pElements[static_cast<_uint>(MapEditorGuiType::MONSTER_TYPE_DROPBOX)]);

		if (DropboxElement)
		{
			DropboxElement->Confirm();
			CGuiManager::GetInstance()->SetObjectType(static_cast<_uint>(DropboxElement->GetConfirmedState()));
			m_iObjectType = CGuiManager::GetInstance()->GetInstance()->GetObjectType();
		}
	}
	m_pElements[static_cast<_uint>(MapEditorGuiType::CREATEMODE_CHECKBOX)]->Render(m_iObjectType);
	m_pElements[static_cast<_uint>(MapEditorGuiType::MONSTER_THUMBNAIL)]->Render(m_iObjectType);
}

void CGui_MapEditorPanel::LightRender()
{
	if (m_pElements[static_cast<_uint>(MapEditorGuiType::LIGHT_TYPE_DROPBOX)]->Render())
	{
		auto DropboxElement = dynamic_cast<CGui_Dropbox<MapEditorLightType> *>
			(m_pElements[static_cast<_uint>(MapEditorGuiType::LIGHT_TYPE_DROPBOX)]);

		if (DropboxElement)
		{
			DropboxElement->Confirm();
			CGuiManager::GetInstance()->SetObjectType(static_cast<_uint>(DropboxElement->GetConfirmedState()));
			m_iObjectType = CGuiManager::GetInstance()->GetInstance()->GetObjectType();
		}
	}
}

CGuiBase *CGui_MapEditorPanel::CategoryDropbox_Create()
{
	vector<string> Names{g_MapEditorObjectCategoryCount};
	Names[0] = "Wall";
	Names[1] = "Tile";
	Names[2] = "Env";
	Names[3] = "Monster";
	Names[4] = "Light";

	return CGui_Dropbox<MapEditorObjectCategory>::Create("Category", MapEditorObjectCategory::WALL, Names);
}

CGuiBase *CGui_MapEditorPanel::WalltypeDropbox_Create()
{
	vector<string> Names{ g_MapEditorWallTypeCount };
	Names[0] = "Wall_Hor";
	Names[1] = "Wall_Ver";
	Names[2] = "Incline";
	Names[3] = "Floor";
	Names[4] = "Ceiling";

	return CGui_Dropbox<MapEditorWallType>::Create("WallType", MapEditorWallType::WALL_HOR, Names);
}

CGuiBase *CGui_MapEditorPanel::TiletypeDropbox_Create()
{
	vector<string> Names{ g_MapEditorTileTypeCount };
	Names[0] = "NONE";

	return CGui_Dropbox<MapEditorTileType>::Create("TileType", MapEditorTileType::tmp, Names);
}

CGuiBase *CGui_MapEditorPanel::EnvObjtypeDropbox_Create()
{
	vector<string> Names{ g_MapEditorEnvObjectTypeCount };
	Names[0] = "NONE";

	return CGui_Dropbox<MapEditorEnvObjectType>::Create("EnvType", MapEditorEnvObjectType::tmp, Names);
}

CGuiBase *CGui_MapEditorPanel::MonstertypeDropbox_Create()
{
	vector<string> Names{ g_MapEditorMonsterTypeCount };
	Names[0] = "NONE";

	return CGui_Dropbox<MapEditorMonsterType>::Create("MonsterType", MapEditorMonsterType::tmp, Names);
}

CGuiBase *CGui_MapEditorPanel::LighttypeDropbox_Create()
{
	vector<string> Names{ g_MapEditorLightTypeCount };
	Names[0] = "NONE";

	return CGui_Dropbox<MapEditorLightType>::Create("LightType", MapEditorLightType::tmp, Names);
}

CGuiBase *CGui_MapEditorPanel::CreateButton_Create()
{
	vector<string> _labels{ 2 };
	vector<std::function<void()>> _funcs{ 2 };

	_labels[0] = "Create";
	_funcs[0] =
		[]()->void {
		MapEditorObjectCategory eCategory = CGuiManager::GetInstance()->GetCategory();
		switch (eCategory)
		{
		case MapEditorObjectCategory::WALL:
		{
			// TODO : type에 따른 생성
			CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_DefaultPanel", SCENE_EDITOR, L"Wall_Layer");
		} break;
		case MapEditorObjectCategory::TILE:
		{
			CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_DefaultTile", SCENE_EDITOR, L"Tile Layer");
		} break;
		case MapEditorObjectCategory::ENV_OBJ:
		{
			MSG_BOX("CGui_MapEditorPanel::CreateButton_Create(), write function");
		} break;
		case MapEditorObjectCategory::MONSTER:
		{
			MSG_BOX("CGui_MapEditorPanel::CreateButton_Create(), write function");
		} break;
		case MapEditorObjectCategory::LIGHT:
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
	vector<string> _labels{ 4 };
	vector<std::function<void()>> _funcs{ 4 };

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
	CGui_Thumbnail *pThumbnail = CGui_Thumbnail::Create("Textures", g_MapEditorWallTypeCount);

	AddThumbnail("Default", L"Proto_GridDefault", pThumbnail, MapEditorWallType::WALL_HOR);
	AddThumbnail("Trigger", L"Proto_GridTrigger", pThumbnail, MapEditorWallType::WALL_VER);
	AddThumbnail("NoDraw", L"Proto_GridNoDraw", pThumbnail, MapEditorWallType::INCLINE);
	AddThumbnail("Collider", L"Proto_GridCollider", pThumbnail, MapEditorWallType::FLOOR);
	AddThumbnail("Floor_1", L"Proto_Floor_1", pThumbnail, MapEditorWallType::FLOOR);
	AddThumbnail("Floor_2", L"Proto_Floor_2", pThumbnail, MapEditorWallType::CEILING);

	return pThumbnail;
}

CGuiBase *CGui_MapEditorPanel::TileThumbnail_Create()
{
	CGui_Thumbnail *pThumbnail = CGui_Thumbnail::Create("Textures", g_MapEditorTileTypeCount);

	AddThumbnail("Floor_3", L"Proto_Floor_3", pThumbnail, MapEditorTileType::tmp);
	AddThumbnail("Floor_4", L"Proto_Floor_4", pThumbnail, MapEditorTileType::tmp);
	AddThumbnail("Floor_5", L"Proto_Floor_5", pThumbnail, MapEditorTileType::tmp);
	AddThumbnail("Floor_6", L"Proto_Floor_6", pThumbnail, MapEditorTileType::tmp);
	AddThumbnail("Floor_7", L"Proto_Floor_7", pThumbnail, MapEditorTileType::tmp);

	return pThumbnail;
}

CGuiBase *CGui_MapEditorPanel::EnvThumbnail_Create()
{
	CGui_Thumbnail *pThumbnail = CGui_Thumbnail::Create("Textures", g_MapEditorEnvObjectTypeCount);

	return pThumbnail;
}

CGuiBase *CGui_MapEditorPanel::MonsterThumbnail_Create()
{
	CGui_Thumbnail *pThumbnail = CGui_Thumbnail::Create("Textures", g_MapEditorMonsterTypeCount);

	return pThumbnail;
}

CGuiBase *CGui_MapEditorPanel::CreateModeCheckBox_Create()
{
	CGui_Checkbox *pCheckbox = CGui_Checkbox::Create("CreateMode",
		// TrueEvent
		[this]()->void {
			CGuiManager::GetInstance()->SetCreateMode(TRUE, m_eCategory);
		},
		// FalseEvent
		[this]()->void {
			CGuiManager::GetInstance()->SetCreateMode(FALSE, m_eCategory);
		});
	return pCheckbox;
}
