#pragma once
#include "CGuiBase.h"

class CGui_Button;

class CGui_ButtonList : public CGuiBase
{
private:
	explicit CGui_ButtonList(const string &_label, const vector<string> &_buttonsLabel, const vector<std::function<void()>> &_buttonsEvent);
	virtual ~CGui_ButtonList();

	virtual void Free() override;
public:
	static CGui_ButtonList *Create(const string &_label,const vector<string> &_buttonsLabel, const vector<std::function<void()>> &_buttonsEvent);
	virtual void Render() override;
private:
	HRESULT Ready_ButtonList(const vector<string> &_buttonsLabel,const vector<std::function<void()>> &_buttonsEvent);
private:
	vector<CGui_Button *> m_vecButtons;
};

