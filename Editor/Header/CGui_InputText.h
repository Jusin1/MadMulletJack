#pragma once
#include "CGuiBase.h"

class CGui_InputText : public CGuiBase
{
protected:
	explicit CGui_InputText(const string &_label);
	virtual ~CGui_InputText();

	virtual void Free() override;
public:
	virtual void Render() override;

	std::string GetValue() const { return std::string(cBuffer); }
private:
	char cBuffer[256];
public:
};

