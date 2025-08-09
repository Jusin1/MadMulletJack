#include "CEditorScene.h"
#include "Editor_Define.h"
#include "CGridPanel.h"
#include "CDInputMgr.h"
#include "CFileManager.h"
#include "CGuiManager.h"
#include "CTexture.h"
#include "CEditorPickingManager.h"
#include "CObjectManager.h"
#include "CEditorCamera.h"

CEditorScene::CEditorScene(LPDIRECT3DDEVICE9 pGraphicDevice)
    : CScene(pGraphicDevice)
{
}

CEditorScene::~CEditorScene()
{
}

void CEditorScene::Free()
{
    Engine::CScene::Free();
}

CEditorScene *CEditorScene::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
    CEditorScene *pEditorScene = new CEditorScene(pGraphicDevice);
    if (FAILED(pEditorScene->Ready_Scene()))
    {
        MSG_BOX("EditorScene Create Failed");
        Safe_Release(pEditorScene);
        return nullptr;
    }

    return pEditorScene;
}

HRESULT CEditorScene::Ready_Scene()
{
    if (FAILED(Engine::CScene::Ready_Scene()))
        return E_FAIL;

    if (FAILED(Ready_Camera_Layer(L"Camera_Layer")))
        return E_FAIL;

    if (FAILED(Ready_EditLogic_Layer(L"EditLogic_Layer")))
        return E_FAIL;

    if (FAILED(CEditorPickingManager::GetInstance()->Ready_Picking()))
        return E_FAIL;

    return S_OK;
}

_int CEditorScene::Update_Scene(const _float &fTimeDelta)
{
    _int iExit = Engine::CScene::Update_Scene(fTimeDelta);

    CEditorPickingManager::GetInstance()->Picking();
    return iExit;
}

void CEditorScene::LateUpdate_Scene(const _float &fTimeDelta)
{
    Engine::CScene::LateUpdate_Scene(fTimeDelta);
    if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_1))
    {
        CFileManager::GetInstance()->SaveObjectList(L"test.json", SCENE_EDITOR, L"EditLogic_Layer");
    }
}

void CEditorScene::Render_Scene()
{
}

HRESULT CEditorScene::Ready_Camera_Layer(const _tchar *pLayerTag)
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

    if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Proto_Camera_Edit", SCENE_EDITOR, pLayerTag, &CamInfo)))
        return E_FAIL;

    return S_OK;
}

HRESULT CEditorScene::Ready_EditLogic_Layer(const _tchar *pLayerTag)
{
    if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_SamplePanel", SCENE_EDITOR, pLayerTag)))
        return E_FAIL;

    if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_SamplePanel", SCENE_EDITOR, pLayerTag)))
        return E_FAIL;

    
    CGameObject *pObj = CObjectManager::GetInstance()->Find_Object(SCENE_EDITOR, pLayerTag, 1);
    CGameObject *pObj2 = CObjectManager::GetInstance()->Find_Object(SCENE_EDITOR, pLayerTag, 0);
    static_cast<CGridPanel *>(pObj2)->Change_Texture(SCENE_STATIC,L"Proto_Component_Texture_PanelTest");
    static_cast<CGridPanel *>(pObj)->Change_Buffer(SCENE_LOADING, L"Proto_Component_Buffer_PanelTest");
    return S_OK;
}