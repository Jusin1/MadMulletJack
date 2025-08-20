#include "pch.h"
#include "Clinet_Define.h"
#include "Client_Global.h"
#include "CColiderManager.h"
#include "CVIBuffer_Rect.h"
#include "CColider_Sphere.h"
#include "CTile_Deco.h"
#include "CMapFactory.h"
#include "CObjectManager.h"
#include "CTexture.h"
#include "CTile_Vent.h"
#include "Engine_Define.h"

CTile_Vent::CTile_Vent(LPDIRECT3DDEVICE9 pGraphicDevice)
    : CTileBase(pGraphicDevice, TileType::VENT), m_pProp(nullptr), m_pColliderSphere(nullptr)
{
}

CTile_Vent::CTile_Vent(const CTile_Vent &rhs)
    : CTileBase(rhs, TileType::VENT), m_pProp(nullptr), m_pColliderSphere(nullptr)
{
}

CTile_Vent::~CTile_Vent()
{
}

void CTile_Vent::Free()
{
    CTileBase::Free();
}

CGameObject *CTile_Vent::Clone(void *pArg)
{
    CTile_Vent *pClone = new CTile_Vent(*this);

    if (FAILED(pClone->Initialize(pArg)))
    {
        MSG_BOX("CTile_Vent::Clone, Failed");
        Safe_Release(pClone);
    }

    return pClone;
}

CTile_Vent *CTile_Vent::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
    CTile_Vent *pProto = new CTile_Vent(pGraphicDevice);

    if (FAILED(pProto->Ready_GameObject()))
    {
        Safe_Release(pProto);
        MSG_BOX("TCTile_Vent::Create, Failed");
        return nullptr;
    }

    return pProto;
}

HRESULT CTile_Vent::Ready_GameObject()
{
    return CTileBase::Ready_GameObject();
}

HRESULT CTile_Vent::Initialize(void *pArg)
{
    if (FAILED(CTileBase::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Set_Component(pArg)))
        return E_FAIL;

    return S_OK;
}

_int CTile_Vent::Update_GameObject(const _float &fTimeDelta)
{
    if (m_bDead) return DEAD;

    RotateProp(fTimeDelta);

    CColiderManager::GetInstance()->Add_CollisionGroup(CColiderManager::COLLISION_TILE_VENT, this);
    return __super::Update_GameObject(fTimeDelta);
}

void CTile_Vent::LateUpdate_GameObject(const _float &fTimeDelta)
{
    if (m_bDead) return;

    // 테스트용 추후에 몬스터로
    if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_PLAYER, this, CColiderManager::COLLISION_SPHERE, nullptr))
    {
        if (!m_bKilled)
        {
            m_bKilled = true;
            m_pProp->SetTextureIndex(m_bKilled);
        }
    }

    __super::LateUpdate_GameObject(fTimeDelta);
}

void CTile_Vent::Render_GameObject()
{
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTransformCom->Apply_WorldMatrix();
    m_pTexture->Set_Texture(m_bKilled);

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

HRESULT CTile_Vent::Set_Component(void *pArg)
{
    if (FAILED(Add_Components(L"Com_Texture", SCENE_STATIC, L"Proto_Vent", (CComponent **)&m_pTexture)))
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

    if (MAPOBJECTDATA *pData = reinterpret_cast<MAPOBJECTDATA *>(pArg))
    {
        wstring originName = pData->texture.OriginComponentName;
        pData->texture.OriginComponentName = originName + L"_Prop";
        _matrix vTransformData;
        ::D3DXMatrixIdentity(&vTransformData);
        vTransformData._43 -= 0.01f;
        vTransformData *= (*m_pTransformCom->Get_World());
        ::memcpy(&pData->transform.Right[0], vTransformData.m[0], sizeof(_vec3));
        ::memcpy(&pData->transform.Up[0], vTransformData.m[1], sizeof(_vec3));
        ::memcpy(&pData->transform.Look[0], vTransformData.m[2], sizeof(_vec3));
        ::memcpy(&pData->transform.Pos[0], vTransformData.m[3], sizeof(_vec3));
        if (FAILED(CObjectManager::GetInstance()->Add_GameObject(
            L"Prototype_GameObject_DecoTile",
            CMapFactory::GetInstance()->GetTargetSceneIndex(),
            L"Tile_Layer",
            pData)))
            return E_FAIL;
        m_pProp = static_cast<CTile_Deco*>(CObjectManager::GetInstance()->Get_ObjectList(CMapFactory::GetInstance()->GetTargetSceneIndex(), L"Tile_Layer")->back());
    }
    

    // TODO - Monster와 충돌시 Trigger Component

    return S_OK;
}

void CTile_Vent::RotateProp(const _float &fTimeDelta)
{
    m_pProp->GetTransform()->RotationDegree(_vec3{ 0.f,0.f,1.f }, 180.f * fTimeDelta);
}
