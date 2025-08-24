#include "Engine_Define.h"
#include "CDInputMgr.h"
#include "CFileManager.h"
#include "CGuiManager.h"
#include "CEditorPickingManager.h"
#include "CObjectManager.h"
#include "CEditorCamera.h"
#include "CScene_Tutorial.h"

CScene_Tutorial::CScene_Tutorial(LPDIRECT3DDEVICE9 pGraphicDevice)
	: CScene(pGraphicDevice)
{
}

CScene_Tutorial::~CScene_Tutorial()
{
}

void CScene_Tutorial::Free()
{
}

CScene_Tutorial *CScene_Tutorial::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
	CScene_Tutorial *pScene = new CScene_Tutorial(pGraphicDevice);
	if (FAILED(pScene->Ready_Scene()))
	{
		MSG_BOX("CScene_Tutorial::Create, Failed");
		Safe_Release(pScene);
		return nullptr;
	}

	return pScene;
}

HRESULT CScene_Tutorial::Ready_Scene()
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

_int CScene_Tutorial::Update_Scene(const _float &fTimeDelta)
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

void CScene_Tutorial::LateUpdate_Scene(const _float &fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CScene_Tutorial::Render_Scene()
{
}

HRESULT CScene_Tutorial::SaveData()
{
	if (FAILED(__super::SaveData()))
		return E_FAIL;

	CFileManager::GetInstance()->SaveDataFile(SCENE_TUTORIAL, L"Wall_Layer");
	CFileManager::GetInstance()->SaveDataFile(SCENE_TUTORIAL, L"Tile_Layer");
	CFileManager::GetInstance()->SaveDataFile(SCENE_TUTORIAL, L"Env_Layer");
	CFileManager::GetInstance()->SaveDataFile(SCENE_TUTORIAL, L"Monster_Layer");
	CFileManager::GetInstance()->SaveInstancedPrefabDataFile(SCENE_TUTORIAL);

	return S_OK;
}

HRESULT CScene_Tutorial::Ready_Camera_Layer(const _tchar *pLayerTag)
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

	if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Proto_Camera_Edit", SCENE_TUTORIAL, pLayerTag, &CamInfo)))
		return E_FAIL;

	return S_OK;
}

HRESULT CScene_Tutorial::Ready_Dummy_Layer(const _tchar *pLayerTag)
{
	return S_OK;
}

HRESULT CScene_Tutorial::Ready_EditLogic_Layer(const _tchar *pLayerTag)
{
	return S_OK;
}

HRESULT CScene_Tutorial::Ready_Wall_Layer(const _tchar *pLayerTag)
{
	return S_OK;
}

HRESULT CScene_Tutorial::Ready_Tile_Layer(const _tchar *pLayerTag)
{
	return S_OK;
}

HRESULT CScene_Tutorial::Ready_EnvObj_Layer(const _tchar *pLayerTag)
{
	return S_OK;
}

HRESULT CScene_Tutorial::Ready_Monster_Layer(const _tchar *pLayerTag)
{
	return S_OK;
}

HRESULT CScene_Tutorial::Ready_Light_Layer(const _tchar *pLayerTag)
{
	return S_OK;
}
