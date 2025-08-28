#include "pch.h"
#include "CTutorial.h"
#include "CBackGround.h"
#include "CDataManager.h"
#include "CFileManager.h"
#include "CMapFactory.h"
#include "CObjectManager.h"
#include "CPlayer.h"
#include "CMonster.h"
#include "CGameDataManager.h"
#include "CDynamicCamera.h"
#include "CSkyBox.h"
#include "CPickingManager.h"
#include "CUIManager.h"
#include "CGlobal_Info.h"
#include "CTutorialTracker.h"
#include "CTutorialUI.h"

CTutorial::CTutorial(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CScene(pGraphicDev)
{

}

CTutorial::~CTutorial()
{
}

HRESULT CTutorial::Ready_Scene()
{
    if (FAILED(CScene::Ready_Scene()))
        return E_FAIL;

    SetData(SCENE_TUTORIAL);

    if (FAILED(Ready_Wall_Layer(L"Wall_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Tile_Layer(L"Tile_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Floor_Layer(L"Floor_Layer")))
        return E_FAIL;

    if (FAILED(Ready_SlideWall_Layer(L"SlideWall_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Ceiling_Layer(L"Ceiling_Layer")))
        return E_FAIL;

    /*if (FAILED(Ready_Camera_Layer(L"Camera_Layer")))
        return E_FAIL;*/

    if (FAILED(Ready_EnvObj_Layer(L"Env_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Player_Layer(L"Player_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Camera_Layer(L"Camera_Layer")))
        return E_FAIL;

    if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(L"UI_Layer")))
        return E_FAIL;

    // GameDataManager에 바닥을 z기준 정렬
    CGameDataManager::GetInstance()->Bind_FloorList(CObjectManager::GetInstance()->Get_ObjectList(SCENE_TUTORIAL, L"Floor_Layer"));

    if (FAILED(Ready_Monster_Layer(L"Monster_Layer")))
        return E_FAIL;

    CPickingManager::GetInstance()->Ready_Picking();

    return S_OK;
}

_int CTutorial::Update_Scene(const _float& fTimeDelta)
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
    CUIManager::GetInstance()->Update(fTimeDelta);
    auto p = CObjectManager::GetInstance();
    return iExit;
}

void CTutorial::LateUpdate_Scene(const _float& fTimeDelta)
{
    Engine::CScene::LateUpdate_Scene(fTimeDelta);
    if (GetAsyncKeyState('5') & 0x8000)
    {
        CTutorialTracker::Get().Notify_Door();
    }
    if (GetAsyncKeyState('7') & 0x8000)
    {
        CTutorialTracker::Get().Notify_Soda();
    }
}

void CTutorial::Render_Scene()
{
    // 디버깅용 코드
}

HRESULT CTutorial::Ready_SlideWall_Layer(const _tchar* pLayerTag)
{
    return S_OK;
}

HRESULT CTutorial::Ready_Floor_Layer(const _tchar* pLayerTag)
{
    return S_OK;
}

HRESULT CTutorial::Ready_Wall_Layer(const _tchar* pLayerTag)
{
    InstancingObjects(L"Wall_Layer");
    return S_OK;
}

HRESULT CTutorial::Ready_Ceiling_Layer(const _tchar* pLayerTag)
{
    return S_OK;
}

HRESULT CTutorial::Ready_Tile_Layer(const _tchar* pLayerTag)
{
    InstancingObjects(L"Tile_Layer");
    return S_OK;
}

HRESULT CTutorial::Ready_EnvObj_Layer(const _tchar* pLayerTag)
{
    InstancingObjects(L"Env_Layer");
    return S_OK;
}

HRESULT CTutorial::Ready_Camera_Layer(const _tchar* pLayerTag)
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

    if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Prototype_GameObject_Camera_FPS", SCENE_TUTORIAL, pLayerTag, &CamInfo)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTutorial::Ready_Player_Layer(const _tchar* pLayerTag)
{
    return S_OK;
}

HRESULT CTutorial::Ready_Monster_Layer(const _tchar* pLayerTag)
{
    InstancingObjects(L"Monster_Layer");

    if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Prototype_GameObject_Monster_Soldier", SCENE_DEV, pLayerTag, nullptr)))
    return E_FAIL;

    return S_OK;
}

HRESULT CTutorial::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
    return S_OK;
}

HRESULT CTutorial::Ready_UI_Layer(const _tchar* pLayerTag)
{
    if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Prototype_GameObject_UIRoot", SCENE_TUTORIAL, pLayerTag)))
        return E_FAIL;

    // CLevel_Tutorial::Ready_Layer_UI()
    if (CTutorialTracker::Get().HasPending()) {
        auto* ui = dynamic_cast<CTutorialUI*>(
            CObjectManager::GetInstance()->Clone_GameObject(
                L"Prototype_GameObject_TutorialUI", SCENE_TUTORIAL, L"UI_Layer"));
    }

    return S_OK;
}



void CTutorial::SetData(_uint _iSceneIndex)
{
    CDataManager::GetInstance()->Clear();
    CGameDataManager::GetInstance()->AllClear();

    CFileManager::GetInstance()->LoadDataFile(_iSceneIndex, L"Wall_Layer");
    CFileManager::GetInstance()->LoadDataFile(_iSceneIndex, L"Tile_Layer");
    CFileManager::GetInstance()->LoadDataFile(_iSceneIndex, L"Env_Layer");
    CFileManager::GetInstance()->LoadDataFile(_iSceneIndex, L"Monster_Layer");

    CMapFactory::GetInstance()->SetTargetSceneIndex(_iSceneIndex);
}

void CTutorial::InstancingObjects(const wstring& _Layer)
{
    if (vector<MAPOBJECTDATA>* pVecData = CDataManager::GetInstance()->FindData(_Layer))
    {
        for (MAPOBJECTDATA& element : *pVecData)
        {
            CMapFactory::GetInstance()->Create(element.eCategory, element.iType, &element);
        }
    }
}

CTutorial* CTutorial::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CTutorial* pTutorial = new CTutorial(pGraphicDev);

    if (FAILED(pTutorial->Ready_Scene()))
    {
        MSG_BOX("pTutorial Create Failed");
        Safe_Release(pTutorial);
        return nullptr;
    }

    return pTutorial;
}

void CTutorial::Free()
{
    Engine::CScene::Free();

    CGlobal_Info::Destroy_Instance();
}
