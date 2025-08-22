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

// Weapon UI
#include "CPistol_Gun.h"
#include "CKnife_SubW.h"

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
#pragma endregion 게임 진입 UI들

 
// 몬스터
#include "CMonster_Suit.h"

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

	} break;
	case SCENE_STAGE_2:
	{

	} break;
	case SCENE_STAGE_3:
	{

	} break;
	case SCENE_SNIPE:
	{

	} break;
	case SCENE_BOSS:
	{

	} break;
	case SCENE_CAR:
	{

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

HRESULT CLoader::Loading_Dev()
{
	lstrcpy(m_szLoading, L"텍스쳐 로딩 중");

#pragma region 맵오브젝트 임시
	if (FAILED(Loading_MapObjectTexture_Src()))
		return E_FAIL;
#pragma endregion

	// 객체 생성
	lstrcpy(m_szLoading, L"객체 생성 중.");

#pragma region 슈트 몬스터
	// Monster
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

	// DEATH
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Suit_DEATH1",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/death/fb_death%02d.png", 21))))
		return E_FAIL;

	// Blocking -> Kicked
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Suit_Blocking",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/blocking/blocking%03d.png", 4))))
		return E_FAIL;
	// InstanceKill
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Monster_Suit_InstanceKill",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/suit_monster/instanceKill/sm_IKill%03d.png", 13))))
		return E_FAIL;

#pragma endregion 슈트 몬스터


#pragma region MapObject
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
#pragma endregion

	lstrcpy(m_szLoading, TEXT("모델 로딩 중."));

	// CubeTex
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_CubeBuffer", Engine::VIBuffer_Cube::Create(m_pGraphicDev))))
		return E_FAIL;


	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_Tutorial()
{
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

HRESULT CLoader::Loading_Stage_3()
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
}

HRESULT CLoader::Loading_UI()
{
	// 객체 생성
	lstrcpy(m_szLoading, L"UI 생성 중.");

#pragma region Weapon texture
	// Pistol
	// Pistol - Idle
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Idle",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/idle/PISTOL_IDLE%03d.png", 3))))
		return E_FAIL;
	// Pistol - Op
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Op",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/opening/PISTOL_OPENING%03d.png", 8))))
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
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/car/idle/PISTOL2_IDLE%03d.png", 3))))
		return E_FAIL;
	//PistolC - Attack
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Car_Attack",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/car/attack/PISTOL2_ATTACK%03d.png", 5))))
		return E_FAIL;
	// PistolC - Zooming
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Car_Zooming",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/car/zooming/pistol_zooming%03d.png", 3))))
		return E_FAIL;
	// PistolC - Zoom_Idle
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Car_ZoomIdle",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/car/zoom/idle/PISTOL_ZOOM_IDLE%03d.png", 4))))
		return E_FAIL;
	// PistolC - Zoom_Attack
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_WapPistol_Car_ZoomAtt",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Weapon/pistol/car/zoom/attack/PISTOL_ZOOM_ATTACK%03d.png", 3))))
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


#pragma endregion Weapon texture

#pragma region UI Texture
	// Player UI

	// Arm
	// openig1 - weapon : non
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIArmOp1",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/arm/glove/OPENING_GLOVE%03d.png", 3))))
		return E_FAIL;
	// openig2 - weapon : non
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIArmOp2",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/arm/melee/OPENING_MELEE%03d.png", 3))))
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
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/right/opening/pistol/Pistol_OpR000.png", 1))))
		return E_FAIL;
	// handr - op - shotgun
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_UIHandROpShotgun",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/player/right/opening/shotGun/ShotG_OpR%03d.png", 4))))
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


#pragma endregion UI

#pragma region 일반 UI
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_HealEffect",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/LIKES.png", 1))))
		return E_FAIL;
#pragma endregion 일반UI

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

	// ShopFrame UI
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_PhoneShop_FrameUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/HUD/HUD UPGRADES BUTTON.png", 1))))
		return E_FAIL;

	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_PhoneShop_BoardFrameUI",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/UPGRADE BORDER.png", 1))))
		return E_FAIL;

	// 상점 이미지 UI
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_SlowMo_Art",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/TOXIC MASCULINITY.png", 1))))
		return E_FAIL;


	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_BossKiller_Art",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/WEAPON 3.png", 1))))
		return E_FAIL;

	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Prototype_Component_Texture_Sniper_Art",
		CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/UI/TutorialScene/WEAPON 2.png", 1))))
		return E_FAIL;

	
#pragma endregion 게임 진입 UI

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

	// ImageUI
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_UIImage",
		CImageUI::Create(m_pGraphicDev))))
		return E_FAIL;

	// ButtonUI
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_UIButton",
		CButtonUI::Create(m_pGraphicDev))))
		return E_FAIL;

	// TextUI
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_TextUI",
		CTextUI::Create(m_pGraphicDev))))
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

	// 몬스터 피격 이펙트
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_MonsterHitEffectUI",
		CEffectUI::Create(m_pGraphicDev))))
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

	// SubWeapon UI
	// Knife
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Prototype_GameObject_SubWKnifeUI",
		CKnife_SubW::Create(m_pGraphicDev))))
		return E_FAIL;

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
}
