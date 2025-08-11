#pragma once
#include "Engine_Define.h"
#include "Editor_Define.h"
#include "CBase.h"

class CGuiBase;

class CGui_Panel : public CBase
{
protected:
	explicit CGui_Panel(const string &_title);
	virtual ~CGui_Panel();

	virtual void Free() override;
	virtual HRESULT Ready_Panel();
public:
	void AddElement(_uint _iType, CGuiBase *pElement);
	const string &GetTitle() const { return m_title; }
	CGuiBase *GetElement(_uint _iType);
	virtual void Render() = 0;
protected:
	std::string m_title;
	std::vector<CGuiBase *> m_pElements;
};