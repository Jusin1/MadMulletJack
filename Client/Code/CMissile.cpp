#include "pch.h"
#include "Client_Global.h"
#include "Engine_Define.h"
#include "CEffect_World.h"
#include "CRenderer.h"
#include "CObjectManager.h"
#include "CColiderManager.h"
#include "CColider_Sphere.h"
#include "CVIBuffer_Circle.h"
#include "CComponentMgr.h"
#include "CObjectPoolManager.h"
#include "CMapFactory.h"
#include "CDataManager.h"
#include "CManagement.h"
#include "CPrefab.h"
#include "CMissile.h"

CMissile::CMissile(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CMissile::CMissile(const CMissile &rhs)
    : CGameObject(rhs)
{
}

CMissile::~CMissile()
{
}

void CMissile::Free()
{
    Safe_Release(m_pModel);
    CGameObject::Free();
}

CMissile *CMissile::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMissile *pInstance = new CMissile(pGraphicDev);
    if (FAILED(pInstance->Ready_GameObject()))
    {
        MSG_BOX("CMissile Create Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject *CMissile::Clone(void *pArg)
{
    CMissile *pInstance = new CMissile(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CMissile Clone Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

HRESULT CMissile::Ready_GameObject()
{
    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMissile::Initialize(void *pArg)
{
    if (FAILED(CGameObject::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Set_Component()))
        return E_FAIL;

    if (FAILED(Set_MissileModel()))
        return E_FAIL;

    return S_OK;
}

_int CMissile::Update_GameObject(const _float &fTimeDelta)
{
    if (m_bDead)
        return DEAD;

    CGameObject::Update_GameObject(fTimeDelta);

    UpdateState(fTimeDelta);

    // Logic
    m_fSmokeEffect_Duration += fTimeDelta;
    if (m_fSmokeEffect_Cool <= m_fSmokeEffect_Duration)
    {
        Spawn_Smoke_Effect();
        m_fSmokeEffect_Duration -= m_fSmokeEffect_Cool;
    }

    //
    // m_pModel의 Transform과
    // 현재 Msille의 Transform 을 함께 갱신해주어야함!
    // 내 월드를 memcpy로 복사시켜주자
    //

    CColiderManager::GetInstance()->Add_CollisionGroup(
        CColiderManager::COLLISION_MISSILE, this);
    Set_Buffer_Color(fTimeDelta);
    m_pRendererCom->Add_RenderGroup(RENDER_NONALPHA, this);

    return NO_EVENT;
}

void CMissile::LateUpdate_GameObject(const _float &fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
    m_pColiderCom->Update_ColliderSphere();
    Update_Position(m_pTransformCom->Get_Info(INFO_POS));
    
    // Set_Collider_With_Wall();
    Set_ModelTransform();
}

void CMissile::Render_GameObject()
{
    CGameObject::Render_GameObject();
    m_pTransformCom->Apply_WorldMatrix();
    m_pBufferCom->Render_Buffer();
    // Todo - Launch Effect
}

HRESULT CMissile::Spawn_Pooling(void *pArg)
{
    if (FAILED(CGameObject::Spawn_Pooling()))
        return E_FAIL;

    // Fire
    if (MissileData *pData = reinterpret_cast<MissileData*>(pArg))
    {
        Fire(pData->vLaunchPos, pData->vTargetPos, pData->vDir);
    }
    else
    {
        MSG_BOX("CMissile::Spawn_Pooling, No data");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CMissile::Despawn_Pooling()
{
    if (FAILED(CGameObject::Despawn_Pooling()))
        return E_FAIL;

    m_fSmokeEffect_Duration = 0.00f;
    m_vTargetPos = _vec3{ 0.f,0.f,0.f };
    m_vLaunchPos = _vec3{ 0.f,0.f,0.f };
    m_vMoveDir = _vec3{ 0.f,0.f,1.f };
    m_vFinalTargetPos = _vec3{ 0.f,0.f,0.f };
    m_pTransformCom->Set_Info(INFO::INFO_RIGHT, _vec3{ 1.f,0.f,0.f });
    m_pTransformCom->Set_Info(INFO::INFO_UP, _vec3{ 0.f,1.f,0.f });
    m_pTransformCom->Set_Info(INFO::INFO_LOOK, _vec3{ 0.f,0.f,1.f });
    m_pTransformCom->Set_Info(INFO::INFO_POS, _vec3{ 0.f,0.f,0.f });
    Set_ModelTransform();

    return S_OK;
}

void CMissile::Fire(const _vec3 &vLaunchPos, const _vec3 &vTargetPos, const _vec3 &_vDir)
{
    _vec3 vDir = _vDir;
    if (::D3DXVec3LengthSq(&vDir) < g_Epsilon)
    {
        // 0 ~ 360도
        _float fTheta = Rand_Float(0.f, 2.f * D3DX_PI);
        // -1 ~ 1 cos
        _float fU = Rand_Float(0.f, 1.f);
        _float fS = std::sqrt(1.f - fU * fU);
        vDir = _vec3(fS * std::cos(fTheta), fU, fS * std::sin(fTheta));
    }

    _float fLaunchDist = Rand_Float(3.f, 5.f);

    m_vMoveDir = Normalize_Safe(vDir);
    m_vLaunchPos = vLaunchPos + m_vMoveDir * fLaunchDist;
    m_vTargetPos = vTargetPos;

    ChangeState(State::LAUNCH);
}

void CMissile::ChangeState(State _e)
{
    if (m_eCurrentState == _e)
        return;

    switch (m_eCurrentState)
    {
        case State::LAUNCH:		Launch_Exit();    break;
        case State::TARGET:		Target_Exit();    break;
    }

    m_eCurrentState = _e;

    switch (m_eCurrentState)
    {
        case State::LAUNCH:		Launch_Enter();		break;
        case State::TARGET:		Target_Enter();		break;
    }
}

void CMissile::UpdateState(const float _fDeltaTime)
{
    switch (m_eCurrentState)
    {
        case State::LAUNCH:		Launch_Update(_fDeltaTime);		break;
        case State::TARGET:		Target_Update(_fDeltaTime);		break;
    }
}

void CMissile::Set_Destroy(const _vec3 &vPos)
{
    Spawn_Destroy_Effect(vPos);
    Set_Dead(TRUE);
    m_pWarningCirlce->Set_Dead(TRUE);
    m_pWarningCirlce = nullptr;
}

HRESULT CMissile::Set_Component()
{
    CircleBufferData tData;
    tData.fRadius = 0.2f;
    tData.iSegments = 8;
    if (FAILED(Add_Components(L"Com_Buffer", SCENE_STATIC, L"Proto_Buffer_Circle", (CComponent **)&m_pBufferCom, &tData)))
        return E_FAIL;

    CColider_Sphere::COLLINFO CollSphereInfo{};
    CollSphereInfo.fRadius = 0.5f;
    CollSphereInfo.vOffset = _vec3(0.f, 0.f, 0.f);

    if (FAILED(Add_Components(L"Com_Collider_Sphere", SCENE_STATIC,
        L"Proto_Colider_Sphere", (CComponent **)&m_pColiderCom, &CollSphereInfo)))
        return E_FAIL;

    m_pColiderCom->Set_Transform(m_pTransformCom);

    return S_OK;
}

HRESULT CMissile::Set_MissileModel()
{
    // Prfab 껍데기 들고오기
    CGameObject *pPrototype = CObjectManager::GetInstance()->Find_Prototype(L"Prototype_GameObject_DefaultPrefab");
    if (!pPrototype)
    {
        MSG_BOX("CMissile::Set_MissileModel, Cant find DefaultPrefab");
        return E_FAIL;
    }
    // DataManager에서 Editor로 찍은 Prefab 모델 데이터 들고오기
    Engine::PREFABDATA tData = CDataManager::GetInstance()->GetPrefabData(PrefabType::Missile);
    CGameObject *pGo = pPrototype->Clone(&tData);
    if (!pGo)
    {
        MSG_BOX("CObjectPool::Ready_ObjectPool, Cant Clone DefaultPrefab");
        return E_FAIL;
    }
    if (CPrefab *pPrefab = dynamic_cast<CPrefab *>(pGo))
    {
        m_pModel = pPrefab;
    }
    else
    {
        MSG_BOX("CObjectPool::Ready_ObjectPool, Cant Upcasting Prefab");
        return E_FAIL;
    }

    return S_OK;
}

void CMissile::Set_Collider_With_Wall()
{
    _vec3 vDistance;
    if (CColiderManager::GetInstance()->CollisionGroup(
        CColiderManager::COLLISION_HORWALL, this,
        CColiderManager::COLLISION_SPHERE_CUBE, &vDistance))
    {
        _vec3 vPos = m_pTransformCom->Get_Info(INFO_POS) += vDistance;
        Set_Destroy(vPos);
    }
    if (CColiderManager::GetInstance()->CollisionGroup(
        CColiderManager::COLLISION_VERWALL, this,
        CColiderManager::COLLISION_SPHERE_CUBE, &vDistance))
    {
        _vec3 vPos = m_pTransformCom->Get_Info(INFO_POS) += vDistance;
        Set_Destroy(vPos);
    }
}

void CMissile::Set_Buffer_Color(const _float &fDeltaTime)
{
    m_fChangeColor_Duration += fDeltaTime;
    if (m_fChangeColor_Duration > 60.f)
        m_fChangeColor_Duration -= 60.f;

    _float fOmega = 2.f * D3DX_PI * m_fChangeColor_Frequence;
    _float fSrc = 0.5f * (sinf(m_fChangeColor_Duration * fOmega) + 1.f);
    D3DXCOLOR color_A{ 1.f, 0.f, 0.f, 0.75f };
    D3DXCOLOR color_B{ 1.f, 1.f, 0.2f, 0.75f };
    D3DXCOLOR color_Lerp = color_A + (color_B - color_A) * fSrc;
    m_pBufferCom->Set_Tint(color_Lerp);
}

void CMissile::Spawn_Destroy_Effect(const _vec3 &vPos)
{
    EFFECTINFO tInfo;
    tInfo.eType = WorldEffectType::BIG_EXPLOSION;
    tInfo.fSize = 10.f;
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_WORLD, &tInfo,
        [vPos](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPos + _vec3{0.f, 1.2f, 0.f});
        });
}

void CMissile::Spawn_Smoke_Effect()
{
    EFFECTINFO tInfo;
    tInfo.eType = WorldEffectType::SMOKE;
    tInfo.fSize = 6.f;
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_WORLD, &tInfo,
        [this](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, Get_Position());
        });
}

void CMissile::Spawn_Warning_Effect()
{

}

void CMissile::Launch_Enter()
{
}

void CMissile::Launch_Update(const _float &fDeltaTime)
{
    _vec3 vPosition = m_pTransformCom->Get_Info(INFO::INFO_POS);

    _vec3 vTo = m_vLaunchPos - vPosition;
    _vec3 vDir;
    ::D3DXVec3Normalize(&vDir, &vTo);
    _float fLength = ::D3DXVec3Length(&vTo);

    _float fStep = m_fLaunchSpeed * fDeltaTime;

    Turn_Towards(vDir, 240.f, fDeltaTime);

    _vec3 vLook = m_pTransformCom->Get_Info(INFO::INFO_LOOK);
    ::D3DXVec3Normalize(&vLook, &vLook);
    _float fDot = ::D3DXVec3Dot(&vLook, &vDir);

    _vec3 vForawrd = (fDot >= 0.f) ? vLook : vDir;


    vPosition += vForawrd * fStep;

    // 현재 좌표에서 날아가야할 위치까지의 크기가 속력의 결과보다 작다면 도착판정
    if (fLength <= fStep)
    {
        vPosition = m_vLaunchPos;
        m_pTransformCom->Set_Info(INFO::INFO_POS, vPosition);
        ChangeState(State::TARGET);
        return;
    }

    m_pTransformCom->Set_Info(INFO::INFO_POS, vPosition);
}

void CMissile::Launch_Exit()
{
}

void CMissile::Target_Enter()
{
    _float fSrc = Rand_Float(0.f, 1.f);
    _float fRadius = m_fTargetRadius * std::sqrt(fSrc);
    _float fAngle = Rand_Float(0.f, 2.f * D3DX_PI);

    _vec3 vOffset = _vec3(fRadius * std::cos(fAngle), 0.f, fRadius * std::sin(fAngle));

    // 최종 타겟: 플레이어 XZ + 현재 미사일 Y 유지(수직 이동 억제)
    _vec3 vCurrentPosition = m_pTransformCom->Get_Info(INFO_POS);
    m_vFinalTargetPos = _vec3(m_vTargetPos.x + vOffset.x, m_vTargetPos.y - 0.7f, m_vTargetPos.z + vOffset.z);

    // 이동 방향 갱신
    m_vMoveDir = Normalize_Safe(m_vFinalTargetPos - vCurrentPosition, m_vMoveDir);

    // Todo
    // Spawn_Warning_Effect();
    CObjectPoolManager::GetInstance()->Spawn(PoolType::WARNING_CIRCLE, nullptr,
        [this](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, m_vFinalTargetPos);
            m_pWarningCirlce = pGo;
        });
}

void CMissile::Target_Update(const _float &fDeltaTime)
{
    _vec3 vPos = m_pTransformCom->Get_Info(INFO_POS);
    _vec3 vTo = m_vFinalTargetPos - vPos;
    _float fD = ::D3DXVec3LengthSq(&vTo);

    if (fD < g_Epsilon)
    {
        vPos = m_vFinalTargetPos;
        m_pTransformCom->Set_Info(INFO_POS, vPos);
        Set_Destroy(vPos);
        return;
    }

    _vec3 vDir;
    ::D3DXVec3Normalize(&vDir, &vTo);

    Turn_Towards(vDir, 360.f, fDeltaTime);

    _vec3 vLook = m_pTransformCom->Get_Info(INFO_LOOK);
    ::D3DXVec3Normalize(&vLook, &vLook);

    _float fDot = D3DXVec3Dot(&vLook, &vDir);
    _vec3 vForward = (fDot >= 0.f) ? vLook : vDir;

    _float fStep = m_fTargetingSpeed * fDeltaTime;
    _float fDistance = std::sqrt(fD);

    if (fDistance <= fStep)
    {
        vPos = m_vFinalTargetPos;
        m_pTransformCom->Set_Info(INFO_POS, vPos);
        Set_Destroy(vPos);
        return;
    }

    vPos += vForward * fStep;
    m_pTransformCom->Set_Info(INFO_POS, vPos);
}

void CMissile::Target_Exit()
{
}

void CMissile::Set_ModelTransform()
{
    if (CTransform *pModel_Transform = m_pModel->GetTransform())
    {
        const _matrix *pWorldMat = m_pTransformCom->Get_World();
        pModel_Transform->Set_Info(INFO::INFO_RIGHT, *((_vec3*)pWorldMat->m[0]));
        pModel_Transform->Set_Info(INFO::INFO_UP, *((_vec3 *)pWorldMat->m[1]));
        pModel_Transform->Set_Info(INFO::INFO_LOOK, *((_vec3 *)pWorldMat->m[2]));
        pModel_Transform->Set_Info(INFO::INFO_POS, *((_vec3 *)pWorldMat->m[3]));
    }

    m_pModel->Set_ChildrensMatrix();
}

void CMissile::Turn_Towards(const _vec3 &vDir, _float fMaxDegreePerSeconds, _float fDeltaTime)
{
    // 현재 나의 Look
    _vec3 vCurrent = m_pTransformCom->Get_Info(INFO::INFO_LOOK);
    ::D3DXVec3Normalize(&vCurrent, &vCurrent);

    _float fDot = std::clamp(::D3DXVec3Dot(&vCurrent, &vDir), -1.f, 1.f);

    // 총 회전해야할 Degree 구하고
    _float fDegreeTotal = D3DXToDegree(std::acos(fDot));
    
    // Epsilon보다 작다면 회전하지 않아도 되니 return
    if (fDegreeTotal < g_Epsilon)
        return;

    // 시간당 회전해야할 각도와 총 각도 중 작은것을 선택
    _float fStep = (std::min)(fDegreeTotal, fMaxDegreePerSeconds * fDeltaTime);

    _vec3 vAxis;
    ::D3DXVec3Cross(&vAxis, &vCurrent, &vDir);
    if (::D3DXVec3LengthSq(&vAxis) < g_Epsilon)
        vAxis = _vec3(0, 1, 0);
    ::D3DXVec3Normalize(&vAxis, &vAxis);

    m_pTransformCom->RotationDegree(vAxis, fStep);
}

void CMissile::Turn_Yaw_Towards(const _vec3 &vDir_XZ/* 타겟까지의 방향 */, _float fMaxDegreePerSeconds, _float fDeltaTime)
{
    _vec3 vLook = m_pTransformCom->Get_Info(INFO_LOOK);
    _vec3 vLook_XZ(vLook.x, 0.f, vLook.z);

    if (::D3DXVec3LengthSq(&vLook_XZ) < g_Epsilon)
        vLook_XZ = _vec3(1, 0, 0);

    ::D3DXVec3Normalize(&vLook_XZ, &vLook_XZ);

    // 각도 차이 (-파이 ~ 파이) 초당 최대 회전량으로 클램프!
    _float fYawCur = std::atan2(vLook_XZ.z, vLook_XZ.x);
    _float fYawDst = std::atan2(vDir_XZ.z, vDir_XZ.x);

    _float fDir_Yaw = fYawDst - fYawCur;

    while (fDir_Yaw > D3DX_PI) fDir_Yaw -= 2.f * D3DX_PI;
    while (fDir_Yaw < -D3DX_PI) fDir_Yaw += 2.f * D3DX_PI;

    _float fStep = std::clamp(fDir_Yaw, -D3DXToRadian(fMaxDegreePerSeconds * fDeltaTime), D3DXToRadian(fMaxDegreePerSeconds * fDeltaTime));
    
    if (std::fabs(fStep) < g_Epsilon)
        return;

    m_pTransformCom->RotationDegree(_vec3(0, 1, 0), D3DXToDegree(fStep));
}
