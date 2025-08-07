#pragma once
#include <string>
using std::wstring;

enum SCENE { SCENE_STATIC, SCENE_LOADING, SCENE_EDITOR, SCENE_END };

enum class GuiType
{
	TEXT,
	BUTTON,
	NONE
};