#include "pch.h"
#include "CPlayer.h"
#include "CRenderer.h"
#include "CColiderManager.h"
#include "CTimerMgr.h"
#include "CObjectManager.h"
#include "CUIBase.h"
#include "CDInputMgr.h"
#include "CGlobal_Info.h"
#include "CPlayer_HandR.h"
#include "CPlayer_HandL.h"
#include "CPlayer_Arm.h"
#include "CPlayer_Foot.h"
#include "CHpBarUI.h"
#include "CMan_HpBarUI.h"
#include "CPhone_HpBarUI.h"
#include "CUIManager.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCharacter(pGraphicDev), m_tPlayerInfo({ OPENING, WP_PISTOL ,WP_KICK }), m_tPrePlayerInfo({ PLAYER_END ,WP_END,WP2_END }),
	m_TimerTag(TEXT("")), m_fGround_Height(0.f), m_eMove(MOVE_END), m_fMaxHp(10.f),
	m_bIsKeyInput(true), m_bIsInvincible(true), m_bIsAttack(true), m_bIsCountHp(false)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CCharacter(rhs), m_tPlayerInfo(rhs.m_tPlayerInfo), m_tPrePlayerInfo(rhs.m_tPrePlayerInfo),
	m_TimerTag(rhs.m_TimerTag), m_fGround_Height(rhs.m_fGround_Height), m_eMove(rhs.m_eMove), m_fMaxHp(rhs.m_fMaxHp),
	m_bIsKeyInput(rhs.m_bIsKeyInput), m_bIsInvincible(rhs.m_bIsInvincible), m_bIsAttack(rhs.m_bIsAttack)
	, m_bIsCountHp(rhs.m_bIsCountHp)
{
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
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
	if (MAPOBJECTDATA *p = reinterpret_cast<MAPOBJECTDATA *>(pArg))
	{
		GetTransform()->Set_Info(INFO::INFO_RIGHT, p->transform.Right);
		GetTransform()->Set_Info(INFO::INFO_UP, p->transform.Up);
		GetTransform()->Set_Info(INFO::INFO_LOOK, p->transform.Look);
		GetTransform()->Set_Info(INFO::INFO_POS, p->transform.Pos);
	if (FAILED(Set_HpBarUI()))
		return E_FAIL;

	}

	m_fHp = 10.f; // 플레이어 목숨 초 -> origin : 10, test : 3

	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	/*if (m_bDead)
		return DEAD;*/

	CGameObject::Update_GameObject(fTimeDelta);
	
	// state change & update
	ChangeState(m_tPlayerInfo.ePlayerState);
	StateUpdate(m_tPlayerInfo.ePlayerState, fTimeDelta);

	// collider group 해줌
	CColiderManager::GetInstance()->Add_CollisionGroup(CColiderManager::COLLISION_PLAYER, this);
	m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this);
	return S_OK;
}

void CPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	//Set_OnTerrain();
	Update_Position(m_pTransformCom->Get_Info(INFO_POS));

	// 콜라이더 set
	Set_Collider();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(RENDER_NONALPHA, this);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
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
		m_tPlayerInfo.ePlayerState = PLAYERDEAD;
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
	case HIT:
		HIT_Begin();break;
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
	case HIT:
		HIT_End();break;
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
	case HIT:
		HIT_On(fTimeDelta);break;
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
		//CountHp(fTimeDelta);
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

	m_eMove = MOVE_NORMAL;

	m_pTransformCom->GetTransformInfo().fSpeed = 5.f;

	m_pHpBarUI->Set_Active(true);
	m_pHpBarUI->Set_RenderOn(true);
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
	m_fJumpTime = 0.2f;
	m_bJumping = true;
}

void CPlayer::JUMP_On(const _float& fTimeDelta)
{
	if (!m_bJumping)
		Set_State_Idle();
}

void CPlayer::JUMP_End()
{
	m_fJumpTime = 0.f;
}

// dash attack
void CPlayer::DASH_ATTACK_Begin()
{
	m_eMove = MOVE_STOP;
	m_pTransformCom->GetTransformInfo().fSpeed = 20.f;
}

void CPlayer::DASH_ATTACK_On(const _float& fTimeDelta)
{
	// 만약 일정 속도 이하가 되면 -> state: IDLE
	if (m_pTransformCom->GetTransformInfo().fSpeed <= 10.f)
		Set_State_Idle();

	// 앞으로 움직여라
	m_pTransformCom->Move_Forward(fTimeDelta, m_vPosition.y);

	// speed 깎음 (like 마찰력)
	m_pTransformCom->GetTransformInfo().fSpeed -= fTimeDelta * 8.f;
}

void CPlayer::DASH_ATTACK_End()
{

}

// dash
void CPlayer::DASH_Begin()
{
	m_eMove = MOVE_STOP;
	m_pTransformCom->GetTransformInfo().fSpeed = 20.f;
}

void CPlayer::DASH_On(const _float& fTimeDelta)
{
	// 만약 일정 속도 이하가 되면 -> state: IDLE
	if (m_pTransformCom->GetTransformInfo().fSpeed <= 10.f)
		Set_State_Idle();

	// 앞으로 움직여라
	m_pTransformCom->Move_Forward(fTimeDelta, m_vPosition.y);

	// speed 깎음 (like 마찰력)
	m_pTransformCom->GetTransformInfo().fSpeed -= fTimeDelta * 8.f;
}

void CPlayer::DASH_End()
{

}

// slide
void CPlayer::SLIED_Begin()
{
	m_eMove = MOVE_LR;
	m_bIsAttack = true;
	m_pTransformCom->GetTransformInfo().fSpeed = 15.f;
}

void CPlayer::SLIED_On(const _float& fTimeDelta)
{
	// 앞으로 움직여라
	m_pTransformCom->Move_Forward(fTimeDelta, m_vPosition.y);
	// 속도를 늘리면서
	m_pTransformCom->GetTransformInfo().fSpeed += fTimeDelta * 8.f;
}

void CPlayer::SLIED_End()
{
}

// kick
void CPlayer::KICK_Begin()
{
	m_fStateTime = 0.25f;
}

void CPlayer::KICK_On(const _float& fTimeDelta)
{
	if (StateTime_IsEnd(fTimeDelta, 1.f))
		Set_State_Idle();
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
		Set_State_Idle();
}

void CPlayer::ATTACK_End()
{
}

// attack instant
void CPlayer::ATTACK_INSTANT_Begin()
{
	m_eMove = MOVE_NON;
	m_fStateTime = 0.5f;

	m_pHpBarUI->Set_RenderOn(false);
	m_pHpBarUI->Set_Active(false);
}

void CPlayer::ATTACK_INSTANT_On(const _float& fTimeDelta)
{
	if (StateTime_IsEnd(fTimeDelta, 1.f))
		Set_State_Idle();
}

void CPlayer::ATTACK_INSTANT_End()
{
}

void CPlayer::ZOOMING_Begin()
{
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
	m_fStateTime = 1.f;

	m_pHpBarUI->Set_RenderOn(false);
	m_pHpBarUI->Set_Active(false);
}

void CPlayer::RELOAD_On(const _float& fTimeDelta)
{
	if (StateTime_IsEnd(fTimeDelta, 1.5f))
		Set_State_Idle();
}

void CPlayer::RELOAD_End()
{
}

// hit
void CPlayer::HIT_Begin()
{
	m_bIsKeyInput = true;
	m_bIsAttack = true;
}

void CPlayer::HIT_On(const _float& fTimeDelta)
{
	// hp-
	if (m_fHp <= 0)
		m_tPlayerInfo.ePlayerState = PLAYERDEAD;
}

void CPlayer::HIT_End()
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
}

void CPlayer::DOPING_On(const _float& fTimeDelta)
{
	if (CGlobal_Info::Get_Instance()->IS_STATE_END())
		Set_State_Idle();
}

void CPlayer::DOPING_End()
{

}

// wall
void CPlayer::WALL_Begin()
{
	m_eMove = MOVE_NON;
	m_bIsAttack = true;
}

void CPlayer::WALL_On(const _float& fTimeDelta)
{
	// state가 끝나면
	// jump로 변신
}

void CPlayer::WALL_End()
{
}

// opening
void CPlayer::OPENING_Begin()
{
	m_eMove = MOVE_NON;
	m_bIsCountHp = false;

	m_pHpBarUI->Set_RenderOn(false);
	m_pHpBarUI->Set_Active(false);
}

void CPlayer::OPENING_On(const _float& fTimeDelta)
{
	if (CGlobal_Info::Get_Instance()->IS_STATE_END())
		Set_State_Idle();
}

void CPlayer::OPENING_End()
{
}

// dead
void CPlayer::PLAYERDEAD_Begin()
{
	m_eMove = MOVE_NON;
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
	m_eMove = MOVE_NON;
	m_pPlayerUI->Set_RenderOn(false);
	m_pPlayerUI->Set_Active(false);
	m_pHpBarUI->Set_RenderOn(false);
	m_pHpBarUI->Set_Active(false);
}

void CPlayer::KeyInput(const _float& fTimeDelta)
{
	// 움직임 키
	switch (m_eMove) {
	case MOVE_NORMAL: // 상하좌우
		if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_W) & 0x80)
		{
			m_pTransformCom->Move_Forward(fTimeDelta, m_vPosition.y);
		}

		if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_S) & 0x80)
		{
			m_pTransformCom->Move_Backward(fTimeDelta, m_vPosition.y);
		}
		// break 있으면 안됨
	case MOVE_LR: // 좌우
		if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_A) & 0x80)
		{
			m_pTransformCom->Move_Left(fTimeDelta, m_vPosition.y);
			// camera state -> left
		}

		if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_D) & 0x80)
		{
			m_pTransformCom->Move_Right(fTimeDelta, m_vPosition.y);
			// camera state -> right
		}
		break;
	case MOVE_STOP: // idle로 바뀜
		if ((CDInputMgr::GetInstance()->Get_DIKeyState(DIK_W) & 0x80) ||
			(CDInputMgr::GetInstance()->Get_DIKeyState(DIK_S) & 0x80))
		{
			Set_State_Idle();
		}
		break;
	}

	// 상태 전환 키
	if (m_bIsKeyInput) {
		if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_LSHIFT) & 0x80)
		{
			m_tPlayerInfo.ePlayerState = DASH;
		}

		if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_SPACE) & 0x80)
		{
			m_tPlayerInfo.ePlayerState = JUMP;
		}

		if (m_bIsAttack && (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_Q) & 0x80)) // 좌클릭
		{
			m_tPlayerInfo.ePlayerState = ATTACK;
		}

		if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_E) & 0x80) // 우클릭
		{
			m_tPlayerInfo.ePlayerState = DASH_ATTACK;
		}

		if (m_tPlayerInfo.eWeapon != WP_NON)
		{
			if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_R) & 0x80)
			{
				m_tPlayerInfo.ePlayerState = RELOAD;
			}
		}
	}
}

void CPlayer::Set_State_Idle()
{
	m_tPlayerInfo.ePlayerState = IDLE;
}

void CPlayer::CountHp(const _float& fTimeDelta)
{
	Add_Hp(-1.f * fTimeDelta);

	OutputDebugString((L"m_fHp: " + std::to_wstring(m_fHp) + L"\n").c_str());
}

_bool CPlayer::StateTime_IsEnd(const _float& fTimeDelta, _float fAddTime)
{
	// 누적 시간 더해줌 (초 단위)
	m_fAddTime += fTimeDelta * fAddTime;

	// 누적시간이 스테이트시간 보다 크거나 같으면 return true
	return (m_fAddTime >= m_fStateTime);
}

HRESULT CPlayer::Set_Component()
{
	if (FAILED(CTimerMgr::GetInstance()->Ready_Timer(TEXT("Timer_Player"))))
		return E_FAIL;

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
	CollSphereInfo.fRadius = 1.f;                    // 반지름 1
	CollSphereInfo.vOffset = _vec3(0.f, 0.f, 0.f);    // 중심 오프셋 없음

	// Colider_Sphere
	if (FAILED(Add_Components(L"Com_Collider_Sphere", SCENE_STATIC, L"Proto_Colider_Sphere", (CComponent**)&m_pColiderSphere, &CollSphereInfo)))
		return E_FAIL;
	m_pColiderSphere->Set_Transform(m_pTransformCom);

	return S_OK;
}

void CPlayer::Set_Collider(void)
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
	if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_MONSTER, this, CColiderManager::COLLISION_SPHERE, nullptr))
	{
		_vec3 vPosition = m_pTransformCom->Get_Info(INFO_POS);

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
				m_tPlayerInfo.ePlayerState = ATTACK_INSTANT;
				break;
			}
		}

		// Dash attack이 아니면 hit
		/*else
		{
			m_tPlayerInfo.ePlayerState = HIT;
		}*/
			
	}
	if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_DUMMY, this, CColiderManager::COLLISION_SPHERE_CUBE, nullptr))
	{
		CUIManager::GetInstance()->CreateClearUI();
		m_tPlayerInfo.ePlayerState = CLEAR;
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
	m_pPlayerUI = dynamic_cast<CUIBase*>(
		CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UIRoot", SCENE_STATIC, L"UI_Layer"));

	if (m_pPlayerUI == nullptr)
		return E_FAIL;

	// habdR UI 생성
	CPlayer_HandR* pHandRUI = dynamic_cast<CPlayer_HandR*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_PlayerHandRUI", SCENE_STATIC, L"UI_Layer"));
	if (pHandRUI)
	{
		// pHandRUI -> Initailize()
		pHandRUI->Set_ObjTag(L"HandRUI");
		m_pPlayerUI->Add_Child(pHandRUI); // 루트 UI에 등록
	}
	// handL UI 생성
	CPlayer_HandL* pHandLUI = dynamic_cast<CPlayer_HandL*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_PlayerHandLUI", SCENE_STATIC, L"UI_Layer"));
	if (pHandLUI)
	{
		pHandLUI->Set_ObjTag(L"HandLUI");
		m_pPlayerUI->Add_Child(pHandLUI); // 루트 UI에 등록
	}
	// foot UI 생성
	CPlayer_Foot* pFootUI = dynamic_cast<CPlayer_Foot*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_PlayerFootUI", SCENE_STATIC, L"UI_Layer"));
	if (pFootUI)
	{
		pFootUI->Set_ObjTag(L"FootUI");
		m_pPlayerUI->Add_Child(pFootUI); // 루트 UI에 등록
	}
	// arm UI 생성
	CPlayer_Arm* pArmUI = dynamic_cast<CPlayer_Arm*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_PlayerArmUI", SCENE_STATIC, L"UI_Layer"));
	if (pArmUI)
	{
		pArmUI->Set_ObjTag(L"ArmUI");
		m_pPlayerUI->Add_Child(pArmUI); // 루트 UI에 등록
	}

	return S_OK;
}

HRESULT CPlayer::Set_HpBarUI()
{
	m_pHpBarUI = dynamic_cast<CUIBase*>(
		CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_HpbarUI", SCENE_STATIC, L"UI_Layer"));

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
	case HIT: return TEXT("State: HIT\n");
	case DOPING: return TEXT("State: DOPING\n");
	case WALL: return TEXT("State: WALL\n");
	case OPENING: return TEXT("State: OPENING\n");
	case PLAYERDEAD: return TEXT("State: PLAYERDEAD\n");
	case CLEAR: return TEXT("State: CLEAR\n");
	default: return TEXT("State: UNKNOWN\n");
	}
}