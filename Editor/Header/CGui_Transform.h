#pragma once
#include "CGuiBase.h"

class CGui_Button;
class CGui_InputText;

enum class TransformDataType
{
	SCALE,
	ROTATION,
	POSITION,
	NONE
};


class CGui_Transform : public CGuiBase
{
private:
	explicit CGui_Transform(TransformDataType _eType);
	virtual ~CGui_Transform();

	virtual void Free() override;
public:
	HRESULT Ready_GuiTransform();
	static CGui_Transform *Create(TransformDataType _eType);
	virtual void Render() override;

private:
	void ScaleRender();
	void PositionRender();
	void RotationRender();

	void ApplySetInfo();
	void ResetInfo();
	void AllReset();
private:
	float m_fPadding;
	vector<CGui_Button *> m_vecButtons;
	vector<CGui_InputText*> m_vecTextInfos;
};

