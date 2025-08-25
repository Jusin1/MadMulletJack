#include "pch.h"
#include "CMonster_Suit.h"
#include "CColiderManager.h"
#include "CComponentMgr.h"
#include "CObjectManager.h"
#include "CEffectUI.h"
#include "CPicking.h"
#include "CGlobal_Info.h"
#include "CManagement.h"

#ifdef _DEBUG
namespace {
    static LPD3DXMESH g_pDebugSphereMesh = nullptr;

    static void EnsureDebugSphereMesh(LPDIRECT3DDEVICE9 dev)
    {
        if (!g_pDebugSphereMesh)
            D3DXCreateSphere(dev, 1.f, 16, 16, &g_pDebugSphereMesh, nullptr);
    }

    static D3DCOLOR PartColor(CMonster_Suit::HIT_PART p)
    {
        switch (p)
        {
        case CMonster_Suit::HIT_HEAD:  return D3DCOLOR_ARGB(255, 255, 80, 80);
        case CMonster_Suit::HIT_BALLS: return D3DCOLOR_ARGB(255, 255, 220, 80);
        case CMonster_Suit::HIT_LEG:   return D3DCOLOR_ARGB(255, 80, 150, 255);
        case CMonster_Suit::HIT_BODY:  return D3DCOLOR_ARGB(255, 120, 255, 120);
        default:                       return D3DCOLOR_ARGB(255, 200, 200, 200);
        }
    }
}
#endif

// 정적 콤보 변수 정의
ULONGLONG CMonster_Suit::s_lastKillTimeMs = 0;
int       CMonster_Suit::s_comboCount = 0;

CMonster_Suit::CMonster_Suit(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev, MonsterType::SUIT)
    , m_eMonState(IDLE), m_ePrevState(IDLE)
    , m_pPlayerTr(nullptr)
    , m_fChaseRadius(12.f), m_fAimRadius(6.f), m_fLoseRadius(16.f)
    , m_jumpCD(0.f), m_jumpDir(0), m_bKillAfterHit(false)
{
}

CMonster_Suit::CMonster_Suit(const CMonster_Suit& rhs)
    : CMonster(rhs)
    , m_eMonState(rhs.m_eMonState), m_ePrevState(rhs.m_ePrevState)
    , m_pPlayerTr(nullptr)
    , m_fChaseRadius(rhs.m_fChaseRadius), m_fAimRadius(rhs.m_fAimRadius), m_fLoseRadius(rhs.m_fLoseRadius)
    , m_jumpCD(rhs.m_jumpCD), m_jumpDir(rhs.m_jumpDir), m_bKillAfterHit(rhs.m_bKillAfterHit)
{
}

CMonster_Suit::~CMonster_Suit() {}

HRESULT CMonster_Suit::Ready_GameObject()
{
    if (FAILED(__super::Ready_GameObject())) return E_FAIL;
    return S_OK;
}

HRESULT CMonster_Suit::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg))) return E_FAIL;
    
    m_fHp = 2;

    CTransform::TRANSFORMINFO TransformInfo{};
    TransformInfo.fSpeed = 5.f;
    TransformInfo.fRotationSpeed = D3DXToRadian(90.f);

    m_pTransformCom->SetTransformInfo(TransformInfo);
    m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.f);

    GetPlayerTransform();

    m_jumpCD = 1.f + (rand() % 2001) / 1000.f;
    SetState(IDLE);

    SetupHitSpheres();

    return S_OK;
}

_int CMonster_Suit::Update_GameObject(const _float& fTimeDelta)
{
    if (m_bDead) return DEAD;
    OnUpdateState(m_eMonState, fTimeDelta);
    __super::Update_GameObject(fTimeDelta);
    return NO_EVENT;
}

void CMonster_Suit::LateUpdate_GameObject(const _float& fTimeDelta)
{
    Set_OnTerrain(fTimeDelta);
    Set_Collider();
    __super::LateUpdate_GameObject(fTimeDelta);
}

void CMonster_Suit::Render_GameObject()
{
    if (m_eMonState != INSKILL)
    {
        __super::Render_GameObject();
    }

#ifdef _DEBUG
    if (g_ColiderRender)
        DebugRender_HitSpheres();
#endif
}

void CMonster_Suit::Set_Collider()
{
    _vec3 vDistance;

    if (m_pColiderCom)
        m_pColiderCom->Update_ColliderSphere();

    if (m_eMonState != INSKILL) // inskill일때는 colli를 하지 않음
    {
        if (CColiderManager::GetInstance()->CollisionGroup(
            CColiderManager::COLLISION_PLAYER, this,
            CColiderManager::COLLISION_SPHERE, nullptr))
        {
            _vec3 vPosition = m_pTransformCom->Get_Info(INFO_POS);
            (void)vPosition;

            if (CGlobal_Info::Get_Instance()->Get_PlayerInfo().ePlayerState == KICK)
            {
                SetState(KICKED);
                m_bKillAfterHit = false;
            }

            if (CGlobal_Info::Get_Instance()->Get_PlayerInfo().ePlayerState == ATTACK_INSTANT)
            {
                SetState(INSKILL);
            }
        }

        if (CColiderManager::GetInstance()->CollisionGroup(
            CColiderManager::COLLISION_TILE_ELECTRIC, this,
            CColiderManager::COLLISION_SPHERE, nullptr))
        {
            SetState(HIT_ELECTRIC);
            m_pColiderCom->Set_Active(false);
        }
    }
    

    Set_Collider_With_Wall();
}

_bool CMonster_Suit::Picking(_vec3* PickingPoint)
{
    CPicking* pk = CPicking::GetInstance();
    _vec3 rayO = pk->GetRayOrigin();
    _vec3 rayD = pk->GetRayDir();
    D3DXVec3Normalize(&rayD, &rayD);

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
            if (ps.priority > best.priority) {
                better = true;
            }
            else if (ps.priority == best.priority && t < best.t) {
                better = true;
            }

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

void CMonster_Suit::HitAt(const _vec3& hitPosWorld)
{
    HIT_PART part = m_cachedHitPart;
    if (part == HIT_UNKNOWN) part = HIT_BODY;
    m_cachedHitPart = HIT_UNKNOWN;

    const wchar_t* anim = L"Com_Texture_Hit_Body";
    int dmg = 1;
    switch (part) {
    case HIT_HEAD:  anim = L"Com_Texture_Hit_Head";  dmg = 2; break;
    case HIT_BALLS: anim = L"Com_Texture_Hit_Balls"; dmg = 2; break;
    case HIT_LEG:   anim = L"Com_Texture_Hit_Leg";   dmg = 1; break;
    case HIT_BODY:  anim = L"Com_Texture_Hit_Body";  dmg = 1; break;
    default: break;
    }

    Change_Texture(anim);
    if (m_pTextureCom) { m_pTextureCom->Set_Zero_Frame(); m_pTextureCom->Resume_Anim(); }
    ApplyDamage(part, dmg);
}

void CMonster_Suit::ApplyDamage(HIT_PART part, int dmg)
{
    float prevHp = m_fHp;
    m_fHp -= dmg;

    if (prevHp > 0.f && m_fHp <= 0.f) {
        m_lastFatalPart = part;
        m_pendingDeathUI = true;
    }

    if (m_fHp <= 0) {
        if (part == HIT_HEAD || part == HIT_BALLS) {
            m_bKillAfterHit = true;
            SetState(HIT);
        }
        else {
            m_bKillAfterHit = false;
            SetState(DEATH);
        }
    }
    else {
        m_bKillAfterHit = false;
        SetState(HIT);
    }
}

HRESULT CMonster_Suit::Texture_Clone()
{
    CTexture::TEXINFO info{};

    struct AnimDef { const wchar_t* tag; const wchar_t* proto; int start; int end; float speed; bool loop; };
    AnimDef anims[] = {
        { L"Com_Texture_Idle",      L"Prototype_Component_Texture_Monster_Suit_Idle",   0, 12, 8.f,  true },
        { L"Com_Texture_Chase",     L"Prototype_Component_Texture_Monster_Suit_Chase",  0, 13, 10.f, true },
        { L"Com_Texture_Aim",       L"Prototype_Component_Texture_Monster_Suit_Aim",    0,  9, 10.f, true },
        { L"Com_Texture_Shot",      L"Prototype_Component_Texture_Monster_Suit_Shot",   0,  8, 7.f, true },
        { L"Com_Texture_Jump",      L"Prototype_Component_Texture_Monster_Suit_Jump",   0, 22, 10.f, true },
        { L"Com_Texture_Hit_Head",  L"Prototype_Component_Texture_Monster_Suit_HIT_HEAD",  0, 21, 7.f, true },
        { L"Com_Texture_Hit_Body",  L"Prototype_Component_Texture_Monster_Suit_HIT_BODY",  0,  8, 10.f, true },
        { L"Com_Texture_Hit_Balls", L"Prototype_Component_Texture_Monster_Suit_HIT_BALL",  0, 23, 10.f, true },
        { L"Com_Texture_Death",     L"Prototype_Component_Texture_Monster_Suit_DEATH1",   0, 21, 10.f, true },
        { L"Com_Texture_Hit_Eletric",     L"Prototype_Component_Texture_Monster_Suit_HIT_ELECTRIC",   0, 15, 10.f, false },
        {L"Com_Texture_Blocking",   L"Prototype_Component_Texture_Monster_Suit_Blocking",   0,4,10.f,false}
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

void CMonster_Suit::SetState(MON_STATE next)
{
    m_ePrevState = m_eMonState;
    m_eMonState = next;
    OnEnterState(next);
}

void CMonster_Suit::OnEnterState(MON_STATE s)
{
    const wchar_t* tag = L"Com_Texture_Idle";

    switch (s)
    {
    case IDLE:  tag = L"Com_Texture_Idle";  break;
    case CHASE: tag = L"Com_Texture_Chase"; break;
    case AIM:   tag = L"Com_Texture_Aim";   break;
    case SHOT:  tag = L"Com_Texture_Shot";  break;
    case JUMP:  tag = L"Com_Texture_Jump";  break;
    case HIT_ELECTRIC: tag = L"Com_Texture_Hit_Eletric"; break;

    case HIT:
        if (m_bKillAfterHit) {
            TrySpawnDeathUI();
        }
        if (m_pTextureCom) {
            m_pTextureCom->Set_Zero_Frame();
            m_pTextureCom->Resume_Anim();
        }
        return;

    case KICKED:
        tag = L"Com_Texture_Blocking";
        break;

    case INSKILL:
        break;

    case DEATH:
        tag = L"Com_Texture_Death";
        break;
    }

    Change_Texture(tag);

    if (m_pTextureCom) {
        m_pTextureCom->Set_Zero_Frame();
        m_pTextureCom->Resume_Anim();
    }

    if (s == DEATH) {
        TrySpawnDeathUI();
    }
}

void CMonster_Suit::OnUpdateState(MON_STATE s, const _float& dt)
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
            SetState(JUMP);
            break;
        }
        if (dist > m_fChaseRadius) { SetState(CHASE); break; }
        if (m_pPlayerTr) m_pTransformCom->LookAt(m_pPlayerTr->Get_Info(INFO_POS));
        if (m_pTextureCom->Is_AnimFinished()) SetState(SHOT);
        break;

    case SHOT:
        break;

    case HIT:
        if (m_pTextureCom->Is_AnimFinished()) {
            if (m_bKillAfterHit) m_bDead = true;
            else SetState((m_ePrevState == DEATH) ? DEATH : IDLE);
        }
        break;

    case HIT_ELECTRIC:
        
        if (m_pTextureCom->Is_AnimFinished()) {
            m_bDead = true;
        }
        break;
    case KICKED:
        if (m_pTextureCom->Is_AnimFinished()) { // 애니메이션 끝나면
            if (m_bKillAfterHit) m_bDead = true;
            else SetState((m_ePrevState == DEATH) ? DEATH : IDLE);
        }
        else
        {
            // 뒤로 날아가
            m_pTransformCom->Move_PosDir(dt * 3.f, (m_pTransformCom->Get_Info(INFO_LOOK)));
        }
        break;

    case INSKILL:
    {
        // 만약 attack instant 가 끝나면
        if (CGlobal_Info::Get_Instance()->Get_PlayerInfo().ePlayerState != ATTACK_INSTANT)
            // 죽음 처리
        {
            m_bDead = true;
        } 
    }
        break;

    case JUMP:
    {
        const _matrix& W = *m_pTransformCom->Get_World();
        _vec3 right(W._11, W._12, W._13);
        right.y = 0.f;
        D3DXVec3Normalize(&right, &right);
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

CTransform* CMonster_Suit::GetPlayerTransform()
{

    auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
    if (!m_pPlayerTr)
    {
        m_pPlayerTr = dynamic_cast<CTransform*>(
            CObjectManager::GetInstance()->Get_Component(sceneIdx, L"Player_Layer", L"Com_Transform", 0));
    }
    return m_pPlayerTr;
}

float CMonster_Suit::DistanceToPlayer() const
{
    if (!m_pPlayerTr) return FLT_MAX;
    _vec3 my = m_pTransformCom->Get_Info(INFO_POS);
    _vec3 pl = m_pPlayerTr->Get_Info(INFO_POS);
    _vec3 diff = pl - my;
    return D3DXVec3Length(&diff);
}

void CMonster_Suit::SetupHitSpheres()
{
    m_hitSpheres.clear();

    const int PRI_LEG = 0;
    const int PRI_BODY = 1;
    const int PRI_BALLS = 2; 
    const int PRI_HEAD = 3;

    //                         part                pos                radius   우선순위   x크기
    m_hitSpheres.push_back({ HIT_HEAD,  _vec3(0.00f,  0.38f,  0.0f), 0.10f, PRI_HEAD,  0.50f });
    m_hitSpheres.push_back({ HIT_BODY,  _vec3(0.00f,  0.08f,  0.00f), 0.28f, PRI_BODY,  0.50f });
    m_hitSpheres.push_back({ HIT_BALLS, _vec3(0.00f, -0.07f,  0.0f), 0.06f, PRI_BALLS, 0.2f });
    m_hitSpheres.push_back({ HIT_LEG,   _vec3(0.0f, -0.2f, 0.0f), 0.22f, PRI_LEG,   0.90f });
}

void CMonster_Suit::TrySpawnDeathUI()
{
    if (!m_pendingDeathUI) return;
    m_pendingDeathUI = false;

    const bool  isHead = (m_lastFatalPart == HIT_HEAD);

    const float secsAdd = isHead ? 3.0f : 2.0f;

    if (auto ui = dynamic_cast<CEffectUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_MonsterHitEffectUI", SCENE_STATIC, L"UI_Layer")))
    {
        ui->SetImageSize(40.f, 40.f);
        ui->SetBoxSize(200.f, 50.f);
        ui->Change_Texture(L"Com_Tex_Heal");

        ui->SetNumberEmphasis(L"23", 1.0f);

        ui->ShowFollowTransform(
            isHead ? L"3sec" : L"2sec",
            L"Com_Tex_Heal",
            secsAdd,
            m_pTransformCom, 
            0.9f,            
            240.f,           
            120.f,           
            0.85f,
            L"Font_UI_Effect",
            D3DXCOLOR(1, 1, 1, 1));
    }

    if (auto banner = dynamic_cast<CEffectUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_MonsterHitEffectUI", SCENE_STATIC, L"UI_Layer")))
    {
        banner->Change_Texture(L"Com_Tex_Heal");
        banner->SetImageSize(40.f, 40.f);
        banner->SetBannerShowIcon(true);
        banner->SetImageOffset(27.f, 5.f);
        
        banner->SetBannerRightText(isHead ? L"3sec" : L"2sec");
        banner->SetNumberEmphasis(L"23", 1.35f);
        banner->SetBannerRightFixedScale(1.0f); 
        banner->SetBannerTextOffset(30.f, 5.f);

        banner->SetBannerLabelPop(1.35f, 0.22f);

        banner->SetBannerAngle(0.f);
        banner->LinkBannerTextAngleToBanner(true); 
        banner->SetBannerTextAngle(0.f);


        banner->SetBannerDownSpeed(130.f);

        banner->SetBannerExtraWidth(80.f);
        banner->ShowBanner(
            isHead ? L"헤드샷" : L"처치",
            1.10f,           
            175.f, 180.f,
            1.4f, 1.0f,
            L"Font_UI_Effect",
            D3DXCOLOR(1, 1, 1, 1),
            0.85f,
            4.f);
    }


    {
        const ULONGLONG now = GetTickCount64();
        if (now - s_lastKillTimeMs <= 1500) ++s_comboCount;
        else                                s_comboCount = 1;
        s_lastKillTimeMs = now;

        if (s_comboCount >= 2)
        {
            if (auto left = dynamic_cast<CEffectUI*>(
                CObjectManager::GetInstance()->Clone_GameObject(
                    L"Prototype_GameObject_MonsterHitEffectUI", SCENE_STATIC, L"UI_Layer")))
            {
                left->SetBannerExtraWidth(20.f);
                wchar_t buf[16]; swprintf(buf, 16, L"X%d", s_comboCount);
                left->ShowBanner(
                    buf,
                    1.00f,
                    60.f, 340.f,     
                    3.f, 1.3f,
                    L"Font_UI_Effect",        
                    D3DXCOLOR(1, 1, 1, 1),
                    0.85f,
                    -8.f);
                left->SetBannerTextAngle(10.f);
            }

            // 오른쪽 "COMBO +1 sec"
            if (auto right = dynamic_cast<CEffectUI*>(
                CObjectManager::GetInstance()->Clone_GameObject(
                    L"Prototype_GameObject_MonsterHitEffectUI", SCENE_STATIC, L"UI_Layer")))
            {
                right->SetBannerExtraWidth(40.f);
                right->ShowBanner(
                    L"COMBO",
                    1.00f,
                    260.f, 370.f,  
                    1.2f, 1.0f,
                    L"Font_UI_Effect",
                    D3DXCOLOR(1, 1, 1, 1),
                    0.85f,
                    -8.f);
                right->SetBannerTextOffset(20.f, -10.f);
                right->SetBannerTextAngle(3.f);
            }
        }
    }
}

_vec3 CMonster_Suit::GetHeadWorldPos() const
{
    float headOffsetY = 0.9f;
    if (m_pColiderCom) headOffsetY = m_pColiderCom->Get_SphereDesc().fRadius;
    else headOffsetY = m_pTransformCom->Get_Scale().y * 1.0f;

    _vec3 pos = m_pTransformCom->Get_Info(INFO_POS);
    pos.y += headOffsetY;
    return pos;
}

bool CMonster_Suit::WorldToScreen(const _vec3& world, float& sx, float& sy) const
{
    D3DXMATRIX view, proj, id;
    D3DVIEWPORT9 vp{};
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

void CMonster_Suit::DebugRender_HitSpheres() const
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

CMonster_Suit* CMonster_Suit::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMonster_Suit* pInstance = new CMonster_Suit(pGraphicDev);
    if (FAILED(pInstance->Ready_GameObject()))
    {
        MSG_BOX("CMonster_Suit Create Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CMonster_Suit::Clone(void* pArg)
{
    CMonster_Suit* pInstance = new CMonster_Suit(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CMonster_Suit Clone Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CMonster_Suit::Free()
{
    __super::Free();
}