#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
class ENGINE_DLL CCullingManager :
    public CBase
{
    DECLARE_SINGLETON(CCullingManager)

private:
    explicit CCullingManager();
    virtual ~CCullingManager() = default;

public:
    HRESULT Ready_Culling(LPDIRECT3DDEVICE9 pGrahpicDev);
    HRESULT Update_Culling();
    _bool Is_In_Frustum(_vec3 pGameObjectPos, _float fRadius);

private:
    LPDIRECT3DDEVICE9 m_pGrahpicDev;
    _vec3 m_WorldPoint[8];
    D3DXPLANE m_FrustumPlane[6];

public:
    virtual void Free() override;
};
END

