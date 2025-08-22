#include "pch.h"
#include "CPlayer.h"
#include "CRenderer.h"
#include "CColiderManager.h"
#include "CTimerMgr.h"
#include "CObjectManager.h"
#include "CUIBase.h"
#include "CDInputMgr.h"
#include "CGlobal_Info.h"
#include "CMapFactory.h"
#include "CPlayer_HandR.h"
#include "CPlayer_HandL.h"
#include "CVIBuffer_GridPanelBase.h"
#include "CGameDataManager.h"
#include "CGrounding.h"
#include "CPlayer_Arm.h"
#include "CPlayer_Foot.h"
#include "CHpBarUI.h"
#include "CMan_HpBarUI.h"
#include "CPhone_HpBarUI.h"
#include "CUIManager.h"
#include "CManagement.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCharacter(pGraphicDev), m_tPlayerInfo({ OPENING, PMV_NORMAL, WP_PISTOL ,WP_KNIFE }), m_tPrePlayerInfo({ PLAYER_END ,PMV_END, WP_END,WP2_END }),
	m_TimerTag(TEXT("")), m_fGround_Height(0.f), m_eMoveKey(MVKEY_END),
	m_bIsKeyInput(true), m_bIsInvincible(true), m_bIsAttack(true), m_bIsCountHp(false),
	m_fHitTime(0.f), m_fNormalSpeed(0.f), m_fFixY(0.f), m_bIsFixY(false)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CCharacter(rhs), m_tPlayerInfo(rhs.m_tPlayerInfo), m_tPrePlayerInfo(rhs.m_tPrePlayerInfo),
	m_TimerTag(rhs.m_TimerTag), m_fGround_Height(rhs.m_fGround_Height), m_eMoveKey(rhs.m_eMoveKey),
	m_bIsKeyInput(rhs.m_bIsKeyInput), m_bIsInvincible(rhs.m_bIsInvincible), m_bIsAttack(rhs.m_bIsAttack)
	, m_bIsCountHp(rhs.m_bIsCountHp), m_fHitTime(rhs.m_fHitTime), m_fNormalSpeed(rhs.m_fNormalSpeed), 
	m_fFixY(rhs.m_fFixY), m_bIsFixY(rhs.m_bIsFixY)
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

	if (FAILED(Set_PlayerUI()))
		return E_FAIL;

	// StartPosition 설정
	if (MAPOBJECTDATA* p = reinterpret_cast<MAPOBJECTDATA*>(pArg))
	{
		GetTransform()->Set_Info(INFO::INFO_RIGHT, p->transform.Right);
		GetTransform()->Set_Info(INFO::INFO_UP, p->transform.Up);
		GetTransform()->Set_Info(INFO::INFO_LOOK, p->transform.Look);
		GetTransform()->Set_Info(INFO::INFO_POS, p->transform.Pos);
	}

	if (FAILED(Set_HpBarUI()))
		return E_FAIL;

	GetTransform()->Set_Scale(1.f, 100.f, 1.f);

	m_fHp = 10.f; // 플레이어 목숨 초 -> origin : 10, test : 3

	m_fNormalSpeed = 5.f;

	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	/*if (m_bDead)
		return DEAD;*/
	_float fY = m_pTransformCom->Get_Info(INFO_POS).y;
	CGameObject::Update_GameObject(fTimeDelta);

	// state change & update
	ChangeState(m_tPlayerInfo.ePlayerState); // preEnd -> setNormal -> curBegin
	Move(m_tPlayerInfo.ePlayerMove); // move : speed 와 현재 y, bFixY 결정
	StateUpdate(m_tPlayerInfo.ePlayerState, fTimeDelta); // curOn -> keyInput

	// collider group 해줌
	CColiderManager::GetInstance()->Add_CollisionGroup(CColiderManager::COLLISION_PLAYER, this);

	// render group에 추가
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(RENDER_NONALPHA, this);
	
	return S_OK;
}

void CPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{

	Set_OnTerrain(fTimeDelta);

	Update_Position(m_pTransformCom->Get_Info(INFO_POS));

	// 콜라이더 set
	Set_Collider(fTimeDelta);

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

void CPlayer::Add_Hp(_float _fAddHp)
{
	// 체력을 더함
	m_fHp += _fAddHp;

	// maxHp 보다 넘어가면 값 보정
	if (m_fHp >= m_fMaxHp)
		m_fHp = m_fMaxHp;

	// 만약 체력이 0이 되면 state <- PLAYERDEAD
	if (m_fHp <= 0)
	{
		// 죽음 끄기
		//m_tPlayerInfo.ePlayerState = PLAYERDEAD;
	}
}

void CPlayer::ChangeState(PLAYERSTATE _e)
{
	if (m_tPrePlayerInfo.ePlayerState == _e)
		return;

	StateEnd(m_tPrePlayerInfo.ePlayerState);
	StateNormalSet(); // 전체적으로 적용하는 setting
	StateBegin(_e);
	m_tPrePlayerInfo.ePlayerState = _e;
}


// IDLE, JUMP, DASH_ATTACK, DASH, SLIED, KICK, ATTACK,
// ATTACK_INSTANT, RELOAD, HIT, DOPING, WALL, OPENING, PLAYERDEAD, PLAYER_END
void CPlayer::StateBegin(PLAYERSTATE _e)
{
	switch (_e) {
	case IDLE:
		IDLE_Begin();break;
	case JUMP:
		JUMP_Begin();break;
	case DASH_ATTACK:
		DASH_ATTACK_Begin();break;
	case DASH:
		DASH_Begin();break;
	case SLIED:
		SLIED_Begin();break;
	case KICK:
		KICK_Begin();break;
	case ATTACK:
		ATTACK_Begin();break;
	case ATTACK_INSTANT:
		ATTACK_INSTANT_Begin();break;
	case RELOAD:
		RELOAD_Begin();break;
	case DOPING:
		DOPING_Begin();break;
	case WALL:
		WALL_Begin();break;
	case OPENING:
		OPENING_Begin();break;
	case PLAYERDEAD:
		PLAYERDEAD_Begin();break;
	case CLEAR:
		Clear_Begin(); break;
		break;
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
	case DASH_ATTACK:
		DASH_ATTACK_End();break;
	case DASH:
		DASH_End();break;
	case SLIED:
		SLIED_End();break;
	case KICK:
		KICK_End();break;
	case ATTACK:
		ATTACK_End();break;
	case ATTACK_INSTANT:
		ATTACK_INSTANT_End();break;
	case RELOAD:
		RELOAD_End();break;
	case DOPING:
		DOPING_End();break;
	case WALL:
		WALL_End();break;
	case OPENING:
		OPENING_End();break;
	case PLAYERDEAD:
		PLAYERDEAD_End();break;
	}
}

void CPlayer::StateUpdate(PLAYERSTATE _e, const _float& fTimeDelta)
{
	OutputDebugString(StateToString(_e));

	switch (_e) {
	case IDLE:
		IDLE_On(fTimeDelta);break;
	case JUMP:
		JUMP_On(fTimeDelta);break;
	case DASH_ATTACK:
		DASH_ATTACK_On(fTimeDelta);break;
	case DASH:
		DASH_On(fTimeDelta);break;
	case SLIED:
		SLIED_On(fTimeDelta);break;
	case KICK:
		KICK_On(fTimeDelta);break;
	case ATTACK:
		ATTACK_On(fTimeDelta);break;
	case ATTACK_INSTANT:
		ATTACK_INSTANT_On(fTimeDelta);break;
	case RELOAD:
		RELOAD_On(fTimeDelta);break;
	case DOPING:
		DOPING_On(fTimeDelta);break;
	case WALL:
		WALL_On(fTimeDelta);break;
	case OPENING:
		OPENING_On(fTimeDelta);break;
	case PLAYERDEAD:
		PLAYERDEAD_On(fTimeDelta);break;
	}

	if (m_bIsCountHp)
	{
		CountHp(fTimeDelta);
	}
	
	KeyInput(fTimeDelta);
}

void CPlayer::StateNormalSet()
{
	m_bIsKeyInput		= false;
	m_bIsInvincible = false;
	m_bIsAttack		= false;
	m_bJumping		= false;
	m_bIsCountHp = true;

	m_fAddTime = 0.f;
	m_fStateTime = 0.f;

	m_eMoveKey = MVKEY_NORMAL;

	m_pTransformCom->GetTransformInfo().fSpeed = m_fNormalSpeed;

	m_pHpBarUI->Set_Active(true);
	m_pHpBarUI->Set_RenderOn(true);

	m_pPlayerUI->Set_Active(true);
	m_pPlayerUI->Set_RenderOn(true);
}

// idle
void CPlayer::IDLE_Begin()
{
	m_bIsKeyInput = true;
	m_bIsAttack	= true;
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
	Set_Velocity(5.5f);
	Set_Jumping(true);
}

void CPlayer::JUMP_On(const _float& fTimeDelta)
{
	if (!m_bJumping)
		Set_State_Normal();

	Gravity(fTimeDelta);
}

void CPlayer::JUMP_End()
{
	m_bIsKeyInput = true;
	Set_Velocity(0.f);
	Set_Jumping(false);
}

// dash attack
void CPlayer::DASH_ATTACK_Begin()
{
	m_eMoveKey = MVKEY_STOP;
	m_bIsKeyInput = true;
	m_bIsAttack = true;
	m_pTransformCom->GetTransformInfo().fSpeed = 15.f;
}

void CPlayer::DASH_ATTACK_On(const _float& fTimeDelta)
{
	if ((*CGameDataManager::GetInstance()->Get_SortedFloorEntries())[m_pGroundingCom->GetCurrentIndex()].eType == WallType::INCLINE)
	{
		m_tPlayerInfo.ePlayerState = SLIED;
	}
	// 만약 일정 속도 이하가 되면 -> state: IDLE
	if (m_pTransformCom->GetTransformInfo().fSpeed <= 1.f)
		Set_State_Normal();

	// 앞으로 움직여라
	m_pTransformCom->Move_Forward(fTimeDelta, m_vPosition.y);

	// speed 깎음 (like 마찰력)
	m_pTransformCom->GetTransformInfo().fSpeed -= fTimeDelta * 9.f;
}

void CPlayer::DASH_ATTACK_End()
{

}

// dash
void CPlayer::DASH_Begin()
{
	m_eMoveKey = MVKEY_STOP;
	m_bIsKeyInput = true;
	m_bIsAttack = true;
	m_pTransformCom->GetTransformInfo().fSpeed = 15.f;
}

void CPlayer::DASH_On(const _float& fTimeDelta)
{
	if ((*CGameDataManager::GetInstance()->Get_SortedFloorEntries())[m_pGroundingCom->GetCurrentIndex()].eType == WallType::WALL_SLIDE)
	{
		m_tPlayerInfo.ePlayerState = SLIED;
	}
	// 만약 일정 속도 이하가 되면 -> state: IDLE
	if (m_pTransformCom->GetTransformInfo().fSpeed <= 1.f)
		Set_State_Normal();

	// 앞으로 움직여라
	m_pTransformCom->Move_Forward(fTimeDelta, m_vPosition.y);

	// speed 깎음 (like 마찰력)
	m_pTransformCom->GetTransformInfo().fSpeed -= fTimeDelta * 9.f;
}

void CPlayer::DASH_End()
{

}

// slide
void CPlayer::SLIED_Begin()
{
	m_eMoveKey = MVKEY_LR;
	m_bIsAttack = true;
}

void CPlayer::SLIED_On(const _float& fTimeDelta)
{
	if ((*CGameDataManager::GetInstance()->Get_SortedFloorEntries())[m_pGroundingCom->GetCurrentIndex()].eType != WallType::INCLINE)
	{
		Set_State_Normal();
	}

	// 앞으로 움직여라
	m_pTransformCom->Move_Forward(fTimeDelta, m_vPosition.y);
	
	if (KEY_BUTTON_HOLD(DIK_W))
		m_pTransformCom->GetTransformInfo().fSpeed += fTimeDelta * 2.f;
}

void CPlayer::SLIED_End()
{
}

// kick
void CPlayer::KICK_Begin()
{
	m_fStateTime = 0.5f;
	m_bIsInvincible = true;
	m_pTransformCom->GetTransformInfo().fSpeed = 7.f;
}

void CPlayer::KICK_On(const _float& fTimeDelta)
{
	if (StateTime_IsEnd(fTimeDelta, 1.f))
		Set_State_Normal();
}

void CPlayer::KICK_End()
{
}

// attack
void CPlayer::ATTACK_Begin()
{
	m_bIsKeyInput = true;
}

void CPlayer::ATTACK_On(const _float& fTimeDelta)
{
	if (CGlobal_Info::Get_Instance()->IS_STATE_END())
		Set_State_Normal();
}

void CPlayer::ATTACK_End()
{
}

// attack instant
void CPlayer::ATTACK_INSTANT_Begin()
{
	m_bIsInvincible = true;

	m_eMoveKey = MVKEY_NON;
	m_fStateTime = 1.2f;

	m_pHpBarUI->Set_RenderOn(false);
	m_pHpBarUI->Set_Active(false);
}

void CPlayer::ATTACK_INSTANT_On(const _float& fTimeDelta)
{
	if (StateTime_IsEnd(fTimeDelta, 1.f))
		Set_State_Normal();
}

void CPlayer::ATTACK_INSTANT_End()
{
}

void CPlayer::ZOOMING_Begin()
{
	m_bIsInvincible = true;
}

void CPlayer::ZOOMING_On(const _float& fTimeDelta)
{
}

void CPlayer::ZOOMING_End()
{
}

void CPlayer::ZOOM_Begin()
{
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
	m_bIsKeyInput = true;
	//m_fStateTime = 0.5f; // origin 0.5 debug 2.f

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
}

// doping
void CPlayer::DOPING_Begin()
{
	m_bIsKeyInput = true;
	m_bIsAttack = true;
	Add_Hp(5.f);

	m_pHpBarUI->Set_RenderOn(false);
	m_pHpBarUI->Set_Active(false);

	// hit count reset
	dynamic_cast<CHpBarUI*>(m_pHpBarUI)->HitCount_Reset();
}

void CPlayer::DOPING_On(const _float& fTimeDelta)
{
	if (CGlobal_Info::Get_Instance()->IS_STATE_END())
		Set_State_Normal();
}

void CPlayer::DOPING_End()
{

}

// wall
void CPlayer::WALL_Begin()
{
	m_eMoveKey = MVKEY_NON;
	m_bIsAttack = true;
}

void CPlayer::WALL_On(const _float& fTimeDelta)
{
	// state가 끝나면
	// jump로 변신
	if (CGlobal_Info::Get_Instance()->IS_STATE_END())
		m_tPlayerInfo.ePlayerState = JUMP;
}

void CPlayer::WALL_End()
{
}

// opening
void CPlayer::OPENING_Begin()
{
	m_eMoveKey = MVKEY_NON;
	m_bIsCountHp = false;
	m_bIsInvincible = true;

	m_pHpBarUI->Set_RenderOn(false);
	m_pHpBarUI->Set_Active(false);
}

void CPlayer::OPENING_On(const _float& fTimeDelta)
{
	if (CGlobal_Info::Get_Instance()->IS_STATE_END())
		Set_State_Normal();
}

void CPlayer::OPENING_End()
{
}

// dead
void CPlayer::PLAYERDEAD_Begin()
{
	m_eMoveKey = MVKEY_NON;
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
	m_eMoveKey = MVKEY_NON;
	m_pPlayerUI->Set_RenderOn(false);
	m_pPlayerUI->Set_Active(false);
	m_pHpBarUI->Set_RenderOn(false);
	m_pHpBarUI->Set_Active(false);
}

void CPlayer::KeyInput(const _float& fTimeDelta)
{
	// 움직임 키
	switch (m_eMoveKey) {
	case MVKEY_NORMAL: // 상하좌우
		if (KEY_BUTTON_HOLD(DIK_W))
		{
			m_pTransformCom->Move_Forward(fTimeDelta, m_vPosition.y);
		}

		if (KEY_BUTTON_HOLD(DIK_S))
		{
			m_pTransformCom->Move_Backward(fTimeDelta, m_vPosition.y);
		}
		// break 있으면 안됨
	case MVKEY_LR: // 좌우
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
		if (KEY_BUTTON_DOWN(DIK_LSHIFT))
		{
			m_tPlayerInfo.ePlayerState = DASH;
		}

		//DIK_SPACE
		if (KEY_BUTTON_DOWN(DIK_SPACE))
		{
			m_tPlayerInfo.ePlayerState = JUMP;
		}

		

		if (IS_RBUTTON_DOWN) // 우클릭
		{
			m_tPlayerInfo.ePlayerState = DASH_ATTACK;
		}

		if (m_tPlayerInfo.eWeapon != WP_NON)
		{
			if (KEY_BUTTON_DOWN(DIK_R))
			{
				m_tPlayerInfo.ePlayerState = RELOAD;
			}
		}
	}

	if (m_bIsAttack)
	{
		if (IS_LBUTTON_DOWN) // 좌클릭
		{
			m_tPlayerInfo.ePlayerState = ATTACK;
		}
	}

	//debug
	if (KEY_BUTTON_DOWN(DIK_E))
		m_tPlayerInfo.ePlayerState = ATTACK_INSTANT;
}

void CPlayer::Set_State_Normal()
{
	m_tPlayerInfo.ePlayerState = IDLE;
	m_tPlayerInfo.ePlayerMove = PMV_NORMAL;
}

void CPlayer::CountHp(const _float& fTimeDelta)
{
	Add_Hp(-1.f * fTimeDelta);

	dynamic_cast<CHpBarUI*>(m_pHpBarUI)->Set_Hp(m_fMaxHp, m_fHp);

	OutputDebugString((L"m_fHp: " + std::to_wstring(m_fHp) + L"\n").c_str());
}

_bool CPlayer::StateTime_IsEnd(const _float& fTimeDelta, _float fAddTime)
{
	// 누적 시간 더해줌 (초 단위)
	m_fAddTime += fTimeDelta * fAddTime;

	// 누적시간이 스테이트시간 보다 크거나 같으면 return true
	return (m_fAddTime >= m_fStateTime);
}

//PMV_NORMAL, PMV_DASH,PMV_DASHATT, PMV_SLIDE, PMV_END
void CPlayer::Move(const _float& fTimeDelta)
{
	switch (m_tPlayerInfo.ePlayerMove)
	{
	case PMV_NORMAL:
		break;

	case PMV_DASH:
	case PMV_DASHATT:
	{
		m_bIsAttack = true;
		m_eMoveKey = MVKEY_NON;
		Move_Dash(fTimeDelta);
	}
		break;
	case PMV_SLIDE:
	{
		m_bIsAttack = true;
		m_eMoveKey = MVKEY_LR;
		Move_Slide(fTimeDelta);
	}
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
	if (m_pTransformCom->GetTransformInfo().fSpeed <= 1.f)
		Set_State_Normal();

	// 앞으로 움직여라
	if(m_bIsFixY)
		m_pTransformCom->Move_Forward(fTimeDelta, m_fFixY);
	else
	m_pTransformCom->Move_Forward(fTimeDelta, m_vPosition.y);

	// speed 깎음 (like 마찰력)
	m_pTransformCom->GetTransformInfo().fSpeed -= fTimeDelta * 9.f;
}

void CPlayer::Move_Slide(const _float& fTimeDelta)
{
	// 앞으로 움직여라
	m_pTransformCom->Move_Forward(fTimeDelta, m_vPosition.y);

	if (KEY_BUTTON_HOLD(DIK_W))
		m_pTransformCom->GetTransformInfo().fSpeed += fTimeDelta * 2.f;
}

void CPlayer::Move_Wall(const _float& fTimeDelta)
{
}

void CPlayer::Change_Move(PLAYERMOVE ePlayerMove, _bool bYFix)
{
	// 바뀔때 y값 저장해옴 m_bYFix의 값에 따라 쓸래말래 결정
	m_fFixY = GetTransform()->Get_Info(INFO_POS).y;
	m_bIsFixY = bYFix;

	switch (ePlayerMove)
	{
	case PMV_NORMAL:
		GetTransform()->GetTransformInfo().fSpeed = m_fNormalSpeed + 5.f;
		break;

	case PMV_DASH:
	case PMV_DASHATT:
		GetTransform()->GetTransformInfo().fSpeed = m_fNormalSpeed + 5.f;
	break;
	case PMV_SLIDE:
		GetTransform()->GetTransformInfo().fSpeed = m_fNormalSpeed + 2.f;
	break;
	}

	m_tPlayerInfo.ePlayerMove = ePlayerMove;
}

HRESULT CPlayer::Set_Component()
{
	//// Texture
	//if (Texture_Clone())
	//	return E_FAIL;

	//CColider_Cube::COLLRECTDESC CollCubeDesc;
	//ZeroMemory(&CollCubeDesc, sizeof(CColider_Cube::COLLRECTDESC));
	//CollCubeDesc.fRadiusY = 1.f;
	//CollCubeDesc.fRadiusX = 1.f;
	//CollCubeDesc.fRadiusZ = 1.f;
	//CollCubeDesc.fOffSetX = 0.f;
	//CollCubeDesc.fOffSetY = 0.f;
	//CollCubeDesc.fOffsetZ = 0.f;

	//// Colider_Cube
	//if (FAILED(Add_Components(L"Com_Collider_Cube", SCENE_STATIC, L"Proto_Colider_Cube", (CComponent**)&m_pColliderCom, &CollCubeDesc)))
	//	return E_FAIL;
	//m_pColliderCom->Set_Transform(m_pTransformCom);

	// Collider_Sphere
 	CColider_Sphere::COLLINFO CollSphereInfo;
	ZeroMemory(&CollSphereInfo, sizeof(CColider_Sphere::COLLINFO));
	CollSphereInfo.fRadius = 0.4f;                    // 반지름 1 -> 0.8 eunbi
	CollSphereInfo.vOffset = _vec3(0.f, 0.f, 0.f);    // 중심 오프셋 없음

	// Colider_Sphere
	if (FAILED(Add_Components(L"Com_Collider_Sphere", SCENE_STATIC, L"Proto_Colider_Sphere", (CComponent**)&m_pColiderSphere, &CollSphereInfo)))
		return E_FAIL;
	m_pColiderSphere->Set_Transform(m_pTransformCom);

	return S_OK;
}

void CPlayer::Set_Collider(const _float& fTimeDelta)
{
	//m_pColliderCom->Update_ColliderBox();
	m_pColiderSphere->Update_ColliderSphere();

	//  큐브 충돌
	/*
	if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_MONSTER, this, CColiderManager::COLLISION_CUBE, nullptr))
	{
		_vec3 vPosition = m_pTransformCom->Get_Info(INFO_POS);
	}
	*/
	// 구 충돌

	if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_DUMMY, this, CColiderManager::COLLISION_SPHERE_CUBE, nullptr))
	{
		CUIManager::GetInstance()->CreateClearUI();
		m_pColiderSphere->Set_Active(false);
		m_tPlayerInfo.ePlayerState = CLEAR;
	}
	Set_Colllider_With_Monster(fTimeDelta);
	Set_Collider_With_Wall();
	Set_Collider_With_Door();
}

_float CPlayer::CosRadian(_vec3 v1, _vec3 v2)
{
	D3DXVec3Normalize(&v1, &v1);
	D3DXVec3Normalize(&v2, &v2);
	return D3DXVec3Dot(&v1, &v2); //cos세타
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
	}

	// hit 시간 누적
	m_fHitTime += fTimeDelta;

	// 누적 시간이 5초 이상이면
	if (m_fHitTime >= 5.f)
	{
		// 0초로 초기화
		m_fHitTime = 0.f;
	}

	if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_DUMMY, this, CColiderManager::COLLISION_SPHERE_CUBE, nullptr))
	{
		CUIManager::GetInstance()->CreateClearUI();
		m_tPlayerInfo.ePlayerState = CLEAR;
	}
}

void CPlayer::Set_Collider_With_Wall()
{
	_vec3 vDistance;
	if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_HORWALL, this, CColiderManager::COLLISION_SPHERE_CUBE, &vDistance))
	{
		_vec3 vPos = m_pTransformCom->Get_Info(INFO_POS);
		m_pTransformCom->Set_Info(INFO_POS, vPos += vDistance);
	}
	if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_VERWALL, this, CColiderManager::COLLISION_SPHERE_CUBE, &vDistance))
	{
		_vec3 vPos = m_pTransformCom->Get_Info(INFO_POS);
		m_pTransformCom->Set_Info(INFO_POS, vPos += vDistance);
	}
	if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_CEILING, this, CColiderManager::COLLISION_SPHERE_CUBE, &vDistance))
	{
		_vec3 vPos = m_pTransformCom->Get_Info(INFO_POS);
		m_pTransformCom->Set_Info(INFO_POS, vPos += (vDistance - _vec3{ 0.f, 0.01f, 0.f }));
		Set_Velocity(Get_Velocity() * -1.f);
	}
}

void CPlayer::Set_Collider_With_Door()
{
	if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_DOOR, this, CColiderManager::COLLISION_SPHERE, nullptr))
	{
		// kick 모션 나오게
		m_tPlayerInfo.ePlayerState = KICK;
	}
}

void CPlayer::Set_Colllider_With_Monster(const _float& fTimeDelta)
{
	CGameObject* pColiObj;
	if (CColiderManager::GetInstance()->CollisionGroupWho(CColiderManager::COLLISION_MONSTER, this, CColiderManager::COLLISION_SPHERE, nullptr, pColiObj))
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
				if (m_tPlayerInfo.ePlayerState == DASH_ATTACK)
				{
					// wap2에 따라 state 변경
					switch (m_tPlayerInfo.eWeapon2)
					{
					case WP_KICK:
						m_tPlayerInfo.ePlayerState = KICK;
						//m_pTransformCom->Move_PosDown(0.5);
						break;

					case WP_KNIFE:
					case WP_BOOK:
						// 몬스터 위치로 이동한 다음
						if (Get_Pos().z < vMonPos.z) // z값을 기준으로 움직임 멈춤 조건
						{ 
							while (Get_Pos().z < vMonPos.z)
								m_pTransformCom->Move_PosDir(fTimeDelta * 0.8, vDir); 
						}
						else 
						{ 
							while (Get_Pos().z > vMonPos.z) 
								m_pTransformCom->Move_PosDir(fTimeDelta * 0.8, vDir); 
						}
						m_tPlayerInfo.ePlayerState = ATTACK_INSTANT;
						break;
					}
				}

				// Dash attack이 아니면 hit
				else
				{
					// 부딫힌 obj의 attack을 가져옴
					//HitFromObject(dynamic_cast<CCharacter*>(pColiObj)->Get_Attack());
					HitFromObject(fTimeDelta, 1.f);
				}
			}

			// 앞에 없다면 hit
			else
			{
				// 부딫힌 obj의 attack을 가져옴
				//HitFromObject(dynamic_cast<CCharacter*>(pColiObj)->Get_Attack());
				HitFromObject(fTimeDelta, 1.f);
			}
		}
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

HRESULT CPlayer::Set_PlayerUI()
{
	_uint iSceneIndex = CMapFactory::GetInstance()->GetTargetSceneIndex();
	m_pPlayerUI = dynamic_cast<CUIBase*>(
		CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UIRoot", iSceneIndex, L"UI_Layer"));

	if (m_pPlayerUI == nullptr)
		return E_FAIL;

	// foot UI 생성
	CPlayer_Foot* pFootUI = dynamic_cast<CPlayer_Foot*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_PlayerFootUI", SCENE_STATIC, L"UI_Layer"));
	if (pFootUI)
	{
		pFootUI->Set_ObjTag(L"FootUI");
		m_pPlayerUI->Add_Child(pFootUI); // 루트 UI에 등록
	}
	// habdR UI 생성
	CPlayer_HandR* pHandRUI = dynamic_cast<CPlayer_HandR*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_PlayerHandRUI", iSceneIndex, L"UI_Layer"));
	if (pHandRUI)
	{
		// pHandRUI -> Initailize()
		pHandRUI->Set_ObjTag(L"HandRUI");
		m_pPlayerUI->Add_Child(pHandRUI); // 루트 UI에 등록
	}
	// handL UI 생성
	CPlayer_HandL* pHandLUI = dynamic_cast<CPlayer_HandL*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_PlayerHandLUI", iSceneIndex, L"UI_Layer"));
	if (pHandLUI)
	{
		pHandLUI->Set_ObjTag(L"HandLUI");
		m_pPlayerUI->Add_Child(pHandLUI); // 루트 UI에 등록
	}
	
	// arm UI 생성
	CPlayer_Arm* pArmUI = dynamic_cast<CPlayer_Arm*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_PlayerArmUI", iSceneIndex, L"UI_Layer"));
	if (pArmUI)
	{
		pArmUI->Set_ObjTag(L"ArmUI");
		m_pPlayerUI->Add_Child(pArmUI); // 루트 UI에 등록
	}

	return S_OK;
}

HRESULT CPlayer::Set_HpBarUI()
{
	_uint iSceneIndex = CMapFactory::GetInstance()->GetTargetSceneIndex();

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
	case DASH_ATTACK: return TEXT("State: DASH_ATTACK\n");
	case DASH: return TEXT("State: DASH\n");
	case SLIED: return TEXT("State: SLIED\n");
	case KICK: return TEXT("State: KICK\n");
	case ATTACK: return TEXT("State: ATTACK\n");
	case ATTACK_INSTANT: return TEXT("State: ATTACK_INSTANT\n");
	case RELOAD: return TEXT("State: RELOAD\n");
	case DOPING: return TEXT("State: DOPING\n");
	case WALL: return TEXT("State: WALL\n");
	case OPENING: return TEXT("State: OPENING\n");
	case PLAYERDEAD: return TEXT("State: PLAYERDEAD\n");
	case CLEAR: return TEXT("State: CLEAR\n");
	default: return TEXT("State: UNKNOWN\n");
	}
}