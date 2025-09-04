#include "pch.h"
#include "CMonster_Soldier.h"
#include "CGameDataManager.h"
#include "CColiderManager.h"
#include "CComponentMgr.h"
#include "CObjectManager.h"
#include "CObjectPoolManager.h"
#include "CEffectUI.h"
#include "Engine_Define.h"
#include "CGrounding.h"
#include "CPicking.h"
#include "CGlobal_Info.h"
#include "CManagement.h"
#include "CEffect_Pixel_Sprite.h"
#include "CBullet.h"
#include "CPickingManager.h"
#include "CMonster_Head.h"

#ifdef _DEBUG
namespace {
    static LPD3DXMESH g_pDebugSphereMesh = nullptr;
    static void EnsureDebugSphereMesh(LPDIRECT3DDEVICE9 dev)
    {
        if (!g_pDebugSphereMesh)
            D3DXCreateSphere(dev, 1.f, 16, 16, &g_pDebugSphereMesh, nullptr);
    }
    static D3DCOLOR PartColor(CMonster_Soldier::HIT_PART p)
    {
        switch (p) {
        case CMonster_Soldier::HIT_HEAD:  return D3DCOLOR_ARGB(255, 255, 80, 80);
        case CMonster_Soldier::HIT_BALLS: return D3DCOLOR_ARGB(255, 255, 220, 80);
        case CMonster_Soldier::HIT_LEG:   return D3DCOLOR_ARGB(255, 80, 150, 255);
        case CMonster_Soldier::HIT_BODY:  return D3DCOLOR_ARGB(255, 120, 255, 120);
        default:                       return D3DCOLOR_ARGB(255, 200, 200, 200);
        }
    }
}
#endif

CMonster_Soldier::CMonster_Soldier(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev, MonsterType::SUIT)
    , m_eMonState(IDLE), m_ePrevState(IDLE)
    , m_fChaseRadius(9.f), m_fAimRadius(6.f), m_fLoseRadius(16.f)
    , m_jumpCD(0.f), m_jumpDir(0), m_bKillAfterHit(false)
{
}

CMonster_Soldier::CMonster_Soldier(const CMonster_Soldier& rhs)
    : CMonster(rhs)
    , m_eMonState(rhs.m_eMonState), m_ePrevState(rhs.m_ePrevState)
    , m_fChaseRadius(rhs.m_fChaseRadius), m_fAimRadius(rhs.m_fAimRadius), m_fLoseRadius(rhs.m_fLoseRadius)
    , m_jumpCD(rhs.m_jumpCD), m_jumpDir(rhs.m_jumpDir), m_bKillAfterHit(rhs.m_bKillAfterHit)
{
}

CMonster_Soldier::~CMonster_Soldier() {}

HRESULT CMonster_Soldier::Ready_GameObject()
{
    if (FAILED(__super::Ready_GameObject())) return E_FAIL;
    return S_OK;
}

HRESULT CMonster_Soldier::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg))) return E_FAIL;

    m_fHp = 2;

    CTransform::TRANSFORMINFO ti{};
    ti.fSpeed = 5.f;
    ti.fRotationSpeed = D3DXToRadian(90.f);
    m_pTransformCom->SetTransformInfo(ti);
    m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.f);

    GetPlayerTransform();

    m_jumpCD = 1.f + (rand() % 2001) / 1000.f;
    SetState(IDLE);
    SetupHitSpheres();

    _float fOut{ 0.f };
    m_pGroundingCom->Initialize_CurrentIndex(
        CGameDataManager::GetInstance()->Get_SortedFloorEntries(),
        m_pTransformCom->Get_Info(INFO::INFO_POS).x,
        m_pTransformCom->Get_Info(INFO::INFO_POS).z,
        &fOut);

    _vec3 vPos = m_pTransformCom->Get_Info(INFO::INFO_POS);
    vPos.y = fOut + m_pTransformCom->Get_Scale().y * 0.5f;
    m_pTransformCom->Set_Info(INFO::INFO_POS, vPos);

    return S_OK;
}

_int CMonster_Soldier::Update_GameObject(const _float& fTimeDelta)
{
    if (m_bDead) 
    {
        CMonster::Create_Weapon(7);
        return DEAD;
    }
    OnUpdateState(m_eMonState, fTimeDelta);
    __super::Update_GameObject(fTimeDelta);
    return NO_EVENT;
}

void CMonster_Soldier::LateUpdate_GameObject(const _float& fTimeDelta)
{
    Set_OnTerrain(fTimeDelta);
    Set_Collider();
    Set_Check_Weapon();
    __super::LateUpdate_GameObject(fTimeDelta);
}

void CMonster_Soldier::Render_GameObject()
{
    if (m_eMonState != INSKILL)
        __super::Render_GameObject();

#ifdef _DEBUG
    if (g_ColiderRender) DebugRender_HitSpheres();
#endif
}

void CMonster_Soldier::Set_Collider()
{
    // 모든 충돌체 OFF면 스킵
    if (!m_pColiderCom || !m_pColiderCom->Is_Active()) {
        Set_Collider_With_Wall();
        return;
    }

    // 플레이어와 멀리 떨어진 경우 충돌 스킵
    CTransform* pPlayerTr = GetPlayerTransform();
    if (pPlayerTr) {
        _vec3 diff = pPlayerTr->Get_Info(INFO_POS) - m_pTransformCom->Get_Info(INFO_POS);
        float dist2 = D3DXVec3LengthSq(&diff);
        if (dist2 > 30.f * 30.f) return;
    }

    m_pColiderCom->Update_ColliderSphere();

    if (m_eMonState != INSKILL)
    {
        if (CColiderManager::GetInstance()->CollisionGroup(
            CColiderManager::COLLISION_PLAYER, this, CColiderManager::COLLISION_SPHERE, nullptr))
        {
            if (CGlobal_Info::Get_Instance()->Get_PlayerInfo().ePlayerState == KICK) {
                SetState(KICKED);
                m_bKillAfterHit = false;
            }
            if (CGlobal_Info::Get_Instance()->Get_PlayerInfo().ePlayerState == ATTACK_INSTANT)
                SetState(INSKILL);
        }

        if (CColiderManager::GetInstance()->CollisionGroup(
            CColiderManager::COLLISION_TILE_ELECTRIC, this, CColiderManager::COLLISION_SPHERE, nullptr))
        {
            if (m_eMonState != HIT_ELECTRIC) SetState(HIT_ELECTRIC);
        }

        if (CColiderManager::GetInstance()->CollisionGroup(
            CColiderManager::COLLISION_TILE_VENT, this, CColiderManager::COLLISION_SPHERE, nullptr))
        {
            if (m_eMonState != HIT_BENT) SetState(HIT_BENT);
        }

        CGameObject* pDoor = nullptr;
        if (CColiderManager::GetInstance()->CollisionGroupSphereTagWho(
            CColiderManager::COLLISION_DOOR,
            this,
            L"Com_Collider_Sphere",        // 몬스터 구면
            L"Com_Collider_Sphere_Open",   // 문(열림) 구면
            nullptr,
            pDoor))
        {
            auto sphereWorldCenter = [](CColider_Sphere* s, CTransform* tr) -> _vec3 {
                if (!s || !tr) return _vec3(0, 0, 0);
                const auto d = s->Get_SphereDesc(); // {fRadius, vOffset}
                return tr->Get_Info(INFO_POS)
                    + tr->Get_Info(INFO_RIGHT) * d.vOffset.x
                    + tr->Get_Info(INFO_UP) * d.vOffset.y
                    + tr->Get_Info(INFO_LOOK) * d.vOffset.z;
                };

            // 내/문(열림) 스피어 & 트랜스폼
            auto* myS = m_pColiderCom;
            auto* myTr = m_pTransformCom;
            auto* openS = pDoor ? dynamic_cast<CColider_Sphere*>(pDoor->Find_Component(L"Com_Collider_Sphere_Open")) : nullptr;
            auto* doorTr = pDoor ? pDoor->GetTransform() : nullptr;

            // 중심점
            const _vec3 myC = sphereWorldCenter(myS, myTr);
            const _vec3 openC = sphereWorldCenter(openS, doorTr);

            // 밀릴 방향: 문 open 중심 -> 몬스터 중심 (수평)
            _vec3 dir = myC - openC; dir.y = 0.f;
            if (D3DXVec3LengthSq(&dir) < 1e-6f) {  // 완전 겹침 대비
                dir = -myTr->Get_Info(INFO_LOOK); dir.y = 0.f;
            }
            D3DXVec3Normalize(&dir, &dir);

            // LOOK을 '밀릴 방향'으로 맞춰두면, 이후 HIT_DOOR 상태에서 그 방향으로 이동시킬 수 있음
            const _vec3 myPos = myTr->Get_Info(INFO_POS);
            myTr->LookAt(myPos + dir);   // 내부에서 Right/Up 재정렬 가정

            // 상태 전환 + 내 구면 비활성(반복 충돌 방지)
            SetState(HIT_DOOR);
            if (m_pColiderCom) m_pColiderCom->Set_Active(false);
            m_bPickable = false;

            return; // 이 프레임은 더 안 건드림
        }
    }

    Set_Collider_With_Wall();
}

void CMonster_Soldier::GetDeathUIConfig(DeathUIConfig& cfg, bool isHeadshot) const
{
    CMonster::GetDeathUIConfig(cfg, isHeadshot);

    cfg.bannerBoxW = 360.f;
    cfg.bannerBoxH = 50.f;
    cfg.rightTextNormal = L"2sec";
    cfg.rightTextHead = L"3sec";

    // 기본값
    cfg.killTextNormal = L"처치";
    cfg.killTextHead = L"처치";

    // 부위 판정(헤드/급소) 우선 적용
    switch (m_lastKillKind)
    {
    case KillKind::Balls: cfg.killTextHead = L"급소";   break;
    case KillKind::Head:  cfg.killTextHead = L"헤드샷"; break;
    default: /* 그대로 */ break;
    }

    // 상태별 문구 적용 함수(스위치)
    auto applyByState = [&](MON_STATE st) -> bool {
        switch (st)
        {
        case HIT_KATANA:
            cfg.killTextNormal = L"벽력일섬";
            cfg.killTextHead = L"벽력일섬";
            return true;

        case HIT_ELECTRIC:
            cfg.killTextNormal = L"찌릿찌릿";
            cfg.killTextHead = L"찌릿찌릿";
            return true;

        case HIT_BENT:
            cfg.killTextNormal = L"갈갈갈";
            cfg.killTextHead = L"갈갈갈";
            return true;

        case HIT_DOOR:
            cfg.killTextNormal = L"문 콕";
            cfg.killTextHead = L"문 콕";
            return true;

        case INSKILL:
            cfg.killTextNormal = L"처형";
            cfg.killTextHead = L"처형";
            return true;
        default:
            return false;
        }
        };
    // 현재 상태 우선, 없으면 이전 상태로 보정
    if (!applyByState(m_eMonState))
        (void)applyByState(m_ePrevState);
}

void CMonster_Soldier::Set_Check_Weapon()
{
    static CGameObject* sWinner = nullptr;
    static DWORD        sStamp = 0;
    const DWORD now = GetTickCount();

    if (sWinner && now - sStamp > 120) {
        sWinner = nullptr;
    }

    if (sWinner && sWinner != this) {
        return;
    }

    // 플레이어 위치 가져와서
    CTransform* pPlayerTr = GetPlayerTransform();
    if (!pPlayerTr) return;

    const _vec3 ref = pPlayerTr->Get_Info(INFO_POS);
    const auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
    const float EPS = 1e-4f;

    // 가장 가까운 애 판별
    float bestDist2 = FLT_MAX;
    CGameObject* bestObj = nullptr;

    for (int i = 0;; ++i)
    {
        CGameObject* pObj = CObjectManager::GetInstance()->Find_Object(sceneIdx, L"Monster_Layer", i);
        if (!pObj) break;

        CMonster* pMon = dynamic_cast<CMonster*>(pObj);
        if (!pMon || pMon->Get_Dead()) continue;

        // 실제로 무기와 겹치는 몬스터만 후보
        if (!CColiderManager::GetInstance()->CollisionGroup(
            CColiderManager::COLLISION_WEAPON, pObj, CColiderManager::COLLISION_SPHERE, nullptr))
            continue;

        CTransform* pTr = pMon->GetTransform();
        if (!pTr) continue;

        const _vec3 pos = pTr->Get_Info(INFO_POS);
        _vec3 diff = pos - ref;
        const float d2 = D3DXVec3LengthSq(&diff);

        if (d2 + EPS < bestDist2) {
            bestDist2 = d2;
            bestObj = pObj;
        }
        else if (fabsf(d2 - bestDist2) <= EPS) {
            // 동점일 때 주소가 더 작은 쪽으로 고정
            const uintptr_t key = reinterpret_cast<uintptr_t>(pObj);
            const uintptr_t bestKey = reinterpret_cast<uintptr_t>(bestObj);
            if (key < bestKey) bestObj = pObj;
        }
    }

    if (bestObj != this) return; // 승자가 아니면 패스 

    sWinner = this;
    sStamp = now;
    QueueDeathUI(false);
    if (auto* p = GetPlayerObj())
        p->Add_Hp(2.f);
    SetState(MON_STATE::HIT_KATANA);
    {
        HeadSpawnArg cfg{};
        cfg.texTag = L"Com_Texture_SuitHead_Kill";
        cfg.protoTag = L"Prototype_Component_Texture_Monster_Suit_Katana_HEAD";
        cfg.endFrame = 37;
        cfg.animSpeed = 16.f;
        cfg.loop = false;

        cfg.fallSpeed = 0.8f;
        cfg.gravity = 3.5f;
        cfg.backDrift = 0.0f;

        const auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
        if (auto* head = dynamic_cast<CMonster_Head*>(
            CObjectManager::GetInstance()->Clone_GameObject(
                L"Prototype_GameObject_Monster_Head", sceneIdx, L"Monster_Layer", &cfg)))
        {
            _vec3 pos = GetHeadWorldPos();
            _vec3 right = m_pTransformCom->Get_Info(INFO_RIGHT);
            _vec3 look = m_pTransformCom->Get_Info(INFO_LOOK);

            right.y = 0.f; if (D3DXVec3LengthSq(&right) > 1e-6f) D3DXVec3Normalize(&right, &right);
            look.y = 0.f; if (D3DXVec3LengthSq(&look) > 1e-6f) D3DXVec3Normalize(&look, &look);

            pos += right * 0.55f;
            pos += look * 0.05f;
            pos.y += 0.12f;

            head->GetTransform()->Set_Info(INFO_POS, pos);
            head->GetTransform()->Set_Scale(1.f, 1.f, 1.f);
        }
    }
}

_bool CMonster_Soldier::Picking(_vec3* PickingPoint)
{
    if (CGlobal_Info::Get_Instance()->Get_PlayerInfo().eWeapon == WEAPON::WP_KATANA)
        return false;
    // CMonster::Picking 에서 죽음/비활성 체크
    CPicking* pk = CPicking::GetInstance();
    _vec3 rayO = pk->GetRayOrigin();
    _vec3 rayD = pk->GetRayDir(); D3DXVec3Normalize(&rayD, &rayD);

    const _matrix& W = *m_pTransformCom->Get_World();
    _vec3 axX(W._11, W._12, W._13), axY(W._21, W._22, W._23), axZ(W._31, W._32, W._33);
    float sMax = max(D3DXVec3Length(&axX), max(D3DXVec3Length(&axY), D3DXVec3Length(&axZ)));

    struct HitRec { HIT_PART part; float t; _vec3 hit; int priority; };
    HitRec best{ HIT_UNKNOWN, FLT_MAX, _vec3(), -999 };

    for (const auto& ps : m_hitSpheres)
    {
        _vec3 cW; D3DXVec3TransformCoord(&cW, &ps.localCenter, &W);
        float rW = ps.radius * sMax;
        float sx = (ps.xScale > 1e-6f) ? ps.xScale : 1.0f;
        _matrix S; D3DXMatrixScaling(&S, 1.0f / sx, 1.0f, 1.0f);

        _vec3 o2, d2, c2;
        D3DXVec3TransformCoord(&o2, &rayO, &S);
        D3DXVec3TransformNormal(&d2, &rayD, &S); D3DXVec3Normalize(&d2, &d2);
        D3DXVec3TransformCoord(&c2, &cW, &S);

        float t;
        if (CPicking::IntersectRaySphere(o2, d2, c2, rW, &t) && t >= 0.f)
        {
            bool better = false;
            if (ps.priority > best.priority) better = true;
            else if (ps.priority == best.priority && t < best.t) better = true;

            if (better) {
                _vec3 hit2 = o2 + d2 * t;
                _matrix invS; D3DXMatrixInverse(&invS, nullptr, &S);
                _vec3 hitW;  D3DXVec3TransformCoord(&hitW, &hit2, &invS);
                best = { ps.part, t, hitW, ps.priority };
            }
        }
    }

    if (best.part == HIT_UNKNOWN) return false;
    if (PickingPoint) *PickingPoint = best.hit;
    m_cachedHitPart = best.part;
    return true;
}

void CMonster_Soldier::HitAt(const _vec3& /*hitPosWorld*/)
{
    HIT_PART part = m_cachedHitPart;
    if (part == HIT_UNKNOWN) part = HIT_BODY;
    m_cachedHitPart = HIT_UNKNOWN;

    const wchar_t* anim = L"Com_Texture_Hit_Body";
    int dmg = 1;
    switch (part) {
    case HIT_HEAD:
    {
        const _vec3 myPos = m_pTransformCom ? m_pTransformCom->Get_Info(INFO_POS) : _vec3(0.f, 0.f, 1.f);
        Spawn_HeadExplosion_Effect(myPos);
        anim = L"Com_Texture_Hit_Head";
        dmg = 2; break;
    }
    case HIT_BALLS:
    {
        const _vec3 myPos = m_pTransformCom ? m_pTransformCom->Get_Info(INFO_POS) : _vec3(0.f, 0.f, 1.f);
        Spawn_Hit_Effect(myPos);
        anim = L"Com_Texture_Hit_Balls";
        dmg = 2; break;
    }
    case HIT_LEG:
    {
        const _vec3 myPos = m_pTransformCom ? m_pTransformCom->Get_Info(INFO_POS) : _vec3(0.f, -2.f, 1.f);
        Spawn_Hit_Effect(myPos);
        anim = L"Com_Texture_Hit_Leg";
        dmg = 1; break;
    }
    case HIT_BODY:
    {
        const _vec3 myPos = m_pTransformCom ? m_pTransformCom->Get_Info(INFO_POS) : _vec3(0.f, 0.f, 1.f);
        Spawn_Hit_Effect(myPos);
        anim = L"Com_Texture_Hit_Body";  dmg = 1; break;
    }
    default: break;
    }

    Change_Texture(anim);
    if (m_pTextureCom) { m_pTextureCom->Set_Zero_Frame(); m_pTextureCom->Resume_Anim(); }
    ApplyDamage(part, dmg);
}

void CMonster_Soldier::ApplyDamage(HIT_PART part, int dmg)
{
    if (m_bDead) return;

    const float prevHp = m_fHp;
    if (CGlobal_Info::Get_Instance()->Get_PlayerInfo().eWeapon == WEAPON::WP_SHOTGUN)
    {
        m_fHp -= 3;
    }
    else
        m_fHp -= dmg;

    const bool lethal = (prevHp > 0.f && m_fHp <= 0.f);
    const bool isHead = (part == HIT_HEAD);
    const bool isBalls = (part == HIT_BALLS);
    const bool headshot = (isHead || isBalls);

    if (lethal)
    {
        // ★ 처치 종류 기록(배너 문구 분기)
        if (isBalls)      m_lastKillKind = KillKind::Balls;
        else if (isHead)  m_lastKillKind = KillKind::Head;
        else              m_lastKillKind = KillKind::Normal;

        // 배너는 죽을 때만 뜸
        QueueDeathUI(headshot);
        if (auto* p = GetPlayerObj())
            p->Add_Hp(headshot ? 2.f : 3.f);
        // 충돌/픽킹 차단
        m_bPickable = false;
        if (m_pColiderCom) m_pColiderCom->Set_Active(false);
        CPickingManager::GetInstance()->Remove_PickingGroup(this);

        // 상태 전환
        if (headshot) {          // 머리/급소 즉사: HIT 진입하면서 1회 배너
            m_bKillAfterHit = true;
            SetState(HIT);
        }
        else {                  // 일반 즉사: DEATH 진입하면서 1회 배너
            m_bKillAfterHit = false;
            SetState(DEATH);
        }
        return;
    }

    // 비치명타: 배너 금지(히트 애니만)
    m_bKillAfterHit = false;
    SetState(HIT);
}

HRESULT CMonster_Soldier::Texture_Clone()
{
    CTexture::TEXINFO info{};
    struct AnimDef { const wchar_t* tag; const wchar_t* proto; int start; int end; float speed; bool loop; };
    AnimDef anims[] = {
        { L"Com_Texture_Idle",      L"Prototype_Component_Texture_Monster_Solider_Idle",   0, 12, 8.f,  true },
        { L"Com_Texture_Chase",     L"Prototype_Component_Texture_Monster_Solider_Walk",  0, 13,10.f,  true },
        { L"Com_Texture_Aim",       L"Prototype_Component_Texture_Monster_Solider_Aim",    0,  9,10.f,  true },
        { L"Com_Texture_Shot",      L"Prototype_Component_Texture_Monster_Solider_Shot",   0,  8, 7.f,  true },
        { L"Com_Texture_Jump",      L"Prototype_Component_Texture_Monster_Solider_Jump",   0, 23,10.f,  true },
        { L"Com_Texture_Hit_Head",  L"Prototype_Component_Texture_Monster_Solider_HEAD",  0, 21, 7.f,true },
        { L"Com_Texture_Hit_Body",  L"Prototype_Component_Texture_Monster_Solider_BODY",  0,  8,10.f,true },
        { L"Com_Texture_Hit_Balls", L"Prototype_Component_Texture_Monster_Solider_BALL",  0, 23,10.f,true },
        { L"Com_Texture_Death",     L"Prototype_Component_Texture_Monster_Solider_DEATH",    0, 23,10.f,true },
        { L"Com_Texture_Hit_Eletric", L"Prototype_Component_Texture_Monster_Solider_ELEC", 0, 15, 7.f,false },
        { L"Com_Texture_Hit_Door",    L"Prototype_Component_Texture_Monster_Solider_DOOR",     0, 25, 7.f,false },
        { L"Com_Texture_Blocking",    L"Prototype_Component_Texture_Monster_Solider_BLOCK",      0,  5, 6.f,false },
        { L"Com_Texture_KatanaDeath",    L"Prototype_Component_Texture_Monster_Solider_Katana_BODY",      0,  21, 6.f,false }
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

void CMonster_Soldier::SetState(MON_STATE next)
{
    m_ePrevState = m_eMonState;
    m_eMonState = next;
    OnEnterState(next);
}

void CMonster_Soldier::OnEnterState(MON_STATE s)
{
    const wchar_t* tag = L"Com_Texture_Idle";

    switch (s)
    {
    case IDLE:  tag = L"Com_Texture_Idle";  break;
    case CHASE: tag = L"Com_Texture_Chase"; break;
    case AIM:   tag = L"Com_Texture_Aim";   break;
    case SHOT:  m_shotTimer = 0.7f; tag = L"Com_Texture_Shot"; break;
    case JUMP:  tag = L"Com_Texture_Jump";  break;
    case HIT_ELECTRIC:
    {
        tag = L"Com_Texture_Hit_Eletric";
        QueueDeathUI(false);
        TrySpawnDeathUI_Common();
        const _vec3 myPos = m_pTransformCom ? m_pTransformCom->Get_Info(INFO_POS) : _vec3(0.f, -5.f, 0.f);
        Spawn_Eletric_Effect(myPos);
    }
        break;

    case HIT_BENT:
    {
        QueueDeathUI(false);
        TrySpawnDeathUI_Common();
        const _vec3 myPos = m_pTransformCom ? m_pTransformCom->Get_Info(INFO_POS) : _vec3(0.f, -5.f, 0.f);
        Spawn_Hit_Vent(myPos);
    }
        break;

    case HIT_DOOR:
        tag = L"Com_Texture_Hit_Door";
        if (m_pColiderCom) m_pColiderCom->Set_Active(false);
        m_bPickable = false;
        QueueDeathUI(false);
        TrySpawnDeathUI_Common();
        break;

    case HIT:
        if (m_bKillAfterHit) {
            DisableAllCollisionAndPicking();
            TrySpawnDeathUI_Common();
        }
        if (m_pTextureCom) { m_pTextureCom->Set_Zero_Frame(); m_pTextureCom->Resume_Anim(); }
        return;
    case HIT_KATANA:
        tag = L"Com_Texture_KatanaDeath";
        if (m_pColiderCom) m_pColiderCom->Set_Active(false);
        TrySpawnDeathUI_Common();
        break;

    case KICKED:
        tag = L"Com_Texture_Blocking"; break;

    case INSKILL:

        QueueDeathUI(false);
        TrySpawnDeathUI_Common();
        break;

    case DEATH:
        DisableAllCollisionAndPicking();
        tag = L"Com_Texture_Death";
        break;
    }

    Change_Texture(tag);
    if (m_pTextureCom) { m_pTextureCom->Set_Zero_Frame(); m_pTextureCom->Resume_Anim(); }

    if (s == DEATH) {
        TrySpawnDeathUI_Common(); // 일반 사망 시점 1회 배너
    }
}

void CMonster_Soldier::OnUpdateState(MON_STATE s, const _float& dt)
{
    if (!m_pTextureCom) return;
    GetPlayerTransform();
    const float dist = DistanceToPlayer();

    switch (s)
    {
    case IDLE:
        if (dist <= m_fChaseRadius) SetState(CHASE);
        break;

    case CHASE:
        if (!m_pPlayerTr) break;
        if (dist <= m_fAimRadius) { SetState(AIM); break; }
        if (dist > m_fLoseRadius) { SetState(IDLE); break; }
        {
            _vec3 my = m_pTransformCom->Get_Info(INFO_POS);
            _vec3 pl = m_pPlayerTr->Get_Info(INFO_POS);
            _vec3 dir = pl - my;
            m_pTransformCom->LookAt(pl);
            m_pTransformCom->Move_PosDir(dt, dir);
        }
        break;

    case AIM:
        m_jumpCD -= dt;
        if (m_jumpCD <= 0.f) {
            m_jumpDir = (rand() & 1) ? +1 : -1;
            SetState(JUMP); break;
        }
        if (dist > m_fChaseRadius) { SetState(CHASE); break; }
        if (m_pPlayerTr) m_pTransformCom->LookAt(m_pPlayerTr->Get_Info(INFO_POS));
        if (m_pTextureCom->Is_AnimFinished()) SetState(SHOT);
        break;

    case SHOT:
    {
        if (dist > m_fAimRadius) { SetState(CHASE); break; }
        if (m_pPlayerTr) m_pTransformCom->LookAt(m_pPlayerTr->Get_Info(INFO_POS));

        m_shotTimer += dt;
        if (m_pTextureCom->Is_AnimFinished())
            m_pTextureCom->Stop_Anim();

        if (m_shotTimer >= 0.7f) {
            m_shotTimer = 0.f;
            m_pTextureCom->Set_Zero_Frame();
            m_pTextureCom->Resume_Anim();

            BulletData tData;
            tData.vMuzzlePosition = m_pTransformCom->Get_Info(INFO_POS);
            tData.vLookDir = m_pTransformCom->Get_Info(INFO_LOOK);
            D3DXVec3Normalize(&tData.vLookDir, &tData.vLookDir);
            tData.vMuzzlePosition += tData.vLookDir * 1.2f;
            tData.vMuzzlePosition.y += 0.14f;

            _vec3 vDir = tData.vLookDir;
            vDir.y -= 0.03f;
            D3DXVec3Normalize(&vDir, &vDir);
            tData.vLookDir = vDir;
            CObjectPoolManager::GetInstance()->Spawn(PoolType::BULLET, &tData);
        }
    }
    break;

    case HIT:
        if (m_pTextureCom->Is_AnimFinished()) {
            if (m_bKillAfterHit) m_bDead = true;
            else SetState((m_ePrevState == DEATH) ? DEATH : IDLE);
        }
        break;

    case HIT_ELECTRIC:
    case HIT_BENT:
    case HIT_KATANA:
        if (m_pTextureCom->Is_AnimFinished()) m_bDead = true;
        break;
    case HIT_DOOR:
    {
        // 누적 시간
        m_kbTime += dt;

        const float maxDist = 1.f;   // 밀려날 거리
        const float knockTime = 0.9f;   // 밀리는 시간 

        float t = min(1.f, m_kbTime / knockTime);
        float ratio = 1.f - (1.f - t) * (1.f - t);
        float targetDist = maxDist * ratio;

        float delta = targetDist - m_kbProgress;

        if (delta > 0.f) {
            _vec3 dir = m_pTransformCom->Get_Info(INFO_LOOK);
            dir.y = 0.f;
            D3DXVec3Normalize(&dir, &dir);
            m_pTransformCom->Move_PosDir(delta, dir);
            m_kbProgress += delta;
        }

        if (m_pTextureCom->Is_AnimFinished())
            m_bDead = true;
    }
    break;

    case KICKED:
        if (m_pTextureCom->Is_AnimFinished()) {
            if (m_bKillAfterHit) m_bDead = true;
            else SetState((m_ePrevState == DEATH) ? DEATH : IDLE);
        }
        else {
            m_pTransformCom->Move_PosDir(dt * 3.f, (m_pTransformCom->Get_Info(INFO_LOOK)));
        }
        break;

    case INSKILL:
        if (CGlobal_Info::Get_Instance()->Get_PlayerInfo().ePlayerState != ATTACK_INSTANT)
            m_bDead = true;
        break;

    case JUMP:
    {
        const _matrix& W = *m_pTransformCom->Get_World();
        _vec3 right(W._11, W._12, W._13); right.y = 0.f; D3DXVec3Normalize(&right, &right);
        if (m_jumpDir < 0) right = -right;
        m_pTransformCom->Move_PosDir(dt * 0.25f, right);

        if (m_pTextureCom->Is_AnimFinished()) {
            m_jumpCD = 5.f;
            if (dist <= m_fAimRadius)        SetState(AIM);
            else if (dist <= m_fChaseRadius) SetState(CHASE);
            else                              SetState(IDLE);
        }
    }
    break;

    case DEATH:
        m_pTransformCom->Move_Forward(dt * 0.1f);
        if (m_pTextureCom->Is_AnimFinished()) m_bDead = true;
        break;

    default: break;
    }
}

void CMonster_Soldier::SetupHitSpheres()
{
    m_hitSpheres.clear();
    const int PRI_LEG = 0, PRI_BODY = 1, PRI_BALLS = 2, PRI_HEAD = 3;

    m_hitSpheres.push_back({ HIT_HEAD,  _vec3(0.00f,  0.38f, 0.0f), 0.10f, PRI_HEAD,  0.50f });
    m_hitSpheres.push_back({ HIT_BODY,  _vec3(0.00f,  0.08f, 0.0f), 0.28f, PRI_BODY,  0.50f });
    m_hitSpheres.push_back({ HIT_BALLS, _vec3(0.00f, -0.07f, 0.0f), 0.06f, PRI_BALLS, 0.20f });
    m_hitSpheres.push_back({ HIT_LEG,   _vec3(0.00f, -0.20f,0.0f), 0.22f, PRI_LEG,   0.90f });
}

_vec3 CMonster_Soldier::GetHeadWorldPos() const
{
    float headOffsetY = 0.9f;
    if (m_pColiderCom) headOffsetY = m_pColiderCom->Get_SphereDesc().fRadius;
    else headOffsetY = m_pTransformCom->Get_Scale().y * 1.0f;

    _vec3 pos = m_pTransformCom->Get_Info(INFO_POS);
    pos.y += headOffsetY;
    return pos;
}

bool CMonster_Soldier::WorldToScreen(const _vec3& world, float& sx, float& sy) const
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

#ifdef _DEBUG
void CMonster_Soldier::DebugRender_HitSpheres() const
{
    if (!m_pGraphicDev || m_hitSpheres.empty()) return;
    EnsureDebugSphereMesh(m_pGraphicDev);
    if (!g_pDebugSphereMesh) return;

    const _matrix& W = *m_pTransformCom->Get_World();
    _vec3 axX(W._11, W._12, W._13), axY(W._21, W._22, W._23), axZ(W._31, W._32, W._33);
    float sMax = max(D3DXVec3Length(&axX), max(D3DXVec3Length(&axY), D3DXVec3Length(&axZ)));

    DWORD oldFill = 0, oldCull = 0, oldLight = 0, oldZ = 0, oldTF = 0;
    DWORD oldColorOp = 0, oldColorArg1 = 0;
    m_pGraphicDev->GetRenderState(D3DRS_FILLMODE, &oldFill);
    m_pGraphicDev->GetRenderState(D3DRS_CULLMODE, &oldCull);
    m_pGraphicDev->GetRenderState(D3DRS_LIGHTING, &oldLight);
    m_pGraphicDev->GetRenderState(D3DRS_ZENABLE, &oldZ);
    m_pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &oldTF);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &oldColorOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG1, &oldColorArg1);

    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    m_pGraphicDev->SetTexture(0, nullptr);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);

    for (const auto& ps : m_hitSpheres)
    {
        _vec3 cW; D3DXVec3TransformCoord(&cW, &ps.localCenter, &W);
        float rW = max(0.0001f, ps.radius * sMax);
        float sx = (ps.xScale > 1e-6f) ? ps.xScale : 1.0f;

        _matrix S, T, M;
        D3DXMatrixScaling(&S, rW * sx, rW, rW);
        D3DXMatrixTranslation(&T, cW.x, cW.y, cW.z);
        M = S * T;

        m_pGraphicDev->SetTransform(D3DTS_WORLD, &M);
        m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, PartColor(ps.part));
        g_pDebugSphereMesh->DrawSubset(0);
    }

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, oldColorOp);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, oldColorArg1);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, oldTF);
    m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, oldFill);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, oldCull);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, oldLight);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, oldZ);
}
#endif

CMonster_Soldier* CMonster_Soldier::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMonster_Soldier* pInstance = new CMonster_Soldier(pGraphicDev);
    if (FAILED(pInstance->Ready_GameObject()))
    {
        MSG_BOX("CMonster_Soldier Create Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CMonster_Soldier::Clone(void* pArg)
{
    CMonster_Soldier* pInstance = new CMonster_Soldier(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CMonster_Soldier Clone Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CMonster_Soldier::Free()
{
    __super::Free();
}