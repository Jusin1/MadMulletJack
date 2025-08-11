#include "pch.h"
#include "CGui_Log.h"

CGui_Log::CGui_Log(const string &_label)
	: CGuiBase(_label), m_bScrollToBottom(FALSE)
{
	m_vecLogs.reserve(100);
}

CGui_Log::~CGui_Log()
{
}

void CGui_Log::Free()
{
}

CGui_Log *CGui_Log::Create()
{
	return new CGui_Log();
}

_bool CGui_Log::Render(_int _iState)
{
	for (const auto &Log : m_vecLogs)
	{
		ImGui::TextUnformatted(Log.c_str());
	}

	if (m_bScrollToBottom)
	{
		ImGui::SetScrollHereY(1.0f);
		m_bScrollToBottom = false;
	}

	return FALSE;
}

void CGui_Log::Add_LogV(const char *fmt, va_list args)
{
	char buf[2048];
	vsnprintf(buf, sizeof(buf), fmt, args);
	m_vecLogs.push_back(buf);
	m_bScrollToBottom = true;
}

void CGui_Log::Add_Log(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	Add_LogV(fmt, args);
	va_end(args);
}
