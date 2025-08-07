#pragma once
#include "CBase.h"

#pragma push_macro("new")
#undef new
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
#pragma pop_macro("new")

#include "CGuiManager.h"

class CGuiBase : public CBase
{
protected:
	explicit CGuiBase(const string &_label);
	virtual ~CGuiBase();

	virtual void Free();
public:
	virtual void Render() = 0;

	const string &GetLabel() const { return m_label; }
protected:
	string m_label;
};

