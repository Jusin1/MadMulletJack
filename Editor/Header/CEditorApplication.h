#pragma once
#include "CBase.h"

namespace Engine
{
	class CManagement;
	class CGraphicDev;
	class CRenderer;
}

class CEditorApplication : public CBase
{
private:
	explicit CEditorApplication();
	virtual ~CEditorApplication();

	virtual void Free() override;
public:
	static CEditorApplication *Create();

	HRESULT Ready_EditorApplication();
	void Run_EditorApplication();
	_int Update_EditorApplication();
	void LateUpdate_EditorApplication();
	void Render_EditorApplication();

	LPDIRECT3DDEVICE9 GetGraphicDevice() { return m_pGraphicDevice; }
private:
	HRESULT DefaultSetting(LPDIRECT3DDEVICE9 *ppGraphicDevice);
	HRESULT Ready_Prototype_Component();
	HRESULT Ready_Scene();
	void RenderImGuiRender();
private:
	Engine::CGraphicDev *m_pDeviceClass;
	Engine::CManagement *m_pManagementClass;
	Engine::CRenderer *m_pRenderer;
	LPDIRECT3DDEVICE9	m_pGraphicDevice;

	bool m_bShowDemoWindow;
};

