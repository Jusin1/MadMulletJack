#pragma once
#include "CMonster.h"

class CMonster_Suit : public CMonster
{
    enum MON_STATE { IDLE, JUMP, CHASE, AIM, SHOT, AVOID, HIT, HIT_ELECTRIC, HIT_BENT, HIT_DOOR, KICKED, INSKILL, KATANA_DEATH, LAZER, HIT_SHOTGUN, DEATH };

public:
    enum HIT_PART { HIT_HEAD, HIT_BODY, HIT_BALLS, HIT_LEG, HIT_UNKNOWN };

    struct PartSphere {
        HIT_PART part;
        _vec3    localCenter;
        float    radius;
        int      priority;
        float    xScale;
    };

private:
    explicit CMonster_Suit(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CMonster_Suit(const CMonster_Suit& rhs);
    virtual ~CMonster_Suit();

public:
    virtual HRESULT Ready_GameObject();
    virtual HRESULT Initialize(void* pArg) override;
    virtual _int    Update_GameObject(const _float& fTimeDelta);
    virtual void    LateUpdate_GameObject(const _float& fTimeDelta);
    virtual void    Render_GameObject();

protected:
    virtual HRESULT Texture_Clone() override;
    virtual _bool   Picking(_vec3* PickingPoint) override;

    virtual void    HitAt(const _vec3& hitPosWorld) override;
    void            ApplyDamage(HIT_PART part, int dmg);

    void            Set_Collider();
    void            GetDeathUIConfig(DeathUIConfig& cfg, bool isHeadshot) const override;
    void            Set_Check_Weapon();

public:
    static  CMonster_Suit* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone(void* pArg = nullptr) override;
    virtual void           Free() override;

public:
    // 스나이퍼 모드 강제 진입
    void ForceSniperMode();

private:
    void        SetState(MON_STATE next);
    void        OnEnterState(MON_STATE s);
    void        OnUpdateState(MON_STATE s, const _float& dt);
    _vec3       GetHeadWorldPos() const;
    bool        WorldToScreen(const _vec3& world, float& sx, float& sy) const;
    void        SetupHitSpheres();
    // 레이저 관련 함수
    void  EnsureLaserPrimed(); // 레이저 세팅
    void  UpdateLaser(float dt); // 레이저 업데이트


#ifdef _DEBUG
    void        DebugRender_HitSpheres() const;
#endif

private:
    // 몬스터 상태
    MON_STATE   m_eMonState;
    MON_STATE   m_ePrevState;

    // 추적용 변수
    float       m_fChaseRadius;
    float       m_fAimRadius;
    float       m_fLoseRadius;

    // 점프
    float       m_jumpCD;
    int         m_jumpDir;

    // 충돌 판정 부위
    bool        m_bKillAfterHit;
    HIT_PART    m_cachedHitPart;
    std::vector<PartSphere> m_hitSpheres;
    bool        m_bDebugShowPartSpheres;

    // 발사주기
    float       m_shotTimer;

    // 콤보
    float  m_kbTime;
    float  m_kbDur;
    float  m_kbTotalDist;
    float  m_kbProgress;
    _vec3  m_kbDir;

private:
    // 레이저 상태
    enum LASER_PHASE { LZ_SWEEP, LZ_LOCK, LZ_FIRE, LZ_COOLDOWN };

    // 스나이퍼/레이저
    bool  m_bSniperScene;
    bool  m_bLaserPrimed;
    _vec3 m_laserPos;
    _vec3 m_laserDir;
    float m_laserLength;

    // 레이저이동
    float m_sweepT;
    float m_yawFreq;
    float m_startSway;

    // 높이 보정
    float m_laserMuzzleDown;
    float m_targetYOffset;

    // 피치 제한
    float m_maxPitchDeg;

    // 페이즈 타이밍
    LASER_PHASE m_lzPhase;
    float m_lzTimer;
    float m_lzSweepDuration;
    float m_lzLockDuration;
    float m_lzFireDuration;
    float m_lzCooldown;

    _vec3 m_lzLockedPos;
    _vec3 m_lzLockedDir;

    // 레이저 두께
    float m_laserAimWidthPx;
    float m_laserFireWidthPx;

};
