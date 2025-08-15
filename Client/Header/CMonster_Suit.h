#pragma once
#include "CMonster.h"
class CMonster_Suit : public CMonster
{
    enum MON_STATE { IDLE, JUMP, CHASE, AIM, SHOT, AVOID, HIT, DEATH, };
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

public:
    static  CMonster_Suit* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone(void* pArg = nullptr) override;
    virtual void           Free() override;
private:
    void        SetState(MON_STATE next);
    void        OnEnterState(MON_STATE s);
    void        OnUpdateState(MON_STATE s, const _float& dt);

    _vec3       GetHeadWorldPos() const;
    bool        WorldToScreen(const _vec3& world, float& sx, float& sy) const;

    CTransform* GetPlayerTransform();
    float       DistanceToPlayer() const;

    void        TrySpawnDeathUI();
    void        SetupHitSpheres();
    void SetDebugShowPartSpheres(bool b) { m_bDebugShowPartSpheres = b; }


#ifdef _DEBUG
    void        DebugRender_HitSpheres() const;
#endif

private:
    MON_STATE   m_eMonState;
    MON_STATE   m_ePrevState;
    CTransform* m_pPlayerTr;
    float       m_fChaseRadius;
    float       m_fAimRadius;
    float       m_fLoseRadius;
    float       m_jumpCD;
    int         m_jumpDir;
    bool        m_bKillAfterHit;

    HIT_PART    m_lastFatalPart = HIT_BODY;
    bool        m_pendingDeathUI = false;

    HIT_PART    m_cachedHitPart = HIT_UNKNOWN;

    std::vector<PartSphere> m_hitSpheres;

    bool        m_bDebugShowPartSpheres = false;
};

