#pragma once
#include "CGuiBase.h"
class CGui_InputFloat : public CGuiBase
{
private:
	explicit CGui_InputFloat(string _label, std::function<_float(CGameObject *)> onEvent = nullptr, std::function<void(_float)> endEvnet = nullptr);
	virtual ~CGui_InputFloat();

	virtual void Free() override;
public:
	static CGui_InputFloat *Create(string _label, std::function<_float(CGameObject *)> onEvent = nullptr, std::function<void(_float)> endEvnet = nullptr);
	virtual void Render() override;
	void Reset();
public:
	void SetOnEvent(std::function<_float(CGameObject *)> _func) { m_OnEvent = _func; }
	void SetEndEvent(std::function<void(_float)> _func) { m_EndEvent = _func; }
	void SetWidth(_float _f) { m_fWidth = _f; }

	CGameObject *GetTarget() { return m_pTarget; }
private:
	CGameObject *m_pTarget;
	_float m_fValue;
	_float m_fWidth;
	std::function<_float(CGameObject*)> m_OnEvent;
	std::function<void(_float)> m_EndEvent;
};