#include "CGui_Log.h"
#include "CGui_Console.h"

CGui_Console::CGui_Console()
	:CGui_Panel("Console")
{
}

CGui_Console::~CGui_Console()
{
}

void CGui_Console::Free()
{
	CGui_Panel::Free();
}

CGui_Console *CGui_Console::Create()
{
	CGui_Console *pNew = new CGui_Console();
	if (FAILED(pNew->Ready_Panel()))
	{
		MSG_BOX("CGui_Console::Create, Failed");
		Safe_Release(pNew);
		return nullptr;
	}

	return pNew;
}

void CGui_Console::Render()
{
	m_pElements[0]->Render();
}

HRESULT CGui_Console::Ready_Panel()
{
	CGui_Log *pLog = CGui_Log::Create();
	if (!pLog)
		return E_FAIL;
	m_pElements.push_back(pLog);
	return S_OK;
}
