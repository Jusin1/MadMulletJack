#pragma once
#include "CTileBase.h"

namespace Engine
{
	class CColider_Sphere;
}

class CTile_Deco;

class CTile_NormalDoor : public CTileBase
{
private:
    explicit CTile_NormalDoor(LPDIRECT3DDEVICE9 pGraphicDevice);
    explicit CTile_NormalDoor(const CTile_NormalDoor& rhs);
    virtual ~CTile_NormalDoor();

    virtual void Free() override;
    virtual CGameObject* Clone(void* pArg = nullptr) override;

public:
    static CTile_NormalDoor* Create(LPDIRECT3DDEVICE9 pGraphicDevice);
    virtual HRESULT Ready_GameObject() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual _int    Update_GameObject(const _float& fTimeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void    Render_GameObject() override;

    Engine::CColider_Sphere* GetCollider() { return m_pColiderSphere; }
    bool IsOpened() const { return m_bOpend; }

private:
    HRESULT Set_Component(void* pArg);
    void    PivotRotate();

private:
    bool    m_bOpend{ false };
    _matrix m_matInitDoors[2];
    _float  m_fTargetAngle{ 0.f };
    _float  m_fAngle{ 0.f };

    Engine::CColider_Sphere* m_pColiderSphere = nullptr; // ´ÝÈû¿ë
    Engine::CColider_Sphere* m_pColiderSphereOpen = nullptr; // ¿­¸²¿ë (Ãß°¡)

    vector<CTile_Deco*> m_pDoors;
};

