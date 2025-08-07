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
	static CGui_Panel *Create(const string &_title);
	void AddElement(CGuiBase *pElement);

	void Render();
	const string &GetTitle() const { return m_title; }
	CGuiBase *GetElement(const string &_keyName);
private:
	std::string m_title;
	std::map<string, CGuiBase *> m_pElements;
};

