#pragma once

#include "CBase.h"
#include "Clinet_Define.h"
#include "CGraphicDev.h"
#include "CManagement.h"
#include "CRenderer.h"

namespace Engine
{
	class CManagement;
}

class CMainApp : public CBase
{
private:
	explicit CMainApp();
	virtual ~CMainApp();

public:
	HRESULT			Ready_MainApp();
	int				Update_MainApp(const float& fTimeDelta);
	void			LateUpdate_MainApp(const float& fTimeDelta);
	void			Render_MainApp();

private:
	HRESULT			Ready_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev);
	HRESULT			Ready_Scene(SCENE eScene);
	HRESULT			Ready_Prototype_Component();
	void			Ready_MapFactorFunc();
private:
	Engine::CGraphicDev* m_pDeviceClass;
	Engine::CRenderer* m_pRenderer;
private:
	float m_fFPSTime = 0.f;
	int   m_iFPSCnt = 0;

	LPDIRECT3DDEVICE9		m_pGraphicDev;

public:
	static CMainApp* Create();		// 按眉 积己 窃荐

private:
	virtual void	Free();			// 按眉 家戈 窃荐
};

