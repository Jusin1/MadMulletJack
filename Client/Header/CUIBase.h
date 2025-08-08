#pragma once
#include "CGameObject.h"
#include "CComponentMgr.h"
#include "Clinet_Define.h"
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
    void Add_Child(CUIBase* pChild); // 자식 추가
    CUIBase* Find_Child_ByTag(const _tchar* pTag); // 자식 찾을 때 태그로 찾음
public:
    void Set_ObjTag(const _tchar* pTag) { m_pObjTag = pTag; } // 태그 세팅
    const _tchar* Get_ObjTag() const { return m_pObjTag; } // 태그 가져오기

protected:
    virtual HRESULT			Set_Component();

protected:
    vector<CUIBase*> m_vecChildren; // 자식들

protected:
    const _tchar* m_pObjTag;
    
public:
    static CUIBase* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone(void* pArg = nullptr) override;
    virtual void Free() override;
};

