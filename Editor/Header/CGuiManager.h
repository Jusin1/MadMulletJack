#pragma once
#include "CBase.h"
#include "CGuiBase.h"
#include "Editor_Define.h"
#include "Engine_Define.h"

#define EDITOR_CONSOLE(fmt, ...) \
CGuiManager::GetInstance()->AddLog("[%s] : " fmt, __FUNCSIG__, ##__VA_ARGS__)

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
		PREFAB_INSPECTOR,
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

	void RotationDegree(const _vec3 &vAxis, _float fDegree);

	HRESULT AddThumbnail(const string &ThumnailName, const _tchar *CompName, CGui_Thumbnail *_pThumbnail, _uint iType);

	void AddLog(const char *fmt, ...);

	void Render();
	Engine::CGameObject *GetTarget() const { return m_pTarget; }
	void SetTarget(Engine::CGameObject *_p) { m_pTarget = _p; }	

	ObjectCategory GetCategory() { return m_eCategory; }
	void SetCategory(ObjectCategory _e) { m_eCategory = _e; }

	_uint GetObjectType() { return m_iObjectType; }
	void SetObjectType(_uint _i) { m_iObjectType = _i; }

	ObjectCategory GetLocalCategory() { return m_eLocalCategory; }
	void SetLocalCategory(ObjectCategory _e) { m_eLocalCategory = _e; }

	_uint GetLocalObjectType() { return m_iLocalObjectType; }
	void SetLocalObjectType(_uint _i) { m_iLocalObjectType = _i; }

	std::array<CGui_Panel *, (_ulong)(PANEL::NONE)> *GetPanelList() { return &m_pPanels; }
	CGui_Panel *GetConsole() { return m_pPanels[CONSOLE]; }
	CGui_Panel *GetInspector() { return m_pPanels[INSPECTOR]; }

	_bool IsCreateMode() { return m_bCreateMode; }
	void SetCreateMode(_bool _b, ObjectCategory _e);

	_bool IsSnap() { return m_bSnap; }
	void SetSnap(_bool _b) { m_bSnap = _b; }

	const _tchar *GetSelectedThumnailTexture();
private:
	_bool m_bCreateMode;
	_bool m_bSnap;
	LPDIRECT3DDEVICE9 m_pGraphicDevice;
	Engine::CGameObject *m_pTarget;
	std::array<CGui_Panel *, (_ulong)(PANEL::NONE)> m_pPanels;
	std::array<PANELINFO, (_ulong)(PANEL::NONE)> m_pPanelInfos;
	// prefab Àü¿ë
	ObjectCategory m_eLocalCategory;
	_uint m_iLocalObjectType;
	
	ObjectCategory m_eCategory;
	_uint m_iObjectType;
};