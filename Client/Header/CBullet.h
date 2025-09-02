#pragma once
#include "CGameObject.h"

namespace Engine
{
    class CTexture;
    class CColider_Sphere;
    class CVIBuffer_Rect;
}

class CBullet : public CGameObject
{
protected:
    explicit CBullet(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CBullet(const CBullet& rhs);
    virtual ~CBullet();

public:
    virtual HRESULT Ready_GameObject();
    virtual HRESULT Initialize(void* pArg);
    virtual _int    Update_GameObject(const _float& fTimeDelta);
    virtual void    LateUpdate_GameObject(const _float& fTimeDelta);
    virtual void    Render_GameObject();

    virtual HRESULT Spawn_Pooling(void *pArg = nullptr) override;
    virtual HRESULT Despawn_Pooling() override;
protected:
    HRESULT Set_Component();
    void    Set_Collider_With_Wall();
    void    SetUp_BillBoard();
    void    Set_Collider(const _float& fTimeDelta);

protected:
    virtual HRESULT Texture_Clone();
    virtual HRESULT Change_Texture(const _tchar* LayerTag);

protected:
    Engine::CColider_Sphere* m_pColiderCom;
    Engine::CTexture* m_pTextureCom;
    Engine::CVIBuffer_Rect* m_pBufferCom;

    _float m_fSpeed;        // 총알 속도
    _vec3  m_vMoveDir;      // 이동 방향

    float m_fLifeTime;      // 현재까지 지난 시간
    float m_fLifeLimit;     // 총알 생존 제한 시간

public:
    void Fire(const _vec3& vPos, const _vec3& vDir);

public:
    static  CBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone(void* pArg = nullptr) override;
    virtual void Free() override;
};

