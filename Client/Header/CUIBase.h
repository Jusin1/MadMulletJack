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

    void		Set_UIPos(_vec3 _vPos, _float _offsetX, _float _offesetY); // pos를 기준으로 offset을 줘서 pos를 셋팅

    // getter setter
public:
    void Set_ObjTag(const _tchar* pTag) { m_pObjTag = pTag; } // 태그 세팅
    const _tchar* Get_ObjTag() const { return m_pObjTag; } // 태그 가져오기

    void Set_RotSum(_float _fRotSum) { m_fRotSum = _fRotSum; }
    _float Get_RotSum() const { return m_fRotSum; }

protected:
    virtual HRESULT			Set_Component();
    void Set_Origin_Rot(); // 전에 local 행렬을 돌렸다면 다시 돌려두기
    void Set_New_TransInfo(_float _fSpeed, _float _fRotSpeed); // 현재 위치 + new speed, new rotSpeed 지정

protected:
    list<CUIBase*> m_vecChildren; // 자식들

protected:
    const _tchar* m_pObjTag;
    _float m_fRotSum; // texture 바뀌고 돌린만큼 다시 돌리기 위해

public:
    static CUIBase* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone(void* pArg = nullptr) override;
    virtual void Free() override;
};

