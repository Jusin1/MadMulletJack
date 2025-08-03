#pragma once
#include "CBase.h"
#include "CGameObject.h"

BEGIN(Engine)
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
    HRESULT Add_CollisionGroup(COLLSION_GROUP _collisionGroup, CGameObject* pGameObject);
    void Remove_CollisionGroup(COLLSION_GROUP _collisionGroup, CGameObject* pGameObject);

    HRESULT Clear_Colider_Group();

    _bool CollisionGroup(COLLSION_GROUP _collisionGroup, class CGameObject* pGameObject, COLLISION_TYPE _collisionType, _vec3* pOutDistance);

private:
    list<CGameObject*> m_GameObjects[COLLISION_END];
public:
    virtual void Free() override;

};
END

