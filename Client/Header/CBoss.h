#pragma once
#include "CCharacter.h"
#include <deque>


struct RigidBodyConfig
{
	_float fHealth = 28.f;

	_float fMoveSpeed			= 6.f;
	_float fArriveRadius		= 0.35f;
	_float fCornerSlowDown		= 0.85f;

	// 진폭
	_float fHoverAmp			= 0.3f;
	// 빈도
	_float fHoverFreq			= 0.5f;

	_float fDash_WindUp = 0.20f;
	_float fDash_Speed = 22.f;
	_float fDash_Duration = 0.30f;
	_float fDash_Recover = 0.22f;
	_float fDash_Cooldown = 2.0f;

	// Missile
	_float fMis_WindUp = 0.22f;
	_float fMis_Interval = 0.22f;
	_int   iMis_Volley = 3;
	_int   iMis_PerVolley = 4;
	_float fMis_Recover = 0.28f;
	_float fMis_VolleyGap = 0.3f;
	_float fMis_Cooldown = 5.2f;

	// Bullet
	_float fBul_WindUp = 0.14f;
	_int   iBul_Burst = 32;
	_float fBul_SpreadDeg = 1.5f;
	_float fBul_Speed = 18.f;
	_float fBul_Recover = 0.22f;
	_float fBul_Interval = 0.1f;
	_float fBul_Cooldown = 1.3f;

	// Idle
	_float fIdle_Min = 0.3f;
	_float fIdle_Max = 1.3f;

	// Move
	_float fMove_Min = 0.5f;
	_float fMove_Max = 1.5f;

	// Range with Player
	_float fDesired = 20.f;
	_float fDeadzone = 1.f;
	_float fKeep = 6.f;
	_float fMaxPush = 18.f;
	_float fOrbitScale = 1.f;
};

struct AfterImage
{
	_vec3 vRight{ 0,0,0 };
	_vec3 vUp{ 0,0,0 };
	_vec3 vLook{ 0,0,0 };
	_vec3 vPos{ 0,0,0 };

	_uint iFrame{ 0 };
	_float fLife{ 0.00f };
};

struct SmoothDamp
{
	_float fVelocity{ 0.f };
	_float fSmoothTime{ 0.10f };
	_float Step(_float fCurrent, _float fTarget, _float fDeltaTime)
	{
		_float _fSmoothTime = (std::max)(1e-4f, fSmoothTime);
		_float fOmega = 2.0f / _fSmoothTime;
		_float fX = fOmega * fDeltaTime;
		_float fExp = 1.0f / (1.0f + fX * (1.0f + 0.48f * fX + 0.235f * fX * fX));
		_float fChange = fCurrent - fTarget;
		_float tmp = (fVelocity + fOmega * fChange) * fDeltaTime;
		fVelocity = (fVelocity - fOmega * tmp) * fExp;
		return fTarget + (fChange + tmp) * fExp;
	}
};

class CBoss : public CCharacter
{
public:
	enum State
	{
		IDLE,
		MOVE,
		DASH,
		BULLET,
		MISSILE,
		DEATH
	};
	enum PathMode
	{
		RECT,
		RECT_RANDOM,
		LR
	};
private:
	explicit CBoss(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBoss(const CBoss &rhs);
	virtual ~CBoss();

	virtual void Free() override;
public:
	static  CBoss *Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject *Clone(void *pArg = nullptr) override;
	
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void *pArg) override;
	virtual			_int		Update_GameObject(const _float &fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float &fTimeDelta);
	virtual			void		Render_GameObject();

	void Set_RectPath(const _vec3 &vCenter, _float fHalfX, _float fHalfZ, _float fY, _bool bRandom);
	void Set_LinearLR(const _vec3 &vLeft, const _vec3 &vRight, _float fY);
	void Set_Player(CGameObject *pPlayer) { m_pPlayer = pPlayer; }

	_float Get_Helath() const { return m_tRigidbodyConfig.fHealth; }
	void Set_Health(_float _f) { m_tRigidbodyConfig.fHealth = _f; }
	void Add_Health(_float _f) { m_tRigidbodyConfig.fHealth += _f; }
	_bool Get_PrevDead() const { return m_prevDead; }
private:
	HRESULT	Set_Component();
	void Set_Collider();
	void SetUp_BillBoard();
	HRESULT Texture_Clone();
	HRESULT Change_Texture(const _tchar *AnimTag);

	void Random_Speak(const _float fDeltaTime);
	void Random_RocketSound();
	virtual _bool Picking(_vec3 *PickingPoint) override;
	virtual void  PickingTrue() override;

	void Spawn_Missile();
	void Spawn_Bullet(const _vec3 &vDir);

	void UpdateSpeed(const float _fDeltaTime);
	void ChangeState(State _e);
	void UpdateState(const float _fDeltaTime);
	void Enter_Idle();
	void Update_Idle(_float fDeltaTime);
	void Exit_Idle();

	void Enter_Move();
	void Update_Move(_float fDeltaTime);
	void Exit_Move();

	void Enter_Dash();
	void Update_Dash(_float fDeltaTime);
	void Exit_Dash();

	void Enter_Bullet();
	void Update_Bullet(_float fDeltaTime);
	void Exit_Bullet();

	void Enter_Missile();
	void Update_Missile(_float fDeltaTime);
	void Exit_Missile();

	void Enter_Death();
	void Update_Death(_float fDeltaTime);

	void Force_Dash() { if (Is_Cooldown_Ready(0)) ChangeState(State::DASH); }
	void Force_Missile() { if (Is_Cooldown_Ready(1)) ChangeState(State::MISSILE); }
	void Force_Bullet() { if (Is_Cooldown_Ready(2)) ChangeState(State::BULLET); }
	void Force_Move() { ChangeState(State::MOVE); }
	void Force_Idle() { ChangeState(State::IDLE); }

	// 움직임쪽
	_float Hover_Y() const;
	_float Fall_Y(const _float fDeltaTime) const;
	void Set_Velocity_Towards(const _vec3 &vTarget , _float fSpeed);
	void Set_Velocity_LR(_float fMoveSpeed, _float fKeepZ, _float fMaxSpeed_Z);
	void Follow_PathSpeed(_float fScale, _float fDeltaTime);
	void Choose_Waypoint();
	_vec3 Dash_Direction();
	_bool Arrived(const _vec3 &v);
	_bool Arrived_X(_float fTarget_X);
	_float Sgnf(_float fV) { return fV >= 0.f ? 1.f : -1.f; }
	_vec3 LR_TargetWorld();
	void Maintain_LR_Anchor_Z(_float fK, _float fMaxZ);
	void Record_AfterImage();
	void Update_AfterImage(const _float fDeltaTime);
	void Render_AfterImage();
	void Random_ExplosionSound();

	void Set_Cooldown(_int iIndex, _float fTime) { m_fCooldown[iIndex] = (std::max)(m_fCooldown[iIndex], fTime); }
	_bool Is_Cooldown_Ready(_int iIndex) const { return m_fCooldown[iIndex] <= 0.f; }

	// 보스 HpBar 생성
	void CreateHpBar();

	//collider with bullet
	void Set_Collider_With_Bullet();
private:
	CGameObject *m_pPlayer{ nullptr };
	State m_ePrevState{ State::IDLE };
	State m_eCurrentState{ State::IDLE };
	PathMode m_ePathMode{ PathMode::RECT };
	_bool m_bPathReady{ FALSE };
	_float m_fStateDuration{ 0.f };
	_float m_fStayTime_Idle{ 0.f };
	_float m_fStayTime_Move{ 0.f };
	_float m_fHoverTime{ 0.f };
	_float m_fBase_Y{ 0.f };

	// 움직임 이쁘게
	_float m_fVelocity_X{ 0.f };
	_float m_fVelocity_Z{ 0.f };
	_float m_fTargetVel_X{ 0.f };
	_float m_fTargetVel_Z{ 0.f };
	_bool m_bSnap_To_Player_Z{ FALSE };
	_float m_fOffset_FromPlayer_Z{ 0.f };
	_float m_fFallSpeed{ -5.f };
	const _float m_fExplosionEffectDuration{ 0.5f };
	SmoothDamp m_tDamping_X;
	SmoothDamp m_tDamping_Z;

	// 움직임 사각형일 때
	_vec3 m_vRectCenter{ 0.f,0.f,0.f };
	_float m_fHalfX{ 0.f };
	_float m_fHalfZ{ 0.f };
	_int m_iWaypoint{ 0 };
	std::vector<_vec3> m_vecWaypoints;

	// 움직임 좌우 일 때
	_vec3 m_vLeft{ 0.f,0.f,0.f };
	_vec3 m_vRight{ 0.f,0.f,0.f };
	_int m_iDirLR{ 1 };

	// 타겟 좌표
	_vec3 m_vTarget{ 0.f,0.f,0.f };
	const _float m_fDistance{ 30.f };

	// 패턴
	// 0 windup
	// 1 active
	// 2 recover
	_int m_iPhase{ 0 };
	_vec3 m_vDashDir{ 1.f,0.f,0.f };
	_int m_iVolley{ 0 };
	_int m_iShots{ 0 };

	// dash, bullet, missle
	_float m_fCooldown[3]{ 0.f, 0.f, 0.f };
	RigidBodyConfig m_tRigidbodyConfig{};
	
	CTexture *m_pTextureCom;
	CColider_Sphere *m_pColiderSphere;
	_bool m_bPickable;

	_vec3 m_vPickedPosition{ 0.f, 0.f, 0.f };

	// 잔상
	deque<AfterImage> m_dequeAfterImage;
	_float m_fDurationRecordTime{ 0.00f };
	const _uint m_iMaxAfterImage{ 15 };
	const _float m_fLifeLimit{ 0.30f };
	const _float m_fRecordTime{ 0.05f };
	const D3DXCOLOR m_AfterIamge_StartColor{ 1.00f, 0.20f, 1.00f, 1.f };
	const D3DXCOLOR m_AfterIamge_EndColor{ 0.20f, 1.00f, 1.00f, 1.f };

	_bool m_prevDead{ false };
	_bool m_bPlaySound{ false };
	_bool m_bAttacked{ false };
	const _float m_fGetAttackCoolTime{ 0.3f };
	_float m_fAttackCoolDuration{ 0.f };

	_float m_fRandomVoiceDuration{ 0.f };
	_float m_fRandomVoiceCoolTime{ 3.f };
};

