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

	if (FAILED(CGuiManager::GetInstance()->Ready_CGuiManager(m_pGraphicDevice)))
		return E_FAIL;

	if (FAILED(Engine::CObjectManager::GetInstance()->Readay_ObjectManager(SCENE_END)))
		return E_FAIL;

	if (FAILED(Engine::CComponentMgr::GetInstance()->Ready_Prototype(SCENE_END)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(Ready_Scene()))
		return E_FAIL;

	if (FAILED(CGuiManager::GetInstance()->Initialize()))
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

HRESULT CEditorApplication::Ready_Prototype_Component()
{
	if (FAILED(Engine::CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Transform", Engine::CTransform::Create(m_pGraphicDevice))))
		return E_FAIL;

	if (FAILED(Engine::CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Renderer", m_pRenderer = Engine::CRenderer::Create(m_pGraphicDevice))))
		return E_FAIL;

	// Console
	{
		CGui_Panel *pConsole = CGuiManager::GetInstance()->GetConsole();
		{
			CGui_Log *pLog = CGui_Log::Create();
			pConsole->AddElement(pLog);
		}
	}

	// Inspector
	{
		CGui_Panel *pInspector = CGuiManager::GetInstance()->GetInspector();
		{
			CGui_Thumbnail *pThumbnail = CGui_Thumbnail::Create("Textures");
			pInspector->AddElement(pThumbnail);

#pragma region GridSize_ButtonsList
			vector<string> _labels{ 4 };
			vector<std::function<void()>> _funcs{ 4 };

			_labels[0] = "++Row";
			_funcs[0] =
			[]()->void {
				if (CGameObject *pGo = CGuiManager::GetInstance()->GetTarget())
				{
					static_cast<CGridPanel *>(pGo)->GetBuffer()->Increase_RowBuffer();
					EDITOR_CONSOLE("test");
				}
			};

			_labels[1] = "--Row";
			_funcs[1] =
				[]()->void {
				if (CGameObject *pGo = CGuiManager::GetInstance()->GetTarget())
				{
					static_cast<CGridPanel *>(pGo)->GetBuffer()->Decrease_RowBuffer();
				}
			};

			_labels[2] = "++Col";
			_funcs[2] =
				[]()->void {
				if (CGameObject *pGo = CGuiManager::GetInstance()->GetTarget())
				{
					static_cast<CGridPanel *>(pGo)->GetBuffer()->Increase_ColBuffer();
				}
			};

			_labels[3] = "--Col";
			_funcs[3] =
				[]()->void {
				if (CGameObject *pGo = CGuiManager::GetInstance()->GetTarget())
				{
					static_cast<CGridPanel *>(pGo)->GetBuffer()->Decrease_ColBuffer();
				}
			};

			CGui_ButtonList *GridSizeButtonsList = CGui_ButtonList::Create("Grid Size", _labels, _funcs);
			pInspector->AddElement(GridSizeButtonsList);

			_labels.clear();
			_funcs.clear();
#pragma endregion

#pragma region Wall_ButtonsList
			_labels.resize(2);
			_funcs.resize(2);

			_labels[0] = "Create";
			_funcs[0] =
				[]()->void {
				CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_SamplePanel", SCENE_EDITOR, L"EditLogic_Layer");
			};

			_labels[1] = "Delete";
			_funcs[1] =
				[]()->void {
				if (CGameObject *pGo = CGuiManager::GetInstance()->GetTarget())
				{
					pGo->Set_Dead(TRUE);
					EDITOR_CONSOLE("SetDead");
				}
			};

			CGui_ButtonList *WallButtonsList = CGui_ButtonList::Create("Wall Create", _labels, _funcs);
			pInspector->AddElement(WallButtonsList);

			_labels.clear();
			_funcs.clear();

#pragma endregion

			CGui_Transform *pTransform = CGui_Transform::Create(TransformDataType::POSITION);
			pInspector->AddElement(pTransform);
		}
	}

	if (FAILED(CGuiManager::GetInstance()->AddTexture_AddThumbnail("GridBox_Default", L"Proto_Component_Texture_PanelDefault",
		L"../../Client/Bin/Resource/MapObject/Grid/GridBox_Default.png")))
		return E_FAIL;

	if (FAILED(CGuiManager::GetInstance()->AddTexture_AddThumbnail("GridBox_Trigger", L"Proto_Component_Texture_PanelTest",
		L"../../Client/Bin/Resource/MapObject/Grid/GridBox_Trigger.png")))
		return E_FAIL;

	if (FAILED(CGuiManager::GetInstance()->AddTexture_AddThumbnail("GridBox_NoDraw", L"Proto_Component_Texture_PanelTest2",
		L"../../Client/Bin/Resource/MapObject/Grid/GridBox_NoDraw.png")))
		return E_FAIL;

	if (FAILED(CGuiManager::GetInstance()->AddTexture_AddThumbnail("GridBox_Collider", L"Proto_Component_Texture_PanelTest3",
		L"../../Client/Bin/Resource/MapObject/Grid/GridBox_Collider.png")))
		return E_FAIL;

	if (FAILED(CGuiManager::GetInstance()->AddTexture_AddThumbnail("FLOOR 1", L"Proto_Component_Texture_PanelTest4",
		L"../../Client/Bin/Resource/MapObject/Floor/FLOOR 1.png")))
		return E_FAIL;

	if (FAILED(CGuiManager::GetInstance()->AddTexture_AddThumbnail("FLOOR 2", L"Proto_Component_Texture_PanelTest5",
		L"../../Client/Bin/Resource/MapObject/Floor/FLOOR 2.png")))
		return E_FAIL;

	if (FAILED(CGuiManager::GetInstance()->AddTexture_AddThumbnail("FLOOR 3", L"Proto_Component_Texture_PanelTest6",
		L"../../Client/Bin/Resource/MapObject/Floor/FLOOR 3.png")))
		return E_FAIL;

	if (FAILED(CGuiManager::GetInstance()->AddTexture_AddThumbnail("FLOOR 4", L"Proto_Component_Texture_PanelTest7",
		L"../../Client/Bin/Resource/MapObject/Floor/FLOOR 4.png")))
		return E_FAIL;

	if (FAILED(CGuiManager::GetInstance()->AddTexture_AddThumbnail("FLOOR 5", L"Proto_Component_Texture_PanelTest8",
		L"../../Client/Bin/Resource/MapObject/Floor/FLOOR 5.png")))
		return E_FAIL;

	if (FAILED(CGuiManager::GetInstance()->AddTexture_AddThumbnail("FLOOR 6", L"Proto_Component_Texture_PanelTest9",
		L"../../Client/Bin/Resource/MapObject/Floor/FLOOR 6.png")))
		return E_FAIL;

	if (FAILED(CGuiManager::GetInstance()->AddTexture_AddThumbnail("FLOOR 7", L"Proto_Component_Texture_PanelTest10",
		L"../../Client/Bin/Resource/MapObject/Floor/FLOOR 7.png")))
		return E_FAIL;

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
