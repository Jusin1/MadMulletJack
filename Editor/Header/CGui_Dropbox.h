#pragma once
#include "CGuiBase.h"

template<typename TEnum>
class CGui_Dropbox : public CGuiBase
{
private:
	explicit CGui_Dropbox(string _label, TEnum _eInit, const vector<std::string> &_Names);
	virtual ~CGui_Dropbox();

	virtual void Free() override;
public:
	static CGui_Dropbox *Create(string _label, TEnum _eInit, const vector<std::string> &_Names);
	virtual _bool Render(_int _iState = -1) override;

	TEnum GetConfirmedState() const { return m_eConfirmState; }
	TEnum GetShowedState() const { return m_eShowState; }

	void Confirm() { m_eConfirmState = m_eShowState; }
	void Cancel() { m_eShowState = m_eConfirmState; }
private:
	TEnum m_eShowState;
	TEnum m_eConfirmState;
	vector<std::string> m_Names;
};

template<typename TEnum>
inline CGui_Dropbox<TEnum>::CGui_Dropbox(string _label, TEnum _eInit, const vector<std::string> &_Names)
	: CGuiBase(_label), m_eConfirmState(_eInit), m_eShowState(_eInit), m_Names(_Names)
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
inline CGui_Dropbox<TEnum> *CGui_Dropbox<TEnum>::Create(string _label, TEnum _eInit, const vector<std::string> &_Names)
{
	return new CGui_Dropbox<TEnum>(_label, _eInit, _Names);
}

template<typename TEnum>
inline _bool CGui_Dropbox<TEnum>::Render(_int _iState)
{
	_int curState{ static_cast<_int>(m_eShowState) };
	_bool changed{ FALSE };
	ImGui::Text("%s", m_label.c_str());
	std::string comboId = "##" + m_label;
	if (ImGui::BeginCombo(comboId.c_str(), m_Names[curState].c_str()))
	{
		for (size_t i = 0; i < m_Names.size(); ++i)
		{
			_bool bSelected = (curState == i);
			if (ImGui::Selectable(m_Names[i].c_str(), bSelected))
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
	ImGui::Spacing();
	ImGui::Spacing();
	return changed;
}
