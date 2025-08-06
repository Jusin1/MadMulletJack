#pragma once
#include "Editor_Define.h"
#include "CBase.h"

class CGuiBase;

class CGui_Panel : public CBase
{
private:
	explicit CGui_Panel(const string &_title);
	virtual ~CGui_Panel();

	virtual void Free() override;
public:
	void AddElement();

	void Render();
};

