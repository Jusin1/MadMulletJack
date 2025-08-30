#pragma once
#include "CMonster.h"

// 스폰설정
struct HeadSpawnArg {
    const _tchar* texTag = L"Com_Texture_Katana_HEAD_DEATH";
    const _tchar* protoTag = L"Prototype_Component_Texture_Monster_Suit_Katana_HEAD";

    // 애니메이션
    int   endFrame = 35;     
    float animSpeed = 7.f;
    bool  loop = false;

    float fallSpeed = 6.f;  
    float gravity = 18.f;   
    float backDrift = 0.15f;  
};

class CMonster_Head : public CMonster
{
private:
    explicit CMonster_Head(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CMonster_Head(const CMonster_Head& rhs);
    virtual ~CMonster_Head();

public:
    virtual HRESULT Ready_GameObject() override;
    virtual HRESULT Initialize(void* pArg) override;           
    virtual _int    Update_GameObject(const _float& fTimeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void    Render_GameObject() override;

    static CMonster_Head* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone(void* pArg = nullptr) override;
    virtual void          Free() override;

private:
    virtual HRESULT Texture_Clone() override;   
    HRESULT         Change_Texture_Safe(const _tchar* pTag);
    HRESULT         EnsureHeadTextureRegisteredAndSelected();

    void            DisableCollisionAndPicking();

private:
    HeadSpawnArg m_cfg{};

    _vec3  m_initLook;
    float  m_backDrift;
    float  m_gravity;   
};