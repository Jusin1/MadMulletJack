#include "pch.h"
#include "Engine_Define.h"
#include "CColiderManager.h"
#include "Clinet_Define.h"
#include "Client_Global.h"
#include "CVIBuffer_Rect.h"
#include "CColider_Sphere.h"
#include "CTexture.h"
#include "CTile_Electric.h"

CTile_Electric::CTile_Electric(LPDIRECT3DDEVICE9 pGraphicDevice)
    : CTileBase(pGraphicDevice, TileType::ELECTRIC), m_pColliderSphere(nullptr)
{
}

CTile_Electric::CTile_Electric(const CTile_Electric &rhs)
    : CTileBase(rhs, TileType::ELECTRIC), m_pColliderSphere(nullptr)
{
}

CTile_Electric::~CTile_Electric()
{
}

void CTile_Electric::Free()
{
    CTileBase::Free();
}

CGameObject *CTile_Electric::Clone(void *pArg)
{
    CTile_Electric *pClone = new CTile_Electric(*this);

    if (FAILED(pClone->Initialize(pArg)))
    {
        MSG_BOX("CTile_Electric::Clone, Failed");
        Safe_Release(pClone);
    }

    return pClone;
}

CTile_Electric *CTile_Electric::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
    CTile_Electric *pProto = new CTile_Electric(pGraphicDevice);

    if (FAILED(pProto->Ready_GameObject()))
    {
        Safe_Release(pProto);
        MSG_BOX("CTile_Electric::Create, Failed");
        return nullptr;
    }

    return pProto;
}

HRESULT CTile_Electric::Ready_GameObject()
{
    return CTileBase::Ready_GameObject();
}

HRESULT CTile_Electric::Initialize(void *pArg)
{
    if (FAILED(CTileBase::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Set_Component(pArg)))
        return E_FAIL;

    return S_OK;
}

_int CTile_Electric::Update_GameObject(const _float &fTimeDelta)
{
    if (m_bDead) return DEAD;
    
    CColiderManager::GetInstance()->Add_CollisionGroup(CColiderManager::COLLISION_TILE_ELECTRIC, this);
    return __super::Update_GameObject(fTimeDelta);
}

void CTile_Electric::LateUpdate_GameObject(const _float &fTimeDelta)
{
    if (m_bDead) return;

    if (!m_bActivated)
    {
        // 테스트용 추후에 몬스터로
        if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_PLAYER, this, CColiderManager::COLLISION_SPHERE, nullptr))
        {
             m_bActivated = true;
        }
    }

    __super::LateUpdate_GameObject(fTimeDelta);
}

void CTile_Electric::Render_GameObject()
{
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTransformCom->Apply_WorldMatrix();
    m_pTexture->Set_Texture(m_bActivated);

    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_pBuffer->Render_Buffer();

    // 원상복귀
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

#ifdef _DEBUG
    if (g_ColiderRender && m_pColliderSphere != nullptr)
    {
        m_pColliderSphere->Render_ColliderSphere();
    }
#endif
}

HRESULT CTile_Electric::Set_Component(void *pArg)
{
    if (FAILED(Add_Components(L"Com_Texture", SCENE_STATIC, L"Proto_Electric_Wall", (CComponent **)&m_pTexture)))
        return E_FAIL;

    CColider_Sphere::COLLINFO CollSphereInfo;
    ZeroMemory(&CollSphereInfo, sizeof(CColider_Sphere::COLLINFO));
    CollSphereInfo.fRadius = 0.5f;
    CollSphereInfo.vOffset = _vec3(0.f, 0.f, 0.f);    // 중심 오프셋 없음

    // Colider_Sphere
    if (FAILED(Add_Components(L"Com_Collider_Sphere", SCENE_STATIC, L"Proto_Colider_Sphere", (CComponent **)&m_pColliderSphere, &CollSphereInfo)))
        return E_FAIL;

    m_pColliderSphere->Set_Transform(m_pTransformCom);
    m_pColliderSphere->Update_ColliderSphere();

    return S_OK;
}
