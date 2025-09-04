#pragma once
#include "CGameObject.h"

class CPrefab;

namespace Engine
{
    class CColider_Sphere;
    class CVIBuffer_Circle;
}

class CWarningCircle;

struct MissileData
{
    _vec3 vLaunchPos{ 0.f,0.f,0.f };
    _vec3 vTargetPos{ 0.f,0.f,0.f };
    _vec3 vDir{ 0.f,0.f,0.f };
};

class CMissile : public CGameObject
{
public:
    enum State
    {
        LAUNCH,
        TARGET
    };
private:
	explicit CMissile(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMissile(const CMissile &rhs);
	virtual ~CMissile();

    virtual void Free() override;
public:
    static  CMissile *Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject *Clone(void *pArg = nullptr) override;
    
    virtual HRESULT Ready_GameObject();
    virtual HRESULT Initialize(void *pArg);
    virtual _int    Update_GameObject(const _float &fTimeDelta);
    virtual void    LateUpdate_GameObject(const _float &fTimeDelta);
    virtual void    Render_GameObject();

    virtual HRESULT Spawn_Pooling(void *pArg = nullptr) override;
    virtual HRESULT Despawn_Pooling() override;
public:
    void Fire(const _vec3 &vLaunchPos, const _vec3 &vTargetPos, const _vec3 &_vDir);

private:
    void ChangeState(State _e);
    void UpdateState(const float _fDeltaTime);
    void Set_Destroy(const _vec3 &vPos);

    HRESULT Set_Component();
    HRESULT Set_MissileModel();
    void    Set_Collider_With_Wall();
    void    Set_Buffer_Color(const _float &fDeltaTime);
    void    Spawn_Destroy_Effect(const _vec3 &vPos);
    void    Spawn_Smoke_Effect();
    void    Spawn_Warning_Effect();

    void    Launch_Enter();
    void    Launch_Update(const _float &fDeltaTime);
    void    Launch_Exit();

    void    Target_Enter();
    void    Target_Update(const _float &fDeltaTime);
    void    Target_Exit();

    void Random_ExplosionSound();
    void Set_ModelTransform();
    void Turn_Towards(const _vec3 &vDir, _float fMaxDegreePerSeconds, _float fDeltaTime);
    void Turn_Yaw_Towards(const _vec3 &vDir_XZ, _float fMaxDegreePerSeconds, _float fDeltaTime);
private:
    State m_eCurrentState{State::LAUNCH};
    CGameObject *m_pPlayer{ nullptr };
    CGameObject *m_pWarningCirlce{ nullptr };
    CColider_Sphere *m_pColiderCom{ nullptr };
    CVIBuffer_Circle *m_pBufferCom{ nullptr };
    _vec3 m_vLaunchPos{ 0.f, 0.f, 0.f };
    _vec3 m_vFinalTargetPos{ 0.f, 0.f, 0.f };
    _vec3 m_vTargetPos{ 0.f, 0.f, 0.f };
    const _float m_fTargetRadius{ 4.f };
    const _float m_fSmokeEffect_Cool{ 0.2f };
    _float m_fSmokeEffect_Duration{ 0.f };
    _float m_fChangeColor_Duration{ 0.f };
    const _float m_fChangeColor_Frequence{ 2.f };
    const _float m_fLaunchSpeed{ 5.f };
    _float m_fTargetingSpeed{ 10.f };
    const _float m_fArriveRadius{ 0.4f };
    const _float m_fArriveEps{ 0.1f };
    _vec3  m_vMoveDir{0.f, 0.f, 1.f};
    CPrefab *m_pModel{ nullptr }; // Prefab에는 충돌체 없도록 오직 update와 렌더만
};

