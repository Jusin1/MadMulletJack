#pragma once
#include "CGui_ButtonList.h"
#include "CGuiBase.h"

template<typename TEnum>
class CGui_Dropbox : public CGuiBase
{
private:
	explicit CGui_Dropbox(string _label, TEnum _eInit, const vector<std::string> &_Names, CGui_ButtonList *_pButtons = nullptr, std::function<void()> _func = nullptr);
	virtual ~CGui_Dropbox();

	virtual void Free() override;
public:
	static CGui_Dropbox *Create(string _label, TEnum _eInit, const vector<std::string> &_Names, CGui_ButtonList *_pButtons = nullptr, std::function<void()> _func = nullptr);
	virtual _bool Render() override;

	TEnum GetConfirmedState() const { return m_eConfirmState; }
	TEnum GetShowedState() const { return m_eShowState; }

	void Confirm() { m_eConfirmState = m_eShowState; }
	void Cancel() { m_eShowState = m_eConfirmState; }

	_bool IsEvent() { return m_OnChangedStateEvent != nullptr; }
	_bool IsButtons() { return m_pButtons != nullptr; }

	void CallEvent() { m_OnChangedStateEvent(); }
private:
	_bool Render_Dropbox();
private:
	TEnum m_eShowState;
	TEnum m_eConfirmState;
	std::function<void()> m_OnChangedStateEvent;
	vector<std::string> m_Names;
	CGui_ButtonList *m_pButtons;
};

template<typename TEnum>
inline CGui_Dropbox<TEnum>::CGui_Dropbox(string _label, TEnum _eInit, const vector<std::string> &_Names, CGui_ButtonList *_pButtons, std::function<void()> _func)
	: CGuiBase(_label), m_eConfirmState(_eInit), m_eShowState(_eInit), m_Names(_Names), m_pButtons(_pButtons), m_OnChangedStateEvent(_func)
{
}

template<typename TEnum>
inline CGui_Dropbox<TEnum>::~CGui_Dropbox()
{
}

template<typename TEnum>
inline void CGui_Dropbox<TEnum>::Free()
{
}

template<typename TEnum>
inline CGui_Dropbox<TEnum> *CGui_Dropbox<TEnum>::Create(string _label, TEnum _eInit, const vector<std::string> &_Names, CGui_ButtonList *_pButtons, std::function<void()> _func)
{
	return new CGui_Dropbox<TEnum>(_label, _eInit, _Names, _pButtons, _func);
}

template<typename TEnum>
inline _bool CGui_Dropbox<TEnum>::Render()
{
	return Render_Dropbox();
}

template<typename TEnum>
inline _bool CGui_Dropbox<TEnum>::Render_Dropbox()
{
	_int curState{ static_cast<_int>(m_eShowState) };
	_bool changed{ FALSE };
	if (ImGui::BeginCombo(m_label.c_str(), m_Names[curState]))
	{
		for (size_t i = 0; i < m_Names.size(); ++i)
		{
			_bool bSelected = (curState == i);
			if (ImGui::Selectable(m_Names[i], bSelected))
			{
				if (curState != i)
				{
					m_eShowState = static_cast<TEnum>(i);
					changed = true;
				}
			}

			if (bSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	return changed;
}
