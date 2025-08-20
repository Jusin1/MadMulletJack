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
    enum COLLISION_GROUP {COLLISION_PLAYER, COLLISION_MONSTER, COLLISION_BLOCK, COLLISION_DUMMY, COLLISION_END};
    enum COLLISION_TYPE {COLLISION_RECT, COLLISION_CUBE, COLLISION_SPHERE, COLLISION_SPHERE_CUBE, COLLISION_CUBE_SPHERE};

private:
    explicit CColiderManager();
    virtual ~CColiderManager();

public:
    // 충돌 그룹에 오브젝트 추가
    HRESULT Add_CollisionGroup(COLLISION_GROUP _collisionGroup, CGameObject* pGameObject);
    // 충돌 그룹에서 오브젝트 제거
    void Remove_CollisionGroup(COLLISION_GROUP _collisionGroup, CGameObject* pGameObject);

    // 모든 충돌 그룹 클리어
    HRESULT Clear_Colider_Group();

    // 특정 오브젝트가 그룹 내 오브젝트와 충돌하는지 검사
    _bool CollisionGroup(COLLISION_GROUP _collisionGroup, class CGameObject* pGameObject, COLLISION_TYPE _collisionType, _vec3* pOutDistance);

    _bool Collision_Check_Group_Multi(COLLISION_GROUP eGroup, vector<class CGameObject*>& vecDamagedObj, class CGameObject* pDamageCauser, COLLISION_TYPE eCollisionType);
private:
    // 각 그룹 별 오브젝트 리스트
    list<CGameObject*> m_GameObjects[COLLISION_END];
public:
    // 메모리 해제
    virtual void Free() override;
};
END

