#pragma once

enum SCENE { SCENE_STATIC, SCENE_LOADING, SCENE_EDITOR, SCENE_END };
enum OBJID { OBJ_WALL, OBJ_FLOOR, OBJ_INCLINE, OBJ_CEILING, OBJ_END };

enum class GuiType
{
	TEXT,
	BUTTON,
	NONE
};

enum class PanelType : unsigned int
{
	WALL_HOR = 0,
	WALL_VER,
	INCLINE,
	FLOOR,
	CEILING,
	NONE
};

typedef struct tagGridPanelData
{
	PanelType eType = PanelType::NONE;
	unsigned short dwIncline = 0;
	unsigned long dwCountX = 0;
	unsigned long dwCountY = 0;
	unsigned long dwCountZ = 0;
	unsigned long dwInterval = 1;
} PANELDATA;