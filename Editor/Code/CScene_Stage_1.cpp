#include "Engine_Define.h"
#include "CDInputMgr.h"
#include "CFileManager.h"
#include "CGuiManager.h"
#include "CEditorPickingManager.h"
#include "CObjectManager.h"
#include "CEditorCamera.h"
#include "CScene_Stage_1.h"

CScene_Stage_1::CScene_Stage_1(LPDIRECT3DDEVICE9 pGraphicDevice)
	: CScene(pGraphicDevice)
{
}

CScene_Stage_1::~CScene_Stage_1()
{
}

void CScene_Stage_1::Free()
{
    Engine::CScene::Free();
}

CScene_Stage_1 *CScene_Stage_1::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
    CScene_Stage_1 *pScene = new CScene_Stage_1(pGraphicDevice);
    if (FAILED(pScene->Ready_Scene()))
    {
        MSG_BOX("CScene_Stage_1::Create, Failed");
        Safe_Release(pScene);
        return nullptr;
    }

    return pScene;
}

HRESULT CScene_Stage_1::Ready_Scene()
{
    if (FAILED(Engine::CScene::Ready_Scene()))
        return E_FAIL;

    if (FAILED(Ready_Camera_Layer(L"Camera_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Dummy_Layer(L"Dummy_Layer")))
        return E_FAIL;

    if (FAILED(Ready_EditLogic_Layer(L"EditLogic_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Wall_Layer(L"Wall_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Tile_Layer(L"Tile_Layer")))
        return E_FAIL;

    if (FAILED(Ready_EnvObj_Layer(L"Env_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Monster_Layer(L"Monster_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Light_Layer(L"Light_Layer")))
        return E_FAIL;

    if (FAILED(CEditorPickingManager::GetInstance()->Ready_Picking()))
        return E_FAIL;

    return S_OK;
}

_int CScene_Stage_1::Update_Scene(const _float &fTimeDelta)
{
    _int iExit = Engine::CScene::Update_Scene(fTimeDelta);
    if (CGuiManager::GetInstance()->IsCreateMode())
    {
        CEditorPickingManager::GetInstance()->Picking_ForDummy();
    }
    else
    {
        CEditorPickingManager::GetInstance()->Picking();
    }
    return iExit;
}

void CScene_Stage_1::LateUpdate_Scene(const _float &fTimeDelta)
{
    Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CScene_Stage_1::Render_Scene()
{
}

HRESULT CScene_Stage_1::SaveData()
{
    if (FAILED(__super::SaveData()))
        return E_FAIL;

    CFileManager::GetInstance()->SaveDataFile(SCENE_STAGE_1, L"Wall_Layer");
    CFileManager::GetInstance()->SaveDataFile(SCENE_STAGE_1, L"Tile_Layer");
    CFileManager::GetInstance()->SaveDataFile(SCENE_STAGE_1, L"Env_Layer");
    CFileManager::GetInstance()->SaveDataFile(SCENE_STAGE_1, L"Monster_Layer");

    return S_OK;
}

HRESULT CScene_Stage_1::Ready_Camera_Layer(const _tchar *pLayerTag)
{
    CEditorCamera::CAMINFO				CamInfo;
    ::ZeroMemory(&CamInfo, sizeof(CEditorCamera::CAMINFO));

    CamInfo.vEye = _vec3(0.f, 2.f, -5.f);
    CamInfo.vAt = _vec3(0.f, 0.f, 0.f);

    CamInfo.fFov = D3DXToRadian(60.0f);
    CamInfo.fAspect = (_float)WINCX / WINCY;
    CamInfo.fNear = 0.1f;
    CamInfo.fFar = 1000.f;

    CamInfo.TransformInfo.fSpeed = 10.f;
    CamInfo.TransformInfo.fRotationSpeed = D3DXToRadian(90.0f);

    if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Proto_Camera_Edit", SCENE_STAGE_1, pLayerTag, &CamInfo)))
        return E_FAIL;

    return S_OK;
}

HRESULT CScene_Stage_1::Ready_Dummy_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}

HRESULT CScene_Stage_1::Ready_EditLogic_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}

HRESULT CScene_Stage_1::Ready_Wall_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}

HRESULT CScene_Stage_1::Ready_Tile_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}

HRESULT CScene_Stage_1::Ready_EnvObj_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}

HRESULT CScene_Stage_1::Ready_Monster_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}

HRESULT CScene_Stage_1::Ready_Light_Layer(const _tchar *pLayerTag)
{
    return S_OK;
}
