#include "pch.h"
//=========================
// Imgui
//=========================
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
//=========================
// ETC
//=========================
#include "Editor_Define.h"
#include "Engine_Define.h"
//=========================
// Manager
//=========================
#include "CGraphicDev.h"
#include "CMapFactory.h"
#include "CObjectManager.h"
#include "CComponentMgr.h"
#include "CDInputMgr.h"
#include "CRenderer.h"
#include "CGuiManager.h"
#include "CFileManager.h"
#include "CManagement.h"
#include "CDataManager.h"
#include "CEditorLoadingScene.h"
#include "CEditorPickingManager.h"
#include "CPicking.h"
//=========================
// Object
//=========================
#include "CGridPanel.h"
#include "CEditorCamera.h"
#include "CVIBuffer_Cube_Color.h"
#include "CVIBuffer_GridPanel_Editor.h"
#include "CDummyTile.h"
#include "CTile.h"
#include "CDummyPlacementObject.h"
#include "CPlacementObject.h"
//=========================
// Component
//=========================

//=========================
// Scene
//=========================
#include "CEditLoader.h"
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
	Engine::CDataManager::GetInstance()->DestroyInstance();
	Engine::CMapFactory::GetInstance()->DestroyInstance();

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

	if (FAILED(Ready_MapObjectTexture()))
		return E_FAIL;

	if (FAILED(Ready_Scene()))
		return E_FAIL;

	if (FAILED(CGuiManager::GetInstance()->Ready_CGuiManager(m_pGraphicDevice)))
		return E_FAIL;

	if (FAILED(CGuiManager::GetInstance()->Initialize()))
		return E_FAIL;

	Ready_MapFactorFunc();

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

	//// Camera_Dynamic
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Proto_Camera_Edit",
		CEditorCamera::Create(m_pGraphicDevice))))
		return E_FAIL;

	// DefaultPanel
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Proto_GameObject_DefaultPanel",
		CGridPanel::Create(m_pGraphicDevice))))
		return E_FAIL;

	// DefaultTile
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Proto_GameObject_DefaultTile",
		CTile::Create(m_pGraphicDevice))))
		return E_FAIL;

	// DummyTile
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Proto_GameObject_DummyTile",
		CDummyTile::Create(m_pGraphicDevice))))
		return E_FAIL;

	// Dummy PlacementObject
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Proto_GameObject_DummyPlacementObject",
		CDummyPlacementObject::Create(m_pGraphicDevice))))
		return E_FAIL;

	// Default PlacementObject
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Proto_GameObject_DefaultPlacementObject",
		CPlacementObject::Create(m_pGraphicDevice))))
		return E_FAIL;

	// Buffer_PanelDefault
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Component_Buffer_PanelDefault",
		CVIBuffer_GridPanel_Editor::Create(m_pGraphicDevice))))
		return E_FAIL;

	// Buffer_TileDefault
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Component_Buffer_TileDefault",
		CVIBuffer_Rect::Create(m_pGraphicDevice))))
		return E_FAIL;

	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Component_Buffer_CubeColor",
		CVIBuffer_Cube_Color::Create(m_pGraphicDevice))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEditorApplication::Ready_Scene()
{
	CEditorLoadingScene *pEditorLoadingScene = CEditorLoadingScene::Create(m_pGraphicDevice, SCENE::SCENE_DEV);
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

void CEditorApplication::Ready_MapFactorFunc()
{
	CMapFactory *pMapFactory = CMapFactory::GetInstance();

	std::function<HRESULT(void *)> _func =
	[](void *pData = nullptr)->HRESULT
	{
		_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
		return CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_DefaultPanel", iTargetScene, L"Wall_Layer", pData);
	};
	for (int i = 0; i < g_WallTypeCount; ++i)
	{
		pMapFactory->Register(ObjectCategory::WALL, i, _func);
	}

	_func =
		[](void *pData = nullptr)->HRESULT
	{
		_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
		return CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_DefaultTile", iTargetScene, L"Tile_Layer", pData);
	};
	for (int i = 0; i < g_TileTypeCount; ++i)
	{
		pMapFactory->Register(ObjectCategory::TILE, i, _func);
	}

	_func =
		[](void *pData = nullptr)->HRESULT
	{
		_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
		return CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_DefaultPlacementObject", iTargetScene, L"Env_Layer", pData);
	};
	for (int i = 0; i < g_EnvTypeCount; ++i)
	{
		pMapFactory->Register(ObjectCategory::ENV_OBJ, i, _func);
	}

	_func =
		[](void *pData = nullptr)->HRESULT
	{
		_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
		return CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_DefaultPlacementObject", iTargetScene, L"Monster_Layer", pData);
	};
	for (int i = 0; i < g_MonsterTypeCount; ++i)
	{
		pMapFactory->Register(ObjectCategory::MONSTER, i, _func);
	}
}

HRESULT CEditorApplication::Ready_MapObjectTexture()
{
	CComponentMgr *pCompMgr = CComponentMgr::GetInstance();
#define AddTextureForThumbnail(CompName, Path) if(FAILED(pCompMgr->Add_Prototype(SCENE_STATIC, CompName,	\
												CTexture::Create(m_pGraphicDevice, TEX_NORMAL, Path, 1))))	\
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
	AddTextureForThumbnail(L"Proto_Concrete_Wall", L"../../Client/Bin/Resource/MapObject/Wall/CONCRETE WALL.png");
	AddTextureForThumbnail(L"Proto_Corner_Wall_1", L"../../Client/Bin/Resource/MapObject/Wall/WALL 1 CORNER.png");
	AddTextureForThumbnail(L"Proto_Corner_Wall_2", L"../../Client/Bin/Resource/MapObject/Wall/WALL 2 CORNER.png");
	AddTextureForThumbnail(L"Proto_Corner_Wall_3", L"../../Client/Bin/Resource/MapObject/Wall/WALL 3 CORNER.png");
	AddTextureForThumbnail(L"Proto_Corner_Wall_4", L"../../Client/Bin/Resource/MapObject/Wall/WALL 4 CORNER.png");
	AddTextureForThumbnail(L"Proto_Corner_Wall_5_1", L"../../Client/Bin/Resource/MapObject/Wall/WALL 5 CORNER_1.png");
	AddTextureForThumbnail(L"Proto_Corner_Wall_5_2", L"../../Client/Bin/Resource/MapObject/Wall/WALL 5 CORNER_2.png");
	AddTextureForThumbnail(L"Proto_Corner_Wall_6", L"../../Client/Bin/Resource/MapObject/Wall/WALL 6 CORNER.png");
	AddTextureForThumbnail(L"Proto_Corner_Wall_7", L"../../Client/Bin/Resource/MapObject/Wall/WALL 7 CORNER.png");
	AddTextureForThumbnail(L"Proto_Corner_Wall_8", L"../../Client/Bin/Resource/MapObject/Wall/WALL 8 CORNER.png");
	AddTextureForThumbnail(L"Proto_Corner_Wall_Boss", L"../../Client/Bin/Resource/MapObject/Wall/WALL BOSS 3 CORNER.png");
	AddTextureForThumbnail(L"Proto_Wall_1A", L"../../Client/Bin/Resource/MapObject/Wall/WALL 1A.png");
	AddTextureForThumbnail(L"Proto_Wall_1B", L"../../Client/Bin/Resource/MapObject/Wall/WALL 1B.png");
	AddTextureForThumbnail(L"Proto_Wall_1C", L"../../Client/Bin/Resource/MapObject/Wall/WALL 1C.png");
	AddTextureForThumbnail(L"Proto_Wall_2A", L"../../Client/Bin/Resource/MapObject/Wall/WALL 2A.png");
	AddTextureForThumbnail(L"Proto_Wall_2B", L"../../Client/Bin/Resource/MapObject/Wall/WALL 2B.png");
	AddTextureForThumbnail(L"Proto_Wall_2C", L"../../Client/Bin/Resource/MapObject/Wall/WALL 2C.png");
	AddTextureForThumbnail(L"Proto_Wall_3A_1", L"../../Client/Bin/Resource/MapObject/Wall/WALL 3A_1.png");
	AddTextureForThumbnail(L"Proto_Wall_3A_2", L"../../Client/Bin/Resource/MapObject/Wall/WALL 3A_2.png");
	AddTextureForThumbnail(L"Proto_Wall_3B_1", L"../../Client/Bin/Resource/MapObject/Wall/WALL 3B_1.png");
	AddTextureForThumbnail(L"Proto_Wall_3B_2", L"../../Client/Bin/Resource/MapObject/Wall/WALL 3B_2.png");
	AddTextureForThumbnail(L"Proto_Wall_3C_1", L"../../Client/Bin/Resource/MapObject/Wall/WALL 3C_1.png");
	AddTextureForThumbnail(L"Proto_Wall_3C_2", L"../../Client/Bin/Resource/MapObject/Wall/WALL 3C_2.png");
	AddTextureForThumbnail(L"Proto_Wall_4A", L"../../Client/Bin/Resource/MapObject/Wall/WALL 4A.png");
	AddTextureForThumbnail(L"Proto_Wall_4B", L"../../Client/Bin/Resource/MapObject/Wall/WALL 4B.png");
	AddTextureForThumbnail(L"Proto_Wall_4C_1", L"../../Client/Bin/Resource/MapObject/Wall/WALL 4C_1.png");
	AddTextureForThumbnail(L"Proto_Wall_4C_2", L"../../Client/Bin/Resource/MapObject/Wall/WALL 4C_2.png");
	AddTextureForThumbnail(L"Proto_Wall_5A", L"../../Client/Bin/Resource/MapObject/Wall/WALL 5A.png");
	AddTextureForThumbnail(L"Proto_Wall_5B", L"../../Client/Bin/Resource/MapObject/Wall/WALL 5B.png");
	AddTextureForThumbnail(L"Proto_Wall_5C", L"../../Client/Bin/Resource/MapObject/Wall/WALL 5C.png");
	AddTextureForThumbnail(L"Proto_Wall_6A", L"../../Client/Bin/Resource/MapObject/Wall/WALL 6A.png");
	AddTextureForThumbnail(L"Proto_Wall_6B", L"../../Client/Bin/Resource/MapObject/Wall/WALL 6B.png");
	AddTextureForThumbnail(L"Proto_Wall_6C", L"../../Client/Bin/Resource/MapObject/Wall/WALL 6C.png");
	AddTextureForThumbnail(L"Proto_Wall_7A", L"../../Client/Bin/Resource/MapObject/Wall/WALL 7A.png");
	AddTextureForThumbnail(L"Proto_Wall_7B", L"../../Client/Bin/Resource/MapObject/Wall/WALL 7B.png");
	AddTextureForThumbnail(L"Proto_Wall_7C", L"../../Client/Bin/Resource/MapObject/Wall/WALL 7C.png");
	AddTextureForThumbnail(L"Proto_Wall_8A", L"../../Client/Bin/Resource/MapObject/Wall/WALL 8A.png");
	AddTextureForThumbnail(L"Proto_Wall_8B", L"../../Client/Bin/Resource/MapObject/Wall/WALL 8B.png");
	AddTextureForThumbnail(L"Proto_Wall_8C", L"../../Client/Bin/Resource/MapObject/Wall/WALL 8C.png");
	AddTextureForThumbnail(L"Proto_Wall_9A", L"../../Client/Bin/Resource/MapObject/Wall/WALL 9A.png");
	AddTextureForThumbnail(L"Proto_Wall_9B", L"../../Client/Bin/Resource/MapObject/Wall/WALL 9B.png");
	AddTextureForThumbnail(L"Proto_Wall_9C", L"../../Client/Bin/Resource/MapObject/Wall/WALL 9C.png");
	AddTextureForThumbnail(L"Proto_Wall_10A", L"../../Client/Bin/Resource/MapObject/Wall/WALL 10A.png");
	AddTextureForThumbnail(L"Proto_Wall_10B", L"../../Client/Bin/Resource/MapObject/Wall/WALL 10B.png");
	AddTextureForThumbnail(L"Proto_Wall_10C", L"../../Client/Bin/Resource/MapObject/Wall/WALL 10C.png");
	AddTextureForThumbnail(L"Proto_Wall_Deco_1", L"../../Client/Bin/Resource/MapObject/Wall/WALL DECO 1.png");
	AddTextureForThumbnail(L"Proto_Wall_Deco_2", L"../../Client/Bin/Resource/MapObject/Wall/WALL DECO 2.png");
	AddTextureForThumbnail(L"Proto_Wall_Deco_3", L"../../Client/Bin/Resource/MapObject/Wall/WALL DECO 3.png");
	AddTextureForThumbnail(L"Proto_Wall_Deco_4", L"../../Client/Bin/Resource/MapObject/Wall/WALL DECO 4.png");
	AddTextureForThumbnail(L"Proto_Wall_Deco_5", L"../../Client/Bin/Resource/MapObject/Wall/WALL DECO 5.png");
	AddTextureForThumbnail(L"Proto_Wall_Deco_6", L"../../Client/Bin/Resource/MapObject/Wall/WALL DECO 6.png");
	AddTextureForThumbnail(L"Proto_Wall_Deco_7", L"../../Client/Bin/Resource/MapObject/Wall/WALL DECO 7.png");
	AddTextureForThumbnail(L"Proto_Wall_Deco_8", L"../../Client/Bin/Resource/MapObject/Wall/WALL DECO 8.png");
	AddTextureForThumbnail(L"Proto_Wall_Hole_1", L"../../Client/Bin/Resource/MapObject/Wall/WALL HOLE 1.png");
	AddTextureForThumbnail(L"Proto_Wall_Hole_2", L"../../Client/Bin/Resource/MapObject/Wall/WALL HOLE 2.png");
	AddTextureForThumbnail(L"Proto_Wall_Hole_3", L"../../Client/Bin/Resource/MapObject/Wall/WALL HOLE 3.png");
	AddTextureForThumbnail(L"Proto_Wall_Hole_4", L"../../Client/Bin/Resource/MapObject/Wall/WALL HOLE 4.png");
	AddTextureForThumbnail(L"Proto_Wall_Boss_1", L"../../Client/Bin/Resource/MapObject/Wall/WALL boss 3 1.png");
	AddTextureForThumbnail(L"Proto_Wall_Boss_2", L"../../Client/Bin/Resource/MapObject/Wall/WALL boss 3 2.png");
	AddTextureForThumbnail(L"Proto_Wall_Boss_3", L"../../Client/Bin/Resource/MapObject/Wall/WALL boss 3 3.png");
	AddTextureForThumbnail(L"Proto_Wall_Border", L"../../Client/Bin/Resource/MapObject/Wall/WALL BORDER.png");
	AddTextureForThumbnail(L"Proto_Fence", L"../../Client/Bin/Resource/MapObject/FENCE.png");
	AddTextureForThumbnail(L"Proto_JumpBorder", L"../../Client/Bin/Resource/MapObject/JUMP BORDER.png");
	AddTextureForThumbnail(L"Proto_Wall_Slidedash", L"../../Client/Bin/Resource/MapObject/Wall/SIDE DASH WALL.png");
	AddTextureForThumbnail(L"Proto_Steira", L"../../Client/Bin/Resource/MapObject/STEIRA.png");
	AddTextureForThumbnail(L"Proto_Windows", L"../../Client/Bin/Resource/MapObject/WINDOWS.png");
	AddTextureForThumbnail(L"Proto_Floor_Elevator", L"../../Client/Bin/Resource/MapObject/Elevator/ELEVATOR FLOOR.png");
	AddTextureForThumbnail(L"Proto_Wall_Elevator", L"../../Client/Bin/Resource/MapObject/Elevator/ELEVATOR COLUMS.png");
	AddTextureForThumbnail(L"Proto_Ceiling_Elevator", L"../../Client/Bin/Resource/MapObject/Elevator/ELEVATOR ROOF.png");
	AddTextureForThumbnail(L"Proto_Platform_1", L"../../Client/Bin/Resource/MapObject/Platform/PLATFORM 1.png");
	AddTextureForThumbnail(L"Proto_Platform_2", L"../../Client/Bin/Resource/MapObject/Platform/PLATFORM 2.png");

	// tile
	AddTextureForThumbnail(L"Proto_Acid_Env", L"../../Client/Bin/Resource/MapObject/Acid/acid dash.png");
	AddTextureForThumbnail(L"Proto_Electric_Wall", L"../../Client/Bin/Resource/MapObject/Wall/ELECTRIC WALL.png");
	AddTextureForThumbnail(L"Proto_Glass", L"../../Client/Bin/Resource/MapObject/GLASS.png");	
	AddTextureForThumbnail(L"Proto_BIO_1", L"../../Client/Bin/Resource/MapObject/Wall/WALL BIO 1.png");
	AddTextureForThumbnail(L"Proto_BIO_2", L"../../Client/Bin/Resource/MapObject/Wall/WALL BIO 2.png");
	AddTextureForThumbnail(L"Proto_Rug_1", L"../../Client/Bin/Resource/MapObject/Rug/RUG 1.png");
	AddTextureForThumbnail(L"Proto_Rug_2", L"../../Client/Bin/Resource/MapObject/Rug/RUG 2.png");
	AddTextureForThumbnail(L"Proto_Rug_3", L"../../Client/Bin/Resource/MapObject/Rug/RUG 3.png");
	AddTextureForThumbnail(L"Proto_Rug_4", L"../../Client/Bin/Resource/MapObject/Rug/RUG 4.png");
	AddTextureForThumbnail(L"Proto_Wall_Katana", L"../../Client/Bin/Resource/MapObject/Wall/WALL KATANAS.png");
	AddTextureForThumbnail(L"Proto_Wall_Transparent_1", L"../../Client/Bin/Resource/MapObject/Wall/WALL TRANSPARENT 1.png");
	AddTextureForThumbnail(L"Proto_Wall_Transparent_2", L"../../Client/Bin/Resource/MapObject/Wall/WALL TRANSPARENT 2.png");
	AddTextureForThumbnail(L"Proto_Wall_Transparent_3", L"../../Client/Bin/Resource/MapObject/Wall/WALL TRANSPARENT 3.png");
	AddTextureForThumbnail(L"Proto_Wall_Transparent_4", L"../../Client/Bin/Resource/MapObject/Wall/WALL TRANSPARENT 4.png");
	AddTextureForThumbnail(L"Proto_Vent", L"../../Client/Bin/Resource/MapObject/Ventilador/BIR AIR CONDITIONER.png");
	AddTextureForThumbnail(L"Proto_NormalDoor_1", L"../../Client/Bin/Resource/MapObject/Door/DOOR 2.png");
	AddTextureForThumbnail(L"Proto_NormalDoor_2", L"../../Client/Bin/Resource/MapObject/Door/DOOR 3.png");
	AddTextureForThumbnail(L"Proto_NormalDoor_3", L"../../Client/Bin/Resource/MapObject/Door/DOOR 4.png");
	AddTextureForThumbnail(L"Proto_OpeningDoor", L"../../Client/Bin/Resource/MapObject/Elevator/DOOR ELEVATOR.png");
	AddTextureForThumbnail(L"Proto_CABLES_1", L"../../Client/Bin/Resource/MapObject/Tile/CABLES 3.png");
	AddTextureForThumbnail(L"Proto_CABLES_2", L"../../Client/Bin/Resource/MapObject/Tile/CABLES 4.png");

	// display
	AddTextureForThumbnail(L"Proto_Bandit_Outdoor", L"../../Client/Bin/Resource/MapObject/DisplayBoard/BANDIT.png");
	AddTextureForThumbnail(L"Proto_Beach_Outdoor", L"../../Client/Bin/Resource/MapObject/DisplayBoard/BEACH.png");
	AddTextureForThumbnail(L"Proto_Beer_Outdoor", L"../../Client/Bin/Resource/MapObject/DisplayBoard/BEER.png");
	AddTextureForThumbnail(L"Proto_Fuck_Outdoor", L"../../Client/Bin/Resource/MapObject/DisplayBoard/FKU001.png");
	AddTextureForThumbnail(L"Proto_Kimono_Outdoor", L"../../Client/Bin/Resource/MapObject/DisplayBoard/KIMONO.png");
	AddTextureForThumbnail(L"Proto_SaveHer_Outdoor", L"../../Client/Bin/Resource/MapObject/DisplayBoard/SAVEHER.png");
	AddTextureForThumbnail(L"Proto_Shoes_Outdoor", L"../../Client/Bin/Resource/MapObject/DisplayBoard/SHOES.png");	

	// bottle
	AddTextureForThumbnail(L"Proto_Bottle_1", L"../../Client/Bin/Resource/MapObject/Bottle/Bottle1.png");
	AddTextureForThumbnail(L"Proto_Bottle_2", L"../../Client/Bin/Resource/MapObject/Bottle/Bottle2.png");
	AddTextureForThumbnail(L"Proto_Bottle_3", L"../../Client/Bin/Resource/MapObject/Bottle/Bottle3.png");
	AddTextureForThumbnail(L"Proto_Bottle_4", L"../../Client/Bin/Resource/MapObject/Bottle/Bottle4.png");
	AddTextureForThumbnail(L"Proto_Bottle_5", L"../../Client/Bin/Resource/MapObject/Bottle/Bottle5.png");
	AddTextureForThumbnail(L"Proto_Bottle_6", L"../../Client/Bin/Resource/MapObject/Bottle/Bottle6.png");
	AddTextureForThumbnail(L"Proto_Bottle_7", L"../../Client/Bin/Resource/MapObject/Bottle/Bottle7.png");

	// vendingmachine
	AddTextureForThumbnail(L"Proto_VendingMachine", L"../../Client/Bin/Resource/MapObject/soda/SODA MACHINE_00.png");

	// signs
	AddTextureForThumbnail(L"Proto_Signs_1", L"../../Client/Bin/Resource/MapObject/Signs/SIGNS 1.png");
	AddTextureForThumbnail(L"Proto_Signs_2", L"../../Client/Bin/Resource/MapObject/Signs/SIGNS 3.png");
	AddTextureForThumbnail(L"Proto_Signs_3", L"../../Client/Bin/Resource/MapObject/Signs/SIGNS 4.png");
	AddTextureForThumbnail(L"Proto_Signs_4", L"../../Client/Bin/Resource/MapObject/Signs/SIGNS 5.png");
	AddTextureForThumbnail(L"Proto_Signs_5", L"../../Client/Bin/Resource/MapObject/Signs/SIGNS 6.png");
	AddTextureForThumbnail(L"Proto_Signs_6", L"../../Client/Bin/Resource/MapObject/Signs/SIGNS 7.png");
	AddTextureForThumbnail(L"Proto_Signs_7", L"../../Client/Bin/Resource/MapObject/Signs/SIGNS 8.png");

	return S_OK;
}
