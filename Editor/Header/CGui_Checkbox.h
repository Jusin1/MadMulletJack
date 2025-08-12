#pragma once
#include "CGuiBase.h"
class CGui_Checkbox : public CGuiBase
{
private:
	explicit CGui_Checkbox(const string &_label, std::function<void()> _trueFunc, std::function<void()> _falseFunc);
	virtual ~CGui_Checkbox();

	virtual void Free() override;
public:
	static CGui_Checkbox *Create(const string &_label, std::function<void()> _trueFunc, std::function<void()> _falseFunc);
	virtual _bool Render(_int _iState = -1) override;

	void OnTrue() { if (m_trueEvent) m_trueEvent(); }
	void OnFalse() { if (m_falseEvent) m_falseEvent(); }
private:
	_bool m_bValue;
	std::function<void()> m_trueEvent;
	std::function<void()> m_falseEvent;
};

