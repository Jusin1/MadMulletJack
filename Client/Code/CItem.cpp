#include "pch.h"
#include "CItem.h"
#include "CTexture.h"
#include "CRenderer.h"
#include "CColiderManager.h"
#include "CColider_Sphere.h"
#include "CObjectPoolManager.h"
#include "CObjectManager.h"
#include "CVIBuffer_Rect.h"
#include "CManagement.h"

CItem::CItem(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
    , m_pColiderCom(nullptr)
    , m_pTextureCom(nullptr)
    , m_pBufferCom(nullptr)
    , m_fLifeTime(0.f)
    , m_fLifeLimit(1.f)
    , m_tItemInfo({ })
    , m_bMove(true)
    , m_fGravity(0.f)
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
    , m_bMove (rhs.m_bMove)
    , m_fGravity(rhs.m_fGravity)

{
}

CItem::~CItem() {}

HRESULT CItem::Ready_GameObject()
{
    if (FAILED(__super::Ready_GameObject()))
        return E_FAIL;
    return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Set_Component()))
        return E_FAIL;

    CTransform::TRANSFORMINFO TransformInfo{};
    TransformInfo.fSpeed = 10.f;
    TransformInfo.fRotationSpeed = 0.f;
    m_pTransformCom->SetTransformInfo(TransformInfo);

    m_fLifeTime = 0.f;
    m_fLifeLimit = 10.f;
    m_fGravity = 5.f;

    if (pArg != nullptr)
    {
        memcpy(&m_tItemInfo, pArg, sizeof(ITEMINFO));

        // 초기 위치 셋팅
        GetTransform()->Set_Info(INFO_POS, m_tItemInfo.vStartPos);
    }

    // item type 에 따른 설정
    if (FAILED(Texture_Clone()))
        return E_FAIL;

    GetTransform()->Set_Scale(0.2f, 0.2f, 0.2f);

    Engine::CTransform* pPlayerTransformCom =
        dynamic_cast<CTransform*>(CObjectManager::GetInstance()->
            Get_Component(CManagement::GetInstance()->Get_CurrentSceneIdx(), L"Player_Layer", L"Com_Transform", 0));
    if (pPlayerTransformCom == nullptr)
        return E_FAIL;

    m_fMinY = pPlayerTransformCom->Get_Info(INFO_POS).y - 1.f;

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
    if(m_bMove)
    {
        _vec3 vPos = m_pTransformCom->Get_Info(INFO_POS);
        vPos.y = vPos.y + m_fGravity * fTimeDelta; // test : 중력 5, 

        if (vPos.y - m_pTransformCom->Get_Scale().y  <= m_fMinY)
        {
            m_bMove = false;
            vPos.y = m_fMinY + m_pTransformCom->Get_Scale().y;
        }

        GetTransform()->Set_Info(INFO_POS, vPos);
        //중력 가속도 적용
        m_fGravity -= 10.f * fTimeDelta * 5.f; /*Mess*/
    }

    else {
        m_fGravity = 10.f; // 중력 원상 복귀
    }


    
    __super::Update_GameObject(fTimeDelta);

    CColiderManager::GetInstance()->Add_CollisionGroup(
        CColiderManager::COLLISIOIN_ITEM, this);

    m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this);

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

    if (FAILED(pInstance->Initialize(pArg)))
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