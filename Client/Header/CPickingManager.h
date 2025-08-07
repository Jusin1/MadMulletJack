#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "Clinet_Define.h"
#include "CGameObject.h"

// 마우스 픽킹을 관리하는 클래스
class CPickingManager :public CBase
{
    DECLARE_SINGLETON(CPickingManager)
private:
    explicit CPickingManager();
    virtual ~CPickingManager();
    

public:
    HRESULT Ready_Picking(); // 초기화
    void Clear_Picking(); // 모든 픽킹 그룹 삭제

public:
    void Add_PickingGroup(CGameObject* pGameObject); // 픽킹 그룹 추가
    void Remove_PickingGroup(CGameObject* pGameObject); // 제거
    void Picking_Mouse_UI(_bool bPick) { m_bMouseInUI = bPick; } // 마우스가 UI 위에있는지 여부 설정


public:
    _vec3 Get_PickingPos() { return m_vPickingPos; } // 마지막으로 픽킹된 위치 반환

public:
    _bool Picking(); // 마우스 클릭 시 픽킹 수행
    void Release_PickingObject(); // 현재 픽킹 객체 해제

private:
    list<CGameObject*>              m_PickingList; //
    _vec3                           m_vPickingPos;
    _bool                           m_bMouseInUI = false;

private:
    CGameObject* m_pPickingObject = nullptr;

public:
    virtual void Free() override;
};

