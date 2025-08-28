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
    m_fOffset(0.f)
{
}

CCameraFPS::CCameraFPS(const CCameraFPS& rhs) : CCamera(rhs),
m_bFix(rhs.m_bFix), m_bShaking(rhs.m_bShaking),
m_bRecoil(rhs.m_bRecoil), m_bZoom(rhs.m_bZoom), m_eCamMode(rhs.m_eCamMode),
m_fOffset(rhs.m_fOffset)
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

    return S_OK;
}

_int CCameraFPS::Update_GameObject(const _float& fTimeDelta)
{
    CCamera::Update_GameObject(fTimeDelta);
    
    // 플레이어의 위치를 가져와서 셋팅
    Set_PlayerPos();

    if (FAILED(Apply_ViewPorjection()))
        return NO_EVENT;

    // transform info 수정
    if (m_pTransformCom)
    {
        CTransform::TRANSFORMINFO tMyTransInfo = m_pTransformCom->GetTransformInfo();
        m_pTransformCom->SetTransformInfo({ tMyTransInfo.vStartPos, 10.f, tMyTransInfo.fRotationSpeed });
    }

    return NO_EVENT;
}

void CCameraFPS::LateUpdate_GameObject(const _float& fTimeDelta)
{
    Engine::CCamera::LateUpdate_GameObject(fTimeDelta);

    //Set_PlayerPos();
    Move_Shaking();

    // 마우스로 바라보는 방향 조절
    // fix가 아니고 clear가 아닐때
    // 카메라 월드행렬 완성
    if (false == m_bFix &&
        CGlobal_Info::Get_Instance()->Get_PlayerInfo().ePlayerState != CLEAR)
    {
        Mouse_Move();
        Mouse_Fix();
    }

    // zoom 상태일때
    if (CGlobal_Info::Get_Instance()->Get_PlayerInfo().ePlayerState == ZOOM ||
        CGlobal_Info::Get_Instance()->Get_PlayerInfo().ePlayerState == ATTACK_ZOOM)
    {
        // 카메라의 look 방향으로 전진
        _vec3 vLook;
        vLook = m_pTransformCom->Get_Info(INFO_LOOK);
        m_pTransformCom->Move_PosDir(1.f, vLook);
    }

    // 카메라의 월드행렬 적용
    if (FAILED(Apply_ViewPorjection()))
        return;
}

void CCameraFPS::Mouse_Move()
{
    _matrix   matCamWorld;
    m_pTransformCom->Get_World(&matCamWorld);

    _vec3 eye = m_pTransformCom->Get_Info(INFO_POS);


    _long   dwMouseMove(0);

    if (dwMouseMove = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Y))
    {
        _vec3   vRight = m_pTransformCom->Get_Info(INFO_RIGHT);


        _vec3   vLook = m_pTransformCom->Get_Info(INFO_LOOK);
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

    // 행렬에서 벡터 추출 -> 플레이어에게 적용
    _vec3 vRight = { matCamWorld._11 * 1.f, matCamWorld._12, matCamWorld._13 };
    _vec3 vUp = { matCamWorld._21, matCamWorld._22 * 2.f, matCamWorld._23 };
    _vec3 vLook = { matCamWorld._31, matCamWorld._32, matCamWorld._33 * 1.f };

    pPlayerTransformCom->Set_Info(INFO_RIGHT, vRight);
    pPlayerTransformCom->Set_Info(INFO_UP, vUp);
    pPlayerTransformCom->Set_Info(INFO_LOOK, vLook);

    if (dwMouseMove = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_X))
    {
        _vec3   vUp{ 0.f, 1.f, 0.f };

        _vec3   vLook = m_pTransformCom->Get_Info(INFO_LOOK);
        _matrix matRot;

        D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(dwMouseMove / 10.f));
        D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

        matCamWorld = matCamWorld * matRot;
    }

    // 행렬에서 벡터 추출
    vRight = { matCamWorld._11, matCamWorld._12, matCamWorld._13 };
    vUp = { matCamWorld._21, matCamWorld._22, matCamWorld._23 };
    vLook = { matCamWorld._31, matCamWorld._32, matCamWorld._33};

    m_pTransformCom->Set_Info(INFO_RIGHT, vRight);
    m_pTransformCom->Set_Info(INFO_UP, vUp);
    m_pTransformCom->Set_Info(INFO_LOOK, vLook);
}

void CCameraFPS::Mouse_Fix()
{
    POINT   ptMouse{ WINCX >> 1, WINCY >> 1 };

    ClientToScreen(g_hWnd, &ptMouse);
    SetCursorPos(ptMouse.x, ptMouse.y);
}

HRESULT CCameraFPS::Set_PlayerPos()
{
    // 플레이어의 위치를 가져와서 셋팅 -> z는 살짝 뒤로
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

    // z축 회전
    // 회전 행렬을 구해서 곱해서 전해줌
    switch (m_eCamMode)
    {
    case CAM_LEFT:
        //오른쪽 살짝 아래로 회전
        m_bFix = true;
        break;
    case CAM_RIGHT:
        //왼쪽 살짝 아래로 회전
        m_bFix = true;
        break;
    }
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