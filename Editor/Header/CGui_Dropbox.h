#pragma once
#include "CGuiBase.h"

template<typename TEnum>
class CGui_Dropbox : public CGuiBase
{
private:
	explicit CGui_Dropbox(string _label, TEnum _eState, const vector<std::string> &_Names);
	virtual ~CGui_Dropbox();

	virtual void Free() override;
public:
	static CGui_Dropbox *Create(string _label, TEnum _eState, const vector<std::string> &_Names);
	virtual _bool Render() override;

	TEnum GetState() const { return m_eState; }
private:
	TEnum m_ePrevState;
	TEnum m_eState;
	vector<std::string> m_Names;
};

template<typename TEnum>
inline CGui_Dropbox<TEnum>::CGui_Dropbox(string _label, TEnum _eState, const vector<std::string> &_Names)
	: CGuiBase(_label), m_eState(_eState), m_Names(_Names)
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
inline CGui_Dropbox<TEnum> *CGui_Dropbox<TEnum>::Create(string _label, TEnum _eState, const vector<std::string> &_Names)
{
	return new CGui_Dropbox<TEnum>(_label, _eState, _Names);
}

template<typename TEnum>
inline _bool CGui_Dropbox<TEnum>::Render()
{
	_int curState{ static_cast<_int>(m_eState) };
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
					m_eState = static_cast<TEnum>(i);
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
