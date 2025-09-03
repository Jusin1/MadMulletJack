#include "pch.h"
#include "CCameraFPS.h"
#include "CMapFactory.h"
#include "CManagement.h"
#include "CObjectManager.h"
#include "CDInputMgr.h"
#include "Clinet_Define.h"
#include "CGlobal_Info.h"

CCameraFPS::CCameraFPS(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CCamera(pGraphicDev),
    m_bFix(false), m_bShaking(true),
    m_bRecoil(false), m_bZoom(false), m_eCamMode(CAM_NORMAL),
    m_fOffset(0.f), m_fZoomTime(0.f)
    , m_fDefaultFov(D3DXToRadian(60.f))
    , m_fZoomFov_Default(D3DXToRadian(28.f))
    , m_fZoomFov_Sniper(D3DXToRadian(14.f))
    , m_fCurFov(D3DXToRadian(60.f))
    , m_fZoomInSpeed(10.f)
    , m_fZoomOutSpeed(8.f)
    , m_fTargetOffset(0.f)
    , m_fCurOffset(0.f)
    , m_fZoomOffset_Default(0.35f)
    , m_fZoomOffset_Sniper(0.65f)
{
}

CCameraFPS::CCameraFPS(const CCameraFPS& rhs) : CCamera(rhs),
m_bFix(rhs.m_bFix), m_bShaking(rhs.m_bShaking),
m_bRecoil(rhs.m_bRecoil), m_bZoom(rhs.m_bZoom), m_eCamMode(rhs.m_eCamMode),
m_fOffset(rhs.m_fOffset), m_fZoomTime(rhs.m_fZoomTime),
// FOV/줌 관련 복사
m_fDefaultFov(rhs.m_fDefaultFov),
m_fZoomFov_Default(rhs.m_fZoomFov_Default), m_fZoomFov_Sniper(rhs.m_fZoomFov_Sniper),
m_fCurFov(rhs.m_fCurFov), m_fZoomInSpeed(rhs.m_fZoomInSpeed), m_fZoomOutSpeed(rhs.m_fZoomOutSpeed),
m_fTargetOffset(rhs.m_fTargetOffset), m_fCurOffset(rhs.m_fCurOffset),
m_fZoomOffset_Default(rhs.m_fZoomOffset_Default), m_fZoomOffset_Sniper(rhs.m_fZoomOffset_Sniper)
{

}


CCameraFPS::~CCameraFPS()
{
}



HRESULT CCameraFPS::Ready_GameObject()
{
    if (FAILED(CCamera::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCameraFPS::Initialize(void* pArg)
{
    if (FAILED(CCamera::Initialize(pArg)))
        return E_FAIL;

    // transform info 수정
    if (m_pTransformCom)
    {
        CTransform::TRANSFORMINFO tMyTransInfo = m_pTransformCom->GetTransformInfo();

        if(CMapFactory::GetInstance()->GetTargetSceneIndex() == SCENE_SNIPE)
            m_pTransformCom->SetTransformInfo({ tMyTransInfo.vStartPos, 0., tMyTransInfo.fRotationSpeed });
        else
            m_pTransformCom->SetTransformInfo({ tMyTransInfo.vStartPos, 10.f, tMyTransInfo.fRotationSpeed });

    }

    // 현재 프로젝션에서 기본 FOV를 읽어두고 싶다면, 여기서 m_fDefaultFov를 보정해도 됨.
    m_fCurFov = m_fDefaultFov;

    return S_OK;
}

_int CCameraFPS::Update_GameObject(const _float& fTimeDelta)
{
    CCamera::Update_GameObject(fTimeDelta);

    if (CGlobal_Info::Get_Instance()->Get_PlayerInfo().eWeapon != WP_SNIPER)
    {
        // zoom 상태일때
        if (CGlobal_Info::Get_Instance()->Get_PlayerInfo().ePlayerState == ZOOMING)
        {
            // 카메라의 look 방향으로 전진
            _vec3 vLook;
            vLook = m_pTransformCom->Get_Info(INFO_LOOK);


            m_pTransformCom->Move_PosDir(fTimeDelta, vLook);
            m_fZoomTime += fTimeDelta;
        }

        else if (CGlobal_Info::Get_Instance()->Get_PlayerInfo().ePlayerState == ZOOMOUT)
        {
            // 카메라의 look 방향으로 전진
            _vec3 vLook;
            vLook = m_pTransformCom->Get_Info(INFO_LOOK);
            vLook *= -1.f;
         
            m_pTransformCom->Move_PosDir(fTimeDelta, vLook);
            m_fZoomTime = 0.f;
        }
        // 플레이어의 위치를 가져와서 셋팅
        else
            Set_PlayerPos();
    }
    else
    {
        TickZoom(fTimeDelta);
        m_camInfo.fFov = m_fCurFov;
        Set_PlayerPos();
    }

    if (FAILED(Apply_ViewPorjection()))
        return NO_EVENT;
    return NO_EVENT;
}

void CCameraFPS::LateUpdate_GameObject(const _float& fTimeDelta)
{
    Engine::CCamera::LateUpdate_GameObject(fTimeDelta);



    if (false == m_bFix &&
        (CGlobal_Info::Get_Instance()->Get_PlayerInfo().ePlayerState != CLEAR &&
            CGlobal_Info::Get_Instance()->Get_PlayerInfo().ePlayerState != ZOOMING &&
            CGlobal_Info::Get_Instance()->Get_PlayerInfo().ePlayerState != ZOOMOUT))
    {
        Mouse_Move();
        Mouse_Fix();
    }
    
    if (CGlobal_Info::Get_Instance()->Get_PlayerInfo().eWeapon != WP_SNIPER)
    {
        if (CGlobal_Info::Get_Instance()->Get_PlayerInfo().ePlayerState == ZOOM ||
            CGlobal_Info::Get_Instance()->Get_PlayerInfo().ePlayerState == ATTACK_ZOOM)
        {
            _vec3 vLook;
            vLook = GetTransform()->Get_Info(INFO_LOOK);
            m_pTransformCom->Move_PosDir(m_fZoomTime, vLook);
        }
    }
    else
    {
        if (m_fCurOffset != 0.f)
        {
            _vec3 vLook = GetTransform()->Get_Info(INFO_LOOK);
            GetTransform()->Move_PosDir(m_fCurOffset, vLook);
        }
    }
    

    // 카메라의 월드행렬 적용
    if (FAILED(Apply_ViewPorjection()))
        return;
}

void CCameraFPS::Mouse_Move()
{
    _matrix matCamWorld;
    m_pTransformCom->Get_World(&matCamWorld);

    _long dwMouseMove(0);

    if (dwMouseMove = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Y))
    {
        _vec3 vRight = m_pTransformCom->Get_Info(INFO_RIGHT);
        _matrix matRot;
        D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(dwMouseMove / 10.f));
        matCamWorld = matCamWorld * matRot;
    }

    // 플레이어에게 y축 회전 넘겨줌
    Engine::CTransform* pPlayerTransformCom =
        dynamic_cast<CTransform*>(CObjectManager::GetInstance()->
            Get_Component(CManagement::GetInstance()->Get_CurrentSceneIdx(), L"Player_Layer", L"Com_Transform", 0));
    if (pPlayerTransformCom == nullptr)
        return;

    _vec3 vRight = { matCamWorld._11 * 1.f, matCamWorld._12, matCamWorld._13 };
    _vec3 vUp = { matCamWorld._21, matCamWorld._22 * 2.f, matCamWorld._23 };
    _vec3 vLook = { matCamWorld._31, matCamWorld._32, matCamWorld._33 * 1.f };

    pPlayerTransformCom->Set_Info(INFO_RIGHT, vRight);
    pPlayerTransformCom->Set_Info(INFO_UP, vUp);
    pPlayerTransformCom->Set_Info(INFO_LOOK, vLook);

    if (dwMouseMove = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_X))
    {
        _vec3 vUpAxis{ 0.f, 1.f, 0.f };
        _vec3 vLookTmp = m_pTransformCom->Get_Info(INFO_LOOK);
        _matrix matRot;
        D3DXMatrixRotationAxis(&matRot, &vUpAxis, D3DXToRadian(dwMouseMove / 10.f));
        D3DXVec3TransformNormal(&vLookTmp, &vLookTmp, &matRot);
        matCamWorld = matCamWorld * matRot;
    }

    vRight = { matCamWorld._11, matCamWorld._12, matCamWorld._13 };
    vUp = { matCamWorld._21, matCamWorld._22, matCamWorld._23 };
    vLook = { matCamWorld._31, matCamWorld._32, matCamWorld._33 };

    m_pTransformCom->Set_Info(INFO_RIGHT, vRight);
    m_pTransformCom->Set_Info(INFO_UP, vUp);
    m_pTransformCom->Set_Info(INFO_LOOK, vLook);
}

void CCameraFPS::Mouse_Fix()
{
    POINT ptMouse{ WINCX >> 1, WINCY >> 1 };
    ClientToScreen(g_hWnd, &ptMouse);
    SetCursorPos(ptMouse.x, ptMouse.y);
}

HRESULT CCameraFPS::Set_PlayerPos()
{
    Engine::CTransform* pPlayerTransformCom =
        dynamic_cast<CTransform*>(CObjectManager::GetInstance()->
            Get_Component(CManagement::GetInstance()->Get_CurrentSceneIdx(), L"Player_Layer", L"Com_Transform", 0));
    if (pPlayerTransformCom == nullptr)
        return E_FAIL;

    _vec3 vPlayerPos = pPlayerTransformCom->Get_Info(INFO_POS);
    m_pTransformCom->Set_Info(INFO_POS, vPlayerPos);
    return S_OK;
}

void CCameraFPS::Move_Shaking()
{
    if (!m_bShaking)
        return;

    switch (m_eCamMode)
    {
    case CAM_LEFT:
        //오른쪽 살짝 아래로 회전
        // z 축 양의 방향으로 회전
        m_bFix = true;
        break;
    case CAM_RIGHT:
        //왼쪽 살짝 아래로 회전
        // z 축 음의 방향으로 회전
        m_bFix = true;
        break;

    default:
        m_bFix = false;
    }
}


_bool CCameraFPS::IsZoomWanted() const
{
    const auto& st = CGlobal_Info::Get_Instance()->Get_PlayerInfo().ePlayerState;
    // “줌 유지” 계열 상태는 모두 줌 의도로 본다
    return (st == ZOOMING || st == ZOOM || st == ATTACK_ZOOM);
}

void CCameraFPS::TickZoom(const _float dt)
{
    const bool wantZoom = IsZoomWanted();


    const bool isSniper = (CGlobal_Info::Get_Instance()->Get_PlayerInfo().eWeapon == WP_SNIPER);
    const float targetFov = wantZoom ? (isSniper ? m_fZoomFov_Sniper : m_fZoomFov_Default) : m_fDefaultFov;
    m_fTargetOffset = wantZoom ? (isSniper ? m_fZoomOffset_Sniper : m_fZoomOffset_Default) : 0.0f;

    const float inK = 1.f - expf(-m_fZoomInSpeed * dt);
    const float outK = 1.f - expf(-m_fZoomOutSpeed * dt);
    const float kFov = (targetFov < m_fCurFov) ? inK : outK;

    m_fCurFov = m_fCurFov + (targetFov - m_fCurFov) * kFov;

    const float kOff = (m_fTargetOffset > m_fCurOffset) ? inK : outK;
    m_fCurOffset = m_fCurOffset + (m_fTargetOffset - m_fCurOffset) * kOff;
}

CCameraFPS* CCameraFPS::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CCameraFPS* pCamera = new CCameraFPS(pGraphicDev);

    if (FAILED(pCamera->Ready_GameObject()))
    {
        Safe_Release(pCamera);
        MSG_BOX("FPSCamera Create Failed");
        return nullptr;
    }
    return pCamera;
}

CCamera* CCameraFPS::Clone(void* pArg)
{
    CCameraFPS* pInstance = new CCameraFPS(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("DynamicCamera Clone Failed");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCameraFPS::Free()
{
    Engine::CCamera::Free();
}