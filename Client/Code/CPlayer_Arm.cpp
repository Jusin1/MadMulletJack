#include "pch.h"
#include "CPlayer_Arm.h"
#include "CTimerMgr.h"


CPlayer_Arm::CPlayer_Arm(LPDIRECT3DDEVICE9 pGraphicDev)
    : CUI(pGraphicDev), m_tInfo({ PLAYER_END, PMV_END, WP_END, WP2_END })
{
}

CPlayer_Arm::CPlayer_Arm(const CPlayer_Arm& rhs)
    : CUI(rhs),m_tInfo(rhs.m_tInfo)
{
}

CPlayer_Arm::~CPlayer_Arm()
{
}

HRESULT CPlayer_Arm::Ready_GameObject()
{
    if (FAILED(__super::Ready_GameObject()))
        return E_FAIL;

    if (FAILED(CTimerMgr::GetInstance()->Ready_Timer(TEXT("Timer_PlayerArm"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer_Arm::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Texture_Clone()))
        return E_FAIL;

    m_bActive = true;
    m_bRenderOn = false;

    return S_OK;
}

_int CPlayer_Arm::Update_GameObject(const _float& fTimeDelta)
{
    if (m_pTextureCom->Is_AnimFinished())
    {

        if (m_CurrentAnimTag == TEXT("Com_Texture_Arm_Op1"))
        {
            Change_Texture(TEXT("Com_Texture_Arm_Op2"));
            Set_UISizeAndPos(1000.f, 500.f, WINCX * 0.5f, WINCY * 0.5f + 300);
        }

        else
        {
            CGlobal_Info::Get_Instance()->Set_STATE(STATE_END);
            m_bRenderOn = false;
        }
    }
    return NO_EVENT;
}

void CPlayer_Arm::LateUpdate_GameObject(const _float& fTimeDelta)
{
    if (m_tInfo != CGlobal_Info::Get_Instance()->Get_PlayerInfo())
    {
        m_tInfo = CGlobal_Info::Get_Instance()->Get_PlayerInfo();
        Set_Texture();
    }
}

void CPlayer_Arm::Render_GameObject()
{
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


    m_pTextureCom->Set_Texture(m_pTextureCom->Get_Frame().m_iCurrentTex);
    m_pTextureCom->MoveFrame();
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x01);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    __super::Render_GameObject();

    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

HRESULT CPlayer_Arm::Texture_Clone()
{
    CTexture::TEXINFO texInfo = {};

    // Op1
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 3;
    texInfo.m_fSpeed = 5.5f;
    texInfo.m_bLoop = false;
    if (FAILED(Add_Components(L"Com_Texture_Arm_Op1", SCENE_STATIC, L"Prototype_Component_Texture_UIArmOp1", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_Arm_Op1"), m_pTextureCom });

    // Op2
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 3;
    texInfo.m_fSpeed = 5.5f;
    texInfo.m_bLoop = false;
    if (FAILED(Add_Components(L"Com_Texture_Arm_Op2", SCENE_STATIC, L"Prototype_Component_Texture_UIArmOp2", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_Arm_Op2"), m_pTextureCom });

    return S_OK;
}

HRESULT CPlayer_Arm::Set_Texture()
{
    if (m_tInfo.ePlayerState == OPENING && m_tInfo.eWeapon == WP_NON) {
        Change_Texture(TEXT("Com_Texture_Arm_Op1"));
        Set_UISizeAndPos(720.f, 680.f, WINCX * 0.5f, WINCY * 0.5f + 200);
        m_bRenderOn = true;
    }

    else {
        m_bRenderOn = false;
    }

    return S_OK;
}

HRESULT CPlayer_Arm::Change_Texture(const _tchar* pTextureTag)
{
    if (FAILED(__super::Change_Component(pTextureTag, (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    m_pTextureCom->Set_Zero_Frame();
    m_CurrentAnimTag = pTextureTag; // 현재 상태 저장
    return S_OK;
}

CPlayer_Arm* CPlayer_Arm::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CPlayer_Arm* pInstance = new CPlayer_Arm(pGraphicDev);
    if (FAILED(pInstance->Ready_GameObject()))
    {
        MSG_BOX("CPlayer_Arms Create Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CPlayer_Arm::Clone(void* pArg)
{
    CPlayer_Arm* pInstance = new CPlayer_Arm(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CPlayer_Arms Clone Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CPlayer_Arm::Free()
{
    __super::Free();
}