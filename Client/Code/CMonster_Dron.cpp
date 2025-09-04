#include "pch.h"
#include "CMonster_Dron.h"
#include "CGameDataManager.h"
#include "CColiderManager.h"
#include "CComponentMgr.h"
#include "CObjectManager.h"
#include "CMapFactory.h"
#include "CEffectUI.h"
#include "CGrounding.h"
#include "CPicking.h"
#include "CGlobal_Info.h"
#include "CManagement.h"
#include "CBullet.h"
#include "Sound_Manager.h"

CMonster_Dron::CMonster_Dron(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev, MonsterType::SUIT) 
{
}

CMonster_Dron::CMonster_Dron(const CMonster_Dron& rhs)
    : CMonster(rhs)
    , m_eMonState(rhs.m_eMonState)
    , m_ePrevState(rhs.m_ePrevState)
    , m_fDetectRadius(rhs.m_fDetectRadius)
    , m_fLoseRadius(rhs.m_fLoseRadius)
{
}

CMonster_Dron::~CMonster_Dron() {}

HRESULT CMonster_Dron::Ready_GameObject()
{
    if (FAILED(__super::Ready_GameObject()))
        return E_FAIL;
    return S_OK;
}

HRESULT CMonster_Dron::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_fHp = 1.f;

    _int iSceneIndex = CMapFactory::GetInstance()->GetTargetSceneIndex();

    CTransform::TRANSFORMINFO ti{};
    if(iSceneIndex == SCENE_CAR)
        ti.fSpeed = 0.5f;
    else
        ti.fSpeed = 6.0f;
    ti.fRotationSpeed = D3DXToRadian(180); // 방향 전환 빠르게
    m_pTransformCom->SetTransformInfo(ti);
    m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.5f);

    GetPlayerTransform();
    SetState(IDLE);
    return S_OK;
}

_int CMonster_Dron::Update_GameObject(const _float& fTimeDelta)
{
    if (m_bDead) return DEAD;

    OnUpdateState(m_eMonState, fTimeDelta);
    __super::Update_GameObject(fTimeDelta);
    return NO_EVENT;
}

void CMonster_Dron::LateUpdate_GameObject(const _float& fTimeDelta)
{
    Set_OnTerrain(fTimeDelta); 
    Set_Collider();
    Set_Check_Weapon();       
    __super::LateUpdate_GameObject(fTimeDelta);
}

void CMonster_Dron::Render_GameObject()
{
    __super::Render_GameObject();
}

void CMonster_Dron::Set_Collider()
{
    if (!m_pColiderCom) { Set_Collider_With_Wall(); return; }
    if (!m_pColiderCom->Is_Active()) { Set_Collider_With_Wall(); return; }

    // 플레이어와 멀리 떨어진 경우 충돌 스킵
    CTransform* pPlayerTr = GetPlayerTransform();
    if (pPlayerTr) {
        _vec3 diff = pPlayerTr->Get_Info(INFO_POS) - m_pTransformCom->Get_Info(INFO_POS);
        float dist2 = D3DXVec3LengthSq(&diff);
        if (dist2 > 30.f * 30.f) return;
    }

    m_pColiderCom->Update_ColliderSphere();
    Set_Collider_With_Wall();
    Set_Collider_With_Bullet();
}

void CMonster_Dron::GetDeathUIConfig(DeathUIConfig& cfg, bool /*isHeadshot*/) const
{
    CMonster::GetDeathUIConfig(cfg, false);
    cfg.bannerBoxW = 360.f;
    cfg.bannerBoxH = 50.f;

    cfg.rightTextNormal = L"2sec";
    cfg.rightTextHead = L"2sec";    
    cfg.killTextNormal = L"처치";
    cfg.killTextHead = L"처치";

    if (m_eMonState == KATANA_DEATH || m_ePrevState == KATANA_DEATH) {
        cfg.killTextNormal = L"원무";
    }
}

void CMonster_Dron::Set_Check_Weapon()
{
    if (!m_pColiderCom || !m_pColiderCom->Is_Active())
        return;


    if (!CColiderManager::GetInstance()->CollisionGroup(
        CColiderManager::COLLISION_WEAPON, this, CColiderManager::COLLISION_SPHERE, nullptr))
        return;

    static CGameObject* sWinner = nullptr;
    static DWORD sStamp = 0;
    const DWORD now = GetTickCount();
    if (sWinner && now - sStamp > 120) sWinner = nullptr;
    if (sWinner && sWinner != this) return;

    sWinner = this;
    sStamp = now;

    QueueDeathUI(false);
    SetState(KATANA_DEATH);
    if (m_pColiderCom) m_pColiderCom->Set_Active(false);
    m_bPickable = false;
}

_bool CMonster_Dron::Picking(_vec3* PickingPoint)
{
    if (CGlobal_Info::Get_Instance()->Get_PlayerInfo().eWeapon == WEAPON::WP_KATANA)
        return false;

    if (!m_pColiderCom) return false;


    CPicking* pk = CPicking::GetInstance();
    _vec3 rayO = pk->GetRayOrigin();
    _vec3 rayD = pk->GetRayDir(); D3DXVec3Normalize(&rayD, &rayD);

    const auto desc = m_pColiderCom->Get_SphereDesc(); 
    const _vec3 center =
        m_pTransformCom->Get_Info(INFO_POS) +
        m_pTransformCom->Get_Info(INFO_RIGHT) * desc.vOffset.x +
        m_pTransformCom->Get_Info(INFO_UP) * desc.vOffset.y +
        m_pTransformCom->Get_Info(INFO_LOOK) * desc.vOffset.z;

    float t;
    if (!CPicking::IntersectRaySphere(rayO, rayD, center, desc.fRadius, &t) || t < 0.f)
        return false;

    if (PickingPoint) *PickingPoint = rayO + rayD * t;
    return true;
}

void CMonster_Dron::HitAt(const _vec3& /*hitPosWorld*/)
{
    if (m_bDead) return;

    QueueDeathUI(false);
    if (auto* p = GetPlayerObj())
        p->Add_Hp(2.f);
    m_bPickable = false;
    if (m_pColiderCom) m_pColiderCom->Set_Active(false);

    SetState(DEATH);
}

HRESULT CMonster_Dron::Texture_Clone()
{
    CTexture::TEXINFO info{};
    struct AnimDef { const wchar_t* tag; const wchar_t* proto; int start; int end; float speed; bool loop; };
    AnimDef anims[] = {
        { L"Com_Texture_Idle",         L"Prototype_Component_Texture_Monster_Drone_IDLE",    0, 6, 10.f, true  },
        { L"Com_Texture_Wake",         L"Prototype_Component_Texture_Monster_Drone_WAKE",    0, 6, 12.f, false },
        { L"Com_Texture_Attack",       L"Prototype_Component_Texture_Monster_Drone_ATTACK",  0, 10, 12.f, true  },
        { L"Com_Texture_KatanaDeath",  L"Prototype_Component_Texture_Monster_Drone_KATANA_DEATH",  0, 11,  12.f, false },
        { L"Com_Texture_Death",        L"Prototype_Component_Texture_Monster_Drone_DEATH",   0, 4,  13.f, false },
    };

    for (auto& a : anims)
    {
        ZeroMemory(&info, sizeof(info));
        info.m_iStart = a.start;
        info.m_iEndTex = a.end;
        info.m_fSpeed = a.speed;
        info.m_bLoop = a.loop;

        if (FAILED(Add_Components(a.tag, SCENE_STATIC, a.proto, (CComponent**)&m_pTextureCom, &info)))
            return E_FAIL;

        m_mapTexture.insert({ a.tag, m_pTextureCom });
    }
    return S_OK;
}

void CMonster_Dron::SetState(MON_STATE next)
{
    m_ePrevState = m_eMonState;
    m_eMonState = next;
    OnEnterState(next);
}

void CMonster_Dron::OnEnterState(MON_STATE s)
{
    const wchar_t* tag = L"Com_Texture_Idle";

    switch (s)
    {
    case IDLE:          tag = L"Com_Texture_Idle"; break;
    case WAKE:
    {
                tag = L"Com_Texture_Wake";
        auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
        if (sceneIdx != SCENE_CAR)
            CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/sfx_enemy_dogdrone_alert", SOUND_MONSTER, 1.f, false);
    }
        break;
    case ATTACK:        tag = L"Com_Texture_Attack"; break;

    case KATANA_DEATH:
    {
        m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
        tag = L"Com_Texture_KatanaDeath";
        if (m_pColiderCom) m_pColiderCom->Set_Active(false);
        const _vec3 myPos = m_pTransformCom ? m_pTransformCom->Get_Info(INFO_POS) : _vec3(0.f, -5.f, 0.f);
        TrySpawnDeathUI_Common();
        if (auto* p = GetPlayerObj())
            p->Add_Hp(2.f);
        m_bPickable = false;


        CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/enemyDrone.death-002", SOUND_MONSTER, 0.3f, false);
        CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/explosions-001", SOUND_MONSTER, 1.f, false);
        
    }
        break;

    case DEATH:
        // 공통 사망 배너 처리
        DisableAllCollisionAndPicking();
        tag = L"Com_Texture_Death";
        TrySpawnDeathUI_Common();
        CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/enemyDrone.death-002", SOUND_MONSTER, 0.1f, false);
        CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/explosions-001", SOUND_MONSTER, 1.f, false);
        break;
    }

    Change_Texture(tag);
    if (m_pTextureCom) { m_pTextureCom->Set_Zero_Frame(); m_pTextureCom->Resume_Anim(); }
}

void CMonster_Dron::OnUpdateState(MON_STATE s, const _float& dt)
{
    GetPlayerTransform();
    const float dist = DistanceToPlayer();

    switch (s)
    {
    case IDLE:
        if (dist <= m_fDetectRadius) SetState(WAKE);
        break;

    case WAKE:
        if (m_pTextureCom && m_pTextureCom->Is_AnimFinished())
            SetState(ATTACK);
        break;

    case ATTACK:
    {
        if (!m_pPlayerTr) break;

        _vec3 my = m_pTransformCom->Get_Info(INFO_POS);
        _vec3 pl = m_pPlayerTr->Get_Info(INFO_POS);

        _vec3 dir = pl - my;

        float d2 = D3DXVec3LengthSq(&dir);
        if (d2 > 1e-6f)
        {
            float d = sqrtf(d2);
            _vec3 fwd = dir; D3DXVec3Normalize(&fwd, &fwd);
            m_pTransformCom->LookAt(pl);

            const float keep = m_fKeepDistance;   
            const float band = m_fKeepHysteresis;  

            if (d > keep + band)
            {
                m_pTransformCom->Move_PosDir(dt, fwd);
            }
            else
            {

            }
        }

        if (dist > m_fLoseRadius) SetState(IDLE);
    }
        break;

    case KATANA_DEATH:
    case DEATH:
        if (m_pTextureCom && m_pTextureCom->Is_AnimFinished())
        {
            const _vec3 myPos = m_pTransformCom ? m_pTransformCom->Get_Info(INFO_POS) : _vec3(0.f, -5.f, 0.f);
            Spawn_Explosion_Effect(myPos);
            m_bDead = true;
        }
        break;
    }
}

_vec3 CMonster_Dron::GetHeadWorldPos() const
{
    _vec3 pos = m_pTransformCom->Get_Info(INFO_POS);
    pos.y += (m_pColiderCom ? m_pColiderCom->Get_SphereDesc().fRadius : 0.6f);
    return pos;
}

bool CMonster_Dron::WorldToScreen(const _vec3& world, float& sx, float& sy) const
{
    D3DXMATRIX view, proj, id; D3DVIEWPORT9 vp{};
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &view);
    m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &proj);
    m_pGraphicDev->GetViewport(&vp);
    D3DXMatrixIdentity(&id);

    D3DXVECTOR3 in(world.x, world.y, world.z), out;
    D3DXVec3Project(&out, &in, &vp, &proj, &view, &id);
    sx = out.x; sy = out.y;
    return (out.z >= 0.f && out.z <= 1.f);
}

void CMonster_Dron::Set_Collider_With_Bullet()
{
    CGameObject* pColliObj{ nullptr };
    if (CColiderManager::GetInstance()->CollisionGroupWho(CColiderManager::COLLISION_BULLET, this, CColiderManager::COLLISION_SPHERE, nullptr, pColliObj))
    {
        if (!pColliObj) // 예외처리
            return;

        // 플레이어 bullet 일 때
        if (dynamic_cast<CBullet*>(pColliObj)->Get_OwnerType() == BulletData::OWNER::PLAYER)
        {
            pColliObj->Set_Dead(true); // bullet dead 처리
            SetState(DEATH);
        }
    }
}

CMonster_Dron* CMonster_Dron::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMonster_Dron* pInstance = new CMonster_Dron(pGraphicDev);
    if (FAILED(pInstance->Ready_GameObject()))
    {
        MSG_BOX("CMonster_Dron Create Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CMonster_Dron::Clone(void* pArg)
{
    CMonster_Dron* pInstance = new CMonster_Dron(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CMonster_Dron Clone Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CMonster_Dron::Free()
{
    __super::Free();
}