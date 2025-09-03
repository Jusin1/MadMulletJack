#pragma once
#include "CGameObject.h"
#include "Clinet_Define.h"

namespace Engine
{
    class CTexture;
    class CColider_Sphere;
    class CVIBuffer_Rect;
}

class CItem : 
	public CGameObject
{
public:
    typedef struct ItemInfo
    {
        WEAPON2 eWeapon = WP_DOPING;
        _vec3 vStartPos = {0.f,0.f,0.f};
    }ITEMINFO;

private:
    explicit CItem(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CItem(const CItem& rhs);
    virtual ~CItem();

public:
    virtual HRESULT Ready_GameObject();
    virtual HRESULT Initialize(void* pArg);
    virtual _int    Update_GameObject(const _float& fTimeDelta);
    virtual void    LateUpdate_GameObject(const _float& fTimeDelta);
    virtual void    Render_GameObject();

    // getter setter func
public:
    ITEMINFO& Get_ItemInfo()  { return m_tItemInfo; }
    void Set_ItemInfo(ITEMINFO _tItemInfo) { m_tItemInfo = _tItemInfo; }

private:
    HRESULT Set_Component();
    void    Set_Collider(const _float& fTimeDelta);

private:
    virtual HRESULT Texture_Clone();

private:
    Engine::CColider_Sphere* m_pColiderCom;
    Engine::CTexture* m_pTextureCom;
    Engine::CVIBuffer_Rect* m_pBufferCom;

private:
    ITEMINFO m_tItemInfo;

    _bool m_bMove; // 움직일래 말래
    _float m_fMinY;          // 움직임 멈출 y값 
    _float m_fLifeTime;      // 현재까지 지난 시간
    _float m_fLifeLimit;     // 라이프 타임

public:
    static  CItem* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone(void* pArg = nullptr) override;
    virtual void Free() override;
};

