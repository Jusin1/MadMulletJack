#pragma once
#include "CBase.h"
#include "CGameObject.h"

BEGIN(Engine)

// 충돌 관리 매니저 클래스
// 씬 내의 모든 오브젝트들을 충돌 그룹으로 관리하고 충돌 검사 수행
class ENGINE_DLL CColiderManager :
    public CBase
{
    DECLARE_SINGLETON(CColiderManager)

public:
    enum COLLSION_GROUP {COLLISION_PLAYER, COLLISION_MONSTER, COLLISION_END};
    enum COLLISION_TYPE {COLLISION_RECT};

private:
    explicit CColiderManager();
    virtual ~CColiderManager();

public:
    // 충돌 그룹에 오브젝트 추가
    HRESULT Add_CollisionGroup(COLLSION_GROUP _collisionGroup, CGameObject* pGameObject);
    // 충돌 그룹에서 오브젝트 제거
    void Remove_CollisionGroup(COLLSION_GROUP _collisionGroup, CGameObject* pGameObject);

    // 모든 충돌 그룹 클리어
    HRESULT Clear_Colider_Group();

    // 특정 오브젝트가 그룹 내 오브젝트와 충돌하는지 검사
    _bool CollisionGroup(COLLSION_GROUP _collisionGroup, class CGameObject* pGameObject, COLLISION_TYPE _collisionType, _vec3* pOutDistance);

private:
    // 각 그룹 별 오브젝트 리스트
    list<CGameObject*> m_GameObjects[COLLISION_END];
public:
    // 메모리 해제
    virtual void Free() override;

};
END

