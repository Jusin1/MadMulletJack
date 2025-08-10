#pragma once
#include "CGuiBase.h"
class CGui_Dropbox : public CGuiBase
{
private:
	explicit CGui_Dropbox(string _label);
	virtual ~CGui_Dropbox();

	virtual void Free() override;
public:
	static CGui_Dropbox *Create(string _label);
};

