#include "pch.h"
#include "CPlayer.h"
#include "CRenderer.h"
#include "CColiderManager.h"
#include "CTimerMgr.h"
#include "CDInputMgr.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCharacter(pGraphicDev), m_eState(OPENING), m_ePrevState(PLAYER_END),
	m_TimerTag(TEXT("")), m_fGround_Height(0.f), m_eMove(MOVE_END),
	m_bHpBarOn(true), m_bKeyInput(true), m_bIsInvincible(true), m_bAttack(true)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CCharacter(rhs), m_eState(rhs.m_eState), m_ePrevState(rhs.m_ePrevState),
	m_TimerTag(rhs.m_TimerTag), m_fGround_Height(rhs.m_fGround_Height), m_eMove(rhs.m_eMove),
	m_bHpBarOn(rhs.m_bHpBarOn), m_bKeyInput(rhs.m_bKeyInput), m_bIsInvincible(rhs.m_bIsInvincible), m_bAttack(rhs.m_bAttack)
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

	m_vPosition = { 10.f, 1.f, 10.f };
	m_pTransformCom->Set_Info(INFO_POS, m_vPosition);
	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);

	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);
	

	// state change & update
	ChangeState(m_eState);
	StateUpdate(m_eState, fTimeDelta);

	// collider group 해줌
	CColiderManager::GetInstance()->Add_CollisionGroup(CColiderManager::COLLISION_PLAYER, this);
	m_pRenderCom->Add_RenderGroup(RENDER_ALPHA, this);
	return S_OK;
}

void CPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	//Key_Input(fTimeDelta);

	//Set_OnTerrain();
	Update_Position(m_pTransformCom->Get_Info(INFO_POS));

	// 콜라이더 set
	Set_Collider();

	if (nullptr != m_pRenderCom)
		m_pRenderCom->Add_RenderGroup(RENDER_NONALPHA, this);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CPlayer::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTransformCom->Apply_WorldMatrix();

	m_pTextureCom->Set_Texture(m_pTextureCom->Get_Frame().m_iCurrentTex);
	m_pTextureCom->MoveFrame(m_TimerTag);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

#ifdef _DEBUG
	if (g_ColiderRender && m_pColliderCom != nullptr)
	{
		m_pColliderCom->Render_ColliderBox(); // 충돌체 디버그 렌더
	}
	if (g_ColiderRender && m_pColiderSphere != nullptr)
	{
		m_pColiderSphere->Render_ColliderSphere(); // 충돌체 디버그 렌더
	}
#endif
}

void CPlayer::ChangeState(PLAYERSTATE _e)
{

	if (m_ePrevState == _e)
		return;

	StateEnd(m_ePrevState);
	StateNormalSet(); // 전체적으로 적용하는 setting
	StateBegin(_e);
	m_ePrevState = _e;
}


//IDLE, JUMP, DASH_ATTACK, DASH, SLIED, KICK, ATTACK,
//ATTACK_INSTANT, RELOAD, HIT, DOPING, WALL, OPENING, PLAYERDEAD, PLAYER_END
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
	}
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

	KeyInput(fTimeDelta);
}

void CPlayer::StateNormalSet()
{
	m_bKeyInput		= false;
	m_bHpBarOn		= true;
	m_bIsInvincible = false;
	m_bAttack		= false;
	m_bJumping		= false;

	m_eMove = MOVE_NORMAL;

	m_pTransformCom->GetTransformInfo().fSpeed = 5.f;
}

// idle
void CPlayer::IDLE_Begin()
{
	m_bKeyInput = true;
	m_bAttack	= true;
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
	m_pTransformCom->GetTransformInfo().fSpeed = 10.f;
}

void CPlayer::DASH_ATTACK_On(const _float& fTimeDelta)
{
	m_pTransformCom->Move_Forward(fTimeDelta, m_vPosition.y);

	//만약 몬스터와 충돌 했을때
	// if(weapon2 ==  WP_KICK) m_eState = KICK;
	// else m_eState = ATTACK_INSTANT;

	// 만약 문과 충돌 했을때
	// m_eState = KICK;

	// 만약 

	// 가속하다가 느려짐

}

void CPlayer::DASH_ATTACK_End()
{

}

// dash
void CPlayer::DASH_Begin()
{
	m_eMove = MOVE_STOP;
	m_pTransformCom->GetTransformInfo().fSpeed = 10.f;
}

void CPlayer::DASH_On(const _float& fTimeDelta)
{
	m_pTransformCom->Move_Forward(fTimeDelta, m_vPosition.y);
}

void CPlayer::DASH_End()
{
}

// slide
void CPlayer::SLIED_Begin()
{
	m_eMove = MOVE_LR;
	m_bAttack = true;
	m_pTransformCom->GetTransformInfo().fSpeed = 10.f;
}

void CPlayer::SLIED_On(const _float& fTimeDelta)
{
}

void CPlayer::SLIED_End()
{
}

// kick
void CPlayer::KICK_Begin()
{
	// 얘는 다 생각해봐야함

}

void CPlayer::KICK_On(const _float& fTimeDelta)
{
}

void CPlayer::KICK_End()
{
}

// attack
void CPlayer::ATTACK_Begin()
{
	m_bKeyInput = true;
}

void CPlayer::ATTACK_On(const _float& fTimeDelta)
{
	if (Is_Anim_Finished())
		Set_State_Idle();
}

void CPlayer::ATTACK_End()
{
}

// attack instant
void CPlayer::ATTACK_INSTANT_Begin()
{
	m_bHpBarOn = false;
	m_eMove = MOVE_NON;
}

void CPlayer::ATTACK_INSTANT_On(const _float& fTimeDelta)
{
	if (Is_Anim_Finished())
		Set_State_Idle();
}

void CPlayer::ATTACK_INSTANT_End()
{
}

// reload
void CPlayer::RELOAD_Begin()
{
	m_bHpBarOn = false;
	m_bKeyInput = true;
}

void CPlayer::RELOAD_On(const _float& fTimeDelta)
{
	if (Is_Anim_Finished())
		Set_State_Idle();
}

void CPlayer::RELOAD_End()
{
}

// hit
void CPlayer::HIT_Begin()
{
	m_bKeyInput = true;
	m_bAttack = true;
}

void CPlayer::HIT_On(const _float& fTimeDelta)
{
	// hp-
	if (m_fHp <= 0)
		m_eState = PLAYERDEAD;
}

void CPlayer::HIT_End()
{
}

// doping
void CPlayer::DOPING_Begin()
{
	m_bKeyInput = true;
	m_bAttack = true;
	m_fHp += 5.f;
}

void CPlayer::DOPING_On(const _float& fTimeDelta)
{
	if (Is_Anim_Finished())
		Set_State_Idle();
}

void CPlayer::DOPING_End()
{
	
}

// wall
void CPlayer::WALL_Begin()
{
	m_eMove = MOVE_NON;
	m_bAttack = true;
}

void CPlayer::WALL_On(const _float& fTimeDelta)
{
}

void CPlayer::WALL_End()
{
}

// opening
void CPlayer::OPENING_Begin()
{
	m_bHpBarOn = false;
	m_eMove = MOVE_NON;
}

void CPlayer::OPENING_On(const _float& fTimeDelta)
{
	if (Is_Anim_Finished())
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
	if (Is_Anim_Finished())
		m_bDead = true;
	
}

void CPlayer::PLAYERDEAD_End()
{
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
	if (m_bKeyInput) {
		if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_LSHIFT) & 0x80)
		{
			m_eState = DASH;
		}

		if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_Q) & 0x80) // 좌클릭
		{
			m_eState = ATTACK;
		}

		if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_SPACE) & 0x80)
		{
			m_eState = JUMP;
		}

	}

	if (m_bAttack && (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_E) & 0x80)) // 우클릭
	{
		m_eState = DASH_ATTACK;
	}
}

void CPlayer::Set_State_Idle()
{
	m_eState = IDLE;
}

bool CPlayer::Is_Anim_Finished()
{
	Engine::CTexture::TEXINFO textInfo = m_pTextureCom->Get_Frame();
	return (textInfo.m_iCurrentTex == textInfo.m_iEndTex-1);
}

HRESULT CPlayer::Set_Component()
{
	if (FAILED(CTimerMgr::GetInstance()->Ready_Timer(TEXT("Timer_Player"))))
		return E_FAIL;

	m_TimerTag = TEXT("Timer_Player");

	// Render
	if (FAILED(Add_Components(L"Com_Renderer", SCENE_STATIC, L"Proto_Renderer", (CComponent**)&m_pRenderCom)))
		return E_FAIL;

	// VIBuffer
	if (FAILED(Add_Components(L"Com_Buffer", SCENE_STATIC, L"Proto_Rect_Buffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	// Texture
	if (Texture_Clone())
		return E_FAIL;

	CTransform::TRANSFORMINFO TransformInfo;
	ZeroMemory(&TransformInfo, sizeof(CTransform::TRANSFORMINFO));

	TransformInfo.fSpeed = 5.f;
	TransformInfo.fRotationSpeed = D3DXToRadian(90.f);
	TransformInfo.vStartPos = _vec3(0.f, 0.f, 0.f);

	if (FAILED(Add_Components(L"Com_Transform", SCENE_STATIC, L"Proto_Transform", (CComponent**)&m_pTransformCom, &TransformInfo)))
		return E_FAIL;

	CColider_Cube::COLLRECTDESC CollCubeDesc;
	ZeroMemory(&CollCubeDesc, sizeof(CColider_Cube::COLLRECTDESC));
	CollCubeDesc.fRadiusY = 1.f;
	CollCubeDesc.fRadiusX = 1.f;
	CollCubeDesc.fRadiusZ = 1.f;
	CollCubeDesc.fOffSetX = 0.f;
	CollCubeDesc.fOffSetY = 0.f;
	CollCubeDesc.fOffsetZ = 0.f;

	// Colider_Cube
	if (FAILED(Add_Components(L"Com_Collider_Cube", SCENE_STATIC, L"Proto_Colider_Cube", (CComponent**)&m_pColliderCom, &CollCubeDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Transform(m_pTransformCom);

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

	//  충돌 검사만 유지
	/*if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_MONSTER, this, CColiderManager::COLLISION_CUBE, nullptr))
	{
		_vec3 vPosition = m_pTransformCom->Get_Info(INFO_POS);
	}*/
	if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_MONSTER, this, CColiderManager::COLLISION_SPHERE, nullptr))
	{
		_vec3 vPosition = m_pTransformCom->Get_Info(INFO_POS);
	}
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
	if (GetAsyncKeyState(VK_UP))
	{
		m_pTransformCom->Move_Forward(fTimeDelta, m_vPosition.y);
	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		m_pTransformCom->Move_Backward(fTimeDelta, m_vPosition.y);
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		m_pTransformCom->Move_Left(fTimeDelta, m_vPosition.y);
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		m_pTransformCom->Move_Right(fTimeDelta, m_vPosition.y);
	}
}

HRESULT CPlayer::Texture_Clone()
{
	CTexture::TEXINFO		TextureInfo;
	ZeroMemory(&TextureInfo, sizeof(CTexture::TEXINFO));

	// TEST
	TextureInfo.m_iStart = 0;
	TextureInfo.m_iEndTex = 5;
	TextureInfo.m_fSpeed = 2;
	if (FAILED(Add_Components(L"Com_Texture_Test", SCENE_STAGE, L"Prototype_Component_Texture_PlayerTest", (CComponent**)&m_pTextureCom, &TextureInfo)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Test"), m_pTextureCom));

	return S_OK;
}

HRESULT CPlayer::Change_Texture(const _tchar* LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_Zero_Frame();

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
	Engine::CGameObject::Free();
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
	default: return TEXT("State: UNKNOWN\n");
	}
}