#pragma once
#include "CMonster.h"



class CMonster_Head1 : public CMonster
{
private:
    explicit CMonster_Head1(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CMonster_Head1(const CMonster_Head1& rhs);
    virtual ~CMonster_Head1();

public:
    virtual HRESULT Ready_GameObject() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual _int    Update_GameObject(const _float& fTimeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void    Render_GameObject() override;

    static CMonster_Head1* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone(void* pArg = nullptr) override;
    virtual void          Free() override;

private:
    virtual HRESULT Texture_Clone() override;
    HRESULT         EnsureHeadTextureRegisteredAndSelected();
    void            DisableCollisionAndPicking();

private:
    HeadSpawnArg m_cfg{};          
    _vec3  m_initLook{ 0.f,0.f,1.f };
    float  m_backDrift{ 0.15f };
    float  m_gravity{ 4.f };
    float m_deleteTimer{ 0.f };
};
