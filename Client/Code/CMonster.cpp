#include "pch.h"
#include "CMonster.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CObjectManager.h"
#include "CTimerMgr.h"
#include "CPickingManager.h"
#include "CColiderManager.h"
#include "CCullingManager.h"
#include "CEffectUI.h"
#include "CPlayer.h"
#include "CEffect_World.h"
#include "CEffect_Pixel_Sprite.h"
#include "CObjectPoolManager.h"
#include "CMonster_Head1.h"
#include "CItem.h"
#include "Sound_Manager.h"

ULONGLONG CMonster::s_lastKillTimeMs = 0; // 마지막 처치 시각(ms)
int       CMonster::s_comboCount = 0; // 현재 콤보 횟수

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev, MonsterType _eType)
    : CCharacter(pGraphicDev)
    , m_pColiderCom(nullptr)
    , m_pTextureCom(nullptr)
    , m_mapTexture()
    , m_pendingDeathUI(false)
    , m_lastWasHeadshot(false)
    , m_bPickable(true)
    , m_pPlayerTr(nullptr)
    , m_lastKillKind(KillKind::Normal)
    , m_eCategory(ObjectCategory::MONSTER)
    , m_eType(_eType)
    , m_bElecTriggered(false)
{
}

CMonster::CMonster(const CMonster& rhs, MonsterType _eType)
    : CCharacter(rhs)
    , m_pColiderCom(rhs.m_pColiderCom)
    , m_pTextureCom(rhs.m_pTextureCom)
    , m_mapTexture(rhs.m_mapTexture)
    , m_pendingDeathUI(rhs.m_pendingDeathUI)
    , m_lastWasHeadshot(rhs.m_lastWasHeadshot)
    , m_bPickable(rhs.m_bPickable)
    , m_pPlayerTr(rhs.m_pPlayerTr)
    , m_lastKillKind(rhs.m_lastKillKind)
    , m_eCategory(ObjectCategory::MONSTER)
    , m_eType(_eType)
{
}

CMonster::~CMonster() {}


HRESULT CMonster::Ready_GameObject()
{
    if (FAILED(__super::Ready_GameObject())) return E_FAIL;
    return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg))) return E_FAIL;
    if (FAILED(Set_Component())) return E_FAIL;

    if (MAPOBJECTDATA* pData = reinterpret_cast<MAPOBJECTDATA*>(pArg))
    {
        GetTransform()->Set_Info(INFO::INFO_POS, pData->transform.Pos);
        m_pTransformCom->Apply_WorldMatrix();
    }

    m_bPickable = true; 
    return S_OK;
}

_int CMonster::Update_GameObject(const _float& fTimeDelta)
{
    if (m_bDead) return DEAD;

    __super::Update_GameObject(fTimeDelta);

    CPickingManager::GetInstance()->Remove_PickingGroup(this);
    CGameObject::Update_GameObject(fTimeDelta);

    if (m_pColiderCom && m_pColiderCom->Is_Active())
        CColiderManager::GetInstance()->Add_CollisionGroup(CColiderManager::COLLISION_MONSTER, this);


    if (m_pRendererCom)
        m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this);

    return NO_EVENT;
}

void CMonster::LateUpdate_GameObject(const _float& fTimeDelta)
{
    __super::LateUpdate_GameObject(fTimeDelta);

    Update_Position(m_pTransformCom->Get_Info(INFO_POS));
    Compute_CamDistance(Get_Position());

    SetUp_BillBoard();

    if (CCullingManager::GetInstance()->Is_In_Frustum(Get_Position(), m_fRadius))
        if (m_pRendererCom) m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this);

    // 죽었거나 픽킹 금지면 등록 안 함
    if (m_bPickable && !m_bDead)
        CPickingManager::GetInstance()->Add_PickingGroup(this);
}

void CMonster::Render_GameObject()
{
    __super::Render_GameObject();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTransformCom->Apply_WorldMatrix();
    if (m_pTextureCom) {
        m_pTextureCom->Set_Texture(m_pTextureCom->Get_Frame().m_iCurrentTex);
        m_pTextureCom->MoveFrame();
    }

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMonster::Set_Component()
{
    CColider_Sphere::COLLINFO CollSphereInfo{};
    CollSphereInfo.fRadius = 0.5f;
    CollSphereInfo.vOffset = _vec3(0.f, 0.f, 0.f);

    if (FAILED(Add_Components(L"Com_Collider_Sphere", SCENE_STATIC, L"Proto_Colider_Sphere",
        (CComponent**)&m_pColiderCom, &CollSphereInfo)))
        return E_FAIL;

    m_pColiderCom->Set_Transform(m_pTransformCom);

    if (FAILED(Texture_Clone()))
        return E_FAIL;

    return S_OK;
}

_bool CMonster::Picking(_vec3*)
{
    if (m_bDead || !m_bPickable) return false;
    if (m_pColiderCom && !m_pColiderCom->Is_Active()) return false;
    return true;
}

void CMonster::PickingTrue() { m_bPickingTrue = true; }

void CMonster::SetUp_BillBoard()
{
    _matrix _matView;
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &_matView);
    D3DXMatrixInverse(&_matView, nullptr, &_matView);

    _vec3 vRight = *(_vec3*)&_matView.m[0][0];
    _vec3 vUp = *(_vec3*)&_matView.m[1][0];
    _vec3 vLook = *(_vec3*)&_matView.m[2][0];

    m_pTransformCom->Set_Info(INFO_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
    m_pTransformCom->Set_Info(INFO_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
    m_pTransformCom->Set_Info(INFO_LOOK, *D3DXVec3Normalize(&vLook, &vLook) * m_pTransformCom->Get_Scale().z);
}


// 벽 충돌
void CMonster::Set_Collider_With_Wall()
{
    if (!m_pColiderCom || !m_pColiderCom->Is_Active())
        return;

    _vec3 vDistance;
    if (CColiderManager::GetInstance()->CollisionGroup(
        CColiderManager::COLLISION_HORWALL, this, CColiderManager::COLLISION_SPHERE_CUBE, &vDistance))
    {
        _vec3 vPos = m_pTransformCom->Get_Info(INFO_POS);
        m_pTransformCom->Set_Info(INFO_POS, vPos += vDistance);
    }
    if (CColiderManager::GetInstance()->CollisionGroup(
        CColiderManager::COLLISION_VERWALL, this, CColiderManager::COLLISION_SPHERE_CUBE, &vDistance))
    {
        _vec3 vPos = m_pTransformCom->Get_Info(INFO_POS);
        m_pTransformCom->Set_Info(INFO_POS, vPos += vDistance);
    }
}



HRESULT CMonster::Texture_Clone() { return S_OK; }

HRESULT CMonster::Change_Texture(const _tchar* LayerTag)
{
    if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
        return E_FAIL;
    if (m_pTextureCom) m_pTextureCom->Set_Zero_Frame();
    return S_OK;
}

// 플레이어 추적 관련 함수
CTransform* CMonster::GetPlayerTransform()
{
    auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
    if (!m_pPlayerTr)
        m_pPlayerTr = dynamic_cast<CTransform*>(
            CObjectManager::GetInstance()->Get_Component(sceneIdx, L"Player_Layer", L"Com_Transform", 0));
    return m_pPlayerTr;
}

float CMonster::DistanceToPlayer() const
{
    if (!m_pPlayerTr) return FLT_MAX;
    _vec3 my = m_pTransformCom->Get_Info(INFO_POS);
    _vec3 pl = m_pPlayerTr->Get_Info(INFO_POS);
    _vec3 diff = pl - my;
    return D3DXVec3Length(&diff);
}

void CMonster::Spawn_Eletric_Effect(const _vec3& vPos)
{
    _float fRand = Rand_Float(0.1f, 0.4f);
    _float fRand2 = Rand_Float(0.1f, 0.4f);
    _float fRand3 = Rand_Float(0.1f, 0.4f);
    _float fRand4 = Rand_Float(0.1f, 0.4f);
    _float fRand5 = Rand_Float(0.1f, 0.4f);
    _float fRand6 = Rand_Float(0.1f, 0.4f);
    _float fRand7 = Rand_Float(0.1f, 0.4f);
    _float fRand8 = Rand_Float(0.1f, 0.4f);

    EFFECTINFO tInfo;
    EffectOptions Option = Get_Preset_Electric();
    tInfo.eType = WorldEffectType::ELCETRIC;
    tInfo.fSize = 2.f;
    Option.iPixelCount = 36;
    Option.fLife_Max = 1.f;

    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_WORLD, &tInfo,
        [vPos, fRand, fRand2](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPos - _vec3{ fRand, fRand2, fRand2 });
        });
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_PIXEL, &Option,
        [vPos, fRand, fRand2](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPos - _vec3{ fRand, fRand2, fRand2 });
        });
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_WORLD, &tInfo,
        [vPos, fRand3, fRand4](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPos + _vec3{ fRand3, fRand4, fRand4 });
        });
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_PIXEL, &Option,
        [vPos, fRand3, fRand4](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPos + _vec3{ fRand3, fRand4, fRand4 });
        });
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_WORLD, &tInfo,
        [vPos, fRand5, fRand6](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPos - _vec3{ fRand5, fRand6, fRand6 });
        });
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_PIXEL, &Option,
        [vPos, fRand5, fRand6](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPos - _vec3{ fRand5, fRand6, fRand6 });
        });
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_WORLD, &tInfo,
        [vPos, fRand7, fRand8](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPos + _vec3{ fRand7, fRand8, fRand8 });
        });
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_PIXEL, &Option,
        [vPos, fRand7, fRand8](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPos + _vec3{ fRand7, fRand8, fRand8 });
        });
}

void CMonster::Spawn_Explosion_Effect(const _vec3& vPos)
{
    _vec3 vLook = GetTransform()->Get_Info(INFO::INFO_LOOK);
    ::D3DXVec3Normalize(&vLook, &vLook);
    vLook *= 0.1f;
    EFFECTINFO tInfo;
    tInfo.eType = WorldEffectType::EXPLOSION;
    tInfo.fSize = 20.f;
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_WORLD, &tInfo,
        [vPos](CGameObject* pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPos + _vec3{ 0.f, 0.f, 0.f });
        });

    tInfo.eType = WorldEffectType::SMOKE;
    tInfo.fSize = 16.f;
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_WORLD, &tInfo,
        [vPos](CGameObject* pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPos + _vec3{ 0.f, 0.f, 0.f });
        });

    tInfo.eType = WorldEffectType::BLOOD_EXPLOSION;
    tInfo.fSize = 10.f;
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_WORLD, &tInfo,
        [vPos, vLook](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPos + _vec3{ 0.f, 0.f, 0.f });
        });

    EffectOptions Option = Get_Preset_BulletSpark();
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_PIXEL, &Option,
        [vPos](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPos + _vec3{ 0.f, 0.1f, 0.f });
        });

    SpriteParticleOptions Option2;
    Option2.tEffectOption = Get_Preset_Blood();
    Option2.eType = SpriteParticleType::DRONEBODY;
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_PIXEL_SPRITE, &Option2,
        [vPos](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPos + _vec3{ 0.f, 0.25f, 0.f });
        });
}

void CMonster::Spawn_HeadExplosion_Effect(const _vec3& vPos)
{
    EFFECTINFO tInfo;
    tInfo.eType = WorldEffectType::BLOOD_EXPLOSION4;
    tInfo.fSize = 1.f;
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_WORLD, &tInfo,
        [vPos](CGameObject* pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPos + _vec3{ 0.f, 0.2f, 0.f });
        });

    HeadSpawnArg cfg{};
    cfg.texTag = L"Com_Texture_Monster_HEAD";           
    cfg.protoTag = L"Prototype_Component_Texture_Monster_HEAD";
    cfg.endFrame = 11;     
    cfg.animSpeed = 8.f;
    cfg.loop = false;
    cfg.fallSpeed = 0.3f;  
    cfg.gravity = 3.0f;  
    cfg.backDrift = 0.5f;  

    const auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
    if (auto* head = dynamic_cast<CMonster_Head1*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_Monster_Head1", sceneIdx, L"Monster_Layer", &cfg)))
    {
        _vec3 vLeftPos = vPos;
        vLeftPos.x += 1.f; // 원하는 만큼 왼쪽으로 이동
        head->GetTransform()->Set_Info(INFO_POS, vLeftPos);
        head->GetTransform()->Set_Scale(0.2f, 0.2f, 0.2f);
    }
}

void CMonster::Spawn_Hit_Effect(const _vec3& vPos)
{
    EFFECTINFO tInfo;
    tInfo.eType = WorldEffectType::BLOOD_EXPLOSION2;

    tInfo.fSize = 1.f;
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_WORLD, &tInfo,
        [vPos](CGameObject* pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPos + _vec3{ 0.f, 0.2f, 0.f });
        });
}

void CMonster::Spawn_Hit_Vent(const _vec3& vPos)
{
    EFFECTINFO tInfo;
    tInfo.eType = WorldEffectType::BLOOD_EXPLOSION2;

    tInfo.fSize = 3.f;
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_WORLD, &tInfo,
        [vPos](CGameObject* pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPos + _vec3{ 0.f, 0.f, 0.f });
        });

    SpriteParticleOptions Option2;
    Option2.tEffectOption = Get_Preset_Blood();
    Option2.eType = SpriteParticleType::DEADBODY;
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_PIXEL_SPRITE, &Option2,
        [vPos](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPos + _vec3{ 0.f, 0.25f, 0.f });
        });

    EffectOptions Option = Get_Preset_Blood();
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_PIXEL, &Option,
        [vPos](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPos);
        });
    CObjectPoolManager::GetInstance()->Spawn(PoolType::EFFECT_PIXEL, &Option,
        [vPos](CGameObject *pGo)->void
        {
            pGo->GetTransform()->Set_Info(INFO::INFO_POS, vPos);
        });
}

void CMonster::CreateHeadHitSound()
{
    const wchar_t* soundKeys[] = {
        L"../Bin/Resource/Sounds/sfx_gp_enemies_headshot_01",
        L"../Bin/Resource/Sounds/sfx_gp_enemies_headshot_02",
        L"../Bin/Resource/Sounds/sfx_gp_enemies_headshot_03",
        L"../Bin/Resource/Sounds/sfx_gp_enemies_headshot_04"
    };

    const int soundCount = sizeof(soundKeys) / sizeof(soundKeys[0]);

    int idx = rand() % soundCount;

    CSound_Manager::GetInstance()->PlaySoundW(const_cast<wchar_t*>(soundKeys[idx]),SOUND_MONSTER, 1.0f, false);
}

void CMonster::CreateDeathSound()
{
    // 재생할 사운드 키 배열
    const wchar_t* soundKeys[] = {
        L"../Bin/Resource/Sounds/enemyGeneral.death_alt3-001",
        L"../Bin/Resource/Sounds/enemyGeneral.death_alt3-005"
        L"../Bin/Resource/Sounds/enemyGeneral.death_alt3-006"
        L"../Bin/Resource/Sounds/enemyGeneral.death_alt3-009"
    };

    const int soundCount = sizeof(soundKeys) / sizeof(soundKeys[0]);
    int idx = rand() % soundCount;

    CSound_Manager::GetInstance()->PlaySoundW(const_cast<wchar_t*>(soundKeys[idx]), SOUND_MONSTER, 1.0f, false);
}

void CMonster::CreateKatanaHitSound()
{
    // 재생할 사운드 키 배열
    const wchar_t* soundKeys[] = {
        L"../Bin/Resource/Sounds/sfx_gp_blood_medium_01",
        L"../Bin/Resource/Sounds/sfx_gp_blood_medium_02",
        L"../Bin/Resource/Sounds/sfx_gp_blood_medium_03",
        L"../Bin/Resource/Sounds/sfx_gp_blood_medium_04",
        L"../Bin/Resource/Sounds/sfx_gp_blood_medium_05",
    };

    const int soundCount = sizeof(soundKeys) / sizeof(soundKeys[0]);
    int idx = rand() % soundCount;

    CSound_Manager::GetInstance()->PlaySoundW(const_cast<wchar_t*>(soundKeys[idx]), SOUND_MONSTER, 1.0f, false);
}

void CMonster::CreateElectricSound()
{
    // 재생할 사운드 키 배열
    const wchar_t* soundKeys[] = {
        L"../Bin/Resource/Sounds/tiro eletrico-001",
        L"../Bin/Resource/Sounds/tiro eletrico-002",
    };

    const int soundCount = sizeof(soundKeys) / sizeof(soundKeys[0]);
    int idx = rand() % soundCount;

    CSound_Manager::GetInstance()->PlaySoundW(const_cast<wchar_t*>(soundKeys[idx]), SOUND_MONSTER, 1.0f, false);
}

void CMonster::CreateShotSound()
{
    // 재생할 사운드 키 배열
    const wchar_t* soundKeys[] = {
        L"../Bin/Resource/Sounds/pistol.shoot-001",
        L"../Bin/Resource/Sounds/pistol.shoot-002",
    };

    const int soundCount = sizeof(soundKeys) / sizeof(soundKeys[0]);
    int idx = rand() % soundCount;

    CSound_Manager::GetInstance()->PlaySoundW(const_cast<wchar_t*>(soundKeys[idx]), SOUND_MONSTER, 1.0f, false);
}

// 각 몬스터 이펙트 배너 텍스트 설정
void CMonster::GetDeathUIConfig(DeathUIConfig& cfg, bool) const
{

}

void CMonster::QueueDeathUI(bool isHeadshot)
{
    m_lastWasHeadshot = isHeadshot;
    m_pendingDeathUI = true;
}

void CMonster::TrySpawnDeathUI_Common()
{
    if (!m_pendingDeathUI) return;
    m_pendingDeathUI = false;

    DeathUIConfig cfg;
    GetDeathUIConfig(cfg, m_lastWasHeadshot);

    SpawnFollowUI_Common(m_lastWasHeadshot, cfg);
    SpawnBannerUI_Common(m_lastWasHeadshot, cfg);

    const ULONGLONG now = GetTickCount64();
    if (now - s_lastKillTimeMs <= 1500) ++s_comboCount; else s_comboCount = 1;
    s_lastKillTimeMs = now;

    if (s_comboCount >= 2)
        SpawnComboUI_Common();
}

void CMonster::SpawnFollowUI_Common(bool isHeadshot, const DeathUIConfig& cfg)
{
    if (auto ui = dynamic_cast<CEffectUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_MonsterHitEffectUI", SCENE_STATIC, L"UI_Layer")))
    {
        ui->SetImageSize(cfg.iconW, cfg.iconH);
        ui->SetBoxSize(200.f, 50.f);
        ui->Change_Texture(cfg.iconTexTag);
        ui->SetNumberEmphasis(cfg.emDigits, 1.0f);

        const float secsAdd = isHeadshot ? cfg.followSecsHead : cfg.followSecsNormal;
        ui->ShowFollowTransform(
            isHeadshot ? cfg.rightTextHead : cfg.rightTextNormal,
            cfg.iconTexTag,
            secsAdd,
            m_pTransformCom,
            0.9f, 240.f, 120.f,
            0.85f,
            L"Font_UI_Effect",
            D3DXCOLOR(1, 1, 1, 1));
    }
}

void CMonster::SpawnBannerUI_Common(bool isHeadshot, const DeathUIConfig& cfg)
{
    if (auto banner = dynamic_cast<CEffectUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_MonsterHitEffectUI", SCENE_STATIC, L"UI_Layer")))
    {
        banner->Change_Texture(cfg.iconTexTag);
        banner->SetImageSize(cfg.iconW, cfg.iconH);
        banner->SetBannerShowIcon(true);
        banner->SetImageOffset(cfg.iconOffX, cfg.iconOffY);

        banner->SetBannerRightText(isHeadshot ? cfg.rightTextHead : cfg.rightTextNormal);
        banner->SetNumberEmphasis(cfg.emDigits, cfg.emScale);
        banner->SetBannerRightFixedScale(cfg.rightFixedScale);

        banner->SetBannerTextOffset(cfg.textOffX, cfg.textOffY);
        banner->SetBannerAngle(0.f);
        banner->LinkBannerTextAngleToBanner(true);
        banner->SetBannerTextAngle(cfg.textAngleDeg);
        banner->SetBannerDownSpeed(cfg.bannerDownSpeed);

        banner->SetBannerLabelPop(cfg.labelPopStart, cfg.labelPopDur);

        banner->ShowBanner(
            isHeadshot ? cfg.killTextHead : cfg.killTextNormal,
            1.10f,
            cfg.bannerX, cfg.bannerY,
            1.0f, 1.0f,
            L"Font_UI_Effect",
            D3DXCOLOR(1, 1, 1, 1),
            0.85f,
            4.f);

        banner->SetBoxSize(cfg.bannerBoxW, cfg.bannerBoxH);

        banner->SetBannerLabelPop(cfg.labelPopStart, cfg.labelPopDur);
    }
}

void CMonster::SpawnComboUI_Common()
{
    if (auto left = dynamic_cast<CEffectUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_MonsterHitEffectUI", SCENE_STATIC, L"UI_Layer")))
    {
        left->SetBannerExtraWidth(20.f);
        wchar_t buf[16]; swprintf(buf, 16, L"X%d", s_comboCount);
        left->ShowBanner(buf, 1.00f, 60.f, 340.f, 3.f, 1.3f,
            L"Font_UI_Effect", D3DXCOLOR(1, 1, 1, 1), 0.85f, -8.f);
        left->SetBannerTextAngle(10.f);
    }

    if (auto right = dynamic_cast<CEffectUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_MonsterHitEffectUI", SCENE_STATIC, L"UI_Layer")))
    {
        right->SetBannerExtraWidth(40.f);
        right->ShowBanner(L"COMBO", 1.00f, 260.f, 370.f, 1.2f, 1.0f,
            L"Font_UI_Effect", D3DXCOLOR(1, 1, 1, 1), 0.85f, -8.f);
        right->SetBannerTextOffset(20.f, -10.f);
        right->SetBannerTextAngle(3.f);
    }
}

void CMonster::DisableAllCollisionAndPicking()
{
    m_bPickable = false;
    if (m_pColiderCom) m_pColiderCom->Set_Active(false);
    CPickingManager::GetInstance()->Remove_PickingGroup(this);
}

HRESULT CMonster::Create_Weapon(_int iRate)
{

    if (rand() % iRate) // iDropRate가 0이 아니면 -> 생성하지 않은
        return S_OK;
   
    CItem::ITEMINFO tInfo{};

    _vec3 vPos = GetTransform()->Get_Info(INFO_POS); // 시작 위치
    vPos.y += 0.1f; // 살짝 위로

    _vec3 vLook = GetTransform()->Get_Info(INFO_LOOK); // 자판기 보다 살짝 앞으로 보내기 위해
    D3DXVec3Normalize(&vLook, &vLook); // 정규화
    vPos = vPos - vLook * 0.1f;

    tInfo.vStartPos = vPos;
    tInfo.eWeapon = WP_KNIFE;
    if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Prototype_GameObject_Item",
        CManagement::GetInstance()->Get_CurrentSceneIdx(), L"GameLogic_Layer", &tInfo)))
        return E_FAIL;

    return S_OK;
}

CGameObject* CMonster::Clone(void* pArg)
{
    CMonster* pInstance = new CMonster(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("pMonster Clone Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CMonster::Free()
{
    Engine::CGameObject::Free();
}