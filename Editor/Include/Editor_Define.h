#pragma once
#include <string>
using std::wstring;

enum SCENE { SCENE_STATIC, SCENE_LOADING, SCENE_EDITOR, SCENE_END };
enum OBJID { OBJ_MAP, OBJ_FLOOR, OBJ_INCLINE, OBJ_CEILING, OBJ_END };

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

typedef struct tagTransformData
{
	float Right[3]	{ 1.f, 0.f, 0.f };
	float Up[3]		{ 0.f, 1.f, 0.f };
	float Look[3]	{ 0.f, 0.f, 1.f };
	float Pos[3]	{ 0.f, 0.f, 0.f };
} TRANSFORMDATA;

typedef struct tagTextureData
{
	wstring TexturePath = L"";
} TEXTUREDATA;

typedef struct tagMapObjectData
{
	OBJID ObjType;
	TRANSFORMDATA transform;
	TEXTUREDATA texture;
	PANELDATA panelBuffer;
} MAPOBJECTDATA;