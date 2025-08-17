#include "pch.h"
#include "CMonster_Suit.h"
#include "CColiderManager.h"
#include "CComponentMgr.h"
#include "CObjectManager.h"
#include "CEffectUI.h"
#include "CPicking.h"
#include "CGlobal_Info.h"

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



CMonster_Suit::CMonster_Suit(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
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
    TransformInfo.vStartPos = _vec3(4.f, 0.f, 0.f);

    m_pTransformCom->SetTransformInfo(TransformInfo);
    m_pTransformCom->Set_Info(INFO_POS, _vec3(4.f, 1.f, 0.f));
    m_pTransformCom->Set_Scale(2.f, 2.f, 2.f);

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
    Set_Collider();
    __super::LateUpdate_GameObject(fTimeDelta);
}

void CMonster_Suit::Render_GameObject()
{
    __super::Render_GameObject();

#ifdef _DEBUG  // 충돌체 랜더링 일부로 본체 구는 안했음 (원래 존재)
    if (g_ColiderRender)
        DebugRender_HitSpheres(); 
#endif
}

void CMonster_Suit::Set_Collider() // 콜라이더 설정
{
    if (m_pColiderCom)
        m_pColiderCom->Update_ColliderSphere();

    if (CColiderManager::GetInstance()->CollisionGroup(
        CColiderManager::COLLISION_PLAYER, this,
        CColiderManager::COLLISION_SPHERE, nullptr))
    {
        _vec3 vPosition = m_pTransformCom->Get_Info(INFO_POS);
        (void)vPosition;

        if (CGlobal_Info::Get_Instance()->Get_PlayerInfo().ePlayerState == DASH_ATTACK)
        {
            // 임시방편 -> 살짝 뒤로 보내기
            m_pTransformCom->Move_PosDown(0.1);
        }
    }
}

_bool CMonster_Suit::Picking(_vec3* PickingPoint) // 픽킹 구충돌로 수정 -> 본체구는 픽킹처리안함 본체구는 밀어내기 용도로만
{
    CPicking* pk = CPicking::GetInstance();
    _vec3 rayOrigin = pk->GetRayOrigin();
    _vec3 rayDir = pk->GetRayDir();

    D3DXVec3Normalize(&rayDir, &rayDir);

    const _matrix& W = *m_pTransformCom->Get_World();
    _vec3 axX(W._11, W._12, W._13), axY(W._21, W._22, W._23), axZ(W._31, W._32, W._33);
    float sMax = max(D3DXVec3Length(&axX), max(D3DXVec3Length(&axY), D3DXVec3Length(&axZ)));

    struct HitRec { HIT_PART part; float t; _vec3 hit; int priority; };
    HitRec best{ HIT_UNKNOWN, FLT_MAX, _vec3(), -999 };

    for (const auto& pSphere : m_hitSpheres)
    {
        _vec3 cW;
        D3DXVec3TransformCoord(&cW, &pSphere.localCenter, &W);
        float rW = pSphere.radius * sMax;

        float t;
        if (CPicking::IntersectRaySphere(rayOrigin, rayDir, cW, rW, &t))
        {
            if (t < 0.f) continue;

            bool better = (t < best.t);
            const float EPS = 0.015f;
            if (!better && fabsf(t - best.t) <= EPS)
                better = (pSphere.priority > best.priority);

            if (better) best = { pSphere.part, t, rayOrigin + rayDir * t, pSphere.priority };
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

    if (part == HIT_UNKNOWN)
        part = HIT_BODY;

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


void CMonster_Suit::ApplyDamage(HIT_PART part, int dmg) // 부위별 데미지 계산
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


HRESULT CMonster_Suit::Texture_Clone() // 애니메이션 설정
{
    CTexture::TEXINFO info{};

    struct AnimDef { const wchar_t* tag; const wchar_t* proto; int start; int end; float speed; bool loop; };
    AnimDef anims[] = {
        { L"Com_Texture_Idle",      L"Prototype_Component_Texture_Monster_Suit_Idle",   0, 12,  13.f,  true },
        { L"Com_Texture_Chase",     L"Prototype_Component_Texture_Monster_Suit_Chase",  0, 13, 13.f,  true },
        { L"Com_Texture_Aim",       L"Prototype_Component_Texture_Monster_Suit_Aim",    0,  9, 13.f,  true },
        { L"Com_Texture_Shot",      L"Prototype_Component_Texture_Monster_Suit_Shot",   0,  8, 13.f,  true },
        { L"Com_Texture_Jump",      L"Prototype_Component_Texture_Monster_Suit_Jump",   0, 22, 17.f,  true },
        { L"Com_Texture_Hit_Head",  L"Prototype_Component_Texture_Monster_Suit_HIT_HEAD",  0, 21, 13.f, true },
        { L"Com_Texture_Hit_Body",  L"Prototype_Component_Texture_Monster_Suit_HIT_BODY",  0,  8, 13.f, true },
        { L"Com_Texture_Hit_Balls", L"Prototype_Component_Texture_Monster_Suit_HIT_BALL",  0, 23, 13.f, true },
        { L"Com_Texture_Death",     L"Prototype_Component_Texture_Monster_Suit_DEATH1",   0, 21, 13.f, true },
    };

    for (auto& a : anims)
    {
        ZeroMemory(&info, sizeof(info));
        info.m_iStart = a.start;
        info.m_iEndTex = a.end;
        info.m_fSpeed = a.speed;
        info.m_bLoop = a.loop;

        if (FAILED(Add_Components(a.tag, SCENE_STAGE, a.proto, (CComponent**)&m_pTextureCom, &info)))
            return E_FAIL;

        m_mapTexture.insert({ a.tag, m_pTextureCom });
    }

    return S_OK;
}

void CMonster_Suit::SetState(MON_STATE next) // 상태 설정
{
    m_ePrevState = m_eMonState;
    m_eMonState = next;
    OnEnterState(next);
}

void CMonster_Suit::OnEnterState(MON_STATE s) // 상태 애니메이션 처리
{
    const wchar_t* tag = L"Com_Texture_Idle";

    switch (s)
    {
    case IDLE:  tag = L"Com_Texture_Idle";  break;
    case CHASE: tag = L"Com_Texture_Chase"; break;
    case AIM:   tag = L"Com_Texture_Aim";   break;
    case SHOT:  tag = L"Com_Texture_Shot";  break;
    case JUMP:  tag = L"Com_Texture_Jump";  break;

    case HIT:
        if (m_bKillAfterHit) {
            TrySpawnDeathUI();
        }
        if (m_pTextureCom) {
            m_pTextureCom->Set_Zero_Frame();
            m_pTextureCom->Resume_Anim();
        }
        return;

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

void CMonster_Suit::OnUpdateState(MON_STATE s, const _float& dt) // 상태 업데이트
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
        if (m_pTextureCom->Is_AnimFinished()) SetState(AIM);
        break;

    case HIT:
        if (m_pTextureCom->Is_AnimFinished()) {
            if (m_bKillAfterHit) m_bDead = true;
            else SetState((m_ePrevState == DEATH) ? DEATH : IDLE);
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
        if (m_pTextureCom->Is_AnimFinished()) m_bDead = true;
        break;

    default: break;
    }
}

CTransform* CMonster_Suit::GetPlayerTransform()
{
    if (!m_pPlayerTr)
    {
        m_pPlayerTr = dynamic_cast<CTransform*>(
            CObjectManager::GetInstance()->Get_Component(SCENE_STAGE, L"Player_Layer", L"Com_Transform", 0));
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

void CMonster_Suit::SetupHitSpheres() // 상세 조정 필요
{
    m_hitSpheres.clear();
    const int PRI_BODY = 0;
    const int PRI_LEG = 1;
    const int PRI_BALLS = 2;
    const int PRI_HEAD = 3;

    m_hitSpheres.push_back({ HIT_HEAD,  _vec3(0.00f,  0.4f, 0.06f), 0.1f, PRI_HEAD });
    m_hitSpheres.push_back({ HIT_BODY,  _vec3(0.00f,  0.10f,  0.00f), 0.30f, PRI_BODY });
    m_hitSpheres.push_back({ HIT_BALLS, _vec3(0.00f, -0.12f, 0.03f), 0.16f, PRI_BALLS });
    m_hitSpheres.push_back({ HIT_LEG,   _vec3(-0.18f, -0.55f, -0.02f), 0.22f, PRI_LEG });
}

void CMonster_Suit::TrySpawnDeathUI() // Effect UI 띄우기 -> UIManager로 옮길거임
{
    if (!m_pendingDeathUI) return;
    m_pendingDeathUI = false;

    _vec3 headW = GetHeadWorldPos();
    float sx = 0.f, sy = 0.f;
    WorldToScreen(headW, sx, sy);
    sy -= 40.f;

    const bool isHead = (m_lastFatalPart == HIT_HEAD || m_lastFatalPart == HIT_BALLS);
    const float secsAdd = isHead ? 3.0f : 2.0f;

    if (auto ui = dynamic_cast<CEffectUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_MonsterHitEffectUI", SCENE_STAGE, L"UI_Layer")))
    {
        ui->SetImageSize(36.f, 36.f);
        ui->SetBoxSize(230.f, 50.f);
        ui->SetTargetBounds(130.f, 1250.f);
        ui->SetMoveSpeed(1000.f, false);

        ui->Show(isHead ? L"+ 3 SEC" : L"+ 2 SEC",
            L"Com_Tex_Heal", secsAdd,
            sx, sy, 0.f, 0.85f,
            L"DefaultFont", D3DXCOLOR(1, 1, 1, 1));
    }

    if (auto banner = dynamic_cast<CEffectUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_MonsterHitEffectUI", SCENE_STAGE, L"UI_Layer")))
    {
        banner->SetBannerExtraWidth(80.f);
        banner->ShowBanner(
            isHead ? L"HEADSHOT" : L"FINISHED",
            1.10f,
            175.f, 200.f,
            4.f, 1.0f,
            L"DefaultFont",
            D3DXCOLOR(1, 1, 1, 1),
            0.85f,
            -5.f
        );
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

        _matrix S, T, M;
        D3DXMatrixScaling(&S, rW, rW, rW);
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