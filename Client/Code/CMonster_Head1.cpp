#include "pch.h"
#include "CMonster_Head1.h"
#include "CManagement.h"
#include "CObjectManager.h"

CMonster_Head1::CMonster_Head1(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev, MonsterType::SUIT) {
}

CMonster_Head1::CMonster_Head1(const CMonster_Head1& rhs)
    : CMonster(rhs), m_cfg(rhs.m_cfg),
    m_initLook(rhs.m_initLook), m_backDrift(rhs.m_backDrift), m_gravity(rhs.m_gravity) {
}

CMonster_Head1::~CMonster_Head1() {}

HRESULT CMonster_Head1::Ready_GameObject()
{
    if (FAILED(__super::Ready_GameObject())) return E_FAIL;
    return S_OK;
}

HRESULT CMonster_Head1::Initialize(void* pArg)
{
    if (pArg) {
        const HeadSpawnArg* cfg = reinterpret_cast<const HeadSpawnArg*>(pArg);
        if (cfg) m_cfg = *cfg;
    }

    if (!m_cfg.texTag)   m_cfg.texTag = L"Com_Texture_Monster_HEAD";
    if (!m_cfg.protoTag) m_cfg.protoTag = L"Prototype_Component_Texture_Monster_HEAD";
    if (m_cfg.endFrame < 0)  m_cfg.endFrame = 11;
    if (m_cfg.animSpeed <= 0) m_cfg.animSpeed = 6.f;
    if (m_cfg.fallSpeed <= 0) m_cfg.fallSpeed = 0.8f;
    if (m_cfg.gravity <= 0) m_cfg.gravity = 3.5f;
    if (m_cfg.backDrift < 0)  m_cfg.backDrift = 0.f;

    if (FAILED(__super::Initialize(pArg))) return E_FAIL;

    DisableCollisionAndPicking();
    if (FAILED(EnsureHeadTextureRegisteredAndSelected())) return E_FAIL;

    m_bJumping = true;
    m_fVelocity = -m_cfg.fallSpeed;
    m_gravity = m_cfg.gravity;
    m_backDrift = m_cfg.backDrift;

    _vec3 look = m_pTransformCom->Get_Info(INFO_LOOK);
    look.y = 0.f;
    if (D3DXVec3LengthSq(&look) > 1e-6f) D3DXVec3Normalize(&look, &look);
    m_initLook = look;

    return S_OK;
}

_int CMonster_Head1::Update_GameObject(const _float& dt)
{
    if (m_pTextureCom && m_pTextureCom->Is_AnimFinished()) {
        m_deleteTimer += dt;
        if (m_deleteTimer > 1.5f) 
            return DEAD;
    }

    if (m_bJumping) {
        m_fVelocity -= m_gravity * dt;
        _vec3 pos = m_pTransformCom->Get_Info(INFO_POS);
        pos.y += m_fVelocity * dt;  // 실제 낙하 적용
        m_pTransformCom->Set_Info(INFO_POS, pos);
        Set_OnTerrain(dt);
    }

    if (m_backDrift > 0.f)
        m_pTransformCom->Move_PosDir(m_backDrift * dt, -m_initLook);

    return __super::Update_GameObject(dt);
}

void CMonster_Head1::LateUpdate_GameObject(const _float& dt) { __super::LateUpdate_GameObject(dt); }
void CMonster_Head1::Render_GameObject() { __super::Render_GameObject(); }

HRESULT CMonster_Head1::Texture_Clone()
{
    if (m_mapTexture.find(m_cfg.texTag) != m_mapTexture.end())
        return S_OK;

    CTexture::TEXINFO tex{};
    tex.m_iStart = 0;
    tex.m_iEndTex = m_cfg.endFrame;   // 0~11
    tex.m_fSpeed = m_cfg.animSpeed;
    tex.m_bLoop = m_cfg.loop;

    if (FAILED(Add_Components(m_cfg.texTag, SCENE_STATIC, m_cfg.protoTag, (CComponent**)&m_pTextureCom, &tex)))
        return E_FAIL;

    m_mapTexture.insert({ m_cfg.texTag, m_pTextureCom });
    return S_OK;
}

HRESULT CMonster_Head1::EnsureHeadTextureRegisteredAndSelected()
{
    if (!m_cfg.texTag || !m_cfg.protoTag) return E_FAIL;

    if (m_mapTexture.find(m_cfg.texTag) == m_mapTexture.end()) {
        CTexture::TEXINFO tex{};
        tex.m_iStart = 0;
        tex.m_iEndTex = (m_cfg.endFrame >= 0 ? m_cfg.endFrame : 11);
        tex.m_fSpeed = (m_cfg.animSpeed > 0 ? m_cfg.animSpeed : 16.f);
        tex.m_bLoop = m_cfg.loop;

        if (FAILED(Add_Components(m_cfg.texTag, SCENE_STATIC, m_cfg.protoTag, (CComponent**)&m_pTextureCom, &tex)))
            return E_FAIL;

        m_mapTexture.insert({ m_cfg.texTag, m_pTextureCom });
    }

    if (FAILED(__super::Change_Component(m_cfg.texTag, (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    if (m_pTextureCom) { m_pTextureCom->Set_Zero_Frame(); m_pTextureCom->Resume_Anim(); }
    return S_OK;
}

void CMonster_Head1::DisableCollisionAndPicking()
{
    m_bPickable = false;
    if (m_pColiderCom) m_pColiderCom->Set_Active(false);
}

CMonster_Head1* CMonster_Head1::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    auto* p = new CMonster_Head1(pGraphicDev);
    if (FAILED(p->Ready_GameObject())) { MSG_BOX("CMonster_Head Create Failed"); Safe_Release(p); }
    return p;
}

CGameObject* CMonster_Head1::Clone(void* pArg)
{
    auto* p = new CMonster_Head1(*this);
    if (FAILED(p->Initialize(pArg))) { MSG_BOX("CMonster_Head Clone Failed"); Safe_Release(p); }
    return p;
}

void CMonster_Head1::Free() { __super::Free(); }