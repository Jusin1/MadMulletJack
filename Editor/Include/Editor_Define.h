#pragma once

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
	SCENE_TYPE_DROPBOX,
	CREATEMODE_CHECKBOX,
	SNAPMODE_CHECKBOX,
	CREATE_BUTTONS,
	PANEL_SIZE_BUTTONS,
	SAVE_DATA_BUTTON,
	POSITION,
	ROTATION,
	WALL_THUMBNAIL,
	TILE_THUMBNAIL,
	ENV_THUMBNAIL,
	MONSTER_THUMBNAIL,
	NONE
};
constexpr size_t g_MapEditorGuiTypeCount{ static_cast<size_t>(MapEditorGuiType::NONE) };

enum class MapEditorSceneType : unsigned int
{
	DEV,
	TUTORIAL,
	STAGE_1,
	STAGE_2,
	STAGE_3,
	SNIPE,
	ROOFTOP,
	ROAD,
	NONE
};
constexpr size_t g_MapEditorSceneTypeCount{ static_cast<size_t>(MapEditorSceneType::NONE) };

// TILE
// Horizon
// Vrtical (À¯¸®)
inline int FastRound(float x) { return static_cast<int>(x + (x >= 0 ? 0.5f : -0.5f)); }