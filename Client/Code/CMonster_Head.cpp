#include "pch.h"
#include "CMonster_Head.h"
#include "CManagement.h"
#include "CObjectManager.h"

CMonster_Head::CMonster_Head(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev, MonsterType::SUIT)
    , m_cfg{}                        
    , m_initLook{ 0.f, 0.f, 1.f }         
    , m_backDrift(0.15f)              
    , m_gravity(18.f)              
{
}

CMonster_Head::CMonster_Head(const CMonster_Head& rhs)
    : CMonster(rhs)
    , m_cfg(rhs.m_cfg)                  // 스폰 설정 그대로 복사
    , m_initLook(rhs.m_initLook)
    , m_backDrift(rhs.m_backDrift)
    , m_gravity(rhs.m_gravity)
{
}

CMonster_Head::~CMonster_Head() {}

HRESULT CMonster_Head::Ready_GameObject()
{
    if (FAILED(__super::Ready_GameObject())) return E_FAIL;
    return S_OK;
}

HRESULT CMonster_Head::Initialize(void* pArg)
{
    if (pArg) {
        const HeadSpawnArg* cfg = reinterpret_cast<const HeadSpawnArg*>(pArg);
        if (cfg) m_cfg = *cfg;
    }
    if (!m_cfg.texTag)   m_cfg.texTag = L"Com_Texture_Katana_HEAD_DEATH";
    if (!m_cfg.protoTag) m_cfg.protoTag = L"Prototype_Component_Texture_Monster_Suit_Katana_HEAD";
    if (m_cfg.endFrame <= 0) m_cfg.endFrame = 35;
    if (m_cfg.animSpeed <= 0) m_cfg.animSpeed = 7.f;
    if (m_cfg.fallSpeed <= 0) m_cfg.fallSpeed = 6.f;
    if (m_cfg.gravity <= 0) m_cfg.gravity = 18.f;
    if (m_cfg.backDrift < 0) m_cfg.backDrift = 0.f;

    if (FAILED(__super::Initialize(pArg))) return E_FAIL;

    DisableCollisionAndPicking(); // 픽킹 충돌 비활성화

    if (FAILED(EnsureHeadTextureRegisteredAndSelected())) return E_FAIL; // 중복 텍스쳐 생성 막기

    m_bJumping = true;                              
    m_fVelocity = -m_cfg.fallSpeed;                
    m_gravity = m_cfg.gravity;                
    m_backDrift = m_cfg.backDrift;

    // 방향
    _vec3 look = m_pTransformCom->Get_Info(INFO_LOOK);
    look.y = 0.f;
    if (D3DXVec3LengthSq(&look) > 1e-6f) D3DXVec3Normalize(&look, &look);
    m_initLook = look;

    return S_OK;
}

_int CMonster_Head::Update_GameObject(const _float& fTimeDelta)
{
    if (m_pTextureCom && m_pTextureCom->Is_AnimFinished())
        return DEAD;

    // 낙하
    if (m_bJumping)
    {
        m_fVelocity -= m_gravity * fTimeDelta;
        Set_OnTerrain(fTimeDelta);    
    }
    else
    {
        // 착지 후에도 지면 고정 유지
        Set_OnTerrain(fTimeDelta);
    }

    // (옵션) 뒤로(혹은 옆) 드리프트
    if (m_backDrift > 0.f)
        m_pTransformCom->Move_PosDir(m_backDrift * fTimeDelta, -m_initLook);

    return __super::Update_GameObject(fTimeDelta);
}

void CMonster_Head::LateUpdate_GameObject(const _float& fTimeDelta)
{
    __super::LateUpdate_GameObject(fTimeDelta);
}

void CMonster_Head::Render_GameObject()
{
    __super::Render_GameObject();
}


HRESULT CMonster_Head::Texture_Clone()
{
    // 동일 태그 이미 보유 시 스킵
    if (m_mapTexture.find(m_cfg.texTag) != m_mapTexture.end())
        return S_OK;

    CTexture::TEXINFO tex{};
    tex.m_iStart = 0;
    tex.m_iEndTex = m_cfg.endFrame;
    tex.m_fSpeed = m_cfg.animSpeed;
    tex.m_bLoop = m_cfg.loop;

    if (FAILED(Add_Components(m_cfg.texTag, SCENE_STATIC,
        m_cfg.protoTag, (CComponent**)&m_pTextureCom, &tex)))
        return E_FAIL;

    m_mapTexture.insert({ m_cfg.texTag, m_pTextureCom });
    return S_OK;
}

HRESULT CMonster_Head::Change_Texture_Safe(const _tchar* pTag)
{
    if (FAILED(__super::Change_Component(pTag, (CComponent**)&m_pTextureCom)))
        return E_FAIL;
    if (m_pTextureCom) m_pTextureCom->Set_Zero_Frame();
    return S_OK;
}

HRESULT CMonster_Head::EnsureHeadTextureRegisteredAndSelected()
{
    if (!m_cfg.texTag || !m_cfg.protoTag) return E_FAIL;

    if (m_mapTexture.find(m_cfg.texTag) == m_mapTexture.end())
    {
        CTexture::TEXINFO tex{};
        tex.m_iStart = 0;
        tex.m_iEndTex = (m_cfg.endFrame > 0 ? m_cfg.endFrame : 35);
        tex.m_fSpeed = (m_cfg.animSpeed > 0 ? m_cfg.animSpeed : 7.f);
        tex.m_bLoop = m_cfg.loop;

        if (FAILED(Add_Components(m_cfg.texTag, SCENE_STATIC,
            m_cfg.protoTag, (CComponent**)&m_pTextureCom, &tex)))
            return E_FAIL;

        m_mapTexture.insert({ m_cfg.texTag, m_pTextureCom });
    }

    if (FAILED(__super::Change_Component(m_cfg.texTag, (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    if (m_pTextureCom) { m_pTextureCom->Set_Zero_Frame(); m_pTextureCom->Resume_Anim(); }
    return S_OK;
}

void CMonster_Head::DisableCollisionAndPicking()
{
    m_bPickable = false;
    if (m_pColiderCom) m_pColiderCom->Set_Active(false);
}

CMonster_Head* CMonster_Head::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    auto* p = new CMonster_Head(pGraphicDev);
    if (FAILED(p->Ready_GameObject()))
    {
        MSG_BOX("CMonster_Head Create Failed");
        Safe_Release(p);
    }
    return p;
}

CGameObject* CMonster_Head::Clone(void* pArg)
{
    auto* p = new CMonster_Head(*this);
    if (FAILED(p->Initialize(pArg)))
    {
        MSG_BOX("CMonster_Head Clone Failed");
        Safe_Release(p);
    }
    return p;
}

void CMonster_Head::Free()
{
    __super::Free();
}