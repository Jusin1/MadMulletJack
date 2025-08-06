#pragma once
#include "CBase.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "ImZoomSlider.h"
#include "ImCurveEdit.h"
#include "GraphEditor.h"

class CGuiBase : public CBase
{
private:
	explicit CGuiBase();
	explicit CGuiBase(string _title);
	virtual ~CGuiBase();

	virtual void Free();
private:
	string m_title;
};

