#include "pch.h"
#include "CPlayer.h"
#include "CRenderer.h"
#include "CColiderManager.h"
#include "CTimerMgr.h"
#include "CObjectManager.h"
#include "CUIBase.h"
#include "CEffect_World.h"
#include "CDInputMgr.h"
#include "CGlobal_Info.h"
#include "CObjectPoolManager.h"
#include "CMapFactory.h"
#include "CVIBuffer_GridPanelBase.h"
#include "CGameDataManager.h"
#include "CGrounding.h"
#include "CHpBarUI.h"
#include "CMan_HpBarUI.h"
#include "CPhone_HpBarUI.h"
#include "CUIManager.h"
#include "CManagement.h"
#include "CTutorialTracker.h"
#include "CWeaponUI_Manager.h"
#include "CMainWeapon.h"
#include "CMonster_Dron.h"
#include "CCameraFPS.h"
#include "CBullet.h"
#include "CItem.h"
#include "Sound_Manager.h"

//test bj 0829
#include "CEffect_Pixel.h"
#include "CEffect_Pixel_Sprite.h"


CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCharacter(pGraphicDev), m_tPlayerInfo({ OPENING, PMV_NORMAL, WP_PISTOL , WP_KICK }), m_tPrePlayerInfo({ PLAYER_END ,PMV_END, WP_END,WP2_END }),
	m_TimerTag(TEXT("")), m_fGround_Height(0.f), m_eMoveKey(MVKEY_END),
	m_bIsKeyInput(true), m_bIsInvincible(true), m_bIsAttack(true), m_bIsCountHp(false),
	m_fHitTime(0.f), m_fNormalSpeed(0.f), m_fFixY(0.f), m_bIsFixY(false), m_fDashCoolTime(0.f),
	m_fAttackCoolTime(0.f), m_iCurScene(0), m_bIsZoomStage(false), m_fPlayTime(0.f)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CCharacter(rhs), m_tPlayerInfo(rhs.m_tPlayerInfo), m_tPrePlayerInfo(rhs.m_tPrePlayerInfo),
	m_TimerTag(rhs.m_TimerTag), m_fGround_Height(rhs.m_fGround_Height), m_eMoveKey(rhs.m_eMoveKey),
	m_bIsKeyInput(rhs.m_bIsKeyInput), m_bIsInvincible(rhs.m_bIsInvincible), m_bIsAttack(rhs.m_bIsAttack)
	, m_bIsCountHp(rhs.m_bIsCountHp), m_fHitTime(rhs.m_fHitTime), m_fNormalSpeed(rhs.m_fNormalSpeed), 
	m_fFixY(rhs.m_fFixY), m_bIsFixY(rhs.m_bIsFixY), m_fDashCoolTime(rhs.m_fDashCoolTime),
	m_fAttackCoolTime(rhs.m_fAttackCoolTime), m_iCurScene(rhs.m_iCurScene), 
	m_bIsZoomStage(rhs.m_bIsZoomStage), m_fPlayTime(rhs.m_fPlayTime)
{
}

CPlayer::~CPlayer()
{ 
}
 
HRESULT CPlayer::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(CTimerMgr::GetInstance()->Ready_Timer(TEXT("Timer_Player"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Set_Component()))
		return E_FAIL;

	// ui들 생성
	if (FAILED(Set_UI()))
		return E_FAIL;


	// StartPosition 설정 -> scale 조정
	if (MAPOBJECTDATA* p = reinterpret_cast<MAPOBJECTDATA*>(pArg))
	{
		GetTransform()->Set_Info(INFO::INFO_POS, p->transform.Pos);
	}
	GetTransform()->Set_Scale(1.f, 2.f, 1.f);
	
	m_fMaxHp = 20.f;
	m_fHp = 20.f; // 플레이어 목숨 초 -> origin : 10, test : 3
	m_fNormalSpeed = 5.f; // normal speed 값 -> 이값은 고정

	m_fPlayTime = 0.f;

	// state 변경 해줌
	Change_State(OPENING);
	//Change_Move(PMV_NORMAL);

	// terrain위에 있도록 함
	m_bIsFixY = false;
	Set_OnTerrain(1);

	// test 용으로 scene 별로 나누는데
	// 실제로는 collision 같은걸 만들어야 하지 않을까..
	switch (CMapFactory::GetInstance()->GetTargetSceneIndex())
	{
	case SCENE_DEV:
	case SCENE_TUTORIAL:
		m_bIsZoomStage = false;
		Change_Weapon(WP_PISTOL);
		break;
	case SCENE_STAGE_1:
		m_bIsZoomStage = false;
		Change_Weapon(WP_SHOTGUN);
		break;
	case SCENE_STAGE_2:
		m_bIsZoomStage = false;
		Change_Weapon(WP_KATANA);
		break;

	case SCENE_SNIPE:
		m_bIsZoomStage = true;
		m_fMaxHp = 100.f;
		m_fHp = 100.f;
		Change_Weapon(WP_SNIPER);
		break;

	case SCENE_BOSS:
		m_bIsZoomStage = false;
		m_iCurScene = SCENE_BOSS;
		m_fMaxHp = 100.f;
		m_fHp = 100.f;
		Change_Weapon(WP_PISTOL);
		break;

	case SCENE_CAR:
		m_fNormalSpeed = 15.f;
		m_bIsZoomStage = true;
		m_pHpBarUI->Set_Active(false);
		m_fMaxHp = 100.f;
		m_fHp = 100.f;
		Change_Weapon(WP_MINIGUN);
		break;
	}

	if (m_bIsZoomStage)
	{
		
	}

	// ui 키기
	m_pPlayerUI->Set_Active(true);
	m_pPlayerUI->Set_RenderOn(true);

	m_pWeaponUI->Set_Active(true);
	m_pWeaponUI->Set_RenderOn(true);

	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	/*if (m_bDead)
		return DEAD;*/


	_vec3 vPos = m_pTransformCom->Get_Info(INFO_POS);

	std::wstring dbg = L"Player Pos: (" +
		std::to_wstring(vPos.x) + L", " +
		std::to_wstring(vPos.y) + L", " +
		std::to_wstring(vPos.z) + L")\n";

	OutputDebugString(dbg.c_str());
	CGameObject::Update_GameObject(fTimeDelta);

	if (m_bIsZoomStage)
	{
		ZoomUpdate(fTimeDelta);
	}

	else
		NormalUpdate(fTimeDelta);

	// collider group 해줌
	CColiderManager::GetInstance()->Add_CollisionGroup(CColiderManager::COLLISION_PLAYER, this);

	// render group에 추가
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this);

	return S_OK;
}

void CPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_bIsZoomStage)
	{
		ZoomLateUpdate(fTimeDelta);
	}

	else
	{
		NormalLateUpdate(fTimeDelta);
	}
		
	__super::LateUpdate_GameObject(fTimeDelta);
}

void CPlayer::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTransformCom->Apply_WorldMatrix();

	//m_pTextureCom->Set_Texture(m_pTextureCom->Get_Frame().m_iCurrentTex);
	//m_pTextureCom->MoveFrame();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

#ifdef _DEBUG
	//if (g_ColiderRender && m_pColliderCom != nullptr)
	//{
	//	m_pColliderCom->Render_ColliderBox(); // 충돌체 디버그 렌더
	//}
	if (g_ColiderRender && m_pColiderSphere != nullptr)
	{
		m_pColiderSphere->Render_ColliderSphere(); // 충돌체 디버그 렌더
	}
#endif
}

/////////////// public func
void CPlayer::Add_Hp(_float _fAddHp)
{
	// test : 최소 체력 0으로 맞춤
	if (m_fHp <= 0)
	{
		m_fHp = 0.f;
		if (_fAddHp < 0)
			return;
	}

	// 체력을 더함
	m_fHp += _fAddHp;

	// maxHp 보다 넘어가면 값 보정
	if (m_fHp >= m_fMaxHp)
		m_fHp = m_fMaxHp;

	// 만약 체력이 0이 되면 state <- PLAYERDEAD
	if (m_fHp <= 0)
	{
		// test : 죽음 방지
		/*Change_State(PLAYERDEAD);
		Change_Move(PMV_NORMAL);*/
	}
}

//////////////////////////// zoom 여부에 따른 update, lateupdate func
_int CPlayer::NormalUpdate(const _float& fTimeDelta)
{
	StateUpdate(m_tPlayerInfo.ePlayerState, fTimeDelta); // curOn -> keyInput

	Move(fTimeDelta); // move : speed 와 현재 y, bFixY 결정

	return NO_EVENT;
}

_int CPlayer::ZoomUpdate(const _float& fTimeDelta)
{
	CUIManager::GetInstance()->Create_AimUI();

	StateUpdateZoom(m_tPlayerInfo.ePlayerState, fTimeDelta);

	return NO_EVENT;
}

void CPlayer::NormalLateUpdate(const _float& fTimeDelta)
{
	// y값 조정 ( fixYf로 이동하지 않을때)
	if (!m_bIsFixY)
	{
		Set_OnTerrain(fTimeDelta);
	}

	// 다른 객체가 this 객체의 pos를 쓰려면 update를 해줘야함
	Update_Position(m_pTransformCom->Get_Info(INFO_POS));

	// 콜라이더 set
	Set_Collider(fTimeDelta);
}

void CPlayer::ZoomLateUpdate(const _float& fTimeDelta)
{
	// jump를 하거나 terrain을 타거나
	Set_OnTerrain(fTimeDelta);

	// 다른 객체가 this 객체의 pos를 쓰려면 update를 해줘야함
	Update_Position(m_pTransformCom->Get_Info(INFO_POS));

	// 콜라이더 set
	Set_ColliderZoom(fTimeDelta);
}
////////////////////////////////////////////////////////////////////

/////////////////////// state util func
void CPlayer::StateNormalSet()
{
	m_bIsKeyInput = false;
	m_bIsInvincible = false;
	m_bIsAttack = false;
	m_bJumping = false;
	m_bIsCountHp = true;

	m_fAddTime = 0.f;
	m_fStateTime = 0.f;

	// 움직임 키
	m_eMoveKey = MVKEY_NORMAL;

	// ui 키기
	m_pHpBarUI->Set_Active(true);
	m_pHpBarUI->Set_RenderOn(true);

	m_pPlayerUI->Set_Active(true);
	m_pPlayerUI->Set_RenderOn(true);

	m_pWeaponUI->Set_Active(true);
	m_pWeaponUI->Set_RenderOn(true);
}

void CPlayer::StateZoomSet()
{
	m_bIsKeyInput = false;
	m_bIsInvincible = false;
	m_bJumping = false;
	m_bIsCountHp = true;
	m_bIsAttack = false;

	m_fAddTime = 0.f;
	m_fStateTime = 0.f;

	//m_fMaxHp = 100.f;

	// 움직임 키
	m_eMoveKey = MVKEY_NON;

	// ui 키기
	m_pHpBarUI->Set_Active(true);
	m_pHpBarUI->Set_RenderOn(true);

	m_pWeaponUI->Set_Active(true);
	m_pWeaponUI->Set_RenderOn(true);

	m_pPlayerUI->Set_Active(false);
	m_pPlayerUI->Set_RenderOn(false);
}

void CPlayer::Set_State_Normal()
{
	Change_State(IDLE);
	Change_Move(PMV_NORMAL);
}

// time count
void CPlayer::CountTime(const _float& fTimeDelta)
{
	// 초당 hp 감소
	if(m_iCurScene != SCENE_BOSS)
		Add_Hp(-1.f * fTimeDelta);

	// dashcooltime 이 0초가 아니라면 cooltime 깎아줌
	if (m_fDashCoolTime != 0)
	{
		m_fDashCoolTime -= fTimeDelta;
		if (m_fDashCoolTime <= 0.f)
			m_fDashCoolTime = 0.f;
	}

	//  attack cool 타임 전해줌
	if (m_fAttackCoolTime != 0)
	{
		m_bIsAttack = false;
		m_fAttackCoolTime -= fTimeDelta;
		if (m_fAttackCoolTime <= 0.f)
		{
			m_fAttackCoolTime = 0.f;
			m_bIsAttack = true;
		}
	}

	// hit 시간 누적
	m_fHitTime += fTimeDelta;
	// 누적 시간이 5초 이상이면
	if (m_fHitTime >= 0.3f)
	{
		// 0초로 초기화
		m_fHitTime = 0.f;
	}

	CUIManager::GetInstance()->Update_CureEff(fTimeDelta);

	// hpbar에게 hp 전해줌
	dynamic_cast<CHpBarUI*>(m_pHpBarUI)->Set_Hp(m_fMaxHp, m_fHp);

	// debug
	OutputDebugString((L"m_fHp: " + std::to_wstring(m_fHp) + L"\n").c_str());

	m_fPlayTime += fTimeDelta;
}

void CPlayer::CountTimeZoom(const _float& fTimeDelta)
{
	// hit 시간 누적
	m_fHitTime += fTimeDelta;
	// 누적 시간이 5초 이상이면
	if (m_fHitTime >= 0.3f)
	{
		// 0초로 초기화
		m_fHitTime = 0.f;
	}

	// hpbar에게 hp 전해줌
	dynamic_cast<CHpBarUI*>(m_pHpBarUI)->Set_Hp(m_fMaxHp, m_fHp);

	// debug
	OutputDebugString((L"m_fHp: " + std::to_wstring(m_fHp) + L"\n").c_str());
}

_bool CPlayer::StateTime_IsEnd(const _float& fTimeDelta, _float fAddTime)
{
	// 누적 시간 더해줌 (초 단위)
	m_fAddTime += fTimeDelta * fAddTime;

	// 누적시간이 스테이트시간 보다 크거나 같으면 return true
	return (m_fAddTime >= m_fStateTime);
}


//////////////////// state func
void CPlayer::Change_State(PLAYERSTATE _eState)
{
	// 이전 state 정리 및 업데이트
	StateEnd(m_tPlayerInfo.ePlayerState);
	m_tPrePlayerInfo.ePlayerState = m_tPlayerInfo.ePlayerState;

	// state 저장
	m_tPlayerInfo.ePlayerState = _eState;

	// 새로은 state 시작
	if (m_bIsZoomStage)
	{
		StateZoomSet();
	}
	else
		StateNormalSet(); // 전체적으로 적용하는 setting
	StateBegin(m_tPlayerInfo.ePlayerState); // 바꾸는 state begin 
}

void CPlayer::StateBegin(PLAYERSTATE _e)
{
	switch (_e) {
	case IDLE:
		IDLE_Begin();break;

	case JUMP:
		JUMP_Begin();break;

	case KICK:
		KICK_Begin();break;

	case ATTACK:
		ATTACK_Begin();break;

	case ATTACK_INSTANT:
		ATTACK_INSTANT_Begin();break;

	case ZOOMING:
		ZOOMING_Begin(); break;

	case ZOOM:
		ZOOM_Begin(); break;

	case RELOAD:
		RELOAD_Begin(); break;

	case DOPING:
		DOPING_Begin();break;

	case OPENING:
		OPENING_Begin();break;

	case PLAYERDEAD:
		PLAYERDEAD_Begin();break;

	case CLEAR:
		Clear_Begin(); break;
		break;

	case ATTEND:
		ATTEND_Begin(); break;

	case ATTACK_ZOOM:
		ATTACK_ZOOM_Begin(); break;

	case ZOOMOUT:
		ZOOMOUT_Begin(); break;
	}

	// 변경된 player info 전달
	CGlobal_Info::Get_Instance()->Set_PlayerInfo(m_tPlayerInfo);
	CGlobal_Info::Get_Instance()->Set_STATE(STATE_ON);
}

void CPlayer::StateEnd(PLAYERSTATE _e)
{
	switch (_e) {
	case IDLE:
		IDLE_End();break;

	case JUMP:
		JUMP_End();break;

	case KICK:\
		KICK_End();break;

	case ATTACK:
		ATTACK_End();break;

	case ATTACK_INSTANT:
		ATTACK_INSTANT_End();break;

	case ZOOMING:
		ZOOMING_End(); break;

	case ZOOM:
		ZOOM_End(); break;

	case RELOAD:
		RELOAD_End();break;

	case DOPING:
		DOPING_End();break;

	case OPENING:
		OPENING_End();break;

	case PLAYERDEAD:
		PLAYERDEAD_End();break;

	case ATTEND:
		ATTEND_End(); break;

	case ATTACK_ZOOM:
		ATTACK_ZOOM_End(); break;

	case ZOOMOUT:
		ZOOMOUT_End(); break;
	}
}

void CPlayer::StateUpdate(PLAYERSTATE _e, const _float& fTimeDelta)
{
	//debug
	OutputDebugString(StateToString(_e));
	OutputDebugString(MoveToString(m_tPlayerInfo.ePlayerMove));

	switch (_e) {
	case IDLE:
		IDLE_On(fTimeDelta);break;

	case JUMP:
		JUMP_On(fTimeDelta);break;

	case KICK:
		KICK_On(fTimeDelta);break;

	case ATTACK:
		ATTACK_On(fTimeDelta);break;

	case ATTACK_INSTANT:
		ATTACK_INSTANT_On(fTimeDelta);break;

	case ZOOMING:
		ZOOMING_On(fTimeDelta); break;

	case ZOOM:
		ZOOM_On(fTimeDelta); break;

	case RELOAD:
		RELOAD_On(fTimeDelta);break;

	case DOPING:
		DOPING_On(fTimeDelta);break;

	case OPENING:
		OPENING_On(fTimeDelta);break;

	case PLAYERDEAD:
		PLAYERDEAD_On(fTimeDelta);break;

	case ATTEND:
		ATTEND_On(fTimeDelta); break;
	}

	if (m_bIsCountHp)
	{
		CountTime(fTimeDelta);
	}
	
	KeyInput(fTimeDelta);
}
// zoom
void CPlayer::StateUpdateZoom(PLAYERSTATE _e, const _float& fTimeDelta)
{
	//debug
	OutputDebugString(StateToString(_e));
	OutputDebugString(MoveToString(m_tPlayerInfo.ePlayerMove));

	switch (_e) {
	case IDLE:
		IDLE_On(fTimeDelta); break;

	case ATTACK:
		ATTACK_On(fTimeDelta); break;

	case ZOOMING:
		ZOOMING_On(fTimeDelta); break;

	case ZOOM:
		ZOOM_On(fTimeDelta); break;

	case OPENING:
		OPENING_On(fTimeDelta); break;

	case PLAYERDEAD:
		PLAYERDEAD_On(fTimeDelta); break;

	case ATTEND:
		ATTEND_On(fTimeDelta); break;

	case ATTACK_ZOOM:
		ATTACK_ZOOM_On(fTimeDelta); break;

	case ZOOMOUT:
		ZOOMOUT_On(fTimeDelta); break;
	}

	if (m_bIsCountHp)
	{
		CountTimeZoom(fTimeDelta);
	}

	KeyInputZoom(fTimeDelta);
}

// idle
void CPlayer::IDLE_Begin()
{
	m_bIsKeyInput = true;
	m_bIsAttack	= true;

	// zoom stage에서 move key 값 설정
	if (m_bIsZoomStage)
	{
		m_eMoveKey = MVKEY_NORMAL;
	}
}

void CPlayer::IDLE_On(const _float& fTimeDelta)
{
}

void CPlayer::IDLE_End()
{

}

// jump
void CPlayer::JUMP_Begin()
{
	Set_Velocity(7.f); // origin : 5.5, test .3
	Set_Jumping(true);
	m_bIsKeyInput = true;
	m_bIsFixY = false;
	//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Sounds\eunbi\player\jump\player.jump-002.wav"
	CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/eunbi/player/jump/player.jump-002.wav", SOUND_PLAYER,3.f, false);
}

void CPlayer::JUMP_On(const _float& fTimeDelta)
{
	// 만약 wall slide에 부딫히면 -> state : idle, move : wall
	/*if (Set_Collider_With_SlideWall())
		return;*/

	if (CColiderManager::GetInstance()->CollisionGroupPush(CColiderManager::COLLISION_WALL_SLIDE, this, CColiderManager::COLLISION_SPHERE_CUBE))
	{
		// state, move 바꿈
		Change_Move(PMV_WALL);
		Change_State(IDLE);

		return;
	}

	if (!m_bJumping)
		Set_State_Normal();

	Gravity(fTimeDelta);
}

void CPlayer::JUMP_End()
{
	Set_Velocity(0.f);
	Set_Jumping(false);
	// "C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Sounds\eunbi\player\jumpend\player.touchGround-002.wav"
	CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/eunbi/player/jumpend/player.touchGround-002.wav", SOUND_PLAYER,3.f, false);
}

// kick
void CPlayer::KICK_Begin()
{
	m_bIsInvincible = true;
	m_fStateTime = 0.3f;
	Change_Move(PMV_NORMAL);

	// "C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Sounds\eunbi\player\kick\sfx_gp_kick_hit_02.wav"
	CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/eunbi/player/kick/sfx_gp_kick_hit_02.wav", SOUND_PLAYER, 2.f,false);
}

void CPlayer::KICK_On(const _float& fTimeDelta)
{
	// player가 시간 관리
	if (StateTime_IsEnd(fTimeDelta))
	{
		Change_State(IDLE);
	}
}

void CPlayer::KICK_End()
{
}

// attack
void CPlayer::ATTACK_Begin()
{
	if(!m_bIsZoomStage)
		m_bIsKeyInput = true;

	m_bIsAttack = false;
}

void CPlayer::ATTACK_On(const _float& fTimeDelta)
{
	if (CGlobal_Info::Get_Instance()->IS_STATE_END())
		Change_State(ATTEND);
}

void CPlayer::ATTACK_End()
{
	// 무한 모드 제외

	// 다음 attack 속도 조절
	//Find_Object
	switch (m_tPlayerInfo.eWeapon)
	{
	case WP_NON:
		break;
	case WP_PISTOL:
		m_fAttackCoolTime = 
			dynamic_cast<CMainWeapon*>(m_pWeaponUI->				// 내 weapon ui의 
			Find_Child_ByTag(TEXT("PistolUI")))->Get_CoolTime();	// 무기의 cooltime을 가져와라
		break;

	case WP_SHOTGUN:
		break;
	case WP_MINIGUN:
		break;

	case WP_KATANA:
		//m_fAttackCoolTime =
		//	dynamic_cast<CMainWeapon*>(m_pWeaponUI->				// 내 weapon ui의 
		//		Find_Child_ByTag(TEXT("KatanaUI")))->Get_CoolTime();	// 무기의 cooltime을 가져와라
		break;
	case WP_SNIPER:
		m_fAttackCoolTime =
			dynamic_cast<CMainWeapon*>(m_pWeaponUI->				// 내 weapon ui의 
				Find_Child_ByTag(TEXT("SniperUI")))->Get_CoolTime();	// 무기의 cooltime을 가져와라
		break;
	}
}

// attack instant
void CPlayer::ATTACK_INSTANT_Begin()
{
	m_bIsInvincible = true;

	m_eMoveKey = MVKEY_NON;
	Change_Move(PMV_NORMAL);
	m_fStateTime = 0.9f;

	m_pHpBarUI->Set_RenderOn(false);
	m_pHpBarUI->Set_Active(false);

	CUIManager::GetInstance()->CreateEffectUI(TEXT("돌진 처치"));

	//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Sounds\eunbi\weapon\knife\instancekill.wav"
	CSound_Manager::GetInstance()->PlaySoundPitch(L"../Bin/Resource/Sounds/eunbi/weapon/knife/instancekill.wav", SOUND_PLAYER, 1.2f, 2.f, false);
}

void CPlayer::ATTACK_INSTANT_On(const _float& fTimeDelta)
{
	if (StateTime_IsEnd(fTimeDelta, 1.f))
		Set_State_Normal();
}

void CPlayer::ATTACK_INSTANT_End()
{
	// 무기를 썼으면 kick(non)으로 바꿔라
	Change_Weapon2(WP_KICK);
	CUIManager::GetInstance()->DestroyItemUI();
}

void CPlayer::ZOOMING_Begin()
{
}

void CPlayer::ZOOMING_On(const _float& fTimeDelta)
{
	if (CGlobal_Info::Get_Instance()->IS_STATE_END())
		Change_State(ZOOM);
}

void CPlayer::ZOOMING_End()
{
}

void CPlayer::ZOOM_Begin()
{
	m_bIsKeyInput = true;
	m_bIsAttack = true;

	if (m_tPlayerInfo.eWeapon == WP_SNIPER)
		m_pHpBarUI->Set_Active(false);

}

void CPlayer::ZOOM_On(const _float& fTimeDelta)
{
}

void CPlayer::ZOOM_End()
{

}

// reload
void CPlayer::RELOAD_Begin()
{
	m_pHpBarUI->Set_RenderOn(false);
	m_pHpBarUI->Set_Active(false);
}

void CPlayer::RELOAD_On(const _float& fTimeDelta)
{
	if (CGlobal_Info::Get_Instance()->IS_STATE_END())
		Set_State_Normal();
}

void CPlayer::RELOAD_End()
{
	return;
}

// doping
void CPlayer::DOPING_Begin()
{
	m_bIsKeyInput = true;
	m_bIsAttack = true;
	Add_Hp(5.f);

	m_fStateTime = 1.f;

	m_pHpBarUI->Set_RenderOn(false);
	m_pHpBarUI->Set_Active(false);

	// hit count reset
	dynamic_cast<CHpBarUI*>(m_pHpBarUI)->HitCount_Reset();

	// effect 추가
	CUIManager::GetInstance()->CreateEffectUI(TEXT("생명 소다"));
	CUIManager::GetInstance()->Create_CureEff();

	//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Sounds\eunbi\object\soda\open.can.wav"
	//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Sounds\eunbi\player\doping\player.sodaDrink.wav"
	CSound_Manager::GetInstance()->PlaySoundPitch(L"../Bin/Resource/Sounds/eunbi/object/soda/open.can.wav", SOUND_PLAYER, 2.f,1.f,false);
}

void CPlayer::DOPING_On(const _float& fTimeDelta)
{
	if (StateTime_IsEnd(fTimeDelta))
		Set_State_Normal();
}

void CPlayer::DOPING_End()
{
	//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Sounds\eunbi\player\doping\player.sodaDrink.wav"
	CSound_Manager::GetInstance()->PlaySoundPitch(L"../Bin/Resource/Sounds/eunbi/player/doping/player.sodaDrink.wav", SOUND_PLAYER, 2.5f, 1.5f, false);

	// 전 무기로 바꿈
	Change_Weapon2(m_tPrePlayerInfo.eWeapon2);
}

// opening
void CPlayer::OPENING_Begin()
{
	m_eMoveKey = MVKEY_NON;
	m_bIsCountHp = false;
	m_bIsInvincible = true;

	m_pPlayerUI->Set_Active(true);

	m_pHpBarUI->Set_RenderOn(false);
	m_pHpBarUI->Set_Active(false);

	//WP_NON, WP_PISTOL, WP_SHOTGUN, WP_RIFLE, WP_KATANA, WP_SNIPER, WP_END
	switch (m_tPlayerInfo.eWeapon)
	{
	case WP_NON:
		m_fStateTime = 1.5f;
		break;
	case WP_PISTOL:
		m_fStateTime = 0.5f;
		break;
	case WP_SHOTGUN:
		m_fStateTime = 0.5f;
		break;
	case WP_MINIGUN:
		m_fStateTime = 0.5f;
		break;

	case WP_KATANA:
		m_fStateTime = 2.f;
		break;
	case WP_SNIPER:
		m_fStateTime = 1.65f;
		break;
	}
}

void CPlayer::OPENING_On(const _float& fTimeDelta)
{
	if (StateTime_IsEnd(fTimeDelta))
		Set_State_Normal();
}

void CPlayer::OPENING_End()
{
}

// dead
void CPlayer::PLAYERDEAD_Begin()
{
	m_eMoveKey = MVKEY_NON;
	//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Sounds\eunbi\player\player levando choque-004.wav"
	CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/eunbi/player/player levando choque-004.wav", SOUND_PLAYER, 1.f,false);
}

void CPlayer::PLAYERDEAD_On(const _float& fTimeDelta)
{

}

void CPlayer::PLAYERDEAD_End()
{
	// m_bDead = true; // 객체 dead 설정
}

void CPlayer::Clear_Begin()
{
	m_bIsCountHp = false;
	m_eMoveKey = MVKEY_NON;
	m_pPlayerUI->Set_RenderOn(false);
	m_pPlayerUI->Set_Active(false);
	m_pHpBarUI->Set_RenderOn(false);
	m_pHpBarUI->Set_Active(false);
	m_pWeaponUI->Set_Active(false);

	CUIManager::GetInstance()->CreateEffectUI(TEXT("승 리"));

	// ui 정리
	CUIManager::GetInstance()->DestroyReloadUI();
	CUIManager::GetInstance()->Destory_PlayerEff_ALL();
	CUIManager::GetInstance()->Destory_CureEff();
	CUIManager::GetInstance()->DestroyItemUI();
	CUIManager::GetInstance()->Destroy_AimUI();
	CUIManager::GetInstance()->DestroyEffectUI();
}

void CPlayer::ATTEND_Begin()
{
	m_bIsAttack = false;

	if (m_tPlayerInfo.eWeapon == WP_SNIPER)
		m_pHpBarUI->Set_RenderOn(false);

}

void CPlayer::ATTEND_On(const _float& fTimeDelta)
{	
	if (CGlobal_Info::Get_Instance()->IS_STATE_END())
	{
		if (m_tPlayerInfo.eWeapon != WP_SNIPER &&
			m_tPrePlayerInfo.ePlayerState == ATTACK_ZOOM)
		{
			Change_State(ZOOM);
		}

		else
			Change_State(IDLE);
	}
		
}

void CPlayer::ATTEND_End()
{
}

void CPlayer::ATTACK_ZOOM_Begin()
{
	m_pHpBarUI->Set_Active(false);

	// sniper는 텍스처 유지라서
	if (m_tPlayerInfo.eWeapon == WP_SNIPER)
		m_fStateTime = 1.f;
}

void CPlayer::ATTACK_ZOOM_On(const _float& fTimeDelta)
{
	if (m_tPlayerInfo.eWeapon == WP_SNIPER &&
		StateTime_IsEnd(fTimeDelta))
	{
		Change_State(ATTEND);
		return;
	}

	else if (CGlobal_Info::Get_Instance()->IS_STATE_END())
		Change_State(ATTEND);
}

void CPlayer::ATTACK_ZOOM_End()
{
}

void CPlayer::ZOOMOUT_Begin()
{
}

void CPlayer::ZOOMOUT_On(const _float& fTimeDelta)
{
	if (CGlobal_Info::Get_Instance()->IS_STATE_END())
		Change_State(IDLE);
}

void CPlayer::ZOOMOUT_End()
{
}

void CPlayer::KeyInput(const _float& fTimeDelta)
{
	// 움직임 키
	switch (m_eMoveKey) {
	case MVKEY_NON:
		break;

	case MVKEY_NORMAL: // 상하좌우
		if (KEY_BUTTON_HOLD(DIK_W))
		{
			m_pTransformCom->Move_Forward(fTimeDelta, m_vPosition.y);
			CTutorialTracker::Get().Notify_Move();
		}

		if (KEY_BUTTON_HOLD(DIK_S))
		{
			m_pTransformCom->Move_Backward(fTimeDelta, m_vPosition.y);
			CTutorialTracker::Get().Notify_Move();
		}
		// break 있으면 안됨
	case MVKEY_LR: // 좌우
		if (KEY_BUTTON_HOLD(DIK_A))
		{
			m_pTransformCom->Move_Left(fTimeDelta, m_vPosition.y);
			CTutorialTracker::Get().Notify_Move();
			// camera state -> left
		}

		if (KEY_BUTTON_HOLD(DIK_D))
		{
			m_pTransformCom->Move_Right(fTimeDelta, m_vPosition.y);
			CTutorialTracker::Get().Notify_Move();
			// camera state -> right
		}
		break;

	case MVKEY_STOP: // idle로 바뀜
		if ((KEY_BUTTON_DOWN(DIK_W)) ||
			(KEY_BUTTON_DOWN(DIK_S)))
		{
			Set_State_Normal();
		}
		break;
	}

	// 상태 전환 키
	if (m_bIsKeyInput) {
		//DIK_LSHIFT
		if (KEY_BUTTON_HOLD(DIK_LSHIFT) &&				//  l-shift hold 시
			m_tPlayerInfo.ePlayerMove != PMV_DASH &&	// 이중 dash 금지
			m_tPlayerInfo.ePlayerMove != PMV_DASHATT &&		// 이중 dash attack 금지
			m_fDashCoolTime == 0 &&						// dash cool time이 0이라면
			m_tPlayerInfo.ePlayerMove != PMV_DASHJUMP)						
		{
			if (m_tPlayerInfo.ePlayerState == JUMP)		// jump후 l-shif -> move : jump dash
			{
				Change_Move(PMV_DASHJUMP);
				Change_State(IDLE);
				m_bIsFixY = true;
				return;
			}
				
			else
			{
				CTutorialTracker::Get().Notify_Dash();
				Change_Move(PMV_DASH);
			}
		}

		//DIK_SPACE
		if (KEY_BUTTON_DOWN(DIK_SPACE) &&
			m_tPlayerInfo.ePlayerState != JUMP) // 이중 jump 금지
		{
			Change_State(JUMP);
			CTutorialTracker::Get().Notify_Jump();
		}

		if (IS_RBUTTON_HOLD &&								// 우클릭 hold 시
			m_tPlayerInfo.ePlayerMove != PMV_DASHATT &&		// 이중 dash attack 금지
			m_tPlayerInfo.ePlayerMove != PMV_DASH &&		// 이중 dash attack 금지
			m_fDashCoolTime == 0 &&							// dash cool time이 지나면
			m_tPlayerInfo.ePlayerState != JUMP &&
			m_tPlayerInfo.ePlayerMove != PMV_SLIDE)				// jump dash 막음
		{
			Change_Move(PMV_DASHATT);
		}

		if (m_tPlayerInfo.ePlayerState != RELOAD &&		// 전에가 reload가 아니고
			m_tPlayerInfo.ePlayerState != JUMP &&
			(m_tPlayerInfo.eWeapon != WP_NON &&			// weapon1이 있을 때
			m_tPlayerInfo.eWeapon != WP_KATANA &&
			m_tPlayerInfo.eWeapon != WP_END ))
		{
			if (KEY_BUTTON_DOWN(DIK_R))
			{
				Change_State(RELOAD);
			}
		}
	}

	// 공격이 가능 할때
	if (m_bIsAttack)
	{
		if (IS_LBUTTON_DOWN) // 좌클릭
		{
			CTutorialTracker::Get().Notify_Fire();
			Change_State(ATTACK);
		}
	}

	//debug
	if (KEY_BUTTON_DOWN(DIK_E))
		Change_State(ATTACK_INSTANT);

	if (KEY_BUTTON_DOWN(DIK_O))
		Change_State(OPENING);
	if (KEY_BUTTON_DOWN(DIK_M))
		Change_Weapon2(WP_DOPING);
	if (KEY_BUTTON_DOWN(DIK_T))
		Change_Weapon(WEAPON::WP_KATANA);


	if (KEY_BUTTON_DOWN(DIK_C))
		Change_Weapon2(WP_KICK);
	if (KEY_BUTTON_DOWN(DIK_V))
		Change_Weapon2(WP_KNIFE);

	if (KEY_BUTTON_DOWN(DIK_Z))
		Change_Weapon(WP_NON);
	if (KEY_BUTTON_DOWN(DIK_X))
		Change_Weapon(WP_PISTOL);
	if (KEY_BUTTON_DOWN(DIK_Q))
		Change_Weapon(WP_SHOTGUN);

	if (KEY_BUTTON_DOWN(DIK_B))
	{
		Change_Weapon(WP_SNIPER);

		m_bIsZoomStage = !m_bIsZoomStage;
	}

}

void CPlayer::KeyInputZoom(const _float& fTimeDelta)
{
	if (m_bIsKeyInput && (IS_RBUTTON_DOWN))
	{
		if (m_tPlayerInfo.ePlayerState == IDLE)
			Change_State(ZOOMING);

		else if (m_tPlayerInfo.ePlayerState == ZOOM)
			Change_State(ZOOMOUT);
	}

	// 자동차씬인 경우 무조건 전진
	if (CMapFactory::GetInstance()->GetTargetSceneIndex() == SCENE_CAR)
	{
		m_pTransformCom->Move_Forward(fTimeDelta, m_vPosition.y);

		// 좌우 키
		if (KEY_BUTTON_HOLD(DIK_A))
		{
			m_pTransformCom->Move_Left(fTimeDelta, m_vPosition.y);
			// camera state -> left
		}
		if (KEY_BUTTON_HOLD(DIK_D))
		{
			m_pTransformCom->Move_Right(fTimeDelta, m_vPosition.y);
			// camera state -> right
		}

		// 좌 클릭시 : attack
		if (m_bIsAttack && IS_LBUTTON_HOLD)
		{
			if (m_tPlayerInfo.ePlayerState == ZOOM)
			{
				Change_State(ATTACK_ZOOM);
			}

			else
			{
				Change_State(ATTACK);
			}
		}
	}

	// sniper scene 움직임
	else {
		// 움직임 키
		switch (m_eMoveKey) {

		case MVKEY_NON:
			break;

		case MVKEY_NORMAL: // 상하좌우

			if (KEY_BUTTON_HOLD(DIK_W))
			{
				m_pTransformCom->Move_Forward(fTimeDelta, m_vPosition.y);
			}

			if (KEY_BUTTON_HOLD(DIK_S))
			{
				m_pTransformCom->Move_Backward(fTimeDelta, m_vPosition.y);
			}

			if (KEY_BUTTON_HOLD(DIK_A))
			{
				m_pTransformCom->Move_Left(fTimeDelta, m_vPosition.y);
				// camera state -> left
			}

			if (KEY_BUTTON_HOLD(DIK_D))
			{
				m_pTransformCom->Move_Right(fTimeDelta, m_vPosition.y);
				// camera state -> right
			}
			break;
		}


		//// 좌 클릭시 : attack
		//if (m_bIsAttack && IS_LBUTTON_DOWN)
		//{
		//	if (m_tPlayerInfo.ePlayerState == ZOOM)
		//	{
		//		Change_State(ATTACK_ZOOM);
		//	}

		//	else
		//	{
		//		Change_State(ATTACK);
		//	}
		//}

		//minigun test
		// 좌 클릭시 : attack
		if (m_bIsAttack && IS_LBUTTON_HOLD)
		{
			if (m_tPlayerInfo.ePlayerState == ZOOM)
			{
				Change_State(ATTACK_ZOOM);
			}

			else
			{
				Change_State(ATTACK);
			}
		}
	}
	

	// debug
	if (KEY_BUTTON_DOWN(DIK_B))
	{
		Change_Weapon(WP_PISTOL);

		m_bIsZoomStage = !m_bIsZoomStage;
	}
	if (KEY_BUTTON_DOWN(DIK_O))
		Change_State(OPENING);
	if (KEY_BUTTON_DOWN(DIK_M))
		Change_Weapon(WP_MINIGUN);
}

////////////////// move func
void CPlayer::Move(const _float& fTimeDelta)
{
	_vec3 vDistance;
	switch (m_tPlayerInfo.ePlayerMove)
	{
	case PMV_NORMAL:
		break;

	case PMV_DASHATT:
		// 만약 slide 벽을 만났다면
		if ((*CGameDataManager::GetInstance()->Get_SortedFloorEntries())[m_pGroundingCom->GetCurrentIndex()].eType == WallType::INCLINE)
		{
			// move state : slide
			Change_Move(PMV_SLIDE);
			return;
		}
		// 만약 좌클릭을 땠따면
		if (IS_RBUTTON_UP)
		{
			// move state : normal
			Set_State_Normal();
			return;
		}

		m_bIsAttack = true;
		m_eMoveKey = MVKEY_STOP;
		Move_Dash(fTimeDelta);
		break;
		
	case PMV_DASH:
	{
		// 만약 L-shift 을 땠따면
		if (KEY_BUTTON_UP(DIK_LSHIFT))
		{
			// move state : normal
			Set_State_Normal();
			return;
		}

		m_bIsAttack = true;
		m_eMoveKey = MVKEY_NON;
		Move_Dash(fTimeDelta);

	}
		break;

	case PMV_SLIDE:
	{
		m_bIsAttack = true;
		m_eMoveKey = MVKEY_NON;
		Move_Slide(fTimeDelta);
	}
		break;

	case PMV_WALL:
	{
		m_bIsAttack = true;
		m_bJumping = true;
		m_eMoveKey = MVKEY_NON;
		Move_Wall(fTimeDelta);
	}
		break;

	case PMV_DASHJUMP:
	{
		// 만약 L-shift 을 땠따면
		if (KEY_BUTTON_UP(DIK_LSHIFT))
		{
			// move state : normal
			Change_Move(PMV_FALL);
			Change_State(IDLE);
			return;
		}

		m_eMoveKey = MVKEY_NON;
		Move_JumpDash(fTimeDelta);
	}
		break;

	case PMV_FALL:
		m_eMoveKey = MVKEY_NORMAL;
		// 속도 점점 빨라지게
		GetTransform()->GetTransformInfo().fSpeed += 5.f * fTimeDelta;
		Move_Fall(fTimeDelta);
		break;

	default:
		break;
	}
}

void CPlayer::Move_Normal(const _float& fTimeDelta)
{
}

void CPlayer::Move_Dash(const _float& fTimeDelta)
{
	// 만약 일정 속도 이하가 되면 -> state: IDLE
	if (m_pTransformCom->GetTransformInfo().fSpeed <= m_fNormalSpeed)
	{
		Change_Move(PMV_NORMAL);
		m_eMoveKey = MVKEY_NORMAL;
		m_fDashCoolTime = 0.5f;
		return;
	}

	// 앞으로 움직여라
	if(m_bIsFixY)
		m_pTransformCom->Move_Forward(fTimeDelta, m_fFixY);
	else
		m_pTransformCom->Move_Forward(fTimeDelta, m_vPosition.y);

	// speed 깎음 (like 마찰력)
	m_pTransformCom->GetTransformInfo().fSpeed -= fTimeDelta * 4.f;
}

void CPlayer::Move_Slide(const _float& fTimeDelta)
{
	// 만약 slide에서 벗어나면
	if ((*CGameDataManager::GetInstance()->Get_SortedFloorEntries())[m_pGroundingCom->GetCurrentIndex()].eType != WallType::INCLINE)
	{
		CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/eunbi/player/slide/player.slide_end02.wav", SOUND_PLAYER, 2.f,false);
		Change_Move(PMV_DASH);
		return;
	}

	// jump 불가능
	if (m_tPlayerInfo.ePlayerState == JUMP)
	{
		Change_State(IDLE);
		return;
	}

	// 앞으로 움직여라
	m_pTransformCom->Move_Forward(fTimeDelta, m_vPosition.y);

	//w 키 누르면 더 빨라짐
	if (KEY_BUTTON_HOLD(DIK_W))
		m_pTransformCom->GetTransformInfo().fSpeed += fTimeDelta * 2.f;
}

void CPlayer::Move_Wall(const _float& fTimeDelta)
{
	m_pTransformCom->Move_PosDir(fTimeDelta, {0.f,0.f,1.f});
}

void CPlayer::Move_JumpDash(const _float& fTimeDelta)
{
	m_pTransformCom->Move_Forward(fTimeDelta, m_fFixY);
}

void CPlayer::Move_Fall(const _float& fTimeDelta)
{
	// 만약 터레인에 있다면
	if (Is_OnTerrain())
	{
		Set_State_Normal();
		return;
	}

	_float Y = 0;
	GetTransform()->Move_YDown(fTimeDelta,0.f,false, Y);
}

/////////////////// change weapons
void CPlayer::Change_Weapon(WEAPON _eWeapon)
{
	// 상태 업데이트
	m_tPrePlayerInfo.eWeapon = m_tPlayerInfo.eWeapon; // 전 state 저장
	m_tPlayerInfo.eWeapon = _eWeapon; // state 업데이트
	CGlobal_Info::Get_Instance()->Set_PlayerInfo(m_tPlayerInfo); // global에게도 정보 업데이트

	// state는 opening으로 넘어감
	Change_State(OPENING);
	Change_Move(PMV_NORMAL);
}

void CPlayer::Change_Weapon2(WEAPON2 _eWeapon2)
{
	m_tPrePlayerInfo.eWeapon2 = m_tPlayerInfo.eWeapon2; // 전 state 저장
	m_tPlayerInfo.eWeapon2 = _eWeapon2; // state 업데이트
	CGlobal_Info::Get_Instance()->Set_PlayerInfo(m_tPlayerInfo); // global에게도 정보 업데이트

	// 만약 wp2가 doping이면 state : doping
	if (m_tPlayerInfo.eWeapon2 == WP_DOPING)
		Change_State(DOPING);

	// 충돌때 생성
	// change 후 destroy
	if (m_tPlayerInfo.eWeapon2 == WP_KNIFE)
	{
		CUIManager::GetInstance()->CreateItemUI();
		//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Sounds\eunbi\weapon\knife\sfx_gp_int_item_grab_01.wav"
		CSound_Manager::GetInstance()->PlaySoundPitch(L"../Bin/Resource/Sounds/eunbi/weapon/knife/sfx_gp_int_item_grab_01.wav", SOUND_PMOVE, 2.f, 1.5f,false);
	}
		
}

void CPlayer::Change_Move(PLAYERMOVE ePlayerMove, _bool bYFix)
{
	// dash effect 제거
	CUIManager::GetInstance()->Destory_PlayerEff(PLAYEREFF::DASH); // dash effect 제거

	// 상태 업데이트
	m_tPrePlayerInfo.ePlayerMove = m_tPlayerInfo.ePlayerMove; // 전 state 저장
	m_tPlayerInfo.ePlayerMove = ePlayerMove; // state 업데이트
	CGlobal_Info::Get_Instance()->Set_PlayerInfo(m_tPlayerInfo); // global에게도 정보 업데이트

	// 바뀔때 y값 저장해옴 m_bYFix의 값에 따라 쓸래말래 결정
	m_fFixY = GetTransform()->Get_Info(INFO_POS).y;
	m_bIsFixY = bYFix;

	// 속도를 조정
	switch (ePlayerMove)
	{
	case PMV_NORMAL:
		GetTransform()->GetTransformInfo().fSpeed = m_fNormalSpeed;
		CSound_Manager::GetInstance()->StopSound(SOUND_PMOVE);
		break;

	case PMV_DASHATT:
		GetTransform()->GetTransformInfo().fSpeed = m_fNormalSpeed + 5.f;
		CUIManager::GetInstance()->Create_PlayerEff(PLAYEREFF::DASH); // dash effect 추가

		//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Sounds\eunbi\player\dash\player.dash-004.wav"
		CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/eunbi/player/dash/player.dash-004.wav", SOUND_PMOVE,2.f, false);

	break;

	case PMV_DASH:
		// 만약 전 state가 slide였다면 speed 좀 줄여줌
		if (m_tPrePlayerInfo.ePlayerMove == PMV_SLIDE)
			GetTransform()->GetTransformInfo().fSpeed = m_fNormalSpeed + 1.f;
		else
			GetTransform()->GetTransformInfo().fSpeed = m_fNormalSpeed + 5.f;

		CUIManager::GetInstance()->Create_PlayerEff(PLAYEREFF::DASH); // dash effect 추가

		//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Sounds\eunbi\player\dash\player.dash-004.wav"
		CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/eunbi/player/dash/player.dash-004.wav", SOUND_PMOVE, 2.f,false);


	break;

	case PMV_SLIDE:
		GetTransform()->GetTransformInfo().fSpeed = m_fNormalSpeed + 3.f;
		CUIManager::GetInstance()->Create_PlayerEff(PLAYEREFF::DASH); // dash effect 추가

		//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Sounds\eunbi\player\slide\player.slide_in02.wav"
		CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/eunbi/player/slide/player.slide_loop02.wav", SOUND_PMOVE, 2.f, false);

	break;
	
	case PMV_WALL:
		GetTransform()->GetTransformInfo().fSpeed = m_fNormalSpeed + 2.f;
		m_bIsFixY = true;
		break;

	case PMV_DASHJUMP:
		GetTransform()->GetTransformInfo().fSpeed = m_fNormalSpeed + 5.f;
		CUIManager::GetInstance()->Create_PlayerEff(PLAYEREFF::DASH); // dash effect 추가
		m_bIsFixY = true;
		CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/eunbi/player/slide/player.slide_loop02.wav", SOUND_PMOVE, 2.f, false);

		break;

	case PMV_FALL:
		GetTransform()->GetTransformInfo().fSpeed = m_fNormalSpeed;
		m_bIsFixY = true; // set onterrain 방지
		CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/eunbi/player/slide/player.slide_loop02.wav", SOUND_PMOVE, 2.f, true);

		break;

	break;
	}
}

HRESULT CPlayer::Set_Component()
{
	// Collider_Sphere
	_uint iSceneIndex = CMapFactory::GetInstance()->GetTargetSceneIndex();
	
 	CColider_Sphere::COLLINFO CollSphereInfo;
	ZeroMemory(&CollSphereInfo, sizeof(CColider_Sphere::COLLINFO));
	if (iSceneIndex == SCENE_CAR)
	{
		CollSphereInfo.fRadius = 2.f;
	}
	else
	{
		CollSphereInfo.fRadius = 0.4f;
	}
	CollSphereInfo.vOffset = _vec3(0.f, -0.3f, 0.f);    // 중심 오프셋 없음

	// Colider_Sphere
	if (FAILED(Add_Components(L"Com_Collider_Sphere", SCENE_STATIC, L"Proto_Colider_Sphere", (CComponent**)&m_pColiderSphere, &CollSphereInfo)))
		return E_FAIL;
	m_pColiderSphere->Set_Transform(m_pTransformCom);

	return S_OK;
}

void CPlayer::Set_Collider(const _float& fTimeDelta)
{
	// 구 충돌
	m_pColiderSphere->Update_ColliderSphere();

	Set_Collider_With_Clear();
	Set_Collider_With_Wall();
	Set_Collider_With_Door();
	Set_Colllider_With_Monster(fTimeDelta);
	Set_Collider_With_SpecialTile();
	Set_Collider_With_Item();

	Set_Collider_With_SlideWall();
	Set_Collider_With_Bullet(fTimeDelta);
}

void CPlayer::Set_ColliderZoom(const _float& fTimeDelta)
{
	// 구 충돌
	m_pColiderSphere->Update_ColliderSphere();

	Set_Collider_With_Clear();
	Set_Collider_With_Wall();
	Set_Collider_With_Door();
	Set_Colllider_With_Monster(fTimeDelta);
	Set_Collider_With_Bullet(fTimeDelta);
}

_float CPlayer::CosRadian(_vec3 v1, _vec3 v2)
{
	D3DXVec3Normalize(&v1, &v1);
	D3DXVec3Normalize(&v2, &v2);
	return D3DXVec3Dot(&v1, &v2); //cos세타
}

void CPlayer::PushBack(_vec3 vDistance)
{
	GetTransform()->Set_Info(INFO_POS, Get_Pos() += vDistance);
}

void CPlayer::HitFromObject(const _float& fTimeDelta,_float fHit)
{
	// hit누적 time이 0일때만
	if (m_fHitTime == 0)
	{
		// hp 깎기
		Add_Hp(fHit * -1.f);
		// hit count 증가
		dynamic_cast<CHpBarUI*>(m_pHpBarUI)->HitCount_Up();

		// effect 추가
		CUIManager::GetInstance()->Create_PlayerEff(PLAYEREFF::BLOODR);

		//"C:\Users\Eunbi\jusin\teamProj\SR\project\MadMulletJack\Client\Bin\Resource\Sounds\eunbi\player\MadJack_BonusLine_106_a.wav"
		CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/eunbi/player/MadJack_BonusLine_106_a.wav", SOUND_PLAYER, 2.f, false);
	}
}

void CPlayer::Set_Collider_With_Clear()
{
	if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_DUMMY, this, CColiderManager::COLLISION_SPHERE_CUBE, nullptr))
	{
		CUIManager::GetInstance()->CreateClearUI();
		m_pColiderSphere->Set_Active(false);
		Change_State(CLEAR);
		Change_Move(PMV_NORMAL);
	}
}

void CPlayer::Set_Collider_With_Wall()
{
	if (CColiderManager::GetInstance()->CollisionGroupPush(CColiderManager::COLLISION_HORWALL, 
		this, CColiderManager::COLLISION_SPHERE_CUBE,1.f))
	{
	}
	if (CColiderManager::GetInstance()->CollisionGroupPush(CColiderManager::COLLISION_VERWALL, 
		this, CColiderManager::COLLISION_SPHERE_CUBE, 1.f))
	{
	}

	if (CColiderManager::GetInstance()->CollisionGroupPush(CColiderManager::COLLISION_CEILING, this, CColiderManager::COLLISION_SPHERE_CUBE, -0.01f))
	{
		Set_Velocity(Get_Velocity() * -1.f);
	}

}

void CPlayer::Set_Collider_With_Door()
{
	if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_DOOR, this, CColiderManager::COLLISION_SPHERE, nullptr))
	{
		// kick 모션 나오게
		Change_State(KICK);
		CTutorialTracker::Get().Notify_Door();
	}
}

void CPlayer::Set_Colllider_With_Monster(const _float& fTimeDelta)
{
	// effect 삭제
	CUIManager::GetInstance()->Destory_PlayerEff(PLAYEREFF::BLOODR);

	CGameObject* pColiObj;
	_vec3 vDistance;
	if (CColiderManager::GetInstance()->CollisionGroupWho(CColiderManager::COLLISION_MONSTER, this, CColiderManager::COLLISION_SPHERE, &vDistance, pColiObj))
	{
		//몬스터와 앞에서 충돌했을때만 attack 가능 -> 나머지 hit
		if (!m_bIsInvincible && pColiObj) // 무적이 아니고 몬스터가 있을때
		{
			// monster pos
			_vec3 vMonPos = pColiObj->GetTransform()->Get_Info(INFO_POS);
			// 내가 몬스터를 바라보는 방향벡터
			_vec3 vDir = vMonPos - Get_Pos();
			// 정규화 후 내적
			_float fDot = CosRadian(vDir, Get_Look());

			// 내적 결과가 0 ~90도 이면 -> 앞에
			// 만약 앞에 있다면
			if (fDot >= 0)
			{
				// Dash_attack 중일때 몬스터와 충돌하면 
				if (m_tPlayerInfo.ePlayerMove == PMV_DASHATT)
				{
					PushBack(vDistance);

					// wap2에 따라 state 변경
					switch (m_tPlayerInfo.eWeapon2)
					{
					case WP_KICK:
						Change_State(KICK);
						break;

					case WP_KNIFE:
					case WP_BOOK:
						CTutorialTracker::Get().Notify_Finish();
						Change_State(ATTACK_INSTANT);
						break;
					}
				}

				// Dash attack이 아니면 hit || push
				else
				{
					if (dynamic_cast<CMonster_Dron *>(pColiObj))
					{
						_int iSceneIndex = CMapFactory::GetInstance()->GetTargetSceneIndex();
						if (iSceneIndex == SCENE_CAR)
						{
							dynamic_cast<CMonster_Dron*>(pColiObj)->QueueDeathUI(false);
							dynamic_cast<CMonster_Dron*>(pColiObj)->TrySpawnDeathUI_Common();
							CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/enemyDrone.death-002", SOUND_MONSTER, 0.1f, false);
							CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/explosions-001", SOUND_MONSTER, 1.f, false);
							pColiObj->Set_Dead(TRUE);
							return;
						}
						else
							HitFromObject(fTimeDelta, 1.f);
					}
					PushBack(vDistance);
				}
			}

			// 앞에 없다면 hit || push
			else
			{
				if (dynamic_cast<CMonster_Dron *>(pColiObj))
				{
					_int iSceneIndex = CMapFactory::GetInstance()->GetTargetSceneIndex();
					if (iSceneIndex == SCENE_CAR)
					{
						pColiObj->Set_Dead(TRUE);
						return;
					}
					else
						HitFromObject(fTimeDelta, 1.f);
				}
				PushBack(vDistance);
			}
		}
	}
}

void CPlayer::Set_Collider_With_SlideWall()
{
	if (m_bJumping &&
		(CColiderManager::GetInstance()->CollisionGroupPush
		(CColiderManager::COLLISION_WALL_SLIDE, this, CColiderManager::COLLISION_SPHERE_CUBE)))
	{
		// state, move 바꿈
		Change_Move(PMV_WALL);
		Change_State(IDLE);
	}

	else if (m_tPlayerInfo.ePlayerMove == PMV_WALL &&
		!(CColiderManager::GetInstance()->CollisionGroup
		(CColiderManager::COLLISION_WALL_SLIDE, this, CColiderManager::COLLISION_SPHERE_CUBE, nullptr)))
	{
		Change_Move(PMV_NORMAL);
		Change_State(JUMP);
	}

	return;
}

void CPlayer::Set_Collider_With_Item()
{
	CGameObject* pColliObj{ nullptr };
	//나중에 item으로 바꿔야함 test
	if (CColiderManager::GetInstance()->CollisionGroupWho(CColiderManager::COLLISIOIN_ITEM, this, CColiderManager::COLLISION_SPHERE,nullptr, pColliObj))
	{
		if (pColliObj)
		{
			WEAPON2 wp2 = dynamic_cast<CItem*>(pColliObj)->Get_ItemInfo().eWeapon;

			// 중복 적용 방지
			if (m_tPlayerInfo.eWeapon2 == wp2)
			{
				return;
			}
			else
			{
				pColliObj->Set_Dead(true);
				Change_Weapon2(wp2);
			}
			
		}
	}
}

_bool CPlayer::Set_Collider_With_SpecialTile()
{
	if (m_tPlayerInfo.ePlayerMove == PMV_DASHATT &&
		CColiderManager::GetInstance()->CollisionGroupPush(CColiderManager::COLLISION_TILE_ELECTRIC, this, CColiderManager::COLLISION_SPHERE, 1.f))
	{
	
		// state : kick
		Change_State(KICK);
		Change_Move(PMV_NORMAL);

		return true;
	}

	return false;
}

void CPlayer::Set_Collider_With_Bullet(const _float& fTimeDelta)
{
	CGameObject *pColliObj{ nullptr };
 	CUIManager::GetInstance()->Destory_PlayerEff(PLAYEREFF::BLOODR);
	//나중에 item으로 바꿔야함 test
	if (CColiderManager::GetInstance()->CollisionGroupWho(CColiderManager::COLLISION_BULLET, this, CColiderManager::COLLISION_SPHERE, nullptr,pColliObj))
	{
		if (!pColliObj) // 예외처리
			return;

		// 내 bullet이 아닐때만
		if (dynamic_cast<CBullet*>(pColliObj)->Get_OwnerType() != BulletData::OWNER::PLAYER)
		{
			pColliObj->Set_Dead(true); // bullet dead 처리
			HitFromObject(fTimeDelta, 1.f);
		}
	}

	pColliObj = nullptr;
	// 미사일
	if (CColiderManager::GetInstance()->CollisionGroupWho(CColiderManager::COLLISION_MISSILE, this, CColiderManager::COLLISION_SPHERE, nullptr, pColliObj))
	{
		if (!pColliObj) // 예외처리
			return;

		pColliObj->Set_Dead(true); // bullet dead 처리
		HitFromObject(fTimeDelta, 5.f);
	}
}

HRESULT CPlayer::Texture_Clone()
{
	return S_OK;
}

HRESULT CPlayer::Change_Texture(const _tchar* LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_Zero_Frame();
	return S_OK;
}

HRESULT CPlayer::Set_UI()
{
	_uint iSceneIndex = CMapFactory::GetInstance()->GetTargetSceneIndex();

	// weapon ui
	m_pWeaponUI = dynamic_cast<CUIBase*>(
		CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_WeaponManagerUI", iSceneIndex, L"UI_Layer"));
	if (m_pWeaponUI == nullptr)
		return E_FAIL;

	// player ui
	m_pPlayerUI = dynamic_cast<CUIBase*>(
		CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_PlayerUI", iSceneIndex, L"UI_Layer"));
	if (m_pPlayerUI == nullptr)
		return E_FAIL;

	// hpbar ui
	m_pHpBarUI = dynamic_cast<CUIBase*>(
		CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_HpbarUI", iSceneIndex, L"UI_Layer"));
	if (m_pHpBarUI == nullptr)
		return E_FAIL;

	return S_OK;
}

_vec3 CPlayer::Get_Pos()
{
	return (m_pTransformCom->Get_Info(INFO_POS));
}

_vec3 CPlayer::Get_Look()
{
	return (m_pTransformCom->Get_Info(INFO_LOOK));
}

_vec3 CPlayer::Get_Right()
{
	return (m_pTransformCom->Get_Info(INFO_RIGHT));
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer* pPlayer = new CPlayer(pGraphicDev);

	if (FAILED(pPlayer->Ready_GameObject()))
	{
		Safe_Release(pPlayer);
		MSG_BOX("pPlayer Create Failed");
		return nullptr;
	}
	return pPlayer;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("pPlayer Clone Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();
}

//debug
const TCHAR* CPlayer::StateToString(PLAYERSTATE eState)
{
	switch (eState)
	{
	case IDLE: return TEXT("State: IDLE\n");
	case JUMP: return TEXT("State: JUMP\n");
	case KICK: return TEXT("State: KICK\n");
	case ATTACK: return TEXT("State: ATTACK\n");
	case ATTACK_INSTANT: return TEXT("State: ATTACK_INSTANT\n");
	case RELOAD: return TEXT("State: RELOAD\n");
	case DOPING: return TEXT("State: DOPING\n");
	case OPENING: return TEXT("State: OPENING\n");
	case PLAYERDEAD: return TEXT("State: PLAYERDEAD\n");
	case CLEAR: return TEXT("State: CLEAR\n");
	case ATTEND: return TEXT("State: ATTEND\n");
	default: return TEXT("State: UNKNOWN\n");
	}
}

const TCHAR* CPlayer::MoveToString(PLAYERMOVE eMove)
{
	switch (eMove)
	{
	case PMV_NORMAL: return TEXT("Move: Normal\n");
	case PMV_DASH: return TEXT("Move: Dash\n");
	case PMV_DASHATT: return TEXT("Move: DashAtt\n");
	case PMV_SLIDE: return TEXT("Move: Slide\n");
	case PMV_WALL: return TEXT("Move: Wall\n");
	case PMV_DASHJUMP: return TEXT("Move: JumpDash\n");
	case PMV_END: return TEXT("Move: Unknown\n");
	case PMV_FALL: return TEXT("Move: Fall\n");
	}

	return nullptr;
}