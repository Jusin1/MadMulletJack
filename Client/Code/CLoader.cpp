#include "pch.h"
#include "CLoader.h"
#include "CObjectManager.h"
#include "CComponentMgr.h"


// TEST
#include "CBackGround.h"
#include "CPlayer.h"
#include "CTerrain.h"
#include "CDynamicCamera.h"
#include "CSkyBox.h"
#include "CUIBase.h"
#include "CCameraFPS.h"

// UI
#include "CPlayer_HandR.h"
#include "CPlayer_HandL.h"
#include "CEffectUI.h"


// UI - 게임진입 UI
#include "CHeartUI.h"
#include "CLisaUI.h"


// 몬스터
#include "CMonster_Suit.h"


CLoader::CLoader(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphicDev(pGraphic_Device)
{
	m_pGraphicDev->AddRef();
	ZeroMemory(m_szLoading, sizeof(m_szLoading));
}

CLoader::~CLoader()
{
}

unsigned int APIENTRY Thread_Main(void* pArg)
{
	CLoader* pLoader = (CLoader*)pArg;

	switch (pLoader->Get_NextSceneID())
	{
	case SCENE_LOGO:  pLoader->Loading_Logo();  break;
	case SCENE_STAGE: pLoader->Loading_ForStage(); break;
	}
	return 0;
}

HRESULT CLoader::Ready_Loading(SCENE eNextScene)
{
	m_eNextScene = eNextScene;

	InitializeCriticalSection(&m_Crt);

	m_hThread = (HANDLE)_beginthreadex(NULL, // 보안 속성(기본 값) : 핸들의 상속 여부, null이면 상속에서 제외
		0,	 // 디폴트 스택 사이즈(1바이트)
		Thread_Main,	 // 쓰레드를 이용하여 동작시킬 함수(__stdcall 규약)
		this, // 쓰레드 함수의 매개 변수로 전달할 데이터 주소
		0,	  // 쓰레드의 생성 및 실행을 조절하기 위한 flag
		NULL); // 쓰레드 ID 반환
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_Logo()
{
	// 텍스쳐 로딩
	lstrcpy(m_szLoading, L"텍스쳐 로딩 중");
	// BackGround
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_LOGO, L"Prototype_Component_Texture_BackGround",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Test/BACK.png", 1))))
		return E_FAIL;

	// 객체 생성 중
	lstrcpy(m_szLoading, L"객체 생성 중.");
	// BackGround
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_BackGround",
		CBackGround::Create(m_pGraphicDev))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Loading_ForStage()
{
	lstrcpy(m_szLoading, L"텍스쳐 로딩 중");
	// Terriann
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_Terrian",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/Terrain0.png", 1))))
		return E_FAIL;

	// SkyBox
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_SkyBox",
		CTexture::Create(m_pGraphicDev, TEX_CUBE, L"../Bin/Resource/Texture/SkyBox/burger3.dds", 4))))
		return E_FAIL;
#pragma region 플레이어 테스트
	// Player
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_PlayerTest",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Test/aim%03d.png", 9))))
		return E_FAIL;
#pragma endregion 플레이어 테스트

#pragma region 슈트 몬스터
	// Monster
	// IDLE
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_Monster_Suit_Idle",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/idle/sm_idle%03d.png", 12))))
		return E_FAIL;

	// CHASE
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_Monster_Suit_Chase",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/chasing/SM_CHASING%03d.png", 12))))
		return E_FAIL;

	// AIM
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_Monster_Suit_Aim",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/Aim/aim%03d.png", 9))))
		return E_FAIL;

	// SHOT
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_Monster_Suit_Shot",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/Shot/shoot%02d.png", 8))))
		return E_FAIL;

	// Jump
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_Monster_Suit_Jump",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/jump/jumping/sm_jump%03d.png", 22))))
		return E_FAIL;

	// HEAD_HIT
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_Monster_Suit_HIT_HEAD",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/hit_gun/head/normal/flyb_death%02d.png", 21))))
		return E_FAIL;

	// BODY_HIT
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_Monster_Suit_HIT_BODY",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/hit_gun/hit/SM_HIT%03d.png", 8))))
		return E_FAIL;
	
	// BALL_HIT
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_Monster_Suit_HIT_BALL",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/hit_gun/ball/SM_HIT_BALL%03d.png", 23))))
		return E_FAIL;

	// DEATH
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_Monster_Suit_DEATH1",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/death/fb_death%02d.png", 21))))
		return E_FAIL;

#pragma endregion 슈트 몬스터

#pragma region UI 테스트
	// Player UI
	// Idle
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIHandIdle",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Test/HAND_IDLE%03d.png", 9))))
		return E_FAIL;

	// Shot
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIHandShot",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Test/PISTOL_SHOT%03d.png", 6))))
		return E_FAIL;

#pragma endregion UI 테스트


#pragma region 일반 UI
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_HealEffect",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/LIKES.png", 1))))
		return E_FAIL;
#pragma endregion 일반UI

#pragma region 게임 진입 UI
	// HEART
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_HeartUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/HEART%03d.png", 5))))
		return E_FAIL;
	// HEART LINE
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_HeartUI_LINE",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/HEART LINE.png", 1))))
		return E_FAIL;

	// LISA UI
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_LisaUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Lisa/Default/Lisa_Default_%03d.png", 12))))
		return E_FAIL;
#pragma endregion 게임 진입 UI

	// 객체 생성
	lstrcpy(m_szLoading, L"객체 생성 중.");
	//// Terrian
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_Terrian",
		CTerrain::Create(m_pGraphicDev))))
		return E_FAIL;
	// SkyBox
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_SkyBox",
		CSkyBox::Create(m_pGraphicDev))))
		return E_FAIL;

	//// Camera_Dynamic
	//if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_Camera_Dynamic",
	//	CDynamicCamera::Create(m_pGraphicDev))))
	//	return E_FAIL;

	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_Camera_FPS",
		CCameraFPS::Create(m_pGraphicDev))))
		return E_FAIL;


	// Player
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_Player",
		CPlayer::Create(m_pGraphicDev))))
		return E_FAIL;

	//// Monster
	// Monster_Suit 생성
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_Monster_Suit",
		CMonster_Suit::Create(m_pGraphicDev))))
		return E_FAIL;

	// UI
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_UIRoot",
		CUIBase::Create(m_pGraphicDev))))
		return E_FAIL;

	// 손 UI 생성
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_PlayerHandUI",
		CPlayer_HandR::Create(m_pGraphicDev))))
		return E_FAIL;

	// 손 UI 생성
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_PlayerHandLUI",
		CPlayer_HandL::Create(m_pGraphicDev))))
		return E_FAIL;

	// 몬스터 피격 이펙트
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_MonsterHitEffectUI",
		CEffectUI::Create(m_pGraphicDev))))
		return E_FAIL;

#pragma region 게임 진입 UI들 생성

	// Heart UI
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_HeartUI",
		CHeartUI::Create(m_pGraphicDev))))
		return E_FAIL;

	// Lisa UI
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_LisaUI",
		CLisaUI::Create(m_pGraphicDev))))
		return E_FAIL;


#pragma endregion 게임 진입 UI들 생성

	lstrcpy(m_szLoading, TEXT("모델 로딩 중."));
	// TerrianTex
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_LOADING, L"Proto_TerrianBuffer", Engine::CVIBuffer_Terrian::Create(m_pGraphicDev))))
		return E_FAIL;

	// CubeTex
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_LOADING, L"Proto_CubeBuffer", Engine::VIBuffer_Cube::Create(m_pGraphicDev))))
		return E_FAIL;


	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;
	
	return S_OK;
}

CLoader* CLoader::Create(LPDIRECT3DDEVICE9 pGrahpicDev, SCENE eNextScene)
{
	CLoader* pInstance = new CLoader(pGrahpicDev);
	if (FAILED(pInstance->Ready_Loading(eNextScene)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_Crt);
	Safe_Release(m_pGraphicDev);
}


