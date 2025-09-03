#include "pch.h"
#include "CItem.h"
#include "CTexture.h"
#include "CRenderer.h"
#include "CColiderManager.h"
#include "CColider_Sphere.h"
#include "CObjectPoolManager.h"
#include "Client_Global.h"
#include "CCullingManager.h"
#include "CEffect_World.h"
#include "CVIBuffer_Rect.h"
#include "CManagement.h"

CItem::CItem(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
    , m_pColiderCom(nullptr)
    , m_pTextureCom(nullptr)
    , m_pBufferCom(nullptr)
    , m_fLifeTime(0.f)
    , m_fLifeLimit(1.f)
    , m_tItemInfo({ WP_DOPING, {0.f,0.f,0.f} })
{
}

CItem::CItem(const CItem& rhs)
    : CGameObject(rhs)
    , m_pColiderCom(rhs.m_pColiderCom)
    , m_pTextureCom(rhs.m_pTextureCom)
    , m_pBufferCom(rhs.m_pBufferCom)
    , m_fLifeTime(rhs.m_fLifeTime)
    , m_fLifeLimit(rhs.m_fLifeLimit)
    , m_tItemInfo(rhs.m_tItemInfo)

{
}

CItem::~CItem() {}

HRESULT CItem::Ready_GameObject()
{
    if (FAILED(__super::Ready_GameObject()))
        return E_FAIL;
    return S_OK;
}

HRESULT CItem::Initialize(void* pArg, WEAPON2 _eItemType)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Set_Component()))
        return E_FAIL;

    CTransform::TRANSFORMINFO TransformInfo{};
    TransformInfo.fSpeed = 10.f;
    TransformInfo.fRotationSpeed = 0.f;
    m_pTransformCom->SetTransformInfo(TransformInfo);

    m_fLifeTime = 5.f;

    // item type 에 따른 설정

    
    if (FAILED(Texture_Clone()))
        return S_OK;

    return S_OK;
}

_int CItem::Update_GameObject(const _float& fTimeDelta)
{
    if (m_bDead) return DEAD;

    // 생명주기
    if (m_fLifeLimit <= m_fLifeTime)
    {
        m_bDead = true;
        return DEAD;
    }
    else
        m_fLifeTime += fTimeDelta;

    // 위치 update
    
    __super::Update_GameObject(fTimeDelta);

    CColiderManager::GetInstance()->Add_CollisionGroup(
        CColiderManager::COLLISIOIN_ITEM, this);

    m_pRendererCom->Add_RenderGroup(RENDER_NONALPHA, this);

    return NO_EVENT;
}

void CItem::LateUpdate_GameObject(const _float& fTimeDelta)
{
    __super::LateUpdate_GameObject(fTimeDelta);
    
    Update_Position(m_pTransformCom->Get_Info(INFO_POS));

    // 충돌
    Set_Collider(fTimeDelta);
}

void CItem::Render_GameObject()
{
    __super::Render_GameObject();
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTransformCom->Apply_WorldMatrix();

    m_pTextureCom->Set_Texture(0);

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CItem::Set_Component()
{
    if (FAILED(Add_Components(L"Com_Buffer", SCENE_STATIC,
        L"Proto_Rect_Buffer", (CComponent**)&m_pBufferCom)))
        return E_FAIL;

    CColider_Sphere::COLLINFO CollSphereInfo{};
    CollSphereInfo.fRadius = 0.2f;
    CollSphereInfo.vOffset = _vec3(0.f, 0.f, 0.f);

    if (FAILED(Add_Components(L"Com_Collider_Sphere", SCENE_STATIC,
        L"Proto_Colider_Sphere", (CComponent**)&m_pColiderCom, &CollSphereInfo)))
        return E_FAIL;
    m_pColiderCom->Set_Transform(m_pTransformCom);

    return S_OK;
}

void    CItem::Set_Collider(const _float& fTimeDelta)
{
    m_pColiderCom->Update_ColliderSphere();

    return;
}

HRESULT CItem::Texture_Clone()
{
    CTexture::TEXINFO texInfo{};
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 0;
    texInfo.m_fSpeed = 0.f;
    texInfo.m_bLoop = true;

    auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();

    switch (m_tItemInfo.eWeapon)
    {
    case WP_KNIFE:
        if (FAILED(Add_Components(L"Com_Texture_Item", SCENE_STATIC,
            L"Prototype_Component_Texture_SubWKnife_Item",
            (CComponent**)&m_pTextureCom, &texInfo)))
            return E_FAIL;
        break;

    case WP_DOPING:
        if (FAILED(Add_Components(L"Com_Texture_Item", SCENE_STATIC,
            L"Prototype_Component_Texture_ItemDoping",
            (CComponent**)&m_pTextureCom, &texInfo)))
            return E_FAIL;
        break;
    }
   

    return S_OK;
}

CItem* CItem::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CItem* p = new CItem(pGraphicDev);
    if (FAILED(p->Ready_GameObject()))
    {
        MSG_BOX("CItem Create Failed");
        Safe_Release(p);
    }
    return p;
}

CGameObject* CItem::Clone(void* pArg)
{
    CItem* pInstance = new CItem(*this);

    if (FAILED(pInstance->Initialize(pArg, _eItemType)))
    {
        MSG_BOX("pBullet Clone Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CItem::Free()
{
    Engine::CGameObject::Free();
}