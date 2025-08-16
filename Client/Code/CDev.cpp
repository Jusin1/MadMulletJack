#include "pch.h"
#include "CDev.h"
#include "CBackGround.h"
#include "CObjectManager.h"
#include "CPlayer.h"
#include "CMonster.h"
#include "CTerrain.h"
#include "CDynamicCamera.h"
#include "CSkyBox.h"
#include "CPickingManager.h"
#include "CUIManager.h"
#include "CGlobal_Info.h"

CDev::CDev(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CScene(pGraphicDev)
{

}

CDev::~CDev()
{
}

HRESULT CDev::Ready_Scene()
{
    if (FAILED(CScene::Ready_Scene()))
        return E_FAIL;

    if (FAILED(Ready_Wall_Layer(L"Wall_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Wall_Layer(L"Tile_Layer")))
        return E_FAIL;
    
    if (FAILED(Ready_Wall_Layer(L"Trigger_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Wall_Layer(L"EnvObj_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Camera_Layer(L"Camera_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Player_Layer(L"Player_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Monster_Layer(L"Monster_Layer")))
        return E_FAIL;

    if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(L"UI_Layer")))
        return E_FAIL;

    CPickingManager::GetInstance()->Ready_Picking();

    return S_OK;
}

_int CDev::Update_Scene(const _float &fTimeDelta)
{
    _int iExit = Engine::CScene::Update_Scene(fTimeDelta);

    // 디버깅용 
    static _bool bPrevF1 = false;

    if (GetAsyncKeyState(VK_F1) & 0x8000)
    {
        if (!bPrevF1)
        {
            g_ColiderRender = !g_ColiderRender;
            bPrevF1 = true;
        }
    }
    else
    {
        bPrevF1 = false;
    }

    CPickingManager::GetInstance()->Picking();
    return iExit;
}

void CDev::LateUpdate_Scene(const _float &fTimeDelta)
{
    Engine::CScene::LateUpdate_Scene(fTimeDelta);

    // 테스트용
    if (GetAsyncKeyState('P'))
    {
        CUIManager::GetInstance()->CreateEnterUI();
    }
}

void CDev::Render_Scene()
{
    // 디버깅용 코드
}

HRESULT CDev::Ready_Environment_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}

HRESULT CDev::Ready_Wall_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}

HRESULT CDev::Ready_Tile_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}

HRESULT CDev::Ready_Trigger_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}

HRESULT CDev::Ready_EnvObj_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}

HRESULT CDev::Ready_Camera_Layer(const _tchar *pLayerTag)
{
    // Camera
    CDynamicCamera::CAMINFO				CamInfo;
    ZeroMemory(&CamInfo, sizeof(CDynamicCamera::CAMINFO));

    CamInfo.vEye = _vec3(0.f, 2.f, -5.f);
    CamInfo.vAt = _vec3(0.f, 0.f, 0.f);

    CamInfo.fFov = D3DXToRadian(60.0f);
    CamInfo.fAspect = (_float)WINCX / WINCY;
    CamInfo.fNear = 0.1f;
    CamInfo.fFar = 1000.f;

    CamInfo.TransformInfo.fSpeed = 10.f;
    CamInfo.TransformInfo.fRotationSpeed = D3DXToRadian(90.0f);

    if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Prototype_GameObject_Camera_FPS", SCENE_STAGE_1, pLayerTag, &CamInfo)))
        return E_FAIL;


    return S_OK;
}

HRESULT CDev::Ready_Player_Layer(const _tchar *pLayerTag)
{
    // Player
    if (FAILED(CObjectManager::GetInstance()->Add_GameObject(TEXT("Prototype_GameObject_Player"), SCENE_STAGE_1, pLayerTag)))
        return E_FAIL;
    return S_OK;
}

HRESULT CDev::Ready_Monster_Layer(const _tchar *pLayerTag)
{
    const float baseX = -8.f;
    const float gap = 4.f;
    const float posY = 1.f;
    const float posZ = 0.f;

    for (int i = 0; i < 5; ++i) {
        if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Prototype_GameObject_Monster_Suit", SCENE_STAGE_1, pLayerTag))) {
            MSG_BOX("Monster spawn failed");
            // 실패해도 계속 가려면 continue
            return E_FAIL;

        }
    }

    for (int i = 0; i < 5; ++i) {
        auto tr = dynamic_cast<CTransform *>(
            CObjectManager::GetInstance()->Get_Component(SCENE_STAGE_1, pLayerTag, L"Com_Transform", i));
        if (tr) {
            const float x = baseX + gap * i;           // 좌→우로 늘어놓기
            tr->Set_Info(INFO_POS, _vec3(x, posY, posZ));
            tr->LookAt(_vec3(x, posY, posZ + 1.f));    // 필요하면 정면 보정
        }
    }
    return S_OK;
}

HRESULT CDev::Ready_GameLogic_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}

HRESULT CDev::Ready_UI_Layer(const _tchar *pLayerTag)
{
    if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Prototype_GameObject_UIRoot", SCENE_STAGE_1, pLayerTag)))
        return E_FAIL;



    return S_OK;
}



CDev *CDev::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CDev *pStage = new CDev(pGraphicDev);

    if (FAILED(pStage->Ready_Scene()))
    {
        MSG_BOX("pStage Create Failed");
        Safe_Release(pStage);
        return nullptr;
    }

    return pStage;
}

void CDev::Free()
{
    Engine::CScene::Free();

    CGlobal_Info::Destroy_Instance();
}
