#pragma once
#include "CBase.h"
#include "CGuiBase.h"
#include "Engine_Define.h"

namespace Engine
{
	class CGameObject;
}

typedef struct tagPanelInfo
{
	ImVec2 Position;
	ImVec2 Size;
} PANELINFO;

class CGui_Thumbnail;
class CGui_Panel;

class CGuiManager : public CBase
{
	DECLARE_SINGLETON(CGuiManager)
private:
	enum PANEL
	{
		INSPECTOR,
		CONSOLE,
		NONE
	};
private:
	explicit CGuiManager();
	virtual ~CGuiManager();

	virtual void Free() override;
public:
	HRESULT Ready_CGuiManager(LPDIRECT3DDEVICE9 pGraphicDevce);
	HRESULT Initialize();

	void ShowEditorDockspace();
	void ShowInspector();
	void ShowConsole();

	HRESULT AddTexture_AddThumbnail(const string &ThumnailName, const _tchar *CompName, const wstring &Path);

	void Render();
	Engine::CGameObject *GetTarget() const { return m_pTarget; }
	void SetTarget(Engine::CGameObject *_p) { m_pTarget = _p; }
	
	std::array<CGui_Panel *, (size_t)(PANEL::NONE)> *GetPanelList() { return &m_pPanels; }
	CGui_Panel *GetConsole() { return m_pPanels[CONSOLE]; }
	CGui_Panel *GetInspector() { return m_pPanels[INSPECTOR]; }
private:
	LPDIRECT3DDEVICE9 m_pGraphicDevice;
	Engine::CGameObject *m_pTarget;
	std::array<CGui_Panel *, (size_t)(PANEL::NONE)> m_pPanels;
	std::array<PANELINFO, (size_t)(PANEL::NONE)> m_pPanelInfos;
};

