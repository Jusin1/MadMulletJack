#pragma once
#include "Engine_Define.h"
#include "CGuiBase.h"

class CGui_Log : public CGuiBase
{
private:
	explicit CGui_Log(const string & _label = "Log");
	virtual ~CGui_Log();

	virtual void Free() override;
public:
	static CGui_Log *Create();
	virtual _bool Render(_int _iState = -1) override;
	void Add_Log(const char *fmt, ...);
	void Add_LogV(const char *fmt, va_list args);
private:
	_bool m_bScrollToBottom;
	std::vector<std::string> m_vecLogs;
};

