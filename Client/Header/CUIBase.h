#pragma once
#include "CGameObject.h"


class CUIBase :
    public CGameObject
{
protected:
    explicit CUIBase(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CUIBase(const CUIBase& rhs);
    virtual ~CUIBase();

public:
    virtual HRESULT Ready_GameObject() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual _int Update_GameObject(const _float& fTimeDelta) override;
    virtual void LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void Render_GameObject() override;

public:
    void Add_Child(CUIBase* pChild);

protected:
    vector<CUIBase*> m_vecChildren; // ÀÚ½Äµé

public:
    static CUIBase* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone(void* pArg = nullptr) override;
    virtual void Free() override;


};

