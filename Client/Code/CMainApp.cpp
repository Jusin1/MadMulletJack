#include "pch.h"
#include "CMainApp.h"
#include "CManagement.h"
#include "CLoading_Scene.h"
#include "CLogo.h"
#include "CTimerMgr.h"
#include "CFrameMgr.h"
#include "CRenderer.h"
#include "CFontMgr.h"
#include "CDInputMgr.h"
#include "CColiderManager.h"
#include "CObjectManager.h"
#include "CComponentMgr.h"

CMainApp::CMainApp() : m_pGraphicDev(nullptr)
, m_pRenderer(nullptr)
{
}

CMainApp::~CMainApp()
{
	
}

HRESULT CMainApp::Ready_MainApp()
{
	if (FAILED(Ready_DefaultSetting(&m_pGraphicDev)))
		return E_FAIL;

	if (FAILED(CObjectManager::GetInstance()->Readay_ObjectManager(SCENE_END)))
		return E_FAIL;

	if (FAILED(CComponentMgr::GetInstance()->Ready_Prototype(SCENE_END)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;


	if (FAILED(Ready_Scene(SCENE_LOGO)))
		return E_FAIL;

	return S_OK;
}

int CMainApp::Update_MainApp(const float& fTimeDelta)
{
	CDInputMgr::GetInstance()->Update_InputDev();

	CManagement::GetInstance()->Update_Scene(fTimeDelta);
	CObjectManager::GetInstance()->Update(fTimeDelta);

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
	CManagement::GetInstance()->LateUpdate_Scene(fTimeDelta);
	CObjectManager::GetInstance()->Late_Update(fTimeDelta);

	CColiderManager::GetInstance()->Clear_Colider_Group();
}

void CMainApp::Render_MainApp()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	
	m_pDeviceClass->Render_Begin(D3DXCOLOR(0.f, 0.f, 1.f, 1.f));
	m_pRenderer->Render_GameObject();
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

HRESULT CMainApp::Ready_Scene(SCENE eScene)
{
	CLoading_Scene* pLoadingScene = CLoading_Scene::Create(m_pGraphicDev, eScene);
	if (nullptr == pLoadingScene)
		return E_FAIL;

	CManagement::GetInstance()->Open_Scene(SCENE_LOADING, pLoadingScene);
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component() // 모든 컴포넌트 최초 등록
{

	// Transform
	if(FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Transform", Engine::CTransform::Create(m_pGraphicDev))))
		return E_FAIL;

	// RectCol
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Rect_Buffer", Engine::CVIBuffer_Rect::Create(m_pGraphicDev))))
		return E_FAIL;

	// Colider
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Colider_Rect", Engine::CColider_Rect::Create(m_pGraphicDev))))
		return E_FAIL;

	// Renderer
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Renderer", m_pRenderer = Engine::CRenderer::Create(m_pGraphicDev))))
		return E_FAIL;

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
	CComponentMgr::GetInstance()->DestroyInstance();
	Engine::Safe_Release(m_pRenderer);
	Engine::Safe_Release(m_pDeviceClass);
	Engine::Safe_Release(m_pGraphicDev);
	CColiderManager::GetInstance()->DestroyInstance();
	CManagement::GetInstance()->DestroyInstance();
	CObjectManager::GetInstance()->DestroyInstance();
	CFontMgr::GetInstance()->DestroyInstance();
	CTimerMgr::GetInstance()->DestroyInstance();
	CFrameMgr::GetInstance()->DestroyInstance();
	CDInputMgr::GetInstance()->DestroyInstance();
	m_pDeviceClass->DestroyInstance();
}
