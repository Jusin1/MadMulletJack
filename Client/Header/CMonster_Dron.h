#pragma once
#include "CMonster.h"
class CMonster_Dron :
    public CMonster
{
public:
    enum MON_STATE { IDLE, WAKE, ATTACK, KATANA_DEATH, DEATH };

private:
    explicit CMonster_Dron(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CMonster_Dron(const CMonster_Dron& rhs);
    virtual ~CMonster_Dron();

public:
    virtual HRESULT Ready_GameObject() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual _int    Update_GameObject(const _float& fTimeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void    Render_GameObject() override;

    float           Get_DetectRadius() const { return m_fDetectRadius; }
    void            Set_DetectRadius(float f) { m_fDetectRadius = f; }
    float           Get_LoseRadius() const { return m_fLoseRadius; }
    void            Set_LoseRadius(float f) { m_fLoseRadius = f; }
protected:
    virtual HRESULT Texture_Clone() override;
    virtual _bool   Picking(_vec3* PickingPoint) override;   // 부위판정 없음(단일 스피어)
    virtual void    HitAt(const _vec3& hitPosWorld) override; // 총/투사체 등 픽킹 히트 → 즉사
    void            Set_Collider();
    void            GetDeathUIConfig(DeathUIConfig& cfg, bool isHeadshot) const override;
    void            Set_Check_Weapon(); // 카타나 겹침 감지 → KATANA_DEATH
    void            Set_Collider_With_Bullet();
public:
    static  CMonster_Dron* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone(void* pArg = nullptr) override;
    virtual void           Free() override;

private:
    void        SetState(MON_STATE next);
    void        OnEnterState(MON_STATE s);
    void        OnUpdateState(MON_STATE s, const _float& dt);

    _vec3       GetHeadWorldPos() const;
    bool        WorldToScreen(const _vec3& world, float& sx, float& sy) const;
#ifdef _DEBUG
    void        DebugRender_HitSpheres() const {} // 드론은 부위 스피어 없음. 필요시 바디 스피어 디버그로 교체 가능
#endif

private:
    MON_STATE   m_eMonState = IDLE;
    MON_STATE   m_ePrevState = IDLE;

    float       m_fDetectRadius = 12.f; // IDLE→WAKE 전환 반경
    float       m_fLoseRadius = 18.f; // ATTACK→IDLE 복귀 반경
    float m_fKeepDistance = 2.4f; // 유지하고 싶은 거리(대략 2.0~3.0 사이로 취향 조절)
    float m_fKeepHysteresis = 0.4f; // 히스테리시스(왕복/덜컥거림 방지)
};

