#include "pch.h"
#include "CLoader.h"
#include "CObjectManager.h"
#include "CComponentMgr.h"


// TEST
#include "CBackGround.h"
#include "CPlayer.h"
#include "CDynamicCamera.h"
#include "CSkyBox.h"
#include "CUIBase.h"
#include "CCameraFPS.h"

// UI
// Player UI
#include "CPlayer_HandR.h"
#include "CPlayer_HandL.h"
#include "CPlayer_Foot.h"
#include "CPlayer_Arm.h"
#include "CPlayerUI_Manager.h"

// Weapon UI
#include "CPistol_Gun.h"
#include "CSniper_Gun.h"
#include "CKnife_SubW.h"
#include "CKatana.h"
#include "CShot_Gun.h"
#include "CWeaponUI_Manager.h"

// Hpbar UI
#include "CHpbarUI.h"
#include "CMan_HpBarUI.h"
#include "CPhone_HpBarUI.h"

// Effect UI
#include "CEffectUI.h"

#pragma region 게임 진입 UI들
#include "CHeartUI.h"
#include "CLisaUI.h"
#include "CPanelUI.h"
#include "CBlackGackGround.h"
#include "CChatUI.h"
#include "CBannerUI.h"
#include "CImageUI.h"
#include "CTextUI.h"
#include "CTalkUI.h"
#include "CPhoneUI.h"
#include "CButtonUI.h"
#include "CItemUI.h"
#include "CTextEffectUI.h"
#pragma endregion 게임 진입 UI들

// 튜토리얼 UI
#include "CTutorialUI.h"
 
// 몬스터
#include "CMonster_Suit.h"
#include "CMonster_Fat.h"
#include "CMonster_Soldier.h"
#include "CMonster_Head.h"
#include "CMonster_Dron.h"

// 총알
#include "CBullet.h"

// 이펙트
#include "CEffect_Pixel.h"
#include "CEffect_World.h"
#include "CEffect_Pixel_Sprite.h"

// MapObject
#include "CGridPanel.h"
#include "CTile_Acid.h"
#include "CTile_Deco.h"
#include "CTile_Glass.h"
#include "CTile_Electric.h"
#include "CTile_Vent.h"
#include "CEndPoint.h"
#include "CTile_NormalDoor.h"
#include "CTile_OpeningDoor.h"
#include "CTile_VendingMachine.h"
#include "CTile_Bottle.h"
#include "CPrefab.h"

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
	case SCENE_DEV:
	{
		pLoader->Loading_UI(); 
		pLoader->Loading_Dev();
	}break;
	case SCENE_TUTORIAL:
	{
		pLoader->Loading_Tutorial();
	} break;
	case SCENE_STAGE_1:
	{
		pLoader->Loading_Stage_1();
	} break;
	case SCENE_STAGE_2:
	{
		pLoader->Loading_Stage_2();
	} break;
	case SCENE_SNIPE:
	{
		pLoader->Loading_Snipe();
	} break;
	case SCENE_BOSS:
	{
		pLoader->Loading_Rooftop();
	} break;
	case SCENE_CAR:
	{
		pLoader->Loading_Road();
	} break;
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
	PhaseBegin(0.f, 1.f, L"로고 리소스 로딩");
	// 텍스쳐 로딩
	PhaseStep(0.15f, L"텍스쳐 로딩 중");
	// LogoBackGround
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_LoadingBG",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI_Loading/REJECT%03d.png", 3))))
		return E_FAIL;
	// BackGround
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_LOGO, L"Prototype_Component_Texture_BackGround",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI_Logo/BackGround.png", 1))))
		return E_FAIL;

	// Logo
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Logo",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI_Logo/LOGO_%03d.png", 20))))
		return E_FAIL;

	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_LogoButton",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI_Logo/MAIN MENU WHITE START.png", 1))))
		return E_FAIL;

	// 객체 생성 중
	PhaseStep(0.55f, L"객체 프로토타입 생성 중");
	// BackGround
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_BackGround",
		CBackGround::Create(m_pGraphicDev))))
		return E_FAIL;

	// ImageUI
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_UIImage",
		CImageUI::Create(m_pGraphicDev))))
		return E_FAIL;

	// TextUI
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_TextUI",
		CTextUI::Create(m_pGraphicDev))))
		return E_FAIL;

	// ButtonUI
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_UIButton",
		CButtonUI::Create(m_pGraphicDev))))
		return E_FAIL;
	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다."));

	PhaseStep(1.f, L"객체 프로토타입 생성 중");
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_Dev()
{
	PhaseBegin(0.5f, 0.5f, L"게임 플레이 자원 로딩");

#pragma region 튜토리얼 UI
	// WASD Tutorial
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Tut_Move",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/WASD.png", 1))))
		return E_FAIL;

	// SHOT Tutorial
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Tut_Shot",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/SHOT.png", 1))))
		return E_FAIL;

	// JUMP Tutorial
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Tut_Jump",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/Jump.png", 1))))
		return E_FAIL;

	// DASH Tutorial
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Tut_Dash",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/DASH.png", 1))))
		return E_FAIL;

	// DOOR Tutorial
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Tut_Door",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/DOOR.png", 1))))
		return E_FAIL;

	// Finish Tutorial
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Tut_Finish",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/FINISH TUTORIAL.png", 1))))
		return E_FAIL;

	// Soda Tutorial
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Tut_Soda",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/GET A LIFE.png", 1))))
		return E_FAIL;


	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_TutorialUI",
		CTutorialUI::Create(m_pGraphicDev))))
		return E_FAIL;

#pragma endregion 튜토리얼 UI

#pragma region 맵오브젝트 임시
	PhaseStep(0.15f, L"맵 오브젝트 텍스쳐");
	if (FAILED(Loading_MapObjectTexture_Src()))
		return E_FAIL;
#pragma endregion

	// 객체 생성
	lstrcpy(m_szLoading, L"객체 생성 중.");

#pragma region 슈트 몬스터
	// Monster
	PhaseStep(0.3f, L"몬스터 텍스쳐");
	// IDLE
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Suit_Idle",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/idle/sm_idle%03d.png", 12))))
		return E_FAIL;

	// CHASE
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Suit_Chase",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/chasing/SM_CHASING%03d.png", 12))))
		return E_FAIL;

	// AIM
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Suit_Aim",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/Aim/aim%03d.png", 9))))
		return E_FAIL;

	// SHOT
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Suit_Shot",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/Shot/shoot%02d.png", 8))))
		return E_FAIL;

	// Jump
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Suit_Jump",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/jump/jumping/sm_jump%03d.png", 22))))
		return E_FAIL;

	// HEAD_HIT
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Suit_HIT_HEAD",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/hit_gun/head/normal/flyb_death%02d.png", 21))))
		return E_FAIL;

	// BODY_HIT
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Suit_HIT_BODY",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/hit_gun/hit/SM_HIT%03d.png", 8))))
		return E_FAIL;

	// BALL_HIT
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Suit_HIT_BALL",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/hit_gun/ball/SM_HIT_BALL%03d.png", 23))))
		return E_FAIL;

	// ELECTRIC - HIT
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Suit_HIT_ELECTRIC",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/hit_environ/electric/SM_HIT_ELEC%03d.png", 15))))
		return E_FAIL;

	// VENT - HIT
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Suit_HIT_VENT",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/death/Blood/BLOOD%03d.png", 4))))
		return E_FAIL;

	// DOOR - HIT
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Suit_HIT_DOOR",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/hit_environ/wall/SM_HIT_WALL%03d.png", 14))))
		return E_FAIL;

	// DEATH
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Suit_DEATH1",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/death/fb_death%02d.png", 21))))
		return E_FAIL;

	// Blocking -> Kicked
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Suit_Blocking",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/blocking/blocking%03d.png", 4))))
		return E_FAIL;

	// KATANA - HIT - BODY
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Suit_Katana_Body",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/Katana/Body/BODY%03d.png", 20))))
		return E_FAIL;

	// KATANA - HIT - HEAD
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Suit_Katana_HEAD",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/Katana/Head/HEAD%03d.png", 35))))
		return E_FAIL;

	// InstanceKill
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Suit_InstanceKill",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/instanceKill/sm_IKill%03d.png", 13))))
		return E_FAIL;

#pragma endregion 슈트 몬스터

#pragma region 뚱뚱한 몬스터
	PhaseStep(0.4f, L"몬스터 텍스쳐");

	// Monster
	// IDLE
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Fat_Idle",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Monster/Monster_2/Idle/fg_idle%02d.png", 12))))
		return E_FAIL;

	// CHASE
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Fat_Chase",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Monster/Monster_2/Walk/fg_walk%02d.png", 14))))
		return E_FAIL;

	// AIM
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Fat_Aim",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Monster/Monster_2/aim/fg_aim%02d.png", 9))))
		return E_FAIL;

	// SHOT
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Fat_Shot",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Monster/Monster_2/shoot/fg_shoot%02d.png", 7))))
		return E_FAIL;

	// Jump
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Fat_Jump",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Monster/Monster_2/jump/ft_jumping%02d.png", 21))))
		return E_FAIL;

	// HEAD_HIT
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Fat_HIT_HEAD",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Monster/Monster_2/Hit/HeadHit/fg_2fly%02d.png", 21))))
		return E_FAIL;

	// BODY_HIT
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Fat_HIT_BODY",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Monster/Monster_2/Hit/Hit2/fg_hit%02d.png", 8))))
		return E_FAIL;

	// BALL_HIT
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Fat_HIT_BALL",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Monster/Monster_2/Hit/HitBall/BALLSHOT%03d.png", 23))))
		return E_FAIL;

	// ELECTRIC - HIT
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Fat_HIT_ELECTRIC",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Monster/Monster_2/Hit/Electric/ELECTRIC%03d.png", 14))))
		return E_FAIL;

	// DOOR - HIT
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Fat_HIT_DOOR",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Monster/Monster_2/Fly_back_Endo/fat_Flyback_endo%02d.png", 31))))
		return E_FAIL;

	// DEATH
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Fat_HIT_DEATH",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Monster/Monster_2/Death/fg_death%02d.png", 20))))
		return E_FAIL;


	// Blocking -> Kicked
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Fat_HIT_BLOCK",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Monster/Monster_2/Blocking/Fat_blocking%02d.png", 5))))
		return E_FAIL;

	// Katana -> Body - Death
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Fat_KATANA_BODY",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Monster/Monster_2/Katana/Body/BODY%03d.png", 21))))
		return E_FAIL;

	// Katana -> Head - Death
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Fat_KATANA_HEAD",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Monster/Monster_2/Katana/Head/HEAD%03d.png", 40))))
		return E_FAIL;

	// 추가 예정
	// InstanceKill
	//if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Suit_InstanceKill",
	//	CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/instanceKill/sm_IKill%03d.png", 13))))
	//	return E_FAIL;

#pragma endregion 뚱뚱한 몬스터

#pragma region 드론 몬스터
	// IDLE
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Drone_IDLE",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Monster/Drone/IDLE/Air_brain%02d.png", 6))))
		return E_FAIL;
	// WAKE
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Drone_WAKE",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Monster/Drone/WAKE/air_reveal%02d.png", 6))))
		return E_FAIL;
	// ATTACK
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Drone_ATTACK",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Monster/Drone/Attack/AIRATTACK%03d.png", 10))))
		return E_FAIL;

	// KATANA - DEATH
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Drone_KATANA_DEATH",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Monster/Drone/KATANA/drone_flyback%02d.png", 11))))
		return E_FAIL;

	// DEATH
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Drone_DEATH",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Monster/Drone/Death/drone_flyback%02d.png", 4))))
		return E_FAIL;
#pragma endregion 드론 몬스터
	PhaseStep(0.6f, L"최종 프로토타입 생성");
	// 총알
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Bullet",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Bullet/BULLET%03d.png", 4))))
		return E_FAIL;

#pragma region MapObject
	PhaseStep(0.92f, L"최종 프로토타입 생성");
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_DefaultPanel",
		CGridPanel::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_DecoTile",
		CTile_Deco::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_GlassTile",
		CTile_Glass::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_AcidTile",
		CTile_Acid::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_ElectricTile",
		CTile_Electric::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_NormalDoorTile",
		CTile_NormalDoor::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_VentTile",
		CTile_Vent::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_VendingMachine",
		CTile_VendingMachine::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_Bottle",
		CTile_Bottle::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_EndPoint",
		CEndPoint::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_DefaultPrefab",
		CPrefab::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Proto_PixelEffect",
		CEffect_Pixel::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Proto_EffectWorld",
		CEffect_World::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Proto_PixelEffect_Sprite",
		CEffect_Pixel_Sprite::Create(m_pGraphicDev))))
		return E_FAIL;
#pragma endregion

	// 총알 생성
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_Bullet",
		CBullet::Create(m_pGraphicDev))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("모델 로딩 중."));
	// CubeTex
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_CubeBuffer", Engine::VIBuffer_Cube::Create(m_pGraphicDev))))
		return E_FAIL;

	PhaseStep(1.0f, L"DEV 로딩 완료");
	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다."));

	PhaseDone(L"DEV 로딩 완료");
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_Tutorial()
{

	//// WASD Tutorial
	//if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Tut_Move",
	//	CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/WASD.png", 1))))
	//	return E_FAIL;

	//// SHOT Tutorial
	//if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Tut_Shot",
	//	CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/SHOT.png", 1))))
	//	return E_FAIL;

	//if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_TutorialUI",
	//	CTutorialUI::Create(m_pGraphicDev))))
	//	return E_FAIL;

	


	m_isFinished = true;
	lstrcpy(m_szLoading, TEXT("튜토리얼 로딩이 완료되었습니다."));
	return S_OK;
}

HRESULT CLoader::Loading_Stage_1()
{
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_Stage_2()
{
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_Snipe()
{
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_Rooftop()
{
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_Road()
{
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

// 0818 - 강병준
// TODO - 추후 사용하는 텍스쳐만 로드하도록 분할 예정
HRESULT CLoader::Loading_MapObjectTexture_Src()
{
	CComponentMgr *pCompMgr = CComponentMgr::GetInstance();
#define AddTexture(CompName, Path, Count) if(FAILED(pCompMgr->Add_Prototype(SCENE_STATIC, CompName,	\
												CTexture::Create(m_pGraphicDev, TEX_NORMAL, Path, Count))))	\
												return E_FAIL

	AddTexture(L"Proto_GridDefault", L"../Bin/Resource/MapObject/Grid/GridBox_Default.png", 1);
	AddTexture(L"Proto_GridTrigger", L"../Bin/Resource/MapObject/Grid/GridBox_Trigger.png", 1);
	AddTexture(L"Proto_GridNoDraw", L"../Bin/Resource/MapObject/Grid/GridBox_NoDraw.png", 1);
	AddTexture(L"Proto_GridCollider", L"../Bin/Resource/MapObject/Grid/GridBox_Collider.png", 1);
	AddTexture(L"Proto_Floor_1", L"../Bin/Resource/MapObject/Floor/FLOOR 1.png", 1);
	AddTexture(L"Proto_Floor_2", L"../Bin/Resource/MapObject/Floor/FLOOR 2.png", 1);
	AddTexture(L"Proto_Floor_3", L"../Bin/Resource/MapObject/Floor/FLOOR 3.png", 1);
	AddTexture(L"Proto_Floor_4", L"../Bin/Resource/MapObject/Floor/FLOOR 4.png", 1);
	AddTexture(L"Proto_Floor_5", L"../Bin/Resource/MapObject/Floor/FLOOR 5.png", 1);
	AddTexture(L"Proto_Floor_6", L"../Bin/Resource/MapObject/Floor/FLOOR 6.png", 1);
	AddTexture(L"Proto_Floor_7", L"../Bin/Resource/MapObject/Floor/FLOOR 7.png", 1);
	AddTexture(L"Proto_Acid_Floor_1", L"../Bin/Resource/MapObject/Acid/ACID A.png", 1);
	AddTexture(L"Proto_Acid_Floor_2", L"../Bin/Resource/MapObject/Acid/ACID D.png", 1);
	AddTexture(L"Proto_Acid_Wall_1", L"../Bin/Resource/MapObject/Acid/ACID B.png", 1);
	AddTexture(L"Proto_Acid_Wall_2", L"../Bin/Resource/MapObject/Acid/ACID C.png", 1);
	AddTexture(L"Proto_Concrete_Wall", L"../Bin/Resource/MapObject/Wall/CONCRETE WALL.png", 1);
	AddTexture(L"Proto_Corner_Wall_1", L"../Bin/Resource/MapObject/Wall/WALL 1 CORNER.png", 1);
	AddTexture(L"Proto_Corner_Wall_2", L"../Bin/Resource/MapObject/Wall/WALL 2 CORNER.png", 1);
	AddTexture(L"Proto_Corner_Wall_3", L"../Bin/Resource/MapObject/Wall/WALL 3 CORNER.png", 1);
	AddTexture(L"Proto_Corner_Wall_4", L"../Bin/Resource/MapObject/Wall/WALL 4 CORNER.png", 1);
	AddTexture(L"Proto_Corner_Wall_5_1", L"../Bin/Resource/MapObject/Wall/WALL 5 CORNER_1.png", 1);
	AddTexture(L"Proto_Corner_Wall_5_2", L"../Bin/Resource/MapObject/Wall/WALL 5 CORNER_2.png", 1);
	AddTexture(L"Proto_Corner_Wall_6", L"../Bin/Resource/MapObject/Wall/WALL 6 CORNER.png", 1);
	AddTexture(L"Proto_Corner_Wall_7", L"../Bin/Resource/MapObject/Wall/WALL 7 CORNER.png", 1);
	AddTexture(L"Proto_Corner_Wall_8", L"../Bin/Resource/MapObject/Wall/WALL 8 CORNER.png", 1);
	AddTexture(L"Proto_Corner_Wall_Boss", L"../Bin/Resource/MapObject/Wall/WALL BOSS 3 CORNER.png", 1);
	AddTexture(L"Proto_Wall_1A", L"../Bin/Resource/MapObject/Wall/WALL 1A.png", 1);
	AddTexture(L"Proto_Wall_1B", L"../Bin/Resource/MapObject/Wall/WALL 1B.png", 1);
	AddTexture(L"Proto_Wall_1C", L"../Bin/Resource/MapObject/Wall/WALL 1C.png", 1);
	AddTexture(L"Proto_Wall_2A", L"../Bin/Resource/MapObject/Wall/WALL 2A.png", 1);
	AddTexture(L"Proto_Wall_2B", L"../Bin/Resource/MapObject/Wall/WALL 2B.png", 1);
	AddTexture(L"Proto_Wall_2C", L"../Bin/Resource/MapObject/Wall/WALL 2C.png", 1);
	AddTexture(L"Proto_Wall_3A_1", L"../Bin/Resource/MapObject/Wall/WALL 3A_1.png", 1);
	AddTexture(L"Proto_Wall_3A_2", L"../Bin/Resource/MapObject/Wall/WALL 3A_2.png", 1);
	AddTexture(L"Proto_Wall_3B_1", L"../Bin/Resource/MapObject/Wall/WALL 3B_1.png", 1);
	AddTexture(L"Proto_Wall_3B_2", L"../Bin/Resource/MapObject/Wall/WALL 3B_2.png", 1);
	AddTexture(L"Proto_Wall_3C_1", L"../Bin/Resource/MapObject/Wall/WALL 3C_1.png", 1);
	AddTexture(L"Proto_Wall_3C_2", L"../Bin/Resource/MapObject/Wall/WALL 3C_2.png", 1);
	AddTexture(L"Proto_Wall_4A", L"../Bin/Resource/MapObject/Wall/WALL 4A.png", 1);
	AddTexture(L"Proto_Wall_4B", L"../Bin/Resource/MapObject/Wall/WALL 4B.png", 1);
	AddTexture(L"Proto_Wall_4C_1", L"../Bin/Resource/MapObject/Wall/WALL 4C_1.png", 1);
	AddTexture(L"Proto_Wall_4C_2", L"../Bin/Resource/MapObject/Wall/WALL 4C_2.png", 1);
	AddTexture(L"Proto_Wall_5A", L"../Bin/Resource/MapObject/Wall/WALL 5A.png", 1);
	AddTexture(L"Proto_Wall_5B", L"../Bin/Resource/MapObject/Wall/WALL 5B.png", 1);
	AddTexture(L"Proto_Wall_5C", L"../Bin/Resource/MapObject/Wall/WALL 5C.png", 1);
	AddTexture(L"Proto_Wall_6A", L"../Bin/Resource/MapObject/Wall/WALL 6A.png", 1);
	AddTexture(L"Proto_Wall_6B", L"../Bin/Resource/MapObject/Wall/WALL 6B.png", 1);
	AddTexture(L"Proto_Wall_6C", L"../Bin/Resource/MapObject/Wall/WALL 6C.png", 1);
	AddTexture(L"Proto_Wall_7A", L"../Bin/Resource/MapObject/Wall/WALL 7A.png", 1);
	AddTexture(L"Proto_Wall_7B", L"../Bin/Resource/MapObject/Wall/WALL 7B.png", 1);
	AddTexture(L"Proto_Wall_7C", L"../Bin/Resource/MapObject/Wall/WALL 7C.png", 1);
	AddTexture(L"Proto_Wall_8A", L"../Bin/Resource/MapObject/Wall/WALL 8A.png", 1);
	AddTexture(L"Proto_Wall_8B", L"../Bin/Resource/MapObject/Wall/WALL 8B.png", 1);
	AddTexture(L"Proto_Wall_8C", L"../Bin/Resource/MapObject/Wall/WALL 8C.png", 1);
	AddTexture(L"Proto_Wall_9A", L"../Bin/Resource/MapObject/Wall/WALL 9A.png", 1);
	AddTexture(L"Proto_Wall_9B", L"../Bin/Resource/MapObject/Wall/WALL 9B.png", 1);
	AddTexture(L"Proto_Wall_9C", L"../Bin/Resource/MapObject/Wall/WALL 9C.png", 1);
	AddTexture(L"Proto_Wall_10A", L"../Bin/Resource/MapObject/Wall/WALL 10A.png", 1);
	AddTexture(L"Proto_Wall_10B", L"../Bin/Resource/MapObject/Wall/WALL 10B.png", 1);
	AddTexture(L"Proto_Wall_10C", L"../Bin/Resource/MapObject/Wall/WALL 10C.png", 1);
	AddTexture(L"Proto_Wall_Deco_1", L"../Bin/Resource/MapObject/Wall/WALL DECO 1.png", 1);
	AddTexture(L"Proto_Wall_Deco_2", L"../Bin/Resource/MapObject/Wall/WALL DECO 2.png", 1);
	AddTexture(L"Proto_Wall_Deco_3", L"../Bin/Resource/MapObject/Wall/WALL DECO 3.png", 1);
	AddTexture(L"Proto_Wall_Deco_4", L"../Bin/Resource/MapObject/Wall/WALL DECO 4.png", 1);
	AddTexture(L"Proto_Wall_Deco_5", L"../Bin/Resource/MapObject/Wall/WALL DECO 5.png", 1);
	AddTexture(L"Proto_Wall_Deco_6", L"../Bin/Resource/MapObject/Wall/WALL DECO 6.png", 1);
	AddTexture(L"Proto_Wall_Deco_7", L"../Bin/Resource/MapObject/Wall/WALL DECO 7.png", 1);
	AddTexture(L"Proto_Wall_Deco_8", L"../Bin/Resource/MapObject/Wall/WALL DECO 8.png", 1);
	AddTexture(L"Proto_Wall_Hole_1", L"../Bin/Resource/MapObject/Wall/WALL HOLE 1.png", 1);
	AddTexture(L"Proto_Wall_Hole_2", L"../Bin/Resource/MapObject/Wall/WALL HOLE 2.png", 1);
	AddTexture(L"Proto_Wall_Hole_3", L"../Bin/Resource/MapObject/Wall/WALL HOLE 3.png", 1);
	AddTexture(L"Proto_Wall_Hole_4", L"../Bin/Resource/MapObject/Wall/WALL HOLE 4.png", 1);
	AddTexture(L"Proto_Wall_Hole_4_B", L"../Bin/Resource/MapObject/Wall/WALL HOLE 4_B.png", 1);
	AddTexture(L"Proto_Wall_Boss_1", L"../Bin/Resource/MapObject/Wall/WALL boss 3 1.png", 1);
	AddTexture(L"Proto_Wall_Boss_2", L"../Bin/Resource/MapObject/Wall/WALL boss 3 2.png", 1);
	AddTexture(L"Proto_Wall_Boss_3", L"../Bin/Resource/MapObject/Wall/WALL boss 3 3.png", 1);
	AddTexture(L"Proto_Wall_Border", L"../Bin/Resource/MapObject/Wall/WALL BORDER.png", 1);
	AddTexture(L"Proto_Fence", L"../Bin/Resource/MapObject/FENCE.png", 1);
	AddTexture(L"Proto_JumpBorder", L"../Bin/Resource/MapObject/JUMP BORDER.png", 1);
	AddTexture(L"Proto_Wall_Slidedash", L"../Bin/Resource/MapObject/Wall/SIDE DASH WALL.png", 1);
	AddTexture(L"Proto_Steira", L"../Bin/Resource/MapObject/STEIRA.png", 1);
	AddTexture(L"Proto_Windows", L"../Bin/Resource/MapObject/WINDOWS.png", 1);
	AddTexture(L"Proto_Floor_Elevator", L"../Bin/Resource/MapObject/Elevator/ELEVATOR FLOOR.png", 1);
	AddTexture(L"Proto_Wall_Elevator", L"../Bin/Resource/MapObject/Elevator/ELEVATOR COLUMS.png", 1);
	AddTexture(L"Proto_Ceiling_Elevator", L"../Bin/Resource/MapObject/Elevator/ELEVATOR ROOF.png", 1);
	AddTexture(L"Proto_Platform_1", L"../Bin/Resource/MapObject/Platform/PLATFORM 1.png", 1);
	AddTexture(L"Proto_Platform_2", L"../Bin/Resource/MapObject/Platform/PLATFORM 2.png", 1);

	// tile
	AddTexture(L"Proto_Acid_Env", L"../Bin/Resource/MapObject/Acid/acid dash.png", 1);
	AddTexture(L"Proto_Electric_Wall", L"../Bin/Resource/MapObject/ElectricWall/ELECTRIC WALL_%02d.png", 2);
	AddTexture(L"Proto_Glass", L"../Bin/Resource/MapObject/GLASS.png", 1);
	AddTexture(L"Proto_BIO_1", L"../Bin/Resource/MapObject/Wall/WALL BIO 1.png", 1);
	AddTexture(L"Proto_BIO_2", L"../Bin/Resource/MapObject/Wall/WALL BIO 2.png", 1);
	AddTexture(L"Proto_Rug_1", L"../Bin/Resource/MapObject/Rug/RUG 1.png", 1);
	AddTexture(L"Proto_Rug_2", L"../Bin/Resource/MapObject/Rug/RUG 2.png", 1);
	AddTexture(L"Proto_Rug_3", L"../Bin/Resource/MapObject/Rug/RUG 3.png", 1);
	AddTexture(L"Proto_Rug_4", L"../Bin/Resource/MapObject/Rug/RUG 4.png", 1);
	AddTexture(L"Proto_Wall_Katana", L"../Bin/Resource/MapObject/Wall/WALL KATANAS.png", 1);
	AddTexture(L"Proto_Wall_Transparent_1", L"../Bin/Resource/MapObject/Wall/WALL TRANSPARENT 1.png", 1);
	AddTexture(L"Proto_Wall_Transparent_2", L"../Bin/Resource/MapObject/Wall/WALL TRANSPARENT 2.png", 1);
	AddTexture(L"Proto_Wall_Transparent_3", L"../Bin/Resource/MapObject/Wall/WALL TRANSPARENT 3.png", 1);
	AddTexture(L"Proto_Wall_Transparent_4", L"../Bin/Resource/MapObject/Wall/WALL TRANSPARENT 4.png", 1);
	AddTexture(L"Proto_Vent", L"../Bin/Resource/MapObject/Ventilador/BIR AIR CONDITIONER.png", 1);
	AddTexture(L"Proto_Vent_Prop", L"../Bin/Resource/MapObject/Ventilador/BIR AIR CONDITIONER_%02d.png", 2);
	AddTexture(L"Proto_CABLES_1", L"../../Client/Bin/Resource/MapObject/Tile/CABLES 3.png", 1);
	AddTexture(L"Proto_CABLES_2", L"../../Client/Bin/Resource/MapObject/Tile/CABLES 4.png", 1);
	AddTexture(L"Proto_Laser_Border", L"../../Client/Bin/Resource/MapObject/LASER BORDER.png", 1);

	// door
	AddTexture(L"Proto_NormalDoor_1_1", L"../../Client/Bin/Resource/MapObject/Door/DOOR 2_1.png", 1);
	AddTexture(L"Proto_NormalDoor_1_2", L"../../Client/Bin/Resource/MapObject/Door/DOOR 2_2.png", 1);
	AddTexture(L"Proto_NormalDoor_2_1", L"../../Client/Bin/Resource/MapObject/Door/DOOR 3_1.png", 1);
	AddTexture(L"Proto_NormalDoor_2_2", L"../../Client/Bin/Resource/MapObject/Door/DOOR 3_2.png", 1);
	AddTexture(L"Proto_NormalDoor_3_1", L"../../Client/Bin/Resource/MapObject/Door/DOOR 4_1.png", 1);
	AddTexture(L"Proto_NormalDoor_3_2", L"../../Client/Bin/Resource/MapObject/Door/DOOR 4_2.png", 1);
	AddTexture(L"Proto_OpeningDoor_1", L"../../Client/Bin/Resource/MapObject/Elevator/DOOR ELEVATOR_1.png", 1);
	AddTexture(L"Proto_OpeningDoor_2", L"../../Client/Bin/Resource/MapObject/Elevator/DOOR ELEVATOR_2.png", 1);

	// display
	AddTexture(L"Proto_Bandit_Outdoor", L"../Bin/Resource/MapObject/DisplayBoard/BANDIT.png", 1);
	AddTexture(L"Proto_Beach_Outdoor", L"../Bin/Resource/MapObject/DisplayBoard/BEACH.png", 1);
	AddTexture(L"Proto_Beer_Outdoor", L"../Bin/Resource/MapObject/DisplayBoard/BEER.png", 1);
	AddTexture(L"Proto_Fuck_Outdoor", L"../Bin/Resource/MapObject/DisplayBoard/FKU001.png", 1);
	AddTexture(L"Proto_Kimono_Outdoor", L"../Bin/Resource/MapObject/DisplayBoard/KIMONO.png", 1);
	AddTexture(L"Proto_SaveHer_Outdoor", L"../Bin/Resource/MapObject/DisplayBoard/SAVEHER.png", 1);
	AddTexture(L"Proto_Shoes_Outdoor", L"../Bin/Resource/MapObject/DisplayBoard/SHOES.png", 1);

	// bottle
	AddTexture(L"Proto_Bottle_1", L"../../Client/Bin/Resource/MapObject/Bottle/Bottle1.png", 1);
	AddTexture(L"Proto_Bottle_2", L"../../Client/Bin/Resource/MapObject/Bottle/Bottle2.png", 1);
	AddTexture(L"Proto_Bottle_3", L"../../Client/Bin/Resource/MapObject/Bottle/Bottle3.png", 1);
	AddTexture(L"Proto_Bottle_4", L"../../Client/Bin/Resource/MapObject/Bottle/Bottle4.png", 1);
	AddTexture(L"Proto_Bottle_5", L"../../Client/Bin/Resource/MapObject/Bottle/Bottle5.png", 1);
	AddTexture(L"Proto_Bottle_6", L"../../Client/Bin/Resource/MapObject/Bottle/Bottle6.png", 1);
	AddTexture(L"Proto_Bottle_7", L"../../Client/Bin/Resource/MapObject/Bottle/Bottle7.png", 1);

	// vendingmachine
	AddTexture(L"Proto_VendingMachine", L"../../Client/Bin/Resource/MapObject/soda/SODA MACHINE_%02d.png", 2);

	// signs
	AddTexture(L"Proto_Signs_1", L"../Bin/Resource/MapObject/Signs/SIGNS 1.png", 1);
	AddTexture(L"Proto_Signs_2", L"../Bin/Resource/MapObject/Signs/SIGNS 3.png", 1);
	AddTexture(L"Proto_Signs_3", L"../Bin/Resource/MapObject/Signs/SIGNS 4.png", 1);
	AddTexture(L"Proto_Signs_4", L"../Bin/Resource/MapObject/Signs/SIGNS 5.png", 1);
	AddTexture(L"Proto_Signs_5", L"../Bin/Resource/MapObject/Signs/SIGNS 6.png", 1);
	AddTexture(L"Proto_Signs_6", L"../Bin/Resource/MapObject/Signs/SIGNS 7.png", 1);
	AddTexture(L"Proto_Signs_7", L"../Bin/Resource/MapObject/Signs/SIGNS 8.png", 1);

	// road
	AddTexture(L"Proto_Road_1", L"../../Client/Bin/Resource/MapObject/Road/ROAD 1.png", 1);
	AddTexture(L"Proto_Road_2", L"../../Client/Bin/Resource/MapObject/Road/ROAD 2.png", 1);
	AddTexture(L"Proto_Road_3", L"../../Client/Bin/Resource/MapObject/Road/ROAD 3.png", 1);
	AddTexture(L"Proto_Road_4", L"../../Client/Bin/Resource/MapObject/Road/ROAD 4.png", 1);
	AddTexture(L"Proto_Road_Corner", L"../../Client/Bin/Resource/MapObject/Road/ROAD CORNER.png",1);
	AddTexture(L"Proto_Road_StreetLights", L"../../Client/Bin/Resource/MapObject/Road/STREET LIGHTS.png",1);
	AddTexture(L"Proto_Road_TrafficLights", L"../../Client/Bin/Resource/MapObject/Road/TRAFFIC LIGHTS.png",1);
	AddTexture(L"Proto_Road_Plate_1", L"../../Client/Bin/Resource/MapObject/Road/PLATE 1.png",1);
	AddTexture(L"Proto_Road_Plate_2", L"../../Client/Bin/Resource/MapObject/Road/PLATE 2.png",1);
	AddTexture(L"Proto_Road_Pass_1", L"../../Client/Bin/Resource/MapObject/Road/PASSARELA.png",1);
	AddTexture(L"Proto_Road_Pass_2", L"../../Client/Bin/Resource/MapObject/Road/PASSTAIRS.png",1);
	AddTexture(L"Proto_Road_Lights", L"../../Client/Bin/Resource/MapObject/Road/TRAFFIC LIGHTS.png",1);
	AddTexture(L"Proto_Road_Sign_1", L"../../Client/Bin/Resource/MapObject/Road/TRAFFIC SIGN 1.png",1);
	AddTexture(L"Proto_Road_Sign_2", L"../../Client/Bin/Resource/MapObject/Road/TRAFFIC SIGN 2.png",1);
	AddTexture(L"Proto_Road_Sign_3", L"../../Client/Bin/Resource/MapObject/Road/TRAFFIC SIGN 3.png",1);
	AddTexture(L"Proto_Road_Sign_4", L"../../Client/Bin/Resource/MapObject/Road/TRAFFIC SIGN 4.png",1);
	AddTexture(L"Proto_Road_Sign_5", L"../../Client/Bin/Resource/MapObject/Road/TRAFFIC SIGN 5.png",1);
	AddTexture(L"Proto_Road_Tree", L"../../Client/Bin/Resource/MapObject/Road/ARVORE 2.png",1);

	// effect
	AddTexture(L"Proto_Effect_Splatter", L"../../Client/Bin/Resource/Effect/BloodSpread/thin-splatter_%03d.png", 9);
	AddTexture(L"Proto_Effect_Electric", L"../../Client/Bin/Resource/Effect/Electric/Eletricity_%03d.png", 17);
	AddTexture(L"Proto_Effect_Explosion", L"../../Client/Bin/Resource/Effect/Explosion/Explosion_%02d.png", 16);
	AddTexture(L"Proto_Effect_Spread", L"../../Client/Bin/Resource/Effect/FanSpread/fan_spread_%03d.png", 12);
	AddTexture(L"Proto_Effect_Hitted", L"../../Client/Bin/Resource/Effect/Hitted/Hitted_%02d.png", 6);
	AddTexture(L"Proto_Effect_DeadBody", L"../../Client/Bin/Resource/Effect/MonsterDeadBody/aaa_%03d.png", 8);

	return S_OK;
}

float CLoader::Get_Progress() const
{
	EnterCriticalSection(const_cast<CRITICAL_SECTION*>(&m_Crt));
	float p = m_progress;
	LeaveCriticalSection(const_cast<CRITICAL_SECTION*>(&m_Crt));
	return p;
}

void CLoader::PhaseBegin(float base, float weight, const wchar_t* name)
{
	EnterCriticalSection(&m_Crt);
	m_phaseBase = std::clamp(base, 0.f, 1.f);
	m_phaseWeight = std::clamp(weight, 0.f, 1.f - m_phaseBase);
	m_progress = m_phaseBase;
	if (name) lstrcpyn(m_szLoading, name, _countof(m_szLoading));
	LeaveCriticalSection(&m_Crt);
}

void CLoader::PhaseStep(float local01, const wchar_t* msg)
{
	local01 = std::clamp(local01, 0.f, 1.f);
	EnterCriticalSection(&m_Crt);
	m_progress = m_phaseBase + m_phaseWeight * local01;
	if (msg && msg[0]) lstrcpyn(m_szLoading, msg, _countof(m_szLoading));
	LeaveCriticalSection(&m_Crt);
}

void CLoader::PhaseDone(const wchar_t* msgDone)
{
	EnterCriticalSection(&m_Crt);
	m_progress = m_phaseBase + m_phaseWeight; // 구간 100%
	if (msgDone) lstrcpyn(m_szLoading, msgDone, _countof(m_szLoading));
	LeaveCriticalSection(&m_Crt);
}

HRESULT CLoader::Loading_UI()
{
	// 객체 생성
	PhaseBegin(0.f, 0.5f, L"UI 자원 로딩");
#pragma region Weapon texture
	PhaseStep(0.10f, L"무기 텍스쳐");
	// Pistol
	// Pistol - Idle
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Idle",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/idle/PISTOL_IDLE%03d.png", 3))))
		return E_FAIL;
	// Pistol - Op
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Op",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/opening/PISTOL_OPENING%03d.png", 9))))
		return E_FAIL;
	// Pistol - Attack
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Attack",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/attack/PISTOL_SHOT%03d.png", 6))))
		return E_FAIL;
	// Pistol - Reload
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Re",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/reload/PISTOL_RELOAD%03d.png", 13))))
		return E_FAIL;
	// PistolC - Idle
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Car_Idle",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/car/idle/PISTOL2_IDLE%02d.png", 3))))
		return E_FAIL;
	//PistolC - Attack
	// "C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Weapon\pistol\car\attack\PistolC_Attack004.png"
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Car_Attack",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/car/attack/PistolC_Attack%03d.png", 5))))
		return E_FAIL;
	// PistolC - Zooming
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Car_Zooming",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/car/zooming/PistolC_Zooming%03d.png", 3))))
		return E_FAIL;
	// PistolC - Zoom_Idle
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Car_ZoomIdle",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/car/zoom/idle/PISTOL_ZOOM_IDLE%03d.png", 3))))
		return E_FAIL;
	// PistolC - Zoom_Attack
	// "C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Weapon\pistol\car\zoom\attack\PISTOL_ZOOM_ATTACK02.png"
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Car_ZoomAtt",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/car/zoom/attack/PISTOL_ZOOM_ATTACK%02d.png", 3))))
		return E_FAIL;
	// PistolC - Zoom out
	// "C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Weapon\pistol\car\zoomout\PistolC_ZoomOut002.png"
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Car_ZoomOut",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/car/zoomout/PistolC_ZoomOut%03d.png", 3))))
		return E_FAIL;

	// pistol effect
	//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Weapon\pistol\effect\muzzle_tank_0009.png"muzzle_tank_009
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Eff",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/effect/muzzle_tank_%03d.png", 10))))
		return E_FAIL;
	// pistol aim
	// "C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Weapon\pistol\PISTOL.png"
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_AimEff",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/PISTOL.png", 1))))
		return E_FAIL;
	//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Weapon\pistol\effect\Smoke_014.png"
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_EffSmoke",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/effect/Smoke_%03d.png", 15))))
		return E_FAIL;

	// Katana
	// Katana - IDLE
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Katana_IDLE",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/Katana/IDLE/KATANA.png", 1))))
		return E_FAIL;

	// ShotGun
	// ShotGun - aimUI
	// "C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Weapon\shotGun\ShotG_Aim.png"
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapShot_AimEff",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/shotGun/ShotG_Aim.png", 1))))
		return E_FAIL;
	// ShotGun - eff
	// "C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Weapon\shotGun\effect\muzzle_shotgun_008.png"
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapShot_Eff",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/shotGun/effect/muzzle_shotgun_%03d.png", 9))))
		return E_FAIL;

	// ShotGun - idle
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapShot_Idle",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/shotGun/idle/SHOTGUN_IDLE%03d.png", 5))))
		return E_FAIL;
	// ShotGun - attack
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapShot_Att",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/shotGun/attack/SHOTGUN_attack%03d.png", 2))))
		return E_FAIL;
	// ShotGun - attakend
	// "C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Weapon\shotGun\attack_End\SHOTGUN_attackEnd013.png"
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapShot_AttEnd",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/shotGun/attack_End/SGAEnd%03d.png", 13))))
		return E_FAIL;
	// ShotGun - opening
	// "C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Weapon\shotGun\opening\SHOTGUN_OpL012.png"
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapShot_Op",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/shotGun/opening/SHOTGUN_OpL%03d.png", 13))))
		return E_FAIL;
	// ShotGun - Reload
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapShot_Reload",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/shotGun/reload/ShotG_Reload%03d.png", 14))))
		return E_FAIL;

	// ShotGun - Car
	// ShotGunC - idle
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapShotC_Idle",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/shotGun/car/idle/IdleC%03d.png", 2))))
		return E_FAIL;
	// ShotGunC - attck
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapShotC_Att",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/shotGun/car/attack/attack%03d.png", 2))))
		return E_FAIL;
	// ShotGunC - zoom
	//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Weapon\shotGun\car\idle_zoom\IdleZ003.png"
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapShotC_Zoom",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/shotGun/car/idle_zoom/IdleZ%03d.png", 3))))
		return E_FAIL;
	// ShotGunC - attackzoom
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapShotC_ZoomAtt",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/shotGun/car/attack_zoom/SGAttackZ%03d.png", 4))))
		return E_FAIL;
	// ShotGunC - zooming
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapShotC_Zooming",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/shotGun/car/zooming/zooming%03d.png", 6))))
		return E_FAIL;
	// ShotGunC - zoomout
	// "C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Weapon\shotGun\car\zoomout\zoomout005.png"
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapShotC_ZoomOut",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/shotGun/car/zoomout/zoomout%03d.png", 6))))
		return E_FAIL;

	// Knife
	// Knife - idle
	//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Weapon\cleaver\idle\CLEAVER_IDLE002.png"
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_SubWKnife_Idle",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/cleaver/idle/CLEAVER_IDLE%03d.png", 3))))
		return E_FAIL;
	// Knife - item
	//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Weapon\cleaver\item\CLEAVER_ITEM.png"
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_SubWKnife_Item",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/cleaver/item/CLEAVER_ITEM.png", 1))))
		return E_FAIL;
	// Knife - attack
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_SubWKnife_Att",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/cleaver/attack/CLEAVER_Attack%03d.png", 2))))
		return E_FAIL;

	// Knife - effect
	//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Weapon\cleaver\InsBlood017.png"
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_SubWKnife_Eff",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/cleaver/InsBlood%03d.png", 18))))
		return E_FAIL;

	// Sniper Gun
	// 이거는 나중에 scene_sniper 에서만 load
	// Sniper aimUI
	// "C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Weapon\sniper\SNIPER_WEP.png"
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapSniper_AimEff",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/sniper/SNIPER_WEP.png", 1))))
		return E_FAIL;
	// Sniper - Idle
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapSniper_Idle",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/sniper/idle/sniperG_Idle%03d.png", 3))))
		return E_FAIL;
	// Sniper - Op
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapSniper_Op",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/sniper/opening/SG_Op%03d.png", 30))))
		return E_FAIL;
	// Sniper - attack
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapSniper_Attack",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/sniper/attack/Sniper_Att%03d.png", 2))))
		return E_FAIL;
	// Sniper - attack end
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapSniper_AttEnd",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/sniper/reload/SG_Reload%03d.png", 14))))
		return E_FAIL;
	// Sniper - zooming
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapSniper_Zooming",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/sniper/zooming/sniperG_Zooming%03d.png", 6))))
		return E_FAIL;
	// Sniper - zoom
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapSniper_Zoom",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/sniper/zooming/sniperG_Zoom.png", 1))))
		return E_FAIL;
	// Sniper - zoom attack
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapSniper_ZoomAtt",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/sniper/zoomAtt/sniperG_ZoomAtt%03d.png", 7))))
		return E_FAIL;

#pragma endregion Weapon texture

#pragma region UI Texture
	// Player UI
	PhaseStep(0.35f, L"플레이어/HP바/공용 UI 텍스쳐");
	// Arm
	// openig1 - weapon : non
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIArmOp1",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/arm/glove/OPENING_GLOVE%03d.png", 3))))
		return E_FAIL;
	// openig2 - weapon : non
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIArmOp2",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/arm/melee/OPENING_MELEE%03d.png", 3))))
		return E_FAIL;
	// monster - fucku
	//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\player\arm\FuckU.png"
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_FuckU",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/arm/FuckU.png", 1))))
		return E_FAIL;
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_DrunkU",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/arm/Drunk.png", 1))))
		return E_FAIL;

	// foot
	// foot - kick
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIFootKick",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/foot/kick/kick%03d.png", 3))))
		return E_FAIL;
	// foot - slide
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIFootSlide",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/foot/slide/slide000.png", 1))))
		return E_FAIL;

	//	HandL
	//	HandL - Idle
	/*if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHandLIdle",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/left/idle/IDLE%03d.png", 17))))
		return E_FAIL;*/
		//	HandL - Doping
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHandLDoping",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/left/doping/DRINK%03d.png", 7))))
		return E_FAIL;
	//	HandL - opening : rifle
	//C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\player\left\opening\rifle
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHandLOpRif",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/left/opening/rifle/Lhand_opeing_rifle%03d.png", 12))))
	 	return E_FAIL;
	// HandL - attack_instance - knife
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHandLAt2Knife",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/left/attack2/punch%03d.png", 3))))
		return E_FAIL;
	// HandL - reload - pistol
	//C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\player\left\reload\pistol\END
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHandLRePistol",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/left/reload/pistol/END/PistolR_L%03d.png", 3))))
		return E_FAIL;
	// HandL - reload - shotgun
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHandLReShotgun",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/left/reload/shotgun/ShotR_Left%03d.png", 2))))
		return E_FAIL;

	// HandL - Opening - Katana
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Katana_Hand_L",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/Katana/OPENING/LeftArm.png", 1))))
		return E_FAIL;

	// HandR
	// // HandR - Idle
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHandRIdle",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/right/idle/HandR_Idle%03d.png", 3))))
		return E_FAIL;
	// HandR - attack_instance - knife
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHandRAt2Knife",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/right/attack2/knife/KnifeHR000.png", 1))))
		return E_FAIL;
	// HandR - Dead
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHandRDead",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/right/dead/DEATH%03d.png", 3))))
		return E_FAIL;
	// handr - op - pistol
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHandROpPistol",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/right/opening/pistol/Pistol_OpR%03d.png", 3))))
		return E_FAIL;
	// handr - op - shotgun
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHandROpShotgun",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/right/opening/shotGun/ShotG_OpR%03d.png", 4))))
		return E_FAIL;

	// handr - op - Katana
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHandROpKatana",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/Katana/OPENING/RightArm.png", 1))))
		return E_FAIL;

	// handr - IDLE - Katana
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHandRIdleKatana",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/Katana/OPENING/HandR_Idle%03d.png", 3))))
		return E_FAIL;

	// handr - Attack2 - Katana
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHandRAttack2",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/Katana/Attack/Attack2/Attack2Hand.png", 1))))
		return E_FAIL;

	// handr - Attack3 - Katana
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHandRAttack3",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/Katana/Attack/Attack3/Attack3Hand.png", 1))))
		return E_FAIL;

	// HpBar
	// HpB - PhoneN
	//C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\UI\UI_HpBar\Phone
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHpBarPhoneN",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/UI_HpBar/Phone/PhoneWArm_N%03d.png", 4))))
		return E_FAIL;
	// HpB - PhoneB
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHpBarPhoneB",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/UI_HpBar/Phone/PhoneWArm_B%03d.png", 4))))
		return E_FAIL;
	// HpB - PhoneF
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHpBarPhoneF",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/UI_HpBar/Phone/PhoneWArm_F%03d.png", 4))))
		return E_FAIL;
	// HpB - ManN
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHpBarManN",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/UI_HpBar/Man/normal/Jack_Nomal_%03d.png", 16))))
		return E_FAIL;
	// HpB - ManH
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHpBarManH",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/UI_HpBar/Man/hit/Jack_Good_%03d.png", 16))))
		return E_FAIL;
	// HpB - ManD
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHpBarManD",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/UI_HpBar/Man/dead/Jack_Hurt_%03d.png", 16))))
		return E_FAIL;
	// HpB - Crack
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHpBarCrack",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/UI_HpBar/crack/Phone_Crack000.png", 1))))
		return E_FAIL;

	// Katana - sheath(칼집)
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_KatanaSheath",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/Katana/OPENING/Open_Sheath.png", 1))))
		return E_FAIL;

	// Katana - Handle(칼 손잡이)
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_KatanaHandle",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/Katana/OPENING/Handle.png", 1))))
		return E_FAIL;

	// Katana - Knife(칼)
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Knife",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/Katana/OPENING/Knife.png", 1))))
		return E_FAIL;

	// Katana - Shine Knife(빛나는 칼)
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_ShineKnife",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/Katana/OPENING/Knife_Shiny.png", 1))))
		return E_FAIL;

	// Katana - Attack - 가로베기
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_KatanaAttack1",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/Katana/Attack/Attack1/Attack1.png", 1))))
		return E_FAIL;

	// Katana - Attack - 대각선 오른쪽베기 
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_KatanaAttack2",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/Katana/Attack/Attack2/Attack2.png", 1))))
		return E_FAIL;

	// Katana - Attack - 대각선 왼쪽베기
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_KatanaAttack3",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/Katana/Attack/Attack3/Attack3.png", 1))))
		return E_FAIL;

	// Katana - Attack - 가로베기 이펙트
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_KatanaAttackEffect1",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/Katana/Attack/Attack1/LEFT_SLASH%03d.png", 5))))
		return E_FAIL;

	// Katana - Effect - 대각선 오른쪽베기 이펙트
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_KatanaAttackEffect2",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/Katana/Attack/Attack2/LEFT_SLASH%03d.png", 5))))
		return E_FAIL;

	// Katana - Effect - 대각선 왼쪽베기 이펙트
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_KatanaAttackEffect3",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/Katana/Attack/Attack3/LEFT_SLASH%03d.png", 5))))
		return E_FAIL;

#pragma endregion UI

#pragma region 일반 UI
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_HealEffect",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/LIKES.png", 1))))
		return E_FAIL;
#pragma endregion 일반UI

#pragma region 작은 이펙트 UI
	PhaseStep(0.65f, L"게임 진입 UI 텍스쳐");
	// 여러 상호작용 이펙트 UI
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIDiamondWhite",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/PICKUP_REFILL L I.png", 1))))
		return E_FAIL;

	// 무기 획득 UI - BackGround
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WeaponUIBack",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/PICKUP_REFILL L.png", 1))))
		return E_FAIL;

	// 무기 획득 UI - Weapon
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WeaponUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/HUD/CLEAVER ITEM.png", 1))))
		return E_FAIL;
	
#pragma endregion 효과 UI

#pragma region 패널 UI

	// GridUI
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_FileGridUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/CYBER BACKGROUND.png", 1))))
		return E_FAIL;

	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_FrameUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/Frame.png", 1))))
		return E_FAIL;
#pragma endregion 패널 UI

#pragma region 게임 진입 UI
	// HEART
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_HeartUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/HEART%03d.png", 5))))
		return E_FAIL;
	// HEART LINE
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_HeartUI_LINE",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/HEART LINE.png", 1))))
		return E_FAIL;

	// HEAT BEAT 
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_HeartUI_BEAT",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/HEART BEAT.png", 1))))
		return E_FAIL;

#pragma region LisaUI
	// LISA UI
	// IDLE
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_LisaUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Lisa/Default/Lisa_Default_%03d.png", 12))))
		return E_FAIL;
	// BYE
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_LisaByeUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Lisa/wink/Lisa_Wink_%03d.png", 10))))
		return E_FAIL;

	// WINK
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_LisaSideUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Lisa/Side/Lisa_Side_%03d.png", 10))))
		return E_FAIL;

	// LisaHead - IDLE
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_LisaHair_Default",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Lisa/Default_Hair/HAIR%03d.png", 3))))
		return E_FAIL;

	// LisaHead - Bye
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_LisaHair_Bye",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Lisa/Wink_Hair/MASCOT_HAIR_WINK%03d.png", 3))))
		return E_FAIL;

	// LisaHead - WINK
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_LisaHair_WINK",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Lisa/Side_Hair/Side_Hair_%03d.png", 6))))
		return E_FAIL;

	// Lisa - TalkUI
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Talk",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/BALLON.png", 1))))
		return E_FAIL;
#pragma endregion LisaUI

	// CHAT UI
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_ChatUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/Chat/CHAT%03d.png", 7))))
		return E_FAIL;

	// Arrow UI
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_ArrowUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/ARROW.png", 1))))
		return E_FAIL;

	// LOGO
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_LogoUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/PEACE LOGO 2.png", 1))))
		return E_FAIL;

	// TITLE
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_SmallTitleUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/SMALLTITLE.png", 1))))
		return E_FAIL;

	// TextUI
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_SmallTextUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/MsgText.png", 1))))
		return E_FAIL;

	// LiveIcon
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_LiveIconUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/LIVE ICON.png", 1))))
		return E_FAIL;

	// PhoneUI
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_PhoneUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/PhoneUI/PHONEIDLE_%03d.png", 4))))
		return E_FAIL;

	// Phone Right_Hand UI
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Phone_RightHandUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/PhoneUI/Right_HAND INTERMISSION %03d.png", 2))))
		return E_FAIL;
	// Phone Left_Hand UI
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Phone_LeftHandUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/PhoneUI/LEFT_HAND INTERMISSION %03d.png", 2))))
		return E_FAIL;
	
	// PhoneScreen UI
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Phone_ScreenUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/POLICE BADGE%03d.png", 6))))
		return E_FAIL;

	// PhoneScreenError UI
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Phone_ErrorScreenUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/PhoneUI/ErrorScreen%03d.png", 4))))
		return E_FAIL;

	//// PhoneScreenTitle UI
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Phone_ScreenTitleUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/PhoneUI/HUD UPGRADES BAR.png", 1))))
		return E_FAIL;

	// PhoneScreenTitle UI
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Phone_FrameUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/PhoneUI/INT-sheet_%03d.png", 4))))
		return E_FAIL;

#pragma region 상점관련 UI들

	// ShopFrame UI
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_PhoneShop_FrameUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/UPGRADE BUTTON.png", 1))))
		return E_FAIL;

	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_PhoneShop_BoardFrameUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/UPGRADE BORDER BK.png", 1))))
		return E_FAIL;

	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_BossKiller_Art",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/WEAPON 3.png", 1))))
		return E_FAIL;

	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Sniper_Art",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/WEAPON 2.png", 1))))
		return E_FAIL;

	// 느린 총알 UI - 배경
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Bullet_Slow_Back",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/HERO BACK ROBOT.png", 1))))
		return E_FAIL;

	// 느린 총알
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Bullet_Slow_Bullet",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/BackImage.png", 1))))
		return E_FAIL;
	// 소화기
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Fire_Ex",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/FireIcon.png", 1))))
		return E_FAIL;
	// 헤드샷
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_HeadSHOT",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/HEADSHOT.png", 1))))
		return E_FAIL;


#pragma endregion 상점관련 UI들

	
#pragma endregion 게임 진입 UI
	PhaseStep(0.80f, L"오브젝트 프로토타입 생성");
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

	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_Monster_Fat",
		CMonster_Fat::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_Monster_Soldier",
		CMonster_Soldier::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_Monster_Head",
		CMonster_Head::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_Monster_Drone",
		CMonster_Dron::Create(m_pGraphicDev))))
		return E_FAIL;

	// UI
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_UIRoot",
		CUIBase::Create(m_pGraphicDev))))
		return E_FAIL;

	// ItemUI
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_UIItem",
		CItemUI::Create(m_pGraphicDev))))
		return E_FAIL;

	// 몬스터 피격 이펙트
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_MonsterHitEffectUI",
		CEffectUI::Create(m_pGraphicDev))))
		return E_FAIL;

	// 텍스쳐 상호작용 이펙트
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_TextEffectUI",
		CTextEffectUI::Create(m_pGraphicDev))))
		return E_FAIL;

#pragma region 게임 진입 UI들 생성
	// BacgkGround
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_BlackBackground",
		CBlackGackGround::Create(m_pGraphicDev))))
		return E_FAIL;

	// Heart UI
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_HeartUI",
		CHeartUI::Create(m_pGraphicDev))))
		return E_FAIL;

	// Lisa UI
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_LisaUI",
		CLisaUI::Create(m_pGraphicDev))))
		return E_FAIL;
	// Panel
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_PanelUI",
		CPanelUI::Create(m_pGraphicDev))))
		return E_FAIL;

	// Chat UI
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_ChatUI",
		CChatUI::Create(m_pGraphicDev))))
		return E_FAIL;

	// Bannel UI
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_BannerUI",
		CBannerUI::Create(m_pGraphicDev))))
		return E_FAIL;

	// TALK UI
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_TalkUI",
		CTalkUI::Create(m_pGraphicDev))))
		return E_FAIL;

	// Phone UI
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_PhoneUI",
		CPhoneUI::Create(m_pGraphicDev))))
		return E_FAIL;



#pragma endregion 게임 진입 UI들 생성
	



#pragma endregion 게임 진입 UI들 생성

#pragma region HpBar UI
	// Hpbar UI
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_HpbarUI",
		CHpBarUI::Create(m_pGraphicDev))))
		return E_FAIL;
	// HpBar - Man
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_HpbarUI_Man",
		CMan_HpBarUI::Create(m_pGraphicDev))))
		return E_FAIL;
	// HpBar - Phone
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_HpbarUI_Phone",
		CPhone_HpBarUI::Create(m_pGraphicDev))))
		return E_FAIL;

#pragma endregion HpBar UI

#pragma region Player UI
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
	// player ui manager 생성
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_PlayerUI",
		CPlayerUI_Manager::Create(m_pGraphicDev))))
		return E_FAIL;
#pragma endregion Player UI

#pragma region  weapon  UI
	// Weapon UI
	// Pistol
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_GunPistolUI",
		CPistol_Gun::Create(m_pGraphicDev))))
		return E_FAIL;

	// Katana
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_KatanaUI",
		CKatana::Create(m_pGraphicDev))))
		return E_FAIL;

	// Sniper
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_GunSniperUI",
		CSniper_Gun::Create(m_pGraphicDev))))
		return E_FAIL;

	// ShotGun
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_GunShotGUI",
		CShot_Gun::Create(m_pGraphicDev))))
		return E_FAIL;

	// SubWeapon UI
	// Knife
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_SubWKnifeUI",
		CKnife_SubW::Create(m_pGraphicDev))))
		return E_FAIL;

	//Weapon Manager UI
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_WeaponManagerUI",
		CWeaponUI_Manager::Create(m_pGraphicDev))))
		return E_FAIL;
#pragma endregion weapon  UI

	PhaseStep(1.0f, L"UI 로딩 완료");
	PhaseDone(L"UI 로딩 완료");

	return S_OK;
}
