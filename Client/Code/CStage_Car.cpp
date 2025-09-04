#include "pch.h"
#include "CStage_Car.h"
#include "CBackGround.h"
#include "CDataManager.h"
#include "CFileManager.h"
#include "CMapFactory.h"
#include "CObjectManager.h"
#include "CPrefabRecycleSystem.h"
#include "CPlayer.h"
#include "CMonster.h"
#include "CBoss.h"
#include "CObjectPoolManager.h"
#include "CGameDataManager.h"
#include "CDynamicCamera.h"
#include "CSkyBox.h"
#include "CPickingManager.h"
#include "CUIManager.h"
#include "CGlobal_Info.h"
#include "CTutorialTracker.h"
#include "CTutorialUI.h"
#include "Sound_Manager.h"

CStage_Car::CStage_Car(LPDIRECT3DDEVICE9 pGraphiCStage_Car)
    : Engine::CScene(pGraphiCStage_Car)
{

}

CStage_Car::~CStage_Car()
{
}

HRESULT CStage_Car::Ready_Scene()
{
    if (FAILED(CScene::Ready_Scene()))
        return E_FAIL;

    SetData(SCENE_CAR);

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

    if (FAILED(Ready_Prefab_Layer(L"Prefab_Layer")))
        return E_FAIL;

        // GameDataManager에 바닥을 z기준 정렬
    CGameDataManager::GetInstance()->Bind_FloorList(CObjectManager::GetInstance()->Get_ObjectList(SCENE_CAR, L"Floor_Layer"));

    if (FAILED(Ready_Monster_Layer(L"Monster_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Boss_Layer(L"Boss_Layer")))
        return E_FAIL;

    CPickingManager::GetInstance()->Ready_Picking();
    CObjectPoolManager::GetInstance()->Ready_Pools();

    CPrefabRecycleSystem *pPrefabSystem = CPrefabRecycleSystem::GetInstance();
    if (FAILED(pPrefabSystem->Set_Player(CObjectManager::GetInstance()->Get_ObjectList(SCENE_CAR, L"Player_Layer")->front())))
        return E_FAIL;

    if (FAILED(pPrefabSystem->Set_PrefabList(CObjectManager::GetInstance()->Get_ObjectList(SCENE_CAR, L"Prefab_Layer"))))
        return E_FAIL;

    CSound_Manager::GetInstance()->PlayBGM(L"../Bin//Resource/Sounds/mx_stage_02.wav", 0.6f, true);

    return S_OK;
}

_int CStage_Car::Update_Scene(const _float &fTimeDelta)
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
    CPrefabRecycleSystem::GetInstance()->Update();

    return iExit;
}

void CStage_Car::LateUpdate_Scene(const _float &fTimeDelta)
{
    Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CStage_Car::Render_Scene()
{
    // 디버깅용 코드
}

HRESULT CStage_Car::Ready_SlideWall_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}

HRESULT CStage_Car::Ready_Floor_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}

HRESULT CStage_Car::Ready_Wall_Layer(const _tchar *pLayerTag)
{
    InstancingObjects(L"Wall_Layer");
    return S_OK;
}

HRESULT CStage_Car::Ready_Ceiling_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}

HRESULT CStage_Car::Ready_Tile_Layer(const _tchar *pLayerTag)
{
    InstancingObjects(L"Tile_Layer");
    return S_OK;
}

HRESULT CStage_Car::Ready_Prefab_Layer(const _tchar *pLayerTag)
{
    InstancingPrefabs();
    return S_OK;
}

HRESULT CStage_Car::Ready_EnvObj_Layer(const _tchar *pLayerTag)
{
    InstancingObjects(L"Env_Layer");

    if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Prototype_GameObject_Skybox", SCENE_CAR, L"Skybox_Layer")))
        return E_FAIL;

    return S_OK;
}

HRESULT CStage_Car::Ready_Camera_Layer(const _tchar *pLayerTag)
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

    if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Prototype_GameObject_Camera_FPS", SCENE_CAR, pLayerTag, &CamInfo)))
        return E_FAIL;


    return S_OK;
}

HRESULT CStage_Car::Ready_Player_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}

HRESULT CStage_Car::Ready_Monster_Layer(const _tchar *pLayerTag)
{
    InstancingObjects(L"Monster_Layer");

    return S_OK;
}

HRESULT CStage_Car::Ready_GameLogic_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}

HRESULT CStage_Car::Ready_UI_Layer(const _tchar *pLayerTag)
{
    if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Prototype_GameObject_UIRoot", SCENE_CAR, pLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CStage_Car::Ready_Boss_Layer(const _tchar *pLayerTag)
{
    CGameObject *pGo2 = CObjectManager::GetInstance()->Get_ObjectList(SCENE_CAR, L"Player_Layer")->front();
    if (!pGo2) return E_FAIL;
    CPlayer *pPlayer = static_cast<CPlayer *>(pGo2);
    _float fNormalSpeed = pPlayer->Get_NormalSpeed();

    RigidBodyConfig tConfig;
    tConfig.fHealth = 33.f;
    tConfig.fMoveSpeed = fNormalSpeed;
    tConfig.fDash_Speed = fNormalSpeed * 1.5f;
    CGameObject *pGo = CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_Boss", SCENE_CAR, L"Boss_Layer", &tConfig);
    if (!pGo) return E_FAIL;
    CBoss *pBoss = static_cast<CBoss *>(pGo);
    pBoss->Set_Player(pGo2);
    pBoss->Set_LinearLR(_vec3{ 4.f, 0.f, 35.f }, _vec3{ 20.f, 0.f, 35.f }, 8.f);

    return S_OK;
}



void CStage_Car::SetData(_uint _iSceneIndex)
{
    CDataManager::GetInstance()->Clear();
    CGameDataManager::GetInstance()->AllClear();
    CPickingManager::GetInstance()->Clear_Picking();

    CFileManager::GetInstance()->LoadDataFile(_iSceneIndex, L"Wall_Layer");
    CFileManager::GetInstance()->LoadDataFile(_iSceneIndex, L"Tile_Layer");
    CFileManager::GetInstance()->LoadDataFile(_iSceneIndex, L"Env_Layer");
    CFileManager::GetInstance()->LoadDataFile(_iSceneIndex, L"Monster_Layer");
    for (int i = 0; i < g_PrefabTypeCount; ++i)
    {
        CFileManager::GetInstance()->LoadPrefabDataFile(static_cast<PrefabType>(i));
    }
    CFileManager::GetInstance()->LoadInstancedPrefabDataFile(_iSceneIndex);

    CMapFactory::GetInstance()->SetTargetSceneIndex(_iSceneIndex);
}

void CStage_Car::InstancingObjects(const wstring &_Layer)
{
    if (vector<MAPOBJECTDATA> *pVecData = CDataManager::GetInstance()->FindData(_Layer))
    {
        for (MAPOBJECTDATA &element : *pVecData)
        {
            CMapFactory::GetInstance()->Create(element.eCategory, element.iType, &element);
        }
    }
}

void CStage_Car::InstancingPrefabs()
{
    if (vector<PREFABDATA> *pVecData = CDataManager::GetInstance()->GetInstancedPrefabDataList())
    {
        for (PREFABDATA &element : *pVecData)
        {
            CMapFactory::GetInstance()->Create(ObjectCategory::PREFAB, static_cast<_uint>(element.eType), &element);
        }
    }
}

CStage_Car *CStage_Car::Create(LPDIRECT3DDEVICE9 pGraphiCStage_Car)
{
    CStage_Car *pStage = new CStage_Car(pGraphiCStage_Car);

    if (FAILED(pStage->Ready_Scene()))
    {
        MSG_BOX("pStage Create Failed");
        Safe_Release(pStage);
        return nullptr;
    }

    return pStage;
}

void CStage_Car::Free()
{
    Engine::CScene::Free();

    CGlobal_Info::Destroy_Instance();
    CPrefabRecycleSystem::DestroyInstance();
}
