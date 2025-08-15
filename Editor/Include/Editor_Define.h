#pragma once
#include <string>
using std::wstring;

enum SCENE { SCENE_STATIC, SCENE_LOADING, SCENE_EDITOR, SCENE_END };

enum class EditorType : unsigned int
{
	MAP = 0,
	PREFAB,
	EFFECT,
	NONE
};
constexpr size_t g_EditorTypeCount{ static_cast<size_t>(EditorType::NONE) };

enum class MapEditorGuiType : unsigned int
{
	CATEGORY_DROPBOX = 0,
	WALL_TYPE_DROPBOX,
	TILE_TYPE_DROPBOX,
	ENVOBJ_TYPE_DROPBOX,
	MONSTER_TYPE_DROPBOX,
	LIGHT_TYPE_DROPBOX,
	CREATEMODE_CHECKBOX,
	SNAPMODE_CHECKBOX,
	CREATE_BUTTONS,
	PANEL_SIZE_BUTTONS,
	POSITION,
	ROTATION,
	WALL_THUMBNAIL,
	TILE_THUMBNAIL,
	ENV_THUMBNAIL,
	MONSTER_THUMBNAIL,
	NONE
};
constexpr size_t g_MapEditorGuiTypeCount{ static_cast<size_t>(MapEditorGuiType::NONE) };

enum class MapEditorObjectCategory : unsigned int
{
	WALL = 0,
	TILE,
	ENV_OBJ,
	MONSTER,
	LIGHT,
	NONE
};
constexpr size_t g_MapEditorObjectCategoryCount{ static_cast<size_t>(MapEditorObjectCategory::NONE) };

enum class MapEditorWallType : unsigned int
{
	WALL_HOR = 0,
	WALL_VER,
	INCLINE,
	FLOOR,
	CEILING,
	SIDE_DASH,
	NONE
};
constexpr size_t g_MapEditorWallTypeCount{ static_cast<size_t>(MapEditorWallType::NONE) };

enum class MapEditorTileType : unsigned int
{
	NORMAL,
	GLASS,
	ACID,
	ELECTRIC,
	VENT,
	NONE
};
constexpr size_t g_MapEditorTileTypeCount{ static_cast<size_t>(MapEditorTileType::NONE) };

enum class MapEditorEnvObjectType : unsigned int
{
	DISPLAY,
	BOTTLE,		// 갈색
	VENDINGMACHINE,
	NONE
};
constexpr size_t g_MapEditorEnvObjectTypeCount{ static_cast<size_t>(MapEditorEnvObjectType::NONE) };

enum class MapEditorMonsterType : unsigned int
{
	SUIT,
	NONE
};
constexpr size_t g_MapEditorMonsterTypeCount{ static_cast<size_t>(MapEditorMonsterType::NONE) };

enum class MapEditorLightType : unsigned int
{
	tmp,
	NONE
};
constexpr size_t g_MapEditorLightTypeCount{ static_cast<size_t>(MapEditorLightType::NONE) };

enum class EffectEditorGuiType : unsigned int
{
	tmp,
	NONE
};
constexpr size_t g_EffectEditorGuiTypeCount{ static_cast<size_t>(EffectEditorGuiType::NONE) };

// TILE
// Horizon
// Vrtical (유리)
inline int FastRound(float x) { return static_cast<int>(x + (x >= 0 ? 0.5f : -0.5f)); }