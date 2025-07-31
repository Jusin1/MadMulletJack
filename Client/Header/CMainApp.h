#pragma once

#include "CBase.h"
#include "CGraphicDev.h"

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
	HRESULT			Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	Engine::CGraphicDev* m_pDeviceClass;
	Engine::CManagement* m_pManagementClass;

	LPDIRECT3DDEVICE9		m_pGraphicDev;

public:
	static CMainApp* Create();		// 객체 생성 함수

private:
	virtual void	Free();			// 객체 소멸 함수

};

// 1. 스카이 박스를 띄워와라
// 
// 
// 2. 평면의 방정식을 이용하여 플레이어가 지형을 타도록 구현하라(키보드 이동)
// 평면의 방정식 : ax + by + cz + d = 0
// D3DXPLANE 
// D3DXPlaneFromPoints
// 
// 3. 마우스 피킹을 이용하여 플레이어가 지형을 타도록 구현하라()
// D3DXIntersecTri
