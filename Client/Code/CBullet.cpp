#include "pch.h"
#include "CBullet.h"
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

CBullet::CBullet(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
    , m_pColiderCom(nullptr)
    , m_pTextureCom(nullptr)
    , m_pBufferCom(nullptr)
    , m_fSpeed(21.f)
    , m_vMoveDir(0.f, 0.f, 1.f)
    , m_fLifeTime(0.f)
    , m_fLifeLimit(1.f)   
{
}

CBullet::CBullet(const CBullet& rhs)
    : CGameObject(rhs)
    , m_pColiderCom(rhs.m_pColiderCom)  
    , m_pTextureCom(rhs.m_pTextureCom)
    , m_pBufferCom(rhs.m_pBufferCom)
    , m_fSpeed(rhs.m_fSpeed)
    , m_vMoveDir(rhs.m_vMoveDir)
    , m_fLifeTime(rhs.m_fLifeTime)
    , m_fLifeLimit(rhs.m_fLifeLimit)
{
}

CBullet::~CBullet() {}

HRESULT CBullet::Ready_GameObject()
{
    if (FAILED(__super::Ready_GameObject()))
        return E_FAIL;
    return S_OK;
}

HRESULT CBullet::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Set_Component()))
        return E_FAIL;

    CTransform::TRANSFORMINFO TransformInfo{};
    TransformInfo.fSpeed = m_fSpeed;
    TransformInfo.fRotationSpeed = 0.f;
    m_pTransformCom->SetTransformInfo(TransformInfo);

    m_pTransformCom->Set_Scale(0.06f, 0.06f, 1.f);

    return S_OK;
}

_int CBullet::Update_GameObject(const _float& fTimeDelta)
{
    if (m_bDead) return DEAD;

    __super::Update_GameObject(fTimeDelta);

    _vec3 vPos = m_pTransformCom->Get_Info(INFO_POS);
    vPos += m_vMoveDir * m_fSpeed * fTimeDelta;
    m_pTransformCom->Set_Info(INFO_POS, vPos);

    m_fLifeTime += fTimeDelta;
    if (m_fLifeTime >= m_fLifeLimit) {
        m_bDead = true;  
        return DEAD;
    }

    CColiderManager::GetInstance()->Add_CollisionGroup(
        CColiderManager::COLLISION_BULLET, this);

    m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this);

    return NO_EVENT;
}

void CBullet::LateUpdate_GameObject(const _float& fTimeDelta)
{
    __super::LateUpdate_GameObject(fTimeDelta);
    m_pColiderCom->Update_ColliderSphere();
    Update_Position(m_pTransformCom->Get_Info(INFO_POS));
    Compute_CamDistance(Get_Position());

    SetUp_BillBoard();

    // 컬링
    if (CCullingManager::GetInstance()->Is_In_Frustum(Get_Position(), m_fRadius))
    {
        if (m_pRendererCom)
            m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this);
    }

    // 충돌
    Set_Collider(fTimeDelta);
}

void CBullet::Render_GameObject()
{
    __super::Render_GameObject();
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTransformCom->Apply_WorldMatrix();

    m_pTextureCom->Set_Texture(m_pTextureCom->Get_Frame().m_iCurrentTex);
    m_pTextureCom->MoveFrame();

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CBullet::Spawn_Pooling(void *pArg)
{
    if (FAILED(CGameObject::Spawn_Pooling()))
        return E_FAIL;

    if (m_pTextureCom)
        m_pTextureCom->Set_Zero_Frame();

    if (BulletData *pData = reinterpret_cast<BulletData *>(pArg))
    {
        CTransform::TRANSFORMINFO TransformInfo{};
        TransformInfo.fSpeed = pData->fSpeed;
        TransformInfo.fRotationSpeed = 0.f;
        m_pTransformCom->SetTransformInfo(TransformInfo);
        Fire(pData->vMuzzlePosition, pData->vLookDir);
    }
    else
    {
        MSG_BOX("CBullet::Spawn_Pooling, no data");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CBullet::Despawn_Pooling()
{
    if (FAILED(CGameObject::Despawn_Pooling()))
        return E_FAIL;

    m_fLifeTime = 0.00f;

    return S_OK;
}

HRESULT CBullet::Set_Component()
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

    // Texture
    if (FAILED(Texture_Clone()))
        return E_FAIL;

    return S_OK;
}

void CBullet::Set_Collider_With_Wall()
{
    _vec3 vDistance;
    if (CColiderManager::GetInstance()->CollisionGroup(
        CColiderManager::COLLISION_HORWALL, this,
        CColiderManager::COLLISION_SPHERE_CUBE, &vDistance))
    {
        _vec3 vPos = m_pTransformCom->Get_Info(INFO_POS) += vDistance;
        Spawn_Destroy_Effect(vPos);
        Set_Dead(TRUE);        
    }
    if (CColiderManager::GetInstance()->CollisionGroup(
        CColiderManager::COLLISION_VERWALL, this,
        CColiderManager::COLLISION_SPHERE_CUBE, &vDistance))
    {
        _vec3 vPos = m_pTransformCom->Get_Info(INFO_POS) += vDistance;
        Spawn_Destroy_Effect(vPos);
        Set_Dead(TRUE);
    }
}

void CBullet::SetUp_BillBoard()
{
    _matrix matView;
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixInverse(&matView, nullptr, &matView);

    _vec3 vRight = *(_vec3*)&matView.m[0][0];
    _vec3 vUp = *(_vec3*)&matView.m[1][0];

    D3DXVec3Normalize(&vRight, &vRight);
    D3DXVec3Normalize(&vUp, &vUp);

    // 이동 방향 유지, Quad만 카메라 정면
    m_pTransformCom->Set_Info(INFO_RIGHT, vRight * m_pTransformCom->Get_Scale().x);
    m_pTransformCom->Set_Info(INFO_UP, vUp * m_pTransformCom->Get_Scale().y);
    m_pTransformCom->Set_Info(INFO_LOOK, m_vMoveDir * m_pTransformCom->Get_Scale().z);
}

void CBullet::Spawn_Destroy_Effect(const _vec3 &vPos)
{
    EffectOptions tOption{ Get_Preset_BulletSpark() };
    tOption.fLife_Min = 0.3f;
    tOption.fLife_Max = 0.5f;
    tOption.fSize_Min = 2.f;
    tOption.fSize_Max = 3.3f;
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_PIXEL, &tOption,
        [&vPos](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO_POS, vPos);
        });
    EFFECTINFO tInfo;
    tInfo.eType = WorldEffectType::EXPLOSION;
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_WORLD, &tInfo,
        [&vPos](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPos + _vec3{ 0.f, 0.1f, 0.f });
        });
}

void    CBullet::Set_Collider(const _float& fTimeDelta)
{
    m_pColiderCom->Update_ColliderSphere();

    Set_Collider_With_Wall();

    return;
}

HRESULT CBullet::Texture_Clone()
{
    CTexture::TEXINFO texInfo{};
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 4;
    texInfo.m_fSpeed = 6.f;
    texInfo.m_bLoop = true;

    auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
    if (FAILED(Add_Components(L"Com_Texture_Default", SCENE_STATIC,
        L"Prototype_Component_Texture_Bullet",
        (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBullet::Change_Texture(const _tchar* LayerTag)
{
    if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    if (m_pTextureCom)
        m_pTextureCom->Set_Zero_Frame();

    return S_OK;
}

void CBullet::Fire(const _vec3& vPos, const _vec3& vDir)
{
    // 방향 세팅
    m_vMoveDir = vDir;
    D3DXVec3Normalize(&m_vMoveDir, &m_vMoveDir);

    // 위치 세팅
    m_pTransformCom->Set_Info(INFO_POS, vPos);

    // 축 보정
    _vec3 vWorldUp(0.f, 1.f, 0.f);
    _vec3 vRight, vUp;

    D3DXVec3Cross(&vRight, &vWorldUp, &m_vMoveDir);
    D3DXVec3Normalize(&vRight, &vRight);

    D3DXVec3Cross(&vUp, &m_vMoveDir, &vRight);
    D3DXVec3Normalize(&vUp, &vUp);

    // Transform에 적용
    _vec3 vScale = m_pTransformCom->Get_Scale();
    m_pTransformCom->Set_Info(INFO_RIGHT, vRight * vScale.x);
    m_pTransformCom->Set_Info(INFO_UP, vUp * vScale.y);
    m_pTransformCom->Set_Info(INFO_LOOK, m_vMoveDir * vScale.z);
}

CBullet* CBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CBullet* p = new CBullet(pGraphicDev);
    if (FAILED(p->Ready_GameObject()))
    {
        MSG_BOX("CBullet Create Failed");
        Safe_Release(p);
    }
    return p;
}

CGameObject* CBullet::Clone(void* pArg)
{
    CBullet* pInstance = new CBullet(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("pBullet Clone Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CBullet::Free()
{
    Engine::CGameObject::Free();
}