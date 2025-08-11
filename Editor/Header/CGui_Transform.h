#pragma once
#include "CGuiBase.h"

class CGui_Button;
class CGui_InputFloat;

enum class TransformDataType
{
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
	virtual _bool Render(_int _iState = -1) override;

private:
	void PositionInit();
	void RotationInit();
private:
	float m_fPadding;
	TransformDataType m_eType;
	vector<CGui_InputFloat *> m_vecInfos;
};

