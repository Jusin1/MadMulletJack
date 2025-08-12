#pragma once
#include "CGui_Panel.h"

class CGui_Console : public CGui_Panel
{
private:
	explicit CGui_Console();
	virtual ~CGui_Console();
	
	virtual void Free() override;
public:
	static CGui_Console *Create();
	virtual void Render() override;

private:
	virtual HRESULT Ready_Panel() override;
};

