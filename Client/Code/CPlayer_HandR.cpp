#include "pch.h"
#include "CPlayer_HandR.h"
#include "CTimerMgr.h"
#include "CGlobal_Info.h"
#include "CObjectManager.h"
#include "CPistol_Gun.h"
#include "CKnife_SubW.h"

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

    if (FAILED(Texture_Clone()))
        return E_FAIL;

    if (FAILED(Set_WeaponUI()))
        return E_FAIL;

    return S_OK;
}

_int CPlayer_HandR::Update_GameObject(const _float& fTimeDelta)
{   
    __super::Update_GameObject(fTimeDelta);

    Move_UI(fTimeDelta); // ui 움직임 함수
  
    return NO_EVENT;
}

void CPlayer_HandR::LateUpdate_GameObject(const _float& fTimeDelta)
{
    __super::LateUpdate_GameObject(fTimeDelta);

    Update_Weapon_Pistol();
    Update_Weapon2_Knife();

    if (m_tInfo != CGlobal_Info::Get_Instance()->Get_PlayerInfo())
    {
        m_tInfo = CGlobal_Info::Get_Instance()->Get_PlayerInfo();
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
    texInfo.m_iEndTex = 3;
    texInfo.m_fSpeed = 10.f;
    texInfo.m_bLoop = false;
    if (FAILED(Add_Components(L"Com_Texture_HandR_Idle", SCENE_STATIC, L"Prototype_Component_Texture_UIHandRIdle", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_HandR_Idle"), m_pTextureCom });

    // Attack_Instance - knife
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 1;
    texInfo.m_fSpeed = 1.f;
    texInfo.m_bLoop = false;
    if (FAILED(Add_Components(L"Com_Texture_HandR_At2_Knife", SCENE_STATIC, L"Prototype_Component_Texture_UIHandRAt2Knife", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_HandR_At2_Knife"), m_pTextureCom });

    // Dead
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 3;
    texInfo.m_fSpeed = 2.f;
    texInfo.m_bLoop = true;
    if (FAILED(Add_Components(L"Com_Texture_HandR_Dead", SCENE_STATIC, L"Prototype_Component_Texture_UIHandRDead", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_HandR_Dead"), m_pTextureCom });

    // Opening - Pistol
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 1;
    texInfo.m_fSpeed = 1.f;
    texInfo.m_bLoop = false;
    if (FAILED(Add_Components(L"Com_Texture_HandR_Op_Pistol", SCENE_STATIC, L"Prototype_Component_Texture_UIHandROpPistol", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_HandR_Op_Pistol"), m_pTextureCom });

    // Opening - Shotgun
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 4;
    texInfo.m_fSpeed = 1.f;
    texInfo.m_bLoop = false;
    if (FAILED(Add_Components(L"Com_Texture_HandR_Op_Shotgun", SCENE_STATIC, L"Prototype_Component_Texture_UIHandROpShotgun", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_HandR_Op_Shotgun"), m_pTextureCom });


    return S_OK;
}

HRESULT CPlayer_HandR::Set_Texture()
{
    // 만약 회전이 됐으면 다시 되돌려라
     Set_Origin_Rot();

    m_bRenderOn = true;
    
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

            Set_UISizeAndPos(240.f, 600.f, WINCX * 0.5f + 350.f, WINCY * 0.5f + 300.f); // pos를 정하고

            //// info를 새로 맞춰줌
            Set_New_TransInfo(150.f, 0.f);

            m_tMoveInfo = { MV_UP, false, 10.f, 0.f };
        }

        else if (m_tInfo.eWeapon == WP_SHOTGUN) {
            if (FAILED(Change_Texture(TEXT("Com_Texture_HandR_Op_Shotgun"))))
                return E_FAIL;

            Set_UISizeAndPos(200.f, 200.f, WINCX * 0.5f + 400.f, WINCY * 0.5f + 300.f); // pos를 정하고

            //// info를 새로 맞춰줌
            Set_New_TransInfo(0.f, 0.f);
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

            Set_UISizeAndPos(200.f, 200.f, WINCX * 0.5f + 450.f, WINCY * 0.5f + 350.f); // pos를 정하고

            Set_New_TransInfo(0.f, 0.f);

            m_pTransformCom->Rotation({ 0.f, 0.f,1.f }, 1);
            m_fRotSum += D3DXToRadian(0.f) * 1;
        }

        else {
            m_bRenderOn = false;
        }
    }
        break;

    case ATTACK_INSTANT:
    {
        if (m_tInfo.eWeapon2 == WP_KNIFE) {
            if (FAILED(Change_Texture(TEXT("Com_Texture_HandR_At2_Knife"))))
                return E_FAIL;

            Set_UISizeAndPos(250.f, 450.f, WINCX * 0.5f + 600.f, WINCY * 0.5f  + 150.f); // pos를 정하고

            Set_New_TransInfo(400.f, 15.f);
            m_pTransformCom->Rotation({ 0.f, 0.f,1.f }, 1); // rotation texture
            m_fRotSum += D3DXToRadian(15.f) * 1;

            m_tMoveInfo = { MV_LEFT, false, 10.f, 0.f };
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

        Set_UISizeAndPos(280.f, 280.f, WINCX * 0.5f, WINCY * 0.5f + 240); // pos를 정하고

        Set_New_TransInfo(10.f, 0.f);

        m_tMoveInfo = { MV_DOWN, false, 0.f, 0.f };
    }
        break;
    case CLEAR:
        Set_RenderOn(false);
        break;

    default:
    {
        if (FAILED(Change_Texture(TEXT("Com_Texture_HandR_Idle"))))
            return E_FAIL;

        // idle pos
        Set_UISizeAndPos(400.f, 600.f, WINCX * 0.5f + 450.f, WINCY * 0.5f + 550.f); // pos를 정하고

        Set_New_TransInfo(50.f, 0.f);

        m_tMoveInfo = { MV_RL, false, 30.f, 0.f };
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

HRESULT CPlayer_HandR::Set_WeaponUI()
{
    m_pWeaponUI = dynamic_cast<CUIBase*>(
        CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UIRoot", SCENE_STATIC, L"UI_Layer"));
    Add_Child(m_pWeaponUI);
    if (m_pWeaponUI == nullptr)
        return E_FAIL;

    // pistol 생성 및 list에 넣기
    CPistol_Gun* pPistolUI = dynamic_cast<CPistol_Gun*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_GunPistolUI", SCENE_STATIC, L"UI_Layer"));
    if (pPistolUI)
    {
        pPistolUI->Set_ObjTag(L"PistolUI");
        pPistolUI->Set_WapState(CWeapon::WAPSTATE::WEAPON); //state를 weapon으로 등록
        m_pWeaponUI->Add_Child(pPistolUI); // 루트 UI에 등록
    }

    // knife 생성 및 list에 넣기
    CKnife_SubW* pKnifeUI = dynamic_cast<CKnife_SubW*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_SubWKnifeUI", SCENE_STATIC, L"UI_Layer"));
    if (pKnifeUI)
    {
        pKnifeUI->Set_ObjTag(L"KnifeUI");
        pKnifeUI->Set_WapState(CWeapon::WAPSTATE::WEAPON); //state를 weapon으로 등록
        m_pWeaponUI->Add_Child(pKnifeUI); // 루트 UI에 등록
    }

    return S_OK;
}

void CPlayer_HandR::Update_Weapon_Pistol()
{
    // pistol을 가져옴
    CUIBase* pPistol = m_pWeaponUI->Find_Child_ByTag(L"PistolUI");

    // 지금 pistol이 아니면 update를 하지 않음
    if (m_tInfo.eWeapon != WP_PISTOL && pPistol)
    {
        pPistol->Set_Active(false);
        pPistol->Set_RenderOn(false);
        return;
    }
        
    // pistol이 있다면
    if (pPistol)
    {
        pPistol->Set_Active(true);

        // player state에 따라 맞는 셋팅을 해줌
        switch (m_tInfo.ePlayerState)
        {
        case ATTACK:
            pPistol->Set_RenderOn(true);
            break;

        case OPENING:
            //pPistol->Set_Active(true);
            break;

        case RELOAD:
           // pPistol->Set_Active(true);
            pPistol->Set_RenderOn(true);
            break;
        case CLEAR:
            pPistol->Set_RenderOn(false);

        default:
            // HandR의 texture가 Idle이면 -> pitol idle로 render
            if (m_CurrentAnimTag == TEXT("Com_Texture_HandR_Idle"))
            {
                // pos를 갱신
                pPistol->Set_UIPos(m_pTransformCom->Get_Info(INFO_POS), -120.f, 280.f);
                return;
            }

            else
            {
                pPistol->Set_Active(false);
                pPistol->Set_RenderOn(false);
            }
        }
    }
}

void CPlayer_HandR::Update_Weapon2_Knife()
{
    // pistol을 가져옴
    CUIBase* pKnife = m_pWeaponUI->Find_Child_ByTag(L"KnifeUI");

    if (pKnife && m_tInfo.ePlayerState == ATTACK_INSTANT)
    {
        pKnife->Set_Active(true);
        pKnife->Set_RenderOn(true);
        pKnife->Set_UIPos(m_pTransformCom->Get_Info(INFO_POS), -200.f, 340.f);
    }
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