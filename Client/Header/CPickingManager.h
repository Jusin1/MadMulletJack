#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "Clinet_Define.h"
#include "CGameObject.h"
class CPickingManager :public CBase
{
    DECLARE_SINGLETON(CPickingManager)
private:
    explicit CPickingManager();
    virtual ~CPickingManager();
    

public:
    HRESULT Ready_Picking();
    void Clear_Picking();

public:
    void Add_PickingGroup(CGameObject* pGameObject);
    void Remove_PickingGroup(CGameObject* pGameObject);
    void Picking_Mouse_UI(_bool bPick) { m_bMouseInUI = bPick; }


public:
    _vec3 Get_PickingPos() { return m_vPickingPos; }

public:
    _bool Picking();
    void Release_PickingObject();

private:
    list<CGameObject*>              m_PickingList;
    _vec3                           m_vPickingPos;
    _bool                           m_bMouseInUI = false;

private:
    CGameObject* m_pPickingObject = nullptr;

public:
    virtual void Free() override;
};

