#include "pch.h"
#include "CMainApp.h"

//============
// Component
//============
#include "VIBuffer_Color.h"
#include "CVIBuffer_GridPanel_Horizon.h"
#include "CVIBuffer_GridPanel_Vertical.h"
#include "CVIBuffer_GridPanel_Normal.h"
#include "CVIBuffer_Circle.h"
#include "CGrounding.h"
#include "CRenderer.h"

//============
// Object
//============

//============
// Manager
//============
#include "CManagement.h"
#include "CTimerMgr.h"
#include "CFontMgr.h"
#include "CDInputMgr.h"
#include "CGameDataManager.h"
#include "CColiderManager.h"
#include "CObjectManager.h"
#include "CComponentMgr.h"
#include "CPicking.h"
#include "CPickingManager.h"
#include "CObjectPoolManager.h"
#include "CUIManager.h"
#include "CMapFactory.h"
#include "CFrameMgr.h"
#include "CFileManager.h"
#include "CDataManager.h"
#include "CCullingManager.h"
#include "Sound_Manager.h"

//============
// Scene
//============
#include "CLoading_Scene.h"
#include "CLogo.h"


CMainApp::CMainApp() : m_pGraphicDev(nullptr)
, m_pRenderer(nullptr)
{
}

CMainApp::~CMainApp()
{
	
}

HRESULT CMainApp::Ready_MainApp()
{
	// 전역 랜덤 시드 1회
	LARGE_INTEGER qpc{};
	QueryPerformanceCounter(&qpc);
	unsigned seed = (unsigned)(qpc.QuadPart ^ GetTickCount64() ^ GetCurrentProcessId());
	srand(seed);


	// 디바이스 세팅
	if (FAILED(Ready_DefaultSetting(&m_pGraphicDev)))
		return E_FAIL;

	// 사운드 초기화
	if(FAILED(CSound_Manager::GetInstance()->Initialize()))
		return E_FAIL;

	// 픽킹 초기화
	if(FAILED(CPicking::GetInstance()->Initialize(g_hWnd, m_pGraphicDev)))
		return E_FAIL;

	// 객체 그룹 설정
	if (FAILED(CObjectManager::GetInstance()->Readay_ObjectManager(SCENE_END)))
		return E_FAIL;

	// 컴포넌트 프로토타입 컨테이너 준비
	if (FAILED(CComponentMgr::GetInstance()->Ready_Prototype(SCENE_END)))
		return E_FAIL;

	// 컬링
	if (FAILED(CCullingManager::GetInstance()->Ready_Culling(m_pGraphicDev)))
		return E_FAIL;

	// 기본 컴포넌트 등록
	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	// 로고 씬으로 시작
	if (FAILED(Ready_Scene(SCENE_LOGO)))
		return E_FAIL;

	Ready_MapFactorFunc();

	return S_OK;
}

int CMainApp::Update_MainApp(const float& fTimeDelta)
{
	CDInputMgr::GetInstance()->Update_InputDev(); // 디바이스 갱신

	CManagement::GetInstance()->Update_Scene(fTimeDelta); // 씬 업데이트
	CObjectManager::GetInstance()->Update(fTimeDelta); // 오브젝트 업데이트
	CPicking::GetInstance()->Update();
	CCullingManager::GetInstance()->Update_Culling();

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

	CColiderManager::GetInstance()->Clear_Colider_Group(); // 충돌 그룹을 매프레임마다 비워야 함
}

void CMainApp::Render_MainApp()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE); 
	
	m_pDeviceClass->Render_Begin(D3DXCOLOR(0.f, 0.f, 1.f, 1.f));
	m_pRenderer->Render_GameObject(); // 모든 렌더 대상 렌더링
	m_pDeviceClass->Render_End();

		// FPS 출력
	m_fFPSTime += CTimerMgr::GetInstance()->Get_TimeDelta(TEXT("Timer_60"));
	++m_iFPSCnt;

	if (m_fFPSTime >= 1.f)
	{
		wchar_t szFPS[128];
		swprintf_s(szFPS, 128, L"FPS : %d", m_iFPSCnt);
		SetWindowText(g_hWnd, szFPS);

		m_fFPSTime = 0.f;
		m_iFPSCnt = 0;
	}
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
	CFontMgr::GetInstance()->RegisterPrivateFontFromFile(L"../Bin/Resource/Font/DS-DIGIB.ttf");
	CFontMgr::GetInstance()->RegisterPrivateFontFromFile(L"../Bin/Resource/Font/VCR_OSD_MONO_1 - Japanese Ver s2.ttf");
	CFontMgr::GetInstance()->RegisterPrivateFontFromFile(L"../Bin/Resource/Font/Righteous-Regular.ttf");
	CFontMgr::GetInstance()->RegisterPrivateFontFromFile(L"../Bin/Resource/Font/Perfect DOS VGA 437 - Japanese ver.ttf");
	CFontMgr::GetInstance()->RegisterPrivateFontFromFile(L"Perfect DOS VGA 437 Win.ttf");


	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev,
		L"Font_Time",       
		L"DS-Digital",       
		20, 48, FW_THIN)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev,
		L"Font_UI_Regular",
		L"VCR OSD Mono",
		14, 40, FW_BOLD)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev,
		L"Font_UI_Bold",
		L"Righteous",
		14, 40, FW_BOLD)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev,
		L"Font_UI_ROUGH", // 약간 두드러진 폰트
		L"Perfect DOS VGA 437",
		14, 40, FW_BOLD)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev,
		L"Font_UI_Effect", // 약간 두드러진 폰트
		L"Perfect DOS VGA 437",
		14, 40, FW_BOLD)))
		return E_FAIL;


	CFontMgr::GetInstance()->Ready_Font(
		m_pGraphicDev,
		L"DefaultFont",   // ← CEfffectUI에서 쓰는 태그와 동일해야 함
		L"맑은 고딕",     // 폰트 이름 (설치된 폰트면 뭐든 OK)
		20,               // Width
		40,               // Height
		FW_BOLD);         // Weight

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

// 초기 씬 로딩
HRESULT CMainApp::Ready_Scene(SCENE eScene)
{
	// 로딩 씬 생성
	CLoading_Scene* pLoadingScene = CLoading_Scene::Create(m_pGraphicDev, eScene);
	if (nullptr == pLoadingScene)
		return E_FAIL;

	// 씬 교체
	CManagement::GetInstance()->Open_Scene(SCENE_LOADING, pLoadingScene);
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component() // 모든 컴포넌트 최초 등록
{
	//======================
	// Buffer
	//======================
	// RectBuffer
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Rect_Buffer",
		Engine::CVIBuffer_Rect::Create(m_pGraphicDev))))
		return E_FAIL;
	// RectColorBuffer
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Color_Buffer",
		Engine::VIBuffer_Color::Create(m_pGraphicDev))))
		return E_FAIL;
	// GridPanel Buffers
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Buffer_GridPanel_Horizon",
		Engine::CVIBuffer_GridPanel_Horizon::Create(m_pGraphicDev))))
		return E_FAIL;
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Buffer_GridPanel_Vertical",
		Engine::CVIBuffer_GridPanel_Vertical::Create(m_pGraphicDev))))
		return E_FAIL;
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Buffer_GridPanel_Normal",
		Engine::CVIBuffer_GridPanel_Normal::Create(m_pGraphicDev))))
		return E_FAIL;
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Buffer_Circle",
		Engine::CVIBuffer_Circle::Create(m_pGraphicDev))))
		return E_FAIL;

	//======================
	// Collider
	//======================
	// Colider_Rect
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Colider_Rect",
		Engine::CColider_Rect::Create(m_pGraphicDev))))
		return E_FAIL;
	// Colider_Cube
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Colider_Cube",
		Engine::CColider_Cube::Create(m_pGraphicDev))))
		return E_FAIL;
	// Colider_Sphere
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Colider_Sphere",
		Engine::CColider_Sphere::Create(m_pGraphicDev))))
		return E_FAIL;

	//======================
	// Common
	//======================
	// Transform
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Transform",
		Engine::CTransform::Create(m_pGraphicDev))))
		return E_FAIL;
	// Renderer
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Renderer",
		m_pRenderer = Engine::CRenderer::Create(m_pGraphicDev))))
		return E_FAIL;
	// calculator 
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Calculator",
		Engine::CCalculator::Create(m_pGraphicDev))))
		return E_FAIL;
	// Grounding
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Grounding",
		Engine::CGrounding::Create(m_pGraphicDev))))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Ready_MapFactorFunc()
{
	CMapFactory *pMapFactory = CMapFactory::GetInstance();

#pragma region Wall
	std::function<CGameObject *(void *)> _func =
		[](void *pData = nullptr)->CGameObject *
	{
		_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
		return CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_DefaultPanel", iTargetScene, L"Wall_Layer", pData);
	};
	pMapFactory->Register(ObjectCategory::WALL, static_cast<_uint>(WallType::WALL_HOR), _func);
	pMapFactory->Register(ObjectCategory::WALL, static_cast<_uint>(WallType::WALL_VER), _func);

	_func =
		[](void *pData = nullptr)->CGameObject *
	{
		_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
		return CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_DefaultPanel", iTargetScene, L"Floor_Layer", pData);
	};
	pMapFactory->Register(ObjectCategory::WALL, static_cast<_uint>(WallType::FLOOR), _func);
	pMapFactory->Register(ObjectCategory::WALL, static_cast<_uint>(WallType::INCLINE), _func);

	_func =
		[](void *pData = nullptr)->CGameObject *
	{
		_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
		return CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_DefaultPanel", iTargetScene, L"Ceiling_Layer", pData);
	};
	pMapFactory->Register(ObjectCategory::WALL, static_cast<_uint>(WallType::CEILING), _func);

	_func =
		[](void *pData = nullptr)->CGameObject *
	{
		_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
		return CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_DefaultPanel", iTargetScene, L"SlideWall_Layer", pData);
	};
	pMapFactory->Register(ObjectCategory::WALL, static_cast<_uint>(WallType::WALL_SLIDE), _func);
#pragma endregion

#pragma region Tile
	_func =
		[](void *pData = nullptr)->CGameObject *
	{
		_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
		return CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_DecoTile", iTargetScene, L"Tile_Layer", pData);
	};
	pMapFactory->Register(ObjectCategory::TILE, static_cast<_uint>(TileType::DECO), _func);

	_func =
		[](void *pData = nullptr)->CGameObject *
	{
		_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
		return CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_GlassTile", iTargetScene, L"Tile_Layer", pData);
	};
	pMapFactory->Register(ObjectCategory::TILE, static_cast<_uint>(TileType::GLASS), _func);

	_func =
		[](void *pData = nullptr)->CGameObject *
	{
		_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
		return CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_AcidTile", iTargetScene, L"Tile_Layer", pData);
	};
	pMapFactory->Register(ObjectCategory::TILE, static_cast<_uint>(TileType::ACID), _func);

	_func =
		[](void *pData = nullptr)->CGameObject *
	{
		_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
		return CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_ElectricTile", iTargetScene, L"Tile_Layer", pData);
	};
	pMapFactory->Register(ObjectCategory::TILE, static_cast<_uint>(TileType::ELECTRIC), _func);

	_func =
		[](void *pData = nullptr)->CGameObject *
	{
		_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
		return CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_DecoTile", iTargetScene, L"Tile_Layer", pData);;
	};
	pMapFactory->Register(ObjectCategory::TILE, static_cast<_uint>(TileType::DISPLAY), _func);

	_func =
		[](void *pData = nullptr)->CGameObject *
	{
		_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
		return CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_VentTile", iTargetScene, L"Tile_Layer", pData);
	};
	pMapFactory->Register(ObjectCategory::TILE, static_cast<_uint>(TileType::VENT), _func);

	_func =
		[](void *pData = nullptr)->CGameObject *
	{
		_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
		return CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_NormalDoorTile", iTargetScene, L"Tile_Layer", pData);
	};
	pMapFactory->Register(ObjectCategory::TILE, static_cast<_uint>(TileType::NORMALDOOR), _func);

	_func =
		[](void *pData = nullptr)->CGameObject *
		{
			_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
			return CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_VendingMachine", iTargetScene, L"Tile_Layer", pData);
		};
	pMapFactory->Register(ObjectCategory::TILE, static_cast<_uint>(TileType::VENDINGMACHINE), _func);

	_func =
		[](void *pData = nullptr)->CGameObject *
		{
			_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
			return CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_Bottle", iTargetScene, L"Tile_Layer", pData);
		};
	pMapFactory->Register(ObjectCategory::TILE, static_cast<_uint>(TileType::BOTTLE), _func);
#pragma endregion

#pragma region Env
	_func =
		[](void *pData = nullptr)->CGameObject *
	{
		_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
		return CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_Player", iTargetScene, L"Player_Layer", pData);
	};
	pMapFactory->Register(ObjectCategory::ENV_OBJ, static_cast<_uint>(EnvType::SPAWNPOINT), _func);

	_func =
		[](void *pData = nullptr)->CGameObject *
	{
		_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
		return CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_EndPoint", iTargetScene, L"Env_Layer", pData);
	};
	pMapFactory->Register(ObjectCategory::ENV_OBJ, static_cast<_uint>(EnvType::ENDPOINT), _func);
#pragma endregion

#pragma region Monster
	_func =
		[](void *pData = nullptr)->CGameObject *
	{
		_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
		return CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_Monster_Suit", iTargetScene, L"Monster_Layer", pData);
	};
	pMapFactory->Register(ObjectCategory::MONSTER, static_cast<_uint>(MonsterType::SUIT), _func);
	_func =
		[](void *pData = nullptr)->CGameObject *
	{
		_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
		return CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_Monster_Soldier", iTargetScene, L"Monster_Layer", pData);
	};
	pMapFactory->Register(ObjectCategory::MONSTER, static_cast<_uint>(MonsterType::SOLIDER), _func);
	_func =
		[](void *pData = nullptr)->CGameObject *
	{
		_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
		return CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_Monster_Fat", iTargetScene, L"Monster_Layer", pData);
	};
	pMapFactory->Register(ObjectCategory::MONSTER, static_cast<_uint>(MonsterType::FAT), _func);
	_func =
		[](void *pData = nullptr)->CGameObject *
	{
		_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
		return CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_Monster_Drone", iTargetScene, L"Monster_Layer", pData);
	};
	pMapFactory->Register(ObjectCategory::MONSTER, static_cast<_uint>(MonsterType::DRONE), _func);
#pragma endregion

#pragma region Prefab
	_func =
		[](void *pData = nullptr)->CGameObject *
	{
		_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
		return CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_DefaultPrefab", iTargetScene, L"Prefab_Layer", pData);
	};
	for (int i = 0; i < g_PrefabTypeCount-1; ++i)
	{
		pMapFactory->Register(ObjectCategory::PREFAB, i, _func);
	}
#pragma endregion
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
	CCullingManager::GetInstance()->DestroyInstance();
	CComponentMgr::GetInstance()->DestroyInstance();
	Engine::Safe_Release(m_pRenderer);
	Engine::Safe_Release(m_pDeviceClass);
	Engine::Safe_Release(m_pGraphicDev);
	CColiderManager::GetInstance()->DestroyInstance();
	CSound_Manager::GetInstance()->DestroyInstance();
	CPicking::GetInstance()->DestroyInstance();
	CUIManager::GetInstance()->DestroyInstance();
	CManagement::GetInstance()->DestroyInstance();
	CObjectManager::GetInstance()->DestroyInstance();
	CObjectPoolManager::GetInstance()->DestroyInstance();
	CGameDataManager::GetInstance()->DestroyInstance();
	CFontMgr::GetInstance()->DestroyInstance();
	CTimerMgr::GetInstance()->DestroyInstance();
	CFrameMgr::GetInstance()->DestroyInstance();
	CFileManager::GetInstance()->DestroyInstance();
	CDataManager::GetInstance()->DestroyInstance();
	CMapFactory::GetInstance()->DestroyInstance();
	CDInputMgr::GetInstance()->DestroyInstance();
	CPickingManager::GetInstance()->DestroyInstance();
	m_pDeviceClass->DestroyInstance();
}
