#pragma once
#include "CCharacter.h"
#include <random>

struct RigidBodyConfig
{
	_float fMoveSpeed			= 5.f;
	_float fArriveRadius		= 0.35f;
	_float fCornerSlowDown		= 0.85f;

	// 진폭
	_float fHoverAmp			= 0.1f;
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
	_float fMis_Recover = 0.28f;
	_float fMis_Cooldown = 3.2f;

	// Bullet
	_float fBul_WindUp = 0.14f;
	_int   iBul_Burst = 6;
	_float fBul_SpreadDeg = 7.f;
	_float fBul_Speed = 18.f;
	_float fBul_Recover = 0.22f;
	_float fBul_Interval = 0.1f;
	_float fBul_Cooldown = 1.3f;

	// Idle
	_float fIdle_Min = 0.45f;
	_float fIdle_Max = 1.10f;
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
		MISSILE
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
private:
	HRESULT	Set_Component();
	void Set_Collider();
	void SetUp_BillBoard();
	HRESULT Texture_Clone() { return S_OK; }

	virtual _bool Picking(_vec3 *PickingPoint) override;
	virtual void  PickingTrue() override;

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

	void Force_Dash() { if (Is_Cooldown_Ready(0)) ChangeState(State::DASH); }
	void Force_Missile() { if (Is_Cooldown_Ready(1)) ChangeState(State::MISSILE); }
	void Force_Bullet() { if (Is_Cooldown_Ready(2)) ChangeState(State::BULLET); }
	void Force_Move() { ChangeState(State::MOVE); }
	void Force_Idle() { ChangeState(State::IDLE); }

	// 움직임쪽
	_float Hover_Y() const;
	void Set_Velocity_Towards(const _vec3 &vTarget , _float fSpeed);
	void Follow_PathSpeed(_float fScale);
	void Choose_Waypoint();
	_vec3 Dash_Direction();
	_bool Arrived(const _vec3 &v);

	// 수학쪽
	_float Lerp(_float fA, _float fB, _float fT);
	_vec3 Lerp(_vec3 vA, _vec3 vB, _float fT);
	_float Lenght_XZ(const _vec3 &v) { return std::sqrtf(v.x * v.x + v.z * v.z); }
	_vec3 Norm_XZ(const _vec3 &v)
	{
		_float fLength = Lenght_XZ(v);
		if (fLength < 1e-6f) return D3DXVECTOR3(0, 0, 0);
		return D3DXVECTOR3(v.x / fLength, 0.f, v.z / fLength);
	}
	_float Rand_Float(_float fA, _float fB) { return std::uniform_real_distribution<_float>(fA, fB)(rng); }
	_int Rand_Int(_int iA, _int iB) { return std::uniform_int_distribution<_int>(iA, iB)(rng); }
	void Set_Cooldown(_int iIndex, _float fTime) { m_fCooldown[iIndex] = (std::max)(m_fCooldown[iIndex], fTime); }
	_bool Is_Cooldown_Ready(_int iIndex) const { return m_fCooldown[iIndex] <= 0.f; }
private:
	CGameObject *m_pPlayer{ nullptr };
	State m_ePrevState{ State::IDLE };
	State m_eCurrentState{ State::IDLE };
	PathMode m_ePathMode{ PathMode::RECT };
	_bool m_bPathReady{ FALSE };
	_float m_fStateDuration{ 0.f };
	_float m_fStayTime_Idle{ 0.f };
	_float m_fHoverTime{ 0.f };
	_float m_fBase_Y{ 0.f };

	// 움직임 이쁘게
	_float m_fVelocity_X{ 0.f };
	_float m_fVelocity_Z{ 0.f };
	_float m_fTargetVel_X{ 0.f };
	_float m_fTargetVel_Z{ 0.f };
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
	std::mt19937 rng;
	
	CTexture *m_pTextureCom;
	CColider_Sphere *m_pColiderSphere;
	_bool m_bPickable;
};

