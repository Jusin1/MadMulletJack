#include "pch.h"
#include "CPickingManager.h"
#include "CCullingManager.h"
#include "CManagement.h"
#include "CDataManager.h"
#include "CObjectManager.h"
#include "CObjectPoolManager.h"
#include "CMapFactory.h"
#include "Client_Global.h"
#include "CMissile.h"
#include "Engine_Define.h"
#include "CBoss.h"
#include "CColiderManager.h"
#include "CBullet.h"
#include "CImageUI.h"
#include "CBossHpBar.h"
#include "CLoading_Scene.h"

CBoss::CBoss(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCharacter(pGraphicDev)
{
}

CBoss::CBoss(const CBoss &rhs)
	: CCharacter(rhs)
{
}

CBoss::~CBoss()
{
}

void CBoss::Free()
{
	CCharacter::Free();
}

CBoss *CBoss::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBoss *pInstance = new CBoss(pGraphicDev);
	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CBoss Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject *CBoss::Clone(void *pArg)
{
	CBoss *pInstance = new CBoss(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CBoss Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

HRESULT CBoss::Set_Component()
{
	CColider_Sphere::COLLINFO CollSphereInfo{};
	CollSphereInfo.fRadius = 0.5f;
	CollSphereInfo.vOffset = _vec3(0.f, 0.f, 0.f);

	if (FAILED(Add_Components(L"Com_Collider_Sphere", SCENE_STATIC, L"Proto_Colider_Sphere",
		(CComponent **)&m_pColiderSphere, &CollSphereInfo)))
		return E_FAIL;

	m_pColiderSphere->Set_Transform(m_pTransformCom);

	if (FAILED(Texture_Clone()))
		return E_FAIL;

	return S_OK;
}

void CBoss::Set_Collider()
{
	// 구 충돌
	m_pColiderSphere->Update_ColliderSphere();

	Set_Collider_With_Bullet();
}

void CBoss::SetUp_BillBoard()
{
	_matrix _matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &_matView);
	D3DXMatrixInverse(&_matView, nullptr, &_matView);

	_vec3 vRight = *(_vec3 *)&_matView.m[0][0];
	_vec3 vUp = *(_vec3 *)&_matView.m[1][0];
	_vec3 vLook = *(_vec3 *)&_matView.m[2][0];

	m_pTransformCom->Set_Info(INFO_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_Info(INFO_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_Info(INFO_LOOK, *D3DXVec3Normalize(&vLook, &vLook) * m_pTransformCom->Get_Scale().z);
}

HRESULT CBoss::Texture_Clone()
{
	int iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();

	if (iTargetScene < 0)
	{
		MSG_BOX("CBoss::Texture_Clone, iTargetScene is invalid");
		return E_FAIL;
	}

	CTexture::TEXINFO info{};
	struct AnimDef { const wchar_t *tag; const wchar_t *proto; int start; int end; float speed; bool loop; };
	if (iTargetScene == SCENE_CAR)
	{
		AnimDef anims[] = {
			{ L"Bullet",		L"Proto_BrokenBoss_Attack_Gun",		0,	15, 10.f,	false },
			{ L"Missile",		L"Proto_BrokenBoss_Attack_Missile", 0,	15,	10.f,	false },
			{ L"Block",			L"Proto_BrokenBoss_Bloked",			0,  15,	30.f,	false },
			{ L"Idle",			L"Proto_BrokenBoss_Idle",			0,  7,	7.f,	true },
		};

		for (auto &a : anims)
		{
			ZeroMemory(&info, sizeof(info));
			info.m_iStart = a.start;
			info.m_iEndTex = a.end;
			info.m_fSpeed = a.speed;
			info.m_bLoop = a.loop;

			if (FAILED(Add_Components(a.tag, SCENE_STATIC, a.proto, (CComponent **)&m_pTextureCom, &info)))
				return E_FAIL;
		}
	}
	else
	{
		AnimDef anims[] = {
			{ L"Bullet",		L"Proto_Boss_Attack_Gun",		0,	16, 10.f,	false },
			{ L"Missile",		L"Proto_Boss_Attack_Missile",	0,	16,	10.f,	false },
			{ L"Block",			L"Proto_Boss_Bloked",			0,  15,	30.f,	false },
			{ L"Idle",			L"Proto_Boss_Idle",				0,  6,	6.f,	true },
		};

		for (auto &a : anims)
		{
			ZeroMemory(&info, sizeof(info));
			info.m_iStart = a.start;
			info.m_iEndTex = a.end;
			info.m_fSpeed = a.speed;
			info.m_bLoop = a.loop;

			if (FAILED(Add_Components(a.tag, SCENE_STATIC, a.proto, (CComponent **)&m_pTextureCom, &info)))
				return E_FAIL;
		}
	}
	
	return S_OK;
}

HRESULT CBoss::Change_Texture(const _tchar *AnimTag)
{
	if (FAILED(CGameObject::Change_Component(AnimTag, (CComponent **)&m_pTextureCom)))
		return E_FAIL;

	if (m_pTextureCom) m_pTextureCom->Set_Zero_Frame();

	return S_OK;
}

_bool CBoss::Picking(_vec3 *PickingPoint)
{
	if (m_bDead || !m_bPickable) return false;
	_bool bPick = m_pBufferCom->Picking(m_pTransformCom, PickingPoint);
	if (bPick) m_vPickedPosition = *PickingPoint;
	return bPick;
}

void CBoss::PickingTrue()
{
	m_bPickingTrue = TRUE;
	EffectOptions tOption{Get_Preset_BulletSpark()};
	tOption.fLife_Min = 0.3f;
	tOption.fLife_Max = 0.7f;
	tOption.fSpeed_Min = 10.f;
	tOption.fSpeed_Max = 20.f;
	tOption.fSize_Min = 5.f;
	tOption.fSize_Max = 7.f;
	_vec3 SpawnPosition = m_vPickedPosition;
	CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_PIXEL, &tOption,
		[&SpawnPosition](CGameObject *pGo)->void
		{
			pGo->GetTransform()->Set_Info(INFO_POS, SpawnPosition);
	});

	Add_Health(-1.f);
	// Todo - Dead State 만들고 땅으로 추락하게 만들어야함
	// DeadState 끝나면 SetDead 처리


	if(m_eCurrentState == State::IDLE || m_eCurrentState == State::MOVE)
		if(m_pTextureCom->GetOriginCompName() != L"Proto_Boss_Bloked")
			Change_Texture(L"Block");

	
}

void CBoss::Spawn_Missile()
{
	_vec3 vPos = Get_Position();
	_vec3 vLook = GetTransform()->Get_Info(INFO::INFO_LOOK);
	::D3DXVec3Normalize(&vLook, &vLook);
	vPos -= vLook;
	_vec3 vTarget = m_pPlayer->Get_Position();
	if (m_ePathMode == PathMode::LR)
	{
		vTarget += _vec3{ 0.f, 0.f, 30.f };
	}

	MissileData tData;
	tData.vLaunchPos = vPos;
	tData.vTargetPos = vTarget;
	CObjectPoolManager::GetInstance()->Spawn(PoolType::MISSILE, &tData,
		[vPos](CGameObject *pGo)->void
		{
			pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPos);
		});
}

void CBoss::Spawn_Bullet(const _vec3 &vDir)
{
	// Todo - SpawnBullet
	BulletData tData;
	tData.vMuzzlePosition = m_pTransformCom->Get_Info(INFO_POS);
	tData.vLookDir = vDir;
	if (CMapFactory::GetInstance()->GetTargetSceneIndex() == SCENE_BOSS)
		tData.fSpeed = 90.f;
	else
		tData.fSpeed = 42.f;
	tData.vMuzzlePosition += tData.vLookDir * 2.f;
	CGameObject *pGo = CObjectPoolManager::GetInstance()->Spawn(PoolType::BULLET, &tData);
	pGo->GetTransform()->Set_Scale(0.5f, 0.5f, 1.f);
}

HRESULT CBoss::Ready_GameObject()
{
	if (FAILED(CCharacter::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss::Initialize(void *pArg)
{
	if(FAILED(CCharacter::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Set_Component()))
		return E_FAIL;

	if (RigidBodyConfig *pConfig = reinterpret_cast<RigidBodyConfig *>(pArg))
	{
		::memcpy(&m_tRigidbodyConfig, pConfig, sizeof(RigidBodyConfig));
	}

	m_bPickable = true;
	if (CMapFactory::GetInstance()->GetTargetSceneIndex() == SCENE_BOSS)
	{
		m_pTransformCom->Set_Scale(8.f, 8.f, 1.f);
	}
	else
		m_pTransformCom->Set_Scale(5.f, 5.f, 1.f);

	CreateHpBar();

	return S_OK;
}

_int CBoss::Update_GameObject(const _float &fTimeDelta)
{
	if (m_bDead)
		return DEAD;
	

	CPickingManager::GetInstance()->Remove_PickingGroup(this);

	m_fHoverTime += fTimeDelta;
	for (_float &fCooldown : m_fCooldown)
	{
		fCooldown = (std::max)(0.f, fCooldown - fTimeDelta);
	}

	UpdateState(fTimeDelta);

	if (m_ePathMode == PathMode::LR)
	{
		_float fMaxSpeedZ{ 0.f };
		if (m_eCurrentState == State::DASH)
		{
			fMaxSpeedZ = m_tRigidbodyConfig.fDash_Speed * 0.5f;
		}
		else
		{
			fMaxSpeedZ = m_tRigidbodyConfig.fMoveSpeed;
		}

		Maintain_LR_Anchor_Z(8.f, fMaxSpeedZ);
	}

	UpdateSpeed(fTimeDelta);
	
	_vec3 vPos = m_pTransformCom->Get_Info(INFO::INFO_POS);
	vPos.x += m_fVelocity_X * fTimeDelta;
	vPos.y = m_fBase_Y + Hover_Y();
	vPos.z += m_fVelocity_Z * fTimeDelta;
	m_pTransformCom->Set_Info(INFO::INFO_POS, vPos);

	Update_AfterImage(fTimeDelta);
	CCharacter::Update_GameObject(fTimeDelta);
	return NO_EVENT;
}

void CBoss::LateUpdate_GameObject(const _float &fTimeDelta)
{
	CCharacter::LateUpdate_GameObject(fTimeDelta);
	m_pColiderSphere->Update_ColliderSphere();
	Update_Position(m_pTransformCom->Get_Info(INFO_POS));
	m_pRendererCom->Add_RenderGroup(RENDER_NONALPHA, this);

	SetUp_BillBoard();

	if (m_bPickable && !m_bDead)
		CPickingManager::GetInstance()->Add_PickingGroup(this);

	if (Get_Helath() <= 0)
		m_prevDead = true;

	Set_Collider();
}

void CBoss::Render_GameObject()
{
	CCharacter::Render_GameObject();
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTransformCom->Apply_WorldMatrix();
	
	if (m_pTextureCom)
	{
		m_pTextureCom->Set_Texture(m_pTextureCom->Get_Frame().m_iCurrentTex);
		m_pTextureCom->MoveFrame();
	}

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pBufferCom->Render_Buffer();

	Render_AfterImage();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

#ifdef _DEBUG
	if (g_ColiderRender)
	{
		if (m_pColiderSphere)
			m_pColiderSphere->Render_ColliderSphere();
	}
#endif
}

void CBoss::Set_RectPath(const _vec3 &vCenter, _float fHalfX, _float fHalfZ, _float fY, _bool bRandom)
{
	m_ePathMode = bRandom ? PathMode::RECT_RANDOM : PathMode::RECT;

	m_vRectCenter = vCenter;
	m_fHalfX = fHalfX;
	m_fHalfZ = fHalfZ;

	m_vecWaypoints.clear();

	m_vecWaypoints.push_back({ m_vRectCenter.x - m_fHalfX, fY ,m_vRectCenter.z - m_fHalfZ });
	m_vecWaypoints.push_back({ m_vRectCenter.x + m_fHalfX, fY ,m_vRectCenter.z - m_fHalfZ });
	m_vecWaypoints.push_back({ m_vRectCenter.x + m_fHalfX, fY ,m_vRectCenter.z + m_fHalfZ });
	m_vecWaypoints.push_back({ m_vRectCenter.x - m_fHalfX, fY ,m_vRectCenter.z + m_fHalfZ });
	m_iWaypoint = 0;
	m_bPathReady = TRUE;

	m_pTransformCom->Set_Info(INFO::INFO_POS, m_vecWaypoints[0]);
	m_fBase_Y = fY;
}

void CBoss::Set_LinearLR(const _vec3 &vLeft, const _vec3 &vRight, _float fY)
{
	m_ePathMode = PathMode::LR;
	m_vLeft = vLeft;
	m_vRight = vRight;
	m_iDirLR = 1;
	m_pTransformCom->Set_Info(INFO::INFO_POS, Lerp_Vec3(m_vLeft, m_vRight, 0.5f));
	m_fBase_Y = fY;
	m_vTarget = vRight;
	m_tRigidbodyConfig.fHoverAmp = 0.6f;

	m_fOffset_FromPlayer_Z = std::fabs(m_pPlayer->GetTransform()->Get_Info(INFO::INFO_POS).z - GetTransform()->Get_Info(INFO::INFO_POS).z);

	m_bPathReady = TRUE;
	m_bSnap_To_Player_Z = TRUE;
}

void CBoss::UpdateSpeed(const float _fDeltaTime)
{
	m_fVelocity_X = m_tDamping_X.Step(m_fVelocity_X, m_fTargetVel_X, _fDeltaTime);
	m_fVelocity_Z = m_tDamping_Z.Step(m_fVelocity_Z, m_fTargetVel_Z, _fDeltaTime);

	const _float &fMaxMove = m_tRigidbodyConfig.fMoveSpeed;
	const _float &fMaxDash = m_tRigidbodyConfig.fDash_Speed;
	const _float fMaxSpeed = (m_eCurrentState == State::DASH) ? fMaxDash : fMaxMove;

	if (m_ePathMode == PathMode::LR)
	{
		m_fVelocity_X = std::clamp(m_fVelocity_X, -fMaxSpeed, fMaxSpeed);
		m_fVelocity_Z = std::clamp(m_fVelocity_Z, -fMaxSpeed, fMaxSpeed);
		return;
	}


	const _float fCurSpeed = std::sqrt(m_fVelocity_X * m_fVelocity_X + m_fVelocity_Z * m_fVelocity_Z);
	if (fCurSpeed > fMaxSpeed)
	{
		_float k = fMaxSpeed / fCurSpeed;
		m_fVelocity_X *= k;
		m_fVelocity_Z *= k;
	}
}

void CBoss::ChangeState(State _e)
{
	if (m_eCurrentState == _e)
		return;

	switch (m_eCurrentState)
	{
	case State::IDLE:		Exit_Idle();    break;
	case State::MOVE:		Exit_Move();    break;
	case State::DASH:		Exit_Dash();    break;
	case State::BULLET:		Exit_Bullet();	break;
	case State::MISSILE:	Exit_Missile(); break;
	}

	m_eCurrentState = _e;
	m_fStateDuration = 0.f;

	switch (m_eCurrentState)
	{
	case State::IDLE:		Enter_Idle();		break;
	case State::MOVE:		Enter_Move();		break;
	case State::DASH:		Enter_Dash();		break;
	case State::BULLET:		Enter_Bullet();		break;
	case State::MISSILE:	Enter_Missile();	break;
	}
}
void CBoss::UpdateState(const float _fDeltaTime)
{
	switch (m_eCurrentState)
	{
	case State::IDLE:		Update_Idle(_fDeltaTime);		break;
	case State::MOVE:		Update_Move(_fDeltaTime);		break;
	case State::DASH:		Update_Dash(_fDeltaTime);		break;
	case State::BULLET:		Update_Bullet(_fDeltaTime);		break;
	case State::MISSILE:	Update_Missile(_fDeltaTime);	break;
	}
}
void CBoss::Enter_Idle()
{
	// TODO - IDLE Anim

	m_fStayTime_Idle = Rand_Float(
		m_tRigidbodyConfig.fIdle_Min,
		m_tRigidbodyConfig.fIdle_Max);

	m_fTargetVel_X = 0.f;
	m_fTargetVel_Z = 0.f;
	Change_Texture(L"Idle");
}

void CBoss::Update_Idle(_float fDeltaTime)
{
	m_fStateDuration += fDeltaTime;

	if (m_fStateDuration >= m_fStayTime_Idle)
	{
		if (Is_Cooldown_Ready(0)) { ChangeState(State::DASH);    return; }
		if (Is_Cooldown_Ready(1)) { ChangeState(State::MISSILE); return; }
		if (Is_Cooldown_Ready(2)) { ChangeState(State::BULLET);  return; }

		ChangeState(State::MOVE);
	}

	if (m_ePathMode == PathMode::LR)
	{
		const _float fTarget_X = m_iDirLR > 0 ? m_vRight.x : m_vLeft.x;
		Set_Velocity_LR(m_tRigidbodyConfig.fMoveSpeed, 4.f, m_tRigidbodyConfig.fMoveSpeed);
		if (Arrived_X(fTarget_X))
		{
			Choose_Waypoint();
		}
	}
}

void CBoss::Exit_Idle()
{
	m_fStayTime_Idle = 0.f;
}

void CBoss::Enter_Move()
{
	// TODO - MOVE Anim

	m_fStayTime_Move = Rand_Float(
		m_tRigidbodyConfig.fMove_Min,
		m_tRigidbodyConfig.fMove_Max);

	if (m_ePathMode == PathMode::LR)
		m_vTarget = (m_iDirLR > 0) ? m_vRight : m_vLeft;
	else
		m_vTarget = m_vecWaypoints.empty() ?
		m_pTransformCom->Get_Info(INFO::INFO_POS) :
		m_vecWaypoints[m_iWaypoint];
}

void CBoss::Update_Move(_float fDeltaTime)
{
	m_fStateDuration += fDeltaTime;

	if (m_fStateDuration >= m_fStayTime_Move)
	{
		if (Is_Cooldown_Ready(0)) { ChangeState(State::DASH);    return; }
		if (Is_Cooldown_Ready(1)) { ChangeState(State::MISSILE); return; }
		if (Is_Cooldown_Ready(2)) { ChangeState(State::BULLET);  return; }

		ChangeState(State::IDLE);
	}

	if (m_ePathMode == PathMode::LR)
	{
		const _float fTarget_X = m_iDirLR > 0 ? m_vRight.x : m_vLeft.x;
		Set_Velocity_LR(m_tRigidbodyConfig.fMoveSpeed, 0.f, 0.f);
		if (Arrived_X(fTarget_X))
			Choose_Waypoint();
	}
	else
	{
		Set_Velocity_Towards(m_vTarget, m_tRigidbodyConfig.fMoveSpeed);
		if (Arrived(m_vTarget))
			Choose_Waypoint();
	}
}

void CBoss::Exit_Move()
{
	m_fStayTime_Move = 0.f;
}

void CBoss::Enter_Dash()
{
	m_iPhase = 0;
	m_fStateDuration = 0.f;
	m_vDashDir = Dash_Direction();
	Change_Texture(L"Idle");
}

void CBoss::Update_Dash(_float fDeltaTime)
{
	m_fStateDuration += fDeltaTime;
	if (m_iPhase == 0)
	{
		m_fTargetVel_X = 0.f;
		m_fTargetVel_Z = 0.f;

		if (m_fStateDuration >= m_tRigidbodyConfig.fDash_WindUp)
		{
			m_iPhase = 1;
			m_fStateDuration = 0.f;
			// TODO - DASH effect
		}
	}
	else if (m_iPhase == 1)
	{
		m_fTargetVel_X = m_vDashDir.x * m_tRigidbodyConfig.fDash_Speed;

		if (m_ePathMode == PathMode::LR)
		{
			_float fAnchor_Z = Get_Position().z;
			if (m_bSnap_To_Player_Z && m_pPlayer)
				fAnchor_Z = m_pPlayer->Get_Position().z + m_fOffset_FromPlayer_Z;
			_float fDz = fAnchor_Z - Get_Position().z;
			m_fTargetVel_Z = std::clamp(5.f * fDz, -m_tRigidbodyConfig.fDash_Speed, m_tRigidbodyConfig.fDash_Speed);
		}
		else
		{
			m_fTargetVel_Z = m_vDashDir.z * m_tRigidbodyConfig.fDash_Speed;
		}

		if (m_fStateDuration >= m_tRigidbodyConfig.fDash_Duration)
		{
			m_iPhase = 2;
			m_fStateDuration = 0.f;
		}
	}
	else
	{
		m_fTargetVel_X = 0.f;
		m_fTargetVel_Z = 0.f;

		if (m_fStateDuration >= m_tRigidbodyConfig.fDash_Recover)
		{
			Set_Cooldown(0, m_tRigidbodyConfig.fDash_Cooldown);
			ChangeState(State::IDLE);
		}
	}

	m_fDurationRecordTime += fDeltaTime;
	if (m_fDurationRecordTime >= m_fRecordTime)
	{
		Record_AfterImage();
		m_fDurationRecordTime -= m_fRecordTime;
	}
}

void CBoss::Exit_Dash()
{
}

void CBoss::Enter_Bullet()
{
	m_fStateDuration = 0.f;
	m_iPhase = 0;
	m_iShots = 0;
	Change_Texture(L"Bullet");
}

void CBoss::Update_Bullet(_float fDeltaTime)
{
	m_fStateDuration += fDeltaTime;
	if (m_iPhase == 0)
	{
		Follow_PathSpeed(0.65f, fDeltaTime);
		if (m_fStateDuration >= m_tRigidbodyConfig.fBul_WindUp)
		{
			m_iPhase = 1;
			m_fStateDuration = 0.f;
		}
	}
	else if (m_iPhase == 1)
	{
		Follow_PathSpeed(0.75f, fDeltaTime);
		if (m_iShots < m_tRigidbodyConfig.iBul_Burst &&
			m_fStateDuration >= m_tRigidbodyConfig.fBul_Interval)
		{
			m_fStateDuration = 0.f;

			_vec3 vTo = m_pPlayer->GetTransform()->Get_Info(INFO::INFO_POS) - m_pTransformCom->Get_Info(INFO::INFO_POS);
			::D3DXVec3Normalize(&vTo, &vTo);

			_float fBaseYaw = std::atan2(vTo.z, vTo.x);
			_float fOffDegree = (m_iShots - (m_tRigidbodyConfig.iBul_Burst - 1) * 0.5f) * m_tRigidbodyConfig.fBul_SpreadDeg;
			_float fYaw = fBaseYaw + fOffDegree * (D3DX_PI / 180.f);
			_vec3 vDir(std::cos(fYaw), vTo.y, std::sin(fYaw));

			Spawn_Bullet(vDir);

			++m_iShots;
		}
		if (m_iShots >= m_tRigidbodyConfig.iBul_Burst)
		{
			m_iPhase = 2;
			m_fStateDuration = 0.f;
		}
	}
	else
	{
		Follow_PathSpeed(0.65f, fDeltaTime);

		if (m_fStateDuration >= m_tRigidbodyConfig.fBul_Recover)
		{
			Set_Cooldown(2, m_tRigidbodyConfig.fBul_Cooldown);
			ChangeState(State::IDLE);
		}
	}
}

void CBoss::Exit_Bullet()
{
}

void CBoss::Enter_Missile()
{
	m_fStateDuration = 0.f;
	m_iPhase = 0;
	m_iVolley = 0;
	m_iShots = 0;
	Change_Texture(L"Missile");
}

void CBoss::Update_Missile(_float fDeltaTime)
{
	Follow_PathSpeed(0.65f, fDeltaTime);

	m_fStateDuration += fDeltaTime;

	switch (m_iPhase)
	{
	case 0: // WindUp
		if (m_fStateDuration >= m_tRigidbodyConfig.fMis_WindUp)
		{
			m_iPhase = 1;
			m_fStateDuration = 0.f;
			m_iShots = 0;
		}
		break;

	case 1: // Fire volley (한 볼리 안에서 N발, 간격 fMis_Interval)
		if (m_iShots < m_tRigidbodyConfig.iMis_PerVolley &&
			m_fStateDuration >= m_tRigidbodyConfig.fMis_Interval)
		{
			m_fStateDuration = 0.f;

			// 한 발 발사
			Spawn_Missile();
			++m_iShots;
		}

		// 볼리 완료 → 다음 단계로
		if (m_iShots >= m_tRigidbodyConfig.iMis_PerVolley)
		{
			m_iPhase = 2;
			m_fStateDuration = 0.f;
		}
		break;

	case 2: // Volley gap (다음 볼리까지 휴지)
		if (m_fStateDuration >= m_tRigidbodyConfig.fMis_VolleyGap)
		{
			m_fStateDuration = 0.f;
			++m_iVolley;

			if (m_iVolley < m_tRigidbodyConfig.iMis_Volley)
			{
				// 다음 볼리 시작
				m_iPhase = 1;
				m_iShots = 0;
			}
			else
			{
				// 모든 볼리 끝 → 리커버
				m_iPhase = 3;
			}
		}
		break;

	case 3: // Recover
		if (m_fStateDuration >= m_tRigidbodyConfig.fMis_Recover)
		{
			Set_Cooldown(1, m_tRigidbodyConfig.fMis_Cooldown);
			ChangeState(State::IDLE);
		}
		break;
	}
}

void CBoss::Exit_Missile()
{
}

_float CBoss::Hover_Y() const
{
	return m_tRigidbodyConfig.fHoverAmp * std::sinf(m_fHoverTime * (2.f * D3DX_PI) * m_tRigidbodyConfig.fHoverFreq);
}

void CBoss::Set_Velocity_Towards(const _vec3 &vTarget, _float fSpeed)
{
	_vec3 vTo = vTarget - m_pTransformCom->Get_Info(INFO::INFO_POS);
	_float fDelta = Lenght_XZ(vTo);
	if (fDelta < g_Epsilon) { m_fTargetVel_X = m_fTargetVel_Z = 0.f; return; }
	_float fSpd = fSpeed;
	if (fDelta < 1.2f) fSpd *= (fDelta / 1.2f);
	_vec3 vNorm = Norm_XZ(vTo);
	m_fTargetVel_X = vNorm.x * fSpd;
	m_fTargetVel_Z = vNorm.z * fSpd;
}

void CBoss::Set_Velocity_LR(_float fMoveSpeed, _float fKeepZ, _float fMaxSpeed_Z)
{
	const _vec3 vPosition = Get_Position();
	const _float fTarget_X = (m_iDirLR > 0) ? m_vRight.x : m_vLeft.x;

	_float fDx = fTarget_X - vPosition.x;
	_float fSpeed = fMoveSpeed;
	if (std::fabs(fDx) < 1.2f)
		fSpeed *= (std::fabs(fDx) / 1.2f);
	
	float fVelocity_X{ 0.f };
	if (fabs(fDx) > g_Epsilon)
		fVelocity_X = Sgnf(fDx) * fSpeed;

	m_fTargetVel_X = fVelocity_X;
}

void CBoss::Follow_PathSpeed(_float fScale, _float fDeltaTime)
{
	fScale = std::clamp(fScale, 0.f, 1.f);

	if (m_ePathMode == PathMode::LR)
	{
		const _float fTarget_X = (m_iDirLR > 0) ? m_vRight.x : m_vLeft.x;
		Set_Velocity_LR(m_tRigidbodyConfig.fMoveSpeed * fScale, 4.f, m_tRigidbodyConfig.fMoveSpeed);
		if (Arrived_X(fTarget_X))
		{
			Choose_Waypoint();
		}
	}
	else
	{
		if (Arrived(m_vTarget))
			Choose_Waypoint();
		Set_Velocity_Towards(m_vTarget, m_tRigidbodyConfig.fMoveSpeed * fScale);
	}
}

void CBoss::Choose_Waypoint()
{
	if (m_ePathMode == PathMode::LR)
	{
		m_iDirLR *= -1;
		m_vTarget = (m_iDirLR > 0) ? m_vRight : m_vLeft;
		return;
	}
	if (m_vecWaypoints.empty())
	{
		m_vTarget = m_pTransformCom->Get_Info(INFO::INFO_POS);
		return;
	}

	if (m_ePathMode == PathMode::RECT)
	{
		m_iWaypoint = (m_iWaypoint + 1) % (int)m_vecWaypoints.size();
	}
	else if (m_ePathMode == PathMode::RECT_RANDOM)
	{
		_int iNext;
		do { iNext = Rand_Int(0, (int)m_vecWaypoints.size() - 1); }
		while (iNext == m_iWaypoint);
		m_iWaypoint = iNext;
	}

	m_vTarget = m_vecWaypoints[m_iWaypoint];
}

_vec3 CBoss::Dash_Direction()
{
	if (m_ePathMode == PathMode::LR)
	{
		return _vec3((m_iDirLR > 0) ? _vec3{1.f, 0.f, 0.f} : _vec3{-1.f, 0.f, 0.f});
	}

	_vec3 vDir = m_vTarget - m_pTransformCom->Get_Info(INFO::INFO_POS);

	if (Lenght_XZ(vDir) < g_Epsilon)
		vDir = _vec3(1, 0, 0);

	return Norm_XZ(vDir);
}

_bool CBoss::Arrived(const _vec3 &v)
{
	return Lenght_XZ(v - m_pTransformCom->Get_Info(INFO::INFO_POS)) <= m_tRigidbodyConfig.fArriveRadius;
}

_bool CBoss::Arrived_X(_float fTarget_X)
{
	return std::fabs(m_pTransformCom->Get_Info(INFO::INFO_POS).x - fTarget_X) <= m_tRigidbodyConfig.fArriveRadius;
}

_vec3 CBoss::LR_TargetWorld()
{
	_float fAnchor_Z = m_vTarget.z;
	if (m_bSnap_To_Player_Z && m_pPlayer)
		fAnchor_Z = m_pPlayer->Get_Position().z + m_fOffset_FromPlayer_Z;

	const _float fX = (m_iDirLR > 0) ? m_vRight.x : m_vLeft.x;
	return _vec3{ fX, m_fBase_Y, fAnchor_Z };
}

void CBoss::Maintain_LR_Anchor_Z(_float fK, _float fMaxZ)
{
	if (!m_pPlayer)
		return;

	const _float fAnchor_Z = m_pPlayer->Get_Position().z + m_fOffset_FromPlayer_Z;
	const _float fDz = fAnchor_Z - Get_Position().z;

	const _float fCorr = std::clamp(fK * fDz, -fMaxZ, fMaxZ);

	m_fTargetVel_Z = fCorr;
}

void CBoss::Record_AfterImage()
{
	AfterImage tData;
	tData.vRight = GetTransform()->Get_Info(INFO::INFO_RIGHT);
	tData.vUp = GetTransform()->Get_Info(INFO::INFO_UP);
	tData.vLook = GetTransform()->Get_Info(INFO::INFO_LOOK);
	tData.vPos = GetTransform()->Get_Info(INFO::INFO_POS);
	tData.iFrame = m_pTextureCom->Get_Frame().m_iCurrentTex;

	m_dequeAfterImage.push_back(tData);
	if (m_dequeAfterImage.size() > m_iMaxAfterImage)
		m_dequeAfterImage.pop_front();
}

void CBoss::Update_AfterImage(const _float fDeltaTime)
{
	if (m_dequeAfterImage.size() <= 0)
		return;

	deque<AfterImage>::iterator itr = m_dequeAfterImage.begin();
	for (;itr != m_dequeAfterImage.end(); )
	{
		itr->fLife += fDeltaTime;
		if (itr->fLife >= m_fLifeLimit)
			itr = m_dequeAfterImage.erase(itr);
		else
			++itr;
	}
}

void CBoss::Render_AfterImage()
{
	if (m_dequeAfterImage.size() <= 0)
		return;

	DWORD oldColorOp, oldColorArg1, oldColorArg2;
	DWORD oldAlphaOp, oldAlphaArg1, oldAlphaArg2;

	m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &oldColorOp);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG1, &oldColorArg1);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG2, &oldColorArg2);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &oldAlphaOp);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &oldAlphaArg1);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG2, &oldAlphaArg2);


	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// Texture * TFACTOR
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

	for (const AfterImage &element : m_dequeAfterImage)
	{
		_float fTime = std::clamp(element.fLife / m_fLifeLimit, 0.f, 1.f);
		
		D3DXCOLOR color = Lerp_Color(m_AfterIamge_StartColor, m_AfterIamge_EndColor, fTime);

		DWORD dwTF = D3DCOLOR_COLORVALUE(color.r, color.g, color.b, color.a);
		m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwTF);

		_matrix matSrc;
		::D3DXMatrixIdentity(&matSrc);
		matSrc._11 = element.vRight.x; matSrc._12 = element.vRight.y; matSrc._13 = element.vRight.z;
		matSrc._21 = element.vUp.x; matSrc._22 = element.vUp.y; matSrc._23 = element.vUp.z;
		matSrc._31 = element.vLook.x; matSrc._32 = element.vLook.y; matSrc._33 = element.vLook.z;
		matSrc._41 = element.vPos.x; matSrc._42 = element.vPos.y; matSrc._43 = element.vPos.z;

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matSrc);
		m_pTextureCom->Set_Texture(element.iFrame);
		m_pBufferCom->Render_Buffer();
	}

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, oldColorOp);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, oldColorArg1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, oldColorArg2);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, oldAlphaOp);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, oldAlphaArg1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, oldAlphaArg2);
}

void CBoss::CreateHpBar()
{
	auto* hpBar = dynamic_cast<CBossHpBar*>(
		CObjectManager::GetInstance()->Clone_GameObject(
			L"Prototype_GameObject_HpbarUI_Boss", SCENE_STATIC, L"UI_Layer"));

	if (hpBar)
	{
		hpBar->Set_UIPosition(WINCX * 0.5f - 650.f, -200.f, 370.f, 60.f);
		hpBar->BindBoss(this); 
	}
}

void CBoss::Set_Collider_With_Bullet()
{
	CGameObject* pColliObj{ nullptr };
	if (CColiderManager::GetInstance()->CollisionGroupWho(CColiderManager::COLLISION_BULLET, this, CColiderManager::COLLISION_SPHERE, nullptr, pColliObj))
	{
		if (!pColliObj) // 예외처리
			return;

		// 플레이어 bullet 일 때
		if (dynamic_cast<CBullet*>(pColliObj)->Get_OwnerType() == BulletData::OWNER::PLAYER)
		{
			pColliObj->Set_Dead(true); // bullet dead 처리
			// 목숨 줄기
			m_fHp -= 1.f; // test : eunbi
		}
	}
}
