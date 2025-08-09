#include "pch.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "ImZoomSlider.h"
#include "ImCurveEdit.h"
#include "GraphEditor.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CEditorLoadingScene.h"
#include "CEditorPickingManager.h"
#include "CPicking.h"
#include "CGuiManager.h"
#include "CFileManager.h"
#include "CGui_Panel.h"
#include "CGui_Thumbnail.h"
#include "CGraphicDev.h"
#include "CObjectManager.h"
#include "CComponentMgr.h"
#include "CDInputMgr.h"
#include "Editor_Define.h"
#include "Engine_Define.h"
#include "CEditorApplication.h"

CEditorApplication::CEditorApplication()
	: m_pDeviceClass(nullptr), m_pGraphicDevice(nullptr), m_pManagementClass(nullptr)
	, m_pRenderer(nullptr), m_bShowDemoWindow(false), m_bShowAnotherWindow(false)
{
}

CEditorApplication::~CEditorApplication()
{
}

void CEditorApplication::Free()
{
	Engine::CComponentMgr::GetInstance()->DestroyInstance();
	Engine::CManagement::GetInstance()->DestroyInstance();
	Engine::CObjectManager::GetInstance()->DestroyInstance();
	Engine::CDInputMgr::GetInstance()->DestroyInstance();
	Engine::CPicking::GetInstance()->DestroyInstance();

	Engine::Safe_Release(m_pRenderer);
	Engine::Safe_Release(m_pDeviceClass);
	Engine::Safe_Release(m_pGraphicDevice);

	CGuiManager::GetInstance()->DestroyInstance();
	CFileManager::GetInstance()->DestroyInstance();
	CEditorPickingManager::GetInstance()->DestroyInstance();

	::ImGui_ImplDX9_Shutdown();
	::ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	m_pDeviceClass->DestroyInstance();
}

CEditorApplication *CEditorApplication::Create()
{
	CEditorApplication *pApplication = new CEditorApplication();
	if (FAILED(pApplication->Ready_EditorApplication()))
	{
		MSG_BOX("EditorApplication Ready Failed");
		Safe_Release(pApplication);
		return nullptr;
	}
	return pApplication;
}

HRESULT CEditorApplication::Ready_EditorApplication()
{
	if (FAILED(DefaultSetting(&m_pGraphicDevice)))
		return E_FAIL;

	if (FAILED(Engine::CPicking::GetInstance()->Initialize(g_hWnd, m_pGraphicDevice)))
		return E_FAIL;

	if (FAILED(Engine::CObjectManager::GetInstance()->Readay_ObjectManager(SCENE_END)))
		return E_FAIL;

	if (FAILED(Engine::CComponentMgr::GetInstance()->Ready_Prototype(SCENE_END)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(Ready_Scene()))
		return E_FAIL;

	if (FAILED(ImGuiInitialize()))
		return E_FAIL;

	return S_OK;
}

void CEditorApplication::Run_EditorApplication()
{
	Update_EditorApplication();
	LateUpdate_EditorApplication();

	RenderImGuiRender();

	Render_EditorApplication();
}

_int CEditorApplication::Update_EditorApplication()
{
	Engine::CDInputMgr::GetInstance()->Update_InputDev();
	Engine::CManagement::GetInstance()->Update_Scene(0.00166f);
	Engine::CObjectManager::GetInstance()->Update(0.00166f);
	Engine::CPicking::GetInstance()->Update();
	return 0;
}

void CEditorApplication::LateUpdate_EditorApplication()
{
	Engine::CManagement::GetInstance()->LateUpdate_Scene(0.00166f);
	Engine::CObjectManager::GetInstance()->Late_Update(0.00166f);

	m_pGraphicDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDeviceClass->Render_Begin(D3DXCOLOR(0.f, 0.f, 1.f, 1.f));
}

void CEditorApplication::Render_EditorApplication()
{
	m_pRenderer->Render_GameObject();

	m_pDeviceClass->Render_End();
}

HRESULT CEditorApplication::DefaultSetting(LPDIRECT3DDEVICE9 *ppGraphicDevice)
{
	if (FAILED(Engine::CGraphicDev::GetInstance()->Ready_GraphicDev(g_hWnd, MODE_WIN, WINCX, WINCY, &m_pDeviceClass)))
	{
		MSG_BOX("CGraphicDevice Ready Failed");
		return E_FAIL;
	}

	m_pManagementClass = Engine::CManagement::GetInstance();
	if (!m_pManagementClass)
		return E_FAIL;

	if (FAILED(Engine::CDInputMgr::GetInstance()->Ready_InputDev(g_hInst, g_hWnd)))
		return E_FAIL;

	m_pDeviceClass->Add_Ref();

	(*ppGraphicDevice) = Engine::CGraphicDev::GetInstance()->Get_GraphicDev();
	(*ppGraphicDevice)->AddRef();

	// 텍스처 필터
	(*ppGraphicDevice)->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	(*ppGraphicDevice)->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	(*ppGraphicDevice)->SetRenderState(D3DRS_ZENABLE, TRUE);	// Z버퍼에 값을 기록, Z값 기준 자동 정렬 수행 여부를 물음
	(*ppGraphicDevice)->SetRenderState(D3DRS_ZWRITEENABLE, TRUE); // Z버퍼에 Z값을 기록할 지 결정

	return S_OK;
}

HRESULT CEditorApplication::ImGuiInitialize()
{
	::IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // 키보드 컨트롤 허용
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // 게임패드 컨트롤 허용
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Docking 허용
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // multi-viewport
	
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	ImGuiStyle &style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	::ImGui_ImplWin32_Init(g_hWnd);
	::ImGui_ImplDX9_Init(m_pGraphicDevice);

	m_bShowDemoWindow = true;
	m_bShowAnotherWindow = false;

	return S_OK;
}

HRESULT CEditorApplication::Ready_Prototype_Component()
{
	if (FAILED(Engine::CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Transform", Engine::CTransform::Create(m_pGraphicDevice))))
		return E_FAIL;

	if (FAILED(Engine::CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Renderer", m_pRenderer = Engine::CRenderer::Create(m_pGraphicDevice))))
		return E_FAIL;

	
	// Texture_PanelDefault
	// Panel
	{
		CGui_Panel *pNewPanel = CGui_Panel::Create("Test");
		CGuiManager::GetInstance()->AddPanel(pNewPanel);
		// Test
		{
			CGui_Thumbnail *pThumbnail = CGui_Thumbnail::Create("Test Thumbnail");
			pNewPanel->AddElement(pThumbnail);
		}
	}

	CGui_Thumbnail *pThumbnail = static_cast<CGui_Thumbnail*>(CGuiManager::GetInstance()->GetPanel("Test")->GetElement("Test Thumbnail"));
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Component_Texture_PanelDefault",
		CTexture::Create(m_pGraphicDevice, TEX_NORMAL, L"../../Client/Bin/Resource/MapObject/Grid/GridBox_Default.png", 1))))
		return E_FAIL;
	auto pTexture = static_cast<CTexture*>(CComponentMgr::GetInstance()->Find_Component(SCENE_STATIC, L"Proto_Component_Texture_PanelDefault"))
		->Get_Texture();
	pThumbnail->Add_Thumbnail("GridBox_Default", L"Proto_Component_Texture_PanelDefault", pTexture);

#pragma region 테스트
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Component_Texture_PanelTest",
		CTexture::Create(m_pGraphicDevice, TEX_NORMAL, L"../../Client/Bin/Resource/MapObject/Grid/GridBox_Trigger.png", 1))))
		return E_FAIL;
	pTexture = static_cast<CTexture *>(CComponentMgr::GetInstance()->Find_Component(SCENE_STATIC, L"Proto_Component_Texture_PanelTest"))
		->Get_Texture();
	pThumbnail->Add_Thumbnail("GridBox_Trigger", L"Proto_Component_Texture_PanelTest", pTexture);

	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Component_Texture_PanelTest2",
		CTexture::Create(m_pGraphicDevice, TEX_NORMAL, L"../../Client/Bin/Resource/MapObject/Grid/GridBox_NoDraw.png", 1))))
		return E_FAIL;
	pTexture = static_cast<CTexture *>(CComponentMgr::GetInstance()->Find_Component(SCENE_STATIC, L"Proto_Component_Texture_PanelTest2"))
		->Get_Texture();
	pThumbnail->Add_Thumbnail("GridBox_NoDraw", L"Proto_Component_Texture_PanelTest2", pTexture);

	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Component_Texture_PanelTest3",
		CTexture::Create(m_pGraphicDevice, TEX_NORMAL, L"../../Client/Bin/Resource/MapObject/Grid/GridBox_Collider.png", 1))))
		return E_FAIL;
	pTexture = static_cast<CTexture *>(CComponentMgr::GetInstance()->Find_Component(SCENE_STATIC, L"Proto_Component_Texture_PanelTest3"))
		->Get_Texture();
	pThumbnail->Add_Thumbnail("GridBox_Collider", L"Proto_Component_Texture_PanelTest3", pTexture);

	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Component_Texture_PanelTest4",
		CTexture::Create(m_pGraphicDevice, TEX_NORMAL, L"../../Client/Bin/Resource/MapObject/Floor/FLOOR 1.png", 1))))
		return E_FAIL;
	pTexture = static_cast<CTexture *>(CComponentMgr::GetInstance()->Find_Component(SCENE_STATIC, L"Proto_Component_Texture_PanelTest4"))
		->Get_Texture();
	pThumbnail->Add_Thumbnail("FLOOR 1", L"Proto_Component_Texture_PanelTest4", pTexture);

	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Component_Texture_PanelTest5",
		CTexture::Create(m_pGraphicDevice, TEX_NORMAL, L"../../Client/Bin/Resource/MapObject/Floor/FLOOR 2.png", 1))))
		return E_FAIL;
	pTexture = static_cast<CTexture *>(CComponentMgr::GetInstance()->Find_Component(SCENE_STATIC, L"Proto_Component_Texture_PanelTest5"))
		->Get_Texture();
	pThumbnail->Add_Thumbnail("FLOOR 2", L"Proto_Component_Texture_PanelTest5", pTexture);

	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Component_Texture_PanelTest6",
		CTexture::Create(m_pGraphicDevice, TEX_NORMAL, L"../../Client/Bin/Resource/MapObject/Floor/FLOOR 3.png", 1))))
		return E_FAIL;
	pTexture = static_cast<CTexture *>(CComponentMgr::GetInstance()->Find_Component(SCENE_STATIC, L"Proto_Component_Texture_PanelTest6"))
		->Get_Texture();
	pThumbnail->Add_Thumbnail("FLOOR 3", L"Proto_Component_Texture_PanelTest6", pTexture);

	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Component_Texture_PanelTest7",
		CTexture::Create(m_pGraphicDevice, TEX_NORMAL, L"../../Client/Bin/Resource/MapObject/Floor/FLOOR 4.png", 1))))
		return E_FAIL;
	pTexture = static_cast<CTexture *>(CComponentMgr::GetInstance()->Find_Component(SCENE_STATIC, L"Proto_Component_Texture_PanelTest7"))
		->Get_Texture();
	pThumbnail->Add_Thumbnail("FLOOR 4", L"Proto_Component_Texture_PanelTest7", pTexture);

	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Component_Texture_PanelTest8",
		CTexture::Create(m_pGraphicDevice, TEX_NORMAL, L"../../Client/Bin/Resource/MapObject/Floor/FLOOR 5.png", 1))))
		return E_FAIL;
	pTexture = static_cast<CTexture *>(CComponentMgr::GetInstance()->Find_Component(SCENE_STATIC, L"Proto_Component_Texture_PanelTest8"))
		->Get_Texture();
	pThumbnail->Add_Thumbnail("FLOOR 5", L"Proto_Component_Texture_PanelTest8", pTexture);

	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Component_Texture_PanelTest9",
		CTexture::Create(m_pGraphicDevice, TEX_NORMAL, L"../../Client/Bin/Resource/MapObject/Floor/FLOOR 6.png", 1))))
		return E_FAIL;
	pTexture = static_cast<CTexture *>(CComponentMgr::GetInstance()->Find_Component(SCENE_STATIC, L"Proto_Component_Texture_PanelTest9"))
		->Get_Texture();
	pThumbnail->Add_Thumbnail("FLOOR 6", L"Proto_Component_Texture_PanelTest9", pTexture);

	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Component_Texture_PanelTest10",
		CTexture::Create(m_pGraphicDevice, TEX_NORMAL, L"../../Client/Bin/Resource/MapObject/Floor/FLOOR 7.png", 1))))
		return E_FAIL;
	pTexture = static_cast<CTexture *>(CComponentMgr::GetInstance()->Find_Component(SCENE_STATIC, L"Proto_Component_Texture_PanelTest10"))
		->Get_Texture();
	pThumbnail->Add_Thumbnail("FLOOR 7", L"Proto_Component_Texture_PanelTest10", pTexture);

#pragma endregion


	return S_OK;
}

HRESULT CEditorApplication::Ready_Scene()
{
	CEditorLoadingScene *pEditorLoadingScene = CEditorLoadingScene::Create(m_pGraphicDevice, SCENE::SCENE_EDITOR);
	if (!pEditorLoadingScene)
		return E_FAIL;

	Engine::CManagement::GetInstance()->Open_Scene(SCENE_LOADING, pEditorLoadingScene);
	return S_OK;
}

void CEditorApplication::RenderImGuiRender()
{
	ImVec4 vClear_Color = ImVec4(0.45f, 0.55f, 0.6f, 1.f);

	//ImGui
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO &io = ImGui::GetIO();

	if (m_bShowDemoWindow)
		ImGui::ShowDemoWindow(&m_bShowDemoWindow);

	{
		CGuiManager::GetInstance()->Render();
	}

	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, World");

		ImGui::Text("This is some useful text.");
		ImGui::Checkbox("Demo Window", &m_bShowDemoWindow);
		ImGui::Checkbox("Another Window", &m_bShowAnotherWindow);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		ImGui::ColorEdit3("clear color", (float *)&vClear_Color);

		if (ImGui::Button("Button"))
			counter++;

		ImGui::SameLine();
		ImGui::Text("counter - %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.f / io.Framerate, io.Framerate);

		ImGui::End();
	}

	if (m_bShowAnotherWindow)
	{
		ImGui::Begin("Another Window", &m_bShowAnotherWindow);
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			m_bShowAnotherWindow = false;

		ImGui::End();
	}

	//RenderGui
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}
