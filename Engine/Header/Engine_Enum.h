#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum WINMODE { MODE_FULL, MODE_WIN };

	// Dynamic 컴포넌트 경우 매 프레임마다 갱신해야하는 컴포넌트 집단
	enum COMPONENTID { ID_DYNAMIC, ID_STATIC, ID_END };

	enum INFO {	INFO_RIGHT, INFO_UP, INFO_LOOK, INFO_POS, INFO_END };
	enum ROTATION { ROT_X, ROT_Y, ROT_Z, ROT_END };

	enum TEXTUREID { TEX_NORMAL, TEX_CUBE, TEX_END };

	enum RENDERID { RENDER_PRIORITY, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI, RENDER_UI_FRONT, RENDER_END };

	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE {	DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

	enum OBJID { OBJ_PLAYER, OBJ_MONSTER, OBJ_MAP, OBJ_FLOOR, OBJ_INCLINE, OBJ_CEILING, OBJ_END };

	enum SCENE { SCENE_STATIC, SCENE_LOADING, SCENE_LOGO, SCENE_DEV, SCENE_TUTORIAL, SCENE_STAGE_1, SCENE_STAGE_2, SCENE_SNIPE, SCENE_BOSS, SCENE_CAR, SCENE_PREFAB, SCENE_END };

	enum class ObjectCategory : unsigned int
	{
		WALL = 0,
		TILE,
		ENV_OBJ,
		MONSTER,
		LIGHT,
		PREFAB,
		NONE
	};
	constexpr size_t g_ObjectCategoryCount{ static_cast<size_t>(ObjectCategory::NONE) };

	enum class WallType : unsigned int
	{
		WALL_HOR = 0,
		WALL_VER,
		INCLINE,
		FLOOR,
		CEILING,
		WALL_SLIDE,
		NONE
	};
	constexpr size_t g_WallTypeCount{ static_cast<size_t>(WallType::NONE) };

	enum class TileType : unsigned int
	{
		DECO,
		GLASS,
		ACID,
		ELECTRIC,
		VENT,
		NORMALDOOR,
		OPENINGDOOR,
		DISPLAY,
		BOTTLE,
		VENDINGMACHINE,
		NONE
	};
	constexpr size_t g_TileTypeCount{ static_cast<size_t>(TileType::NONE) };

	enum class EnvType : unsigned int
	{
		SPAWNPOINT,
		ENDPOINT,
		NONE
	};
	constexpr size_t g_EnvTypeCount{ static_cast<size_t>(EnvType::NONE) };

	enum class MonsterType : unsigned int
	{
		SUIT,
		NONE
	};
	constexpr size_t g_MonsterTypeCount{ static_cast<size_t>(MonsterType::NONE) };

	enum class LightType : unsigned int
	{
		tmp,
		NONE
	};
	constexpr size_t g_LightTypeCount{ static_cast<size_t>(LightType::NONE) };

	enum class EffectEditorGuiType : unsigned int
	{
		tmp,
		NONE
	};
	constexpr size_t g_EffectEditorGuiTypeCount{ static_cast<size_t>(EffectEditorGuiType::NONE) };

	enum class PrefabType : unsigned int
	{
		SIGN_PILLAR,
		ROAD,
		SING_PILLAR_2,
		TILESET_1,
		NONE
	};
	constexpr size_t g_PrefabTypeCount{ static_cast<size_t>(PrefabType::NONE) };
}
#endif // Engine_Enum_h__