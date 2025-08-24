#pragma once
#include "CMonster.h"

class CMonster_Suit : public CMonster
{
    enum MON_STATE { IDLE, JUMP, CHASE, AIM, SHOT, AVOID, HIT, KICKED, INSKILL, DEATH, };

public:
    enum HIT_PART { HIT_HEAD, HIT_BODY, HIT_BALLS, HIT_LEG, HIT_UNKNOWN };

    // 부위 판정 구조체
    struct PartSphere {
        HIT_PART part;      // 부위
        _vec3    localCenter; // 몬스터 중심
        float    radius;    // 반지름
        int      priority;  // 우선순위
        float    xScale;    // X축 스케일
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


    // 피격처리 -> 부위 판정
    virtual void    HitAt(const _vec3& hitPosWorld) override;

    void            ApplyDamage(HIT_PART part, int dmg);

    void            Set_Collider();

public:
    static  CMonster_Suit* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone(void* pArg = nullptr) override;
    virtual void           Free() override;

private:
    void        SetState(MON_STATE next);                // 상태 전환
    void        OnEnterState(MON_STATE s);               // 상태 진입시 초기화
    void        OnUpdateState(MON_STATE s, const _float& dt); // 상태별 매 프레임 업데이트

    // 유틸리티 함수
    _vec3       GetHeadWorldPos() const;                 // 머리 위치(world)
    bool        WorldToScreen(const _vec3& world, float& sx, float& sy) const; // 좌표 투영

    CTransform* GetPlayerTransform();                    // 플레이어 Transform 얻기
    float       DistanceToPlayer() const;                // 플레이어까지 거리

    // 사망시 UI 출력
    void        TrySpawnDeathUI();

    // 피격 구체 초기화
    void        SetupHitSpheres();

    // 충돌 구체 시각화
    void SetDebugShowPartSpheres(bool b) { m_bDebugShowPartSpheres = b; }

#ifdef _DEBUG
    void        DebugRender_HitSpheres() const;          // HitSphere 디버그 렌더링
#endif

private:
    // 현재/이전 상태
    MON_STATE   m_eMonState;
    MON_STATE   m_ePrevState;

    CTransform* m_pPlayerTr;

    // 추적/조준/이탈 거리
    float       m_fChaseRadius;
    float       m_fAimRadius;
    float       m_fLoseRadius;

    float       m_jumpCD;       // 점프 쿨타임
    int         m_jumpDir;      // 좌우 점프 방향

    // 즉시 죽음 여부 -> 머리 한 방
    bool        m_bKillAfterHit;

    // 최종으로 맞은 부위
    HIT_PART    m_lastFatalPart = HIT_BODY;

    // 사망 UI 예약
    bool        m_pendingDeathUI = false;

    // 최근 Picking으로 판정된 부위
    HIT_PART    m_cachedHitPart = HIT_UNKNOWN;


    std::vector<PartSphere> m_hitSpheres;

    // 디버그용 플래그
    bool        m_bDebugShowPartSpheres = false;

    private:
    // 콤보 계산용 (모든 몬스터 공통)
    static ULONGLONG s_lastKillTimeMs;
    static int       s_comboCount;
};

