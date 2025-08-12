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
#include "CHpBarUI.h"
#include "CPlayer_HandR.h"
#include "CPlayer_HandL.h"
#include "CPlayer_Foot.h"
#include "CPlayer_Arm.h"
#include "CPistol_Gun.h"
 
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

	EnterCriticalSection(pLoader->Get_Crt());

	switch (pLoader->Get_NextSceneID())
	{
	case SCENE_LOGO:
		pLoader->Loading_Logo();
		break;
	case SCENE_STAGE:
		pLoader->Loading_ForStage();
		break;
	}

	LeaveCriticalSection(pLoader->Get_Crt());
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

#pragma region 몬스터 테스트
	// Monster
	// IDLE
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_Monster_Suit_Idle",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/idle/sm_idle%03d.png", 12))))
		return E_FAIL;

	// AIM
	//if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_Monster_Suit_Aim",
	//	CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Test/aim%03d.png", 9))))
	//	return E_FAIL;
#pragma endregion 몬스터 테스트

#pragma region Weapon texture
	// Pistol
	// Pistol - Idle
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_WapPistol_Idle",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/idle/PISTOL_IDLE%03d.png", 3))))
		return E_FAIL;
	// Pistol - Op
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_WapPistol_Op",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/opening/PISTOL_OPENING%03d.png", 18))))
		return E_FAIL;
	// Pistol - Attack
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_WapPistol_Attack",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/attack/PISTOL_SHOT%03d.png", 6))))
		return E_FAIL;
	// Pistol - Reload
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_WapPistol_Re",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/reload/PISTOL_RELOAD%03d.png", 13))))
		return E_FAIL;
	// PistolC - Idle
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_WapPistol_Car_Idle",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/car/idle/PISTOL2_IDLE%03d.png", 3))))
		return E_FAIL;
	//PistolC - Attack
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_WapPistol_Car_Attack",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/car/attack/PISTOL2_ATTACK%03d.png", 5))))
		return E_FAIL;
	// PistolC - Zooming
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_WapPistol_Car_Zooming",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/car/zooming/pistol_zooming%03d.png", 3))))
		return E_FAIL;
	// PistolC - Zoom_Idle
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_WapPistol_Car_ZoomIdle",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/car/zoom/idle/PISTOL_ZOOM_IDLE%03d.png", 4))))
		return E_FAIL;
	// PistolC - Zoom_Attack
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_WapPistol_Car_ZoomAtt",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/car/zoom/attack/PISTOL_ZOOM_ATTACK%03d.png", 3))))
		return E_FAIL;


#pragma endregion Weapon texture

#pragma region UI Texture
	// Player UI

	// Arm
	// openig1 - weapon : non
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIArmOp1",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/arm/glove/OPENING_GLOVE%03d.png", 3))))
		return E_FAIL;
	// openig2 - weapon : non
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIArmOp2",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/arm/melee/OPENING_MELEE%03d.png", 3))))
		return E_FAIL;

	// foot
	// foot - kick
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIFootKick",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/foot/kick/foot%03d.png", 2))))
		return E_FAIL;
	// foot - slide
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIFootSlide",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/foot/slide/slide000.png", 1))))
		return E_FAIL;
	
	//	HandL
	//	HandL - Idle
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIHandLIdle",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/left/idle/IDLE%03d.png", 17))))
		return E_FAIL;
	//	HandL - Doping
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIHandLDoping",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/left/doping/DRINK%03d.png", 7))))
		return E_FAIL;
	//	HandL - opening : rifle
	//C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\player\left\opening\rifle
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIHandLOpRif",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/left/opening/rifle/Lhand_opeing_rifle%03d.png", 12))))
		return E_FAIL;
	// HandL - attack_instance - knife
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIHandLAt2Knife",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/left/attack2/punch%03d.png", 3))))
		return E_FAIL;
	// HandL - reload - pistol
	//C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\player\left\reload\pistol\END
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIHandLRePistol",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/left/reload/pistol/END/PistolR_L%03d.png", 3))))
		return E_FAIL;
	// HandL - reload - shotgun
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIHandLReShotgun",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/left/reload/shotgun/ShotR_Left%03d.png", 2))))
		return E_FAIL;

	// HandR
	// // HandR - Idle
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIHandRIdle",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/right/idle/HandR_Idle%03d.png", 3))))
		return E_FAIL;
	// HandR - attack_instance - knife
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIHandRAt2Knife",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/right/attack2/knife/A2_knife000.png", 1))))
		return E_FAIL;
	// HandR - Dead
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIHandRDead",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/right/dead/DEATH%03d.png", 3))))
		return E_FAIL;
	// handr - op - pistol
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIHandROpPistol",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/right/opening/pistol/Pistol_OpR000.png", 1))))
		return E_FAIL;
	// handr - op - shotgun
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIHandROpShotgun",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/right/opening/shotGun/ShotG_OpR%03d.png", 4))))
		return E_FAIL;

	// HpBar
	// HpB - PhoneN
	//C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\UI\UI_HpBar\Phone
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIHpBarPhoneN",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/UI_HpBar/Phone/PhoneN%03d.png", 4))))
		return E_FAIL;
	// HpB - PhoneB
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIHpBarPhoneB",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/UI_HpBar/Phone/PhoneB%03d.png", 4))))
		return E_FAIL;
	// HpB - PhoneF
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIHpBarPhoneF",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/UI_HpBar/Phone/PhoneF%03d.png", 4))))
		return E_FAIL;
	// HpB - ManN
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIHpBarManN",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/UI_HpBar/Man/normal/Jack_Nomal_%03d.png", 16))))
		return E_FAIL;
	// HpB - ManH
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIHpBarManH",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/UI_HpBar/Man/hit/Jack_Good_%03d.png", 16))))
		return E_FAIL;
	// HpB - ManD
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIHpBarManD",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/UI_HpBar/Man/dead/Jack_Hurt_%03d.png", 16))))
		return E_FAIL;
	// HpB - Crack
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STAGE, L"Prototype_Component_Texture_UIHpBarCrack",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/UI_HpBar/crack/Phone_Crack000.png", 1))))
		return E_FAIL;


#pragma endregion UI

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

	// Camera_FPS
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

	// Player UI
	// HandR UI 생성
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_PlayerHandRUI",
		CPlayer_HandR::Create(m_pGraphicDev))))
		return E_FAIL;
	// HandL UI 생성
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_PlayerHandLUI",
		CPlayer_HandL::Create(m_pGraphicDev))))
		return E_FAIL;
	// Foot UI 생성
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_PlayerFootUI",
		CPlayer_Foot::Create(m_pGraphicDev))))
		return E_FAIL;
	// Arm UI 생성
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_PlayerArmUI",
		CPlayer_Arm::Create(m_pGraphicDev))))
		return E_FAIL;

	// Weapon UI
	// Pistol
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_GunPistolUI",
		CPistol_Gun::Create(m_pGraphicDev))))
		return E_FAIL;

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


