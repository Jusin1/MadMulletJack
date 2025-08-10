#include "pch.h"
#include "CPlayer_HandR.h"
#include "CTimerMgr.h"
#include "CPlayer_StateInfo.h"



CPlayer_HandR::CPlayer_HandR(LPDIRECT3DDEVICE9 pGraphicDev)
    : CUI(pGraphicDev),m_tInfo({ PLAYER_END, WP_END, WP2_END })
{
}

CPlayer_HandR::CPlayer_HandR(const CPlayer_HandR& rhs)
    : CUI(rhs), m_tInfo(rhs.m_tInfo)
{
}

CPlayer_HandR::~CPlayer_HandR()
{
}

HRESULT CPlayer_HandR::Ready_GameObject()
{
    if (FAILED(__super::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer_HandR::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(CTimerMgr::GetInstance()->Ready_Timer(TEXT("Timer_PlayerHandR"))))
        return E_FAIL;

    Set_UISizeAndPos(100.f, 100.f, WINCX * 0.5f + 450.f , WINCY * 0.5f + 350.f);

    if (FAILED(Texture_Clone()))
        return E_FAIL;

    return S_OK;
}

_int CPlayer_HandR::Update_GameObject(const _float& fTimeDelta)
{   
    __super::Update_GameObject(fTimeDelta);
    if (m_pTextureCom->Is_AnimFinished())
    {
        m_bAniFinish = true;
    }
    
    Move_UI(fTimeDelta); // ui 움직임 함수

    return NO_EVENT;
}

void CPlayer_HandR::LateUpdate_GameObject(const _float& fTimeDelta)
{
    __super::LateUpdate_GameObject(fTimeDelta);

    if (m_tInfo != CPlayer_StateInfo::Get_Instance()->Get_PlayerInfo())
    {
        m_tInfo = CPlayer_StateInfo::Get_Instance()->Get_PlayerInfo();
        Set_Texture();
    }
}

void CPlayer_HandR::Render_GameObject()
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

HRESULT CPlayer_HandR::Texture_Clone()
{
    CTexture::TEXINFO texInfo = {};

    // IDLE
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 1;
    texInfo.m_fSpeed = 1.f;
    texInfo.m_bLoop = false;
    if (FAILED(Add_Components(L"Com_Texture_HandR_Idle", SCENE_STAGE, L"Prototype_Component_Texture_UIHandRIdle", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_HandR_Idle"), m_pTextureCom });

    // Attack_Instance - knife
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 1;
    texInfo.m_fSpeed = 1.f;
    texInfo.m_bLoop = false;
    if (FAILED(Add_Components(L"Com_Texture_HandR_At2_Knife", SCENE_STAGE, L"Prototype_Component_Texture_UIHandRAt2Knife", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_HandR_At2_Knife"), m_pTextureCom });

    // Dead
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 3;
    texInfo.m_fSpeed = 2.f;
    texInfo.m_bLoop = true;
    if (FAILED(Add_Components(L"Com_Texture_HandR_Dead", SCENE_STAGE, L"Prototype_Component_Texture_UIHandRDead", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_HandR_Dead"), m_pTextureCom });

    // Opening - Pistol
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 1;
    texInfo.m_fSpeed = 1.f;
    texInfo.m_bLoop = false;
    if (FAILED(Add_Components(L"Com_Texture_HandR_Op_Pistol", SCENE_STAGE, L"Prototype_Component_Texture_UIHandROpPistol", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_HandR_Op_Pistol"), m_pTextureCom });

    // Opening - Shotgun
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 4;
    texInfo.m_fSpeed = 1.f;
    texInfo.m_bLoop = false;
    if (FAILED(Add_Components(L"Com_Texture_HandR_Op_Shotgun", SCENE_STAGE, L"Prototype_Component_Texture_UIHandROpShotgun", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_HandR_Op_Shotgun"), m_pTextureCom });


    return S_OK;
}

HRESULT CPlayer_HandR::Set_Texture()
{
    CTransform::TRANSFORMINFO TransformInfo;    // 새롭게 transinfo를 저장해줌
    ZeroMemory(&TransformInfo, sizeof(CTransform::TRANSFORMINFO));

    switch (m_tInfo.ePlayerState)
    {
    case ATTACK:
        m_bRenderOn = false;
        break;

    case OPENING:
    {
        if (m_tInfo.eWeapon == WP_PISTOL) {
            if(FAILED(Change_Texture(TEXT("Com_Texture_HandR_Op_Pistol"))))
                return E_FAIL;

            Set_UISizeAndPos(120.f, 320.f, WINCX * 0.5f + 350.f, WINCY * 0.5f + 280.f); // pos를 정하고

            //// info를 새로 맞춰줌
            //TransformInfo.fSpeed = 10.f;
            TransformInfo.fRotationSpeed = D3DXToRadian(90.f);
            TransformInfo.vStartPos = m_pTransformCom->Get_Info(INFO_POS);
            m_pTransformCom->SetTransformInfo(TransformInfo);

            m_bRenderOn = true;
        }

        else if (m_tInfo.eWeapon == WP_SHOTGUN) {
            if (FAILED(Change_Texture(TEXT("Com_Texture_HandR_Op_Shotgun"))))
                return E_FAIL;

            Set_UISizeAndPos(100.f, 100.f, WINCX * 0.5f + 400.f, WINCY * 0.5f + 300.f); // pos를 정하고

            //// info를 새로 맞춰줌
            //TransformInfo.fSpeed = 10.f;
            TransformInfo.fRotationSpeed = D3DXToRadian(90.f);
            TransformInfo.vStartPos = m_pTransformCom->Get_Info(INFO_POS);
            m_pTransformCom->SetTransformInfo(TransformInfo);

            m_bRenderOn = true;
        }

        else {
            m_bRenderOn = false;
        }
    }
        break;

    case RELOAD:
    {
        if (m_tInfo.eWeapon == WP_SHOTGUN) {
            if (FAILED(Change_Texture(TEXT("Com_Texture_Hand_Idle"))))
                return E_FAIL;

            Set_UISizeAndPos(100.f, 100.f, WINCX * 0.5f + 450.f, WINCY * 0.5f + 350.f); // pos를 정하고

            //// info를 새로 맞춰줌
            //TransformInfo.fSpeed = 10.f;
            TransformInfo.fRotationSpeed = D3DXToRadian(90.f);
            TransformInfo.vStartPos = m_pTransformCom->Get_Info(INFO_POS);
            m_pTransformCom->SetTransformInfo(TransformInfo);

            m_pTransformCom->Rotation({ 0.f, 0.f,1.f }, 1);

            m_bRenderOn = true;
        }

        else {
            m_bRenderOn = false;
        }
    }
        break;

    case ATTACK_INSTANT:
    {
        if (m_tInfo.eWeapon == WP_KNIFE) {
            if (FAILED(Change_Texture(TEXT("Com_Texture_HandR_At2_Knife"))))
                return E_FAIL;

            Set_UISizeAndPos(200.f, 200.f, WINCX * 0.5f + 450.f, WINCY * 0.5f - 100.f); // pos를 정하고

            //// info를 새로 맞춰줌
            //TransformInfo.fSpeed = 10.f;
            TransformInfo.fRotationSpeed = D3DXToRadian(5.f);
            TransformInfo.vStartPos = m_pTransformCom->Get_Info(INFO_POS);
            m_pTransformCom->SetTransformInfo(TransformInfo);

            m_pTransformCom->Rotation({ 0.f, 0.f,1.f }, 1); // rotation texture

            m_bRenderOn = true;
        }

        else {
            m_bRenderOn = false;
        }
    }
        break;

    case PLAYERDEAD:
    {
        if (FAILED(Change_Texture(TEXT("Com_Texture_HandR_Dead"))))
            return E_FAIL;

        Set_UISizeAndPos(150.f, 150.f, WINCX * 0.5f, WINCY * 0.5f + 240); // pos를 정하고

        TransformInfo.fSpeed = 5.f;
        TransformInfo.fRotationSpeed = D3DXToRadian(90.f);
        TransformInfo.vStartPos = m_pTransformCom->Get_Info(INFO_POS);
        m_pTransformCom->SetTransformInfo(TransformInfo);

        m_eMove = MV_DOWN;

        m_bRenderOn = true;
    }
        break;

    default:
    {
        if (FAILED(Change_Texture(TEXT("Com_Texture_HandR_Idle"))))
            return E_FAIL;

        // idle pos
        Set_UISizeAndPos(80.f, 80.f, WINCX * 0.5f + 450.f, WINCY * 0.5f + 300.f); // pos를 정하고

        // info를 새로 맞춰줌
        TransformInfo.fSpeed = 50.f;
        TransformInfo.fRotationSpeed = D3DXToRadian(90.f);
        TransformInfo.vStartPos = m_pTransformCom->Get_Info(INFO_POS);
        m_pTransformCom->SetTransformInfo(TransformInfo);

        m_eMove = MV_RL;
        m_fRange = 30.f;

        m_bRenderOn = true;
    }
        break;
    }
    
    return S_OK;
}

HRESULT CPlayer_HandR::Change_Texture(const _tchar* pTextureTag)
{
    if (FAILED(__super::Change_Component(pTextureTag, (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    m_pTextureCom->Set_Zero_Frame();
    m_CurrentAnimTag = pTextureTag; // 현재 상태 저장
    return S_OK;
}

CPlayer_HandR* CPlayer_HandR::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CPlayer_HandR* pInstance = new CPlayer_HandR(pGraphicDev);
    if (FAILED(pInstance->Ready_GameObject()))
    {
        MSG_BOX("CPlayer_HandR Create Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CPlayer_HandR::Clone(void* pArg)
{
    CPlayer_HandR* pInstance = new CPlayer_HandR(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CPlayer_HandR Clone Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CPlayer_HandR::Free()
{
    __super::Free();
}