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
#include "CGui_Button.h"
#include "CRenderer.h"
#include "CVIBuffer_GridPanel.h"
#include "CManagement.h"
#include "CEditorLoadingScene.h"
#include "CEditorPickingManager.h"
#include "CPicking.h"
#include "CGuiManager.h"
#include "CGui_Log.h"
#include "CFileManager.h"
#include "CGridPanel.h"
#include "CGui_Panel.h"
#include "CGui_Thumbnail.h"
#include "CGui_ButtonList.h"
#include "CGui_Transform.h"
#include "CGraphicDev.h"
#include "CObjectManager.h"
#include "CComponentMgr.h"
#include "CDInputMgr.h"
#include "Editor_Define.h"
#include "Engine_Define.h"
#include "CEditorApplication.h"

CEditorApplication::CEditorApplication()
	: m_pDeviceClass(nullptr), m_pGraphicDevice(nullptr), m_pManagementClass(nullptr)
	, m_pRenderer(nullptr), m_bShowDemoWindow(false)
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

	if (FAILED(CGuiManager::GetInstance()->Ready_CGuiManager(m_pGraphicDevice)))
		return E_FAIL;

	if (FAILED(CGuiManager::GetInstance()->Initialize()))
		return E_FAIL;

	return S_OK;
}

void CEditorApplication::Run_EditorApplication()
{
	RenderImGuiRender();

	Update_EditorApplication();
	LateUpdate_EditorApplication();

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

HRESULT CEditorApplication::Ready_Prototype_Component()
{
	if (FAILED(Engine::CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Transform", Engine::CTransform::Create(m_pGraphicDevice))))
		return E_FAIL;

	if (FAILED(Engine::CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Renderer", m_pRenderer = Engine::CRenderer::Create(m_pGraphicDevice))))
		return E_FAIL;

#define AddTextureForThumbnail(CompName, Path) if(FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, CompName,	\
												CTexture::Create(m_pGraphicDevice, TEX_NORMAL, Path, 1))))						\
												return E_FAIL

	AddTextureForThumbnail(L"Proto_GridDefault", L"../../Client/Bin/Resource/MapObject/Grid/GridBox_Default.png");
	AddTextureForThumbnail(L"Proto_GridTrigger", L"../../Client/Bin/Resource/MapObject/Grid/GridBox_Trigger.png");
	AddTextureForThumbnail(L"Proto_GridNoDraw", L"../../Client/Bin/Resource/MapObject/Grid/GridBox_NoDraw.png");
	AddTextureForThumbnail(L"Proto_GridCollider", L"../../Client/Bin/Resource/MapObject/Grid/GridBox_Collider.png");
	AddTextureForThumbnail(L"Proto_Floor_1", L"../../Client/Bin/Resource/MapObject/Floor/FLOOR 1.png");
	AddTextureForThumbnail(L"Proto_Floor_2", L"../../Client/Bin/Resource/MapObject/Floor/FLOOR 2.png");
	AddTextureForThumbnail(L"Proto_Floor_3", L"../../Client/Bin/Resource/MapObject/Floor/FLOOR 3.png");
	AddTextureForThumbnail(L"Proto_Floor_4", L"../../Client/Bin/Resource/MapObject/Floor/FLOOR 4.png");
	AddTextureForThumbnail(L"Proto_Floor_5", L"../../Client/Bin/Resource/MapObject/Floor/FLOOR 5.png");
	AddTextureForThumbnail(L"Proto_Floor_6", L"../../Client/Bin/Resource/MapObject/Floor/FLOOR 6.png");
	AddTextureForThumbnail(L"Proto_Floor_7", L"../../Client/Bin/Resource/MapObject/Floor/FLOOR 7.png");
	AddTextureForThumbnail(L"Proto_Acid_Floor_1", L"../../Client/Bin/Resource/MapObject/Acid/ACID A.png");
	AddTextureForThumbnail(L"Proto_Acid_Floor_2", L"../../Client/Bin/Resource/MapObject/Acid/ACID D.png");
	AddTextureForThumbnail(L"Proto_Acid_Wall_1", L"../../Client/Bin/Resource/MapObject/Acid/ACID B.png");
	AddTextureForThumbnail(L"Proto_Acid_Wall_2", L"../../Client/Bin/Resource/MapObject/Acid/ACID C.png");
	AddTextureForThumbnail(L"Proto_Acid_Env", L"../../Client/Bin/Resource/MapObject/Acid/acid dash.png");
	AddTextureForThumbnail(L"Proto_Rug_1", L"../../Client/Bin/Resource/MapObject/Rug/RUG 1.png");
	AddTextureForThumbnail(L"Proto_Rug_2", L"../../Client/Bin/Resource/MapObject/Rug/RUG 2.png");
	AddTextureForThumbnail(L"Proto_Rug_3", L"../../Client/Bin/Resource/MapObject/Rug/RUG 3.png");
	AddTextureForThumbnail(L"Proto_Rug_4", L"../../Client/Bin/Resource/MapObject/Rug/RUG 4.png");
	AddTextureForThumbnail(L"Proto_Concrete_Wall", L"../../Client/Bin/Resource/MapObject/Wall/CONCRETE WALL.png");
	AddTextureForThumbnail(L"Proto_Electric_Wall", L"../../Client/Bin/Resource/MapObject/Wall/ELECTRIC WALL.png");
	AddTextureForThumbnail(L"Proto_Slidedash_Wall", L"../../Client/Bin/Resource/MapObject/Wall/SIDE DASH WALL.png");


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
	//ImGui
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO &io = ImGui::GetIO();

	/*if (m_bShowDemoWindow)
		ImGui::ShowDemoWindow(&m_bShowDemoWindow);*/

	{
		CGuiManager::GetInstance()->Render();
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
