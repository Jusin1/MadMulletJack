#include "pch.h"
#include "CPlayer_Foot.h"
#include "CTimerMgr.h"


CPlayer_Foot::CPlayer_Foot(LPDIRECT3DDEVICE9 pGraphicDev)
    : CUI(pGraphicDev), m_tInfo({ PLAYER_END, WP_END, WP2_END })
{
}

CPlayer_Foot::CPlayer_Foot(const CPlayer_Foot& rhs)
    : CUI(rhs),m_tInfo(rhs.m_tInfo)
{
}

CPlayer_Foot::~CPlayer_Foot()
{
}

HRESULT CPlayer_Foot::Ready_GameObject()
{
    if (FAILED(__super::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer_Foot::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(CTimerMgr::GetInstance()->Ready_Timer(TEXT("Timer_PlayerFoot"))))
        return E_FAIL;

    if (FAILED(Texture_Clone()))
        return E_FAIL;

    Set_Texture();

    return S_OK;
}

_int CPlayer_Foot::Update_GameObject(const _float& fTimeDelta)
{
    __super::Update_GameObject(fTimeDelta);
  
    Move_UI(fTimeDelta);

    return NO_EVENT;
}

void CPlayer_Foot::LateUpdate_GameObject(const _float& fTimeDelta)
{
    __super::LateUpdate_GameObject(fTimeDelta);

    if (m_tInfo != CGlobal_Info::Get_Instance()->Get_PlayerInfo())
    {
        m_tInfo = CGlobal_Info::Get_Instance()->Get_PlayerInfo();
        Set_Texture();
    }
}

void CPlayer_Foot::Render_GameObject()
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

HRESULT CPlayer_Foot::Texture_Clone()
{
    CTexture::TEXINFO texInfo = {};

    // Kick
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 1;
    texInfo.m_fSpeed = 0.f;
    texInfo.m_bLoop = false;
    if (FAILED(Add_Components(L"Com_Texture_Foots_Kick", SCENE_STATIC, L"Prototype_Component_Texture_UIFootKick", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_Foots_Kick"), m_pTextureCom });

    // Slide
    texInfo.m_iStart = 0;
    texInfo.m_iEndTex = 1;
    texInfo.m_fSpeed = 1.f;
    texInfo.m_bLoop = true;
    if (FAILED(Add_Components(L"Com_Texture_Foots_Slide", SCENE_STATIC, L"Prototype_Component_Texture_UIFootSlide", (CComponent**)&m_pTextureCom, &texInfo)))
        return E_FAIL;
    m_mapTextures.insert({ TEXT("Com_Texture_Foots_Slide"), m_pTextureCom });

    return S_OK;
}

HRESULT CPlayer_Foot::Set_Texture()
{
    m_bRenderOn = true;

    switch (m_tInfo.ePlayerState)
    {
    case KICK:
    {
        if (FAILED(Change_Texture(TEXT("Com_Texture_Foots_Kick"))))
            return E_FAIL;

        Set_UISizeAndPos(240.f, 400.f, WINCX * 0.5f, WINCY * 0.5f + 200.f);

        Set_New_TransInfo(150.f, 0.f);


        m_tMoveInfo = { MV_UpDown, false, 15.f, 0.f };
    }
        break;

    case SLIED:
    {
        if (FAILED(Change_Texture(TEXT("Com_Texture_Foots_Slide"))))
            return E_FAIL;

        Set_UISizeAndPos(700.f, 480.f, WINCX * 0.5f, WINCY * 0.5f + 180);

        Set_New_TransInfo(80.f, 0.f);

        m_tMoveInfo = { MV_UpDown, false, 10.f, 0.f };
    }
        break;

    default:
        m_bRenderOn = false;
    }

    return S_OK;
}

HRESULT CPlayer_Foot::Change_Texture(const _tchar* pTextureTag)
{
    if (FAILED(__super::Change_Component(pTextureTag, (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    m_pTextureCom->Set_Zero_Frame();
    m_CurrentAnimTag = pTextureTag; // 현재 상태 저장
    return S_OK;
}

CPlayer_Foot* CPlayer_Foot::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CPlayer_Foot* pInstance = new CPlayer_Foot(pGraphicDev);
    if (FAILED(pInstance->Ready_GameObject()))
    {
        MSG_BOX("CPlayer_Foot Create Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CPlayer_Foot::Clone(void* pArg)
{
    CPlayer_Foot* pInstance = new CPlayer_Foot(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CPlayer_Foot Clone Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CPlayer_Foot::Free()
{
    __super::Free();
}