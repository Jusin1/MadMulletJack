#include "pch.h"
#include "CPlayer_HandL.h"
#include "CTimerMgr.h"
#include "CObjectManager.h"
#include "CMapFactory.h"

CPlayer_HandL::CPlayer_HandL(LPDIRECT3DDEVICE9 pGraphicDev)
    : CUI(pGraphicDev), m_tInfo({ PLAYER_END,PMV_END, WP_END, WP2_END }), m_fWaitTimer(0.f)
{
}

CPlayer_HandL::CPlayer_HandL(const CPlayer_HandL& rhs)
    : CUI(rhs), m_tInfo(rhs.m_tInfo), m_fWaitTimer(rhs.m_fWaitTimer)
{
}

CPlayer_HandL::~CPlayer_HandL()
{
}

HRESULT CPlayer_HandL::Ready_GameObject()
{
    if (FAILED(__super::Ready_GameObject()))
        return E_FAIL;

    if (FAILED(CTimerMgr::GetInstance()->Ready_Timer(TEXT("Timer_PlayerHandL"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer_HandL::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;


    if (FAILED(Texture_Clone()))
        return E_FAIL;

    return S_OK;
}

_int CPlayer_HandL::Update_GameObject(const _float& fTimeDelta)
{
    Move_UI(fTimeDelta);
    Katana(fTimeDelta);
    return NO_EVENT;
}

void CPlayer_HandL::LateUpdate_GameObject(const _float& fTimeDelta)
{

}

void CPlayer_HandL::Render_GameObject()
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

HRESULT CPlayer_HandL::Texture_Clone()
{
    CTexture::TEXINFO texInfo = {};

    // Doping
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 7;
    texInfo.m_fSpeed = 9.f;
    texInfo.m_bLoop = true;
    if (FAILED(Add_Components(L"Com_Texture_HandL_Doping", SCENE_STATIC, L"Prototype_Component_Texture_UIHandLDoping", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_HandL_Doping"), m_pTextureCom });

    // Op-rifle
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 12;
    texInfo.m_fSpeed = 3.f;
    texInfo.m_bLoop = false;
    if (FAILED(Add_Components(L"Com_Texture_HandL_Op_Rif", SCENE_STATIC, L"Prototype_Component_Texture_UIHandLOpRif", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_HandL_Op_Rif"), m_pTextureCom });

    // Attack_Instance - knife
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 3;
    texInfo.m_fSpeed = 3.f;
    texInfo.m_bLoop = false;
    if (FAILED(Add_Components(L"Com_Texture_HandL_At2_Knife", SCENE_STATIC, L"Prototype_Component_Texture_UIHandLAt2Knife", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_HandL_At2_Knife"), m_pTextureCom });

    // reload - pistol
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 3;
    texInfo.m_fSpeed = 3.8f;
    texInfo.m_bLoop = false;
    if (FAILED(Add_Components(L"Com_Texture_HandL_Re_Pistol", SCENE_STATIC, L"Prototype_Component_Texture_UIHandLRePistol", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_HandL_Re_Pistol"), m_pTextureCom });

    // reload - shotgun
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 2;
    texInfo.m_fSpeed = 0.5f;
    texInfo.m_bLoop = false;
    if (FAILED(Add_Components(L"Com_Texture_HandL_Re_Shotgun", SCENE_STATIC, L"Prototype_Component_Texture_UIHandLReShotgun", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_HandL_Re_Shotgun"), m_pTextureCom });

    // Opening - Katana
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 1;
    texInfo.m_fSpeed = 3.f;
    texInfo.m_bLoop = true;
    if (FAILED(Add_Components(L"Com_Texture_HandL_Op_Katana", SCENE_STATIC, L"Prototype_Component_Texture_Katana_Hand_L", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_HandL_Op_Katana"), m_pTextureCom });

    return S_OK;
}

void CPlayer_HandL::Katana(const _float& fTimeDelta)
{
    if (m_tInfo.ePlayerState == OPENING && m_tInfo.eWeapon == WP_KATANA)
    {
        if (m_tMoveInfo.eUIMove == MV_UP && m_tMoveInfo.IsRangeEnd())
        {
            m_tMoveInfo.eUIMove = MV_LEFT;
            m_tMoveInfo.bStop = true;
            m_tMoveInfo.fRange = 100.f;   
            m_tMoveInfo.fSumRange = 0.f;
            m_tMoveInfo.bRenderStop = false;

            Set_New_TransInfo(1900, 0.f);
        }
        if (m_tMoveInfo.eUIMove == MV_LEFT && m_tMoveInfo.IsRangeEnd())
        {
            m_fWaitTimer += fTimeDelta;
            if (m_fWaitTimer >= 0.6f) 
            {
                m_tMoveInfo.eUIMove = MV_LEFT;
                m_tMoveInfo.bStop = true;
                m_tMoveInfo.fRange = 1000.f;   
                m_tMoveInfo.fSumRange = 0.f;
                m_tMoveInfo.bRenderStop = false;
                Set_New_TransInfo(1850.f, 0.f);
            }
        }
    }
}

HRESULT CPlayer_HandL::Set_Texture()
{
    // 만약 회전이 됐으면 다시 되돌려라
    Set_Origin_Rot();

    m_bRenderOn = true;

    // info 업데이트
    m_tInfo = CGlobal_Info::Get_Instance()->Get_PlayerInfo();
    
    // player state -> weapon 순으로 나누어서 texture 출력
    switch (m_tInfo.ePlayerState) {
    case OPENING:
    {
        if (m_tInfo.eWeapon == WP_RIFLE) {
            if (FAILED(Change_Texture(TEXT("Com_Texture_HandL_Op_Rif"))))
                return E_FAIL;

            Set_UISizeAndPos(300.f, 500.f, WINCX * 0.5f - 250.f, WINCY * 0.5f + 250.f);

            Set_New_TransInfo(10.f, -10.f);

            m_tMoveInfo = { MV_ROTATIONZ, false, 0.f, 0.f };
        }
        if (m_tInfo.eWeapon == WP_KATANA) {
            if (FAILED(Change_Texture(TEXT("Com_Texture_HandL_Op_Katana"))))
                return E_FAIL;

            Set_UISizeAndPos(300.f, 500.f, WINCX * 0.5f - 470.f, WINCY * 0.5f + 500.f);
            Set_New_TransInfo(700.f, 0.f);
            m_tMoveInfo = { MV_UP, true, 220.f, 0.f };
        }

        else {
            m_bRenderOn = false;
        }
    }
        break;

    case RELOAD: 
    {
        if (m_tInfo.eWeapon == WP_PISTOL) {
            if (FAILED(Change_Texture(TEXT("Com_Texture_HandL_Re_Pistol"))))
                return E_FAIL;

            Set_UISizeAndPos(400.f, 800.f, WINCX * 0.5f - 200.f, WINCY * 0.5f + 310.f); //idle pos

            Set_New_TransInfo(1200.f, -40.f);
            m_pTransformCom->Rotation({ 0.f, 0.f,1.f }, 1); // rotation texture
            m_fRotSum += D3DXToRadian(-40.f) * 1;

            m_tMoveInfo = { MV_RIGHT, true, 550.f, 0.f };
        }

        else if (m_tInfo.eWeapon == WP_SHOTGUN) {
            if (FAILED(Change_Texture(TEXT("Com_Texture_HandL_Re_Shotgun"))))
                return E_FAIL;

            Set_UISizeAndPos(240.f, 600.f, WINCX * 0.5f - 200.f, WINCY * 0.5f + 200.f);

            Set_New_TransInfo(200.f, -40.f);
            m_pTransformCom->Rotation({ 0.f, 0.f,1.f }, 1); // rotation texture
            m_fRotSum += D3DXToRadian (-40.f) * 1;

            m_tMoveInfo = { MV_RIGHT, false, 0.f, 0.f };
        }

        else {
            m_bRenderOn = false;
        }
    }
        break;

    case ATTACK_INSTANT:
    {
        if (m_tInfo.eWeapon2 == WP_KNIFE) {
            if (FAILED(Change_Texture(TEXT("Com_Texture_HandL_At2_Knife"))))
                return E_FAIL;

            Set_UISizeAndPos(800.f, 624.f, WINCX * 0.5 - 500.f, WINCY * 0.5f + 290); //idle pos

            Set_New_TransInfo(3000.f, 0.f);
            //m_pTransformCom->Rotation({ 0.f, 0.f, 1.f }, 1); // rotation texture
            //m_fRotSum += D3DXToRadian (-20.f) * 1;

            m_tMoveInfo = { MV_RIGHT, false, 0.f, 0.f };
        }

        else {
            m_bRenderOn = false;
        }
    }
        break;

    case DOPING:
    {
        if (FAILED(Change_Texture(TEXT("Com_Texture_HandL_Doping"))))
            return E_FAIL;

        Set_UISizeAndPos(330.f, 330.f, WINCX * 0.5f, WINCY * 0.5f + 400.f);

        //// info를 새로 맞춰줌
        Set_New_TransInfo(400.f, 0.f);

        m_tMoveInfo = { MV_UP, true, 150.f, 0.f };
    }
    break;

    default:
        m_bRenderOn = false;
    }

    return S_OK;
}

HRESULT CPlayer_HandL::Change_Texture(const _tchar* pTextureTag)
{
    if (FAILED(__super::Change_Component(pTextureTag, (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    m_pTextureCom->Set_Zero_Frame();
    m_CurrentAnimTag = pTextureTag; // 현재 상태 저장
    return S_OK;
}

CPlayer_HandL* CPlayer_HandL::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CPlayer_HandL* pInstance = new CPlayer_HandL(pGraphicDev);
    if (FAILED(pInstance->Ready_GameObject()))
    {
        MSG_BOX("CPlayer_HandL Create Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CPlayer_HandL::Clone(void* pArg)
{
    CPlayer_HandL* pInstance = new CPlayer_HandL(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CPlayer_HandL Clone Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CPlayer_HandL::Free()
{
    __super::Free();
}