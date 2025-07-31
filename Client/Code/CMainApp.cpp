#include "pch.h"
#include "CMainApp.h"
#include "CManagement.h"
#include "CLogo.h"
#include "CTimerMgr.h"
#include "CFrameMgr.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CFontMgr.h"
#include "CDInputMgr.h"

CMainApp::CMainApp() : m_pGraphicDev(nullptr)
, m_pManagementClass(CManagement::GetInstance())
{
}

CMainApp::~CMainApp()
{
	
}

HRESULT CMainApp::Ready_MainApp()
{
	if (FAILED(Ready_DefaultSetting(&m_pGraphicDev)))
		return E_FAIL;

	if (FAILED(Ready_Scene(m_pGraphicDev)))
		return E_FAIL;

	return S_OK;
}

int CMainApp::Update_MainApp(const float& fTimeDelta)
{
	CDInputMgr::GetInstance()->Update_InputDev();

	m_pManagementClass->Update_Scene(fTimeDelta);

	return 0;
}

void CMainApp::LateUpdate_MainApp(const float& fTimeDelta)
{
	// _ulong	dwFlag(0);
	// 
	// if (dwFlag = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Z))
	// {
	// 	int a = 0;
	// }



	m_pManagementClass->LateUpdate_Scene(fTimeDelta);
}

void CMainApp::Render_MainApp()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pDeviceClass->Render_Begin(D3DXCOLOR(0.f, 0.f, 1.f, 1.f));

	m_pManagementClass->Render_Scene(m_pGraphicDev);

	m_pDeviceClass->Render_End();
}

HRESULT CMainApp::Ready_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev)
{
	if (FAILED(CGraphicDev::GetInstance()->Ready_GraphicDev(g_hWnd,
		MODE_WIN, WINCX, WINCY, &m_pDeviceClass)))
	{
		MSG_BOX("CGraphicDev Ready Failed");
		return E_FAIL;
	}

	m_pDeviceClass->Add_Ref();

	(*ppGraphicDev) = CGraphicDev::GetInstance()->Get_GraphicDev();
	(*ppGraphicDev)->AddRef();

	// 폰트 추가
	if(FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_Default", L"바탕", 20, 15, FW_HEAVY)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_Jinji", L"궁서", 20, 10, FW_THIN)))
		return E_FAIL;

	// DInput

	if (FAILED(CDInputMgr::GetInstance()->Ready_InputDev(g_hInst, g_hWnd)))
		return E_FAIL;


	// 텍스처 필터

	(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	(*ppGraphicDev)->SetRenderState(D3DRS_ZENABLE, TRUE);	// Z버퍼에 값을 기록, Z값 기준 자동 정렬 수행 여부를 물음
	(*ppGraphicDev)->SetRenderState(D3DRS_ZWRITEENABLE, TRUE); // Z버퍼에 Z값을 기록할 지 결정

	return S_OK;
}

HRESULT CMainApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	Engine::CScene* pScene = CLogo::Create(pGraphicDev);
	if (nullptr == pScene)
		return E_FAIL;

	if (FAILED(m_pManagementClass->Set_Scene(pScene)))
	{
		MSG_BOX("Scene Setting Failed");
		return E_FAIL;
	}


	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pMainApp = new CMainApp;

	if (FAILED(pMainApp->Ready_MainApp()))
	{
		delete pMainApp;
		pMainApp = nullptr;
	}

	return pMainApp;
}

void CMainApp::Free()
{
	Engine::Safe_Release(m_pDeviceClass);
	Engine::Safe_Release(m_pGraphicDev);

	CFontMgr::GetInstance()->DestroyInstance();
	CRenderer::GetInstance()->DestroyInstance();
	CProtoMgr::GetInstance()->DestroyInstance();
	CTimerMgr::GetInstance()->DestroyInstance();
	CFrameMgr::GetInstance()->DestroyInstance();
	CDInputMgr::GetInstance()->DestroyInstance();

	m_pManagementClass->DestroyInstance();
	m_pDeviceClass->DestroyInstance();
}
