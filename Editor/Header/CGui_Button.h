#pragma once
#include "CGuiBase.h"
class CGui_Button : public CGuiBase
{
private:
	explicit CGui_Button(const string &_label, std::function<void()> _func = nullptr);
	virtual ~CGui_Button();

	virtual void Free() override;
public:
	static CGui_Button *Create(const string &_label, std::function<void()> _func = nullptr);
	virtual void Render() override;
private:
	std::function<void()> m_onClick;
};

