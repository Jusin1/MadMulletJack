#pragma once
#include "CGuiBase.h"

class CGui_InputText : public CGuiBase
{
protected:
	explicit CGui_InputText(const string &_label);
	virtual ~CGui_InputText();

	virtual void Free() override;
public:
	static CGui_InputText *Create(const string &_label);
	virtual _bool Render(_int _iState = -1) override;

private:
	std::vector<char> m_value; 
};

