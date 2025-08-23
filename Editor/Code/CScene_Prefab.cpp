#include "Engine_Define.h"
#include "CDInputMgr.h"
#include "CFileManager.h"
#include "CGuiManager.h"
#include "CEditorPickingManager.h"
#include "CObjectManager.h"
#include "CEditorCamera.h"
#include "CScene_Prefab.h"

CScene_Prefab::CScene_Prefab(LPDIRECT3DDEVICE9 pGraphicDevice)
	: CScene(pGraphicDevice)
{
}

CScene_Prefab::~CScene_Prefab()
{
	
}

void CScene_Prefab::Free()
{
	Engine::CScene::Free();
}

CScene_Prefab *CScene_Prefab::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
	CScene_Prefab *pScene = new CScene_Prefab(pGraphicDevice);
	if (FAILED(pScene->Ready_Scene()))
	{
		MSG_BOX("CScene_Prefab::Create, Failed");
		Safe_Release(pScene);
		return nullptr;
	}

	return pScene;
}

HRESULT CScene_Prefab::Ready_Scene()
{
	if (FAILED(Engine::CScene::Ready_Scene()))
		return E_FAIL;

	if (FAILED(Ready_Camera_Layer(L"Camera_Layer")))
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

	if (FAILED(Ready_Prefab_Layer(L"Prefab_Layer")))
		return E_FAIL;

	if (FAILED(CEditorPickingManager::GetInstance()->Ready_Picking()))
		return E_FAIL;

	return S_OK;
}

_int CScene_Prefab::Update_Scene(const _float &fTimeDelta)
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

void CScene_Prefab::LateUpdate_Scene(const _float &fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CScene_Prefab::Render_Scene()
{
}

HRESULT CScene_Prefab::LoadData()
{
	if (FAILED(__super::LoadData()))
		return E_FAIL;

	_uint i = CGuiManager::GetInstance()->GetObjectType();
	CFileManager::GetInstance()->LoadPrefabDataFile(static_cast<PrefabType>(i));
	
	return S_OK;
}

HRESULT CScene_Prefab::SaveData()
{
	if (FAILED(__super::SaveData()))
		return E_FAIL;

	_uint i = CGuiManager::GetInstance()->GetObjectType();
	CFileManager::GetInstance()->SavePrefabDataFile(static_cast<PrefabType>(i));

	return S_OK;
}

HRESULT CScene_Prefab::Ready_Camera_Layer(const _tchar *pLayerTag)
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

HRESULT CScene_Prefab::Ready_Wall_Layer(const _tchar *pLayerTag)
{
	return S_OK;
}

HRESULT CScene_Prefab::Ready_Tile_Layer(const _tchar *pLayerTag)
{
	return S_OK;
}

HRESULT CScene_Prefab::Ready_EnvObj_Layer(const _tchar *pLayerTag)
{
	return S_OK;
}

HRESULT CScene_Prefab::Ready_Monster_Layer(const _tchar *pLayerTag)
{
	return S_OK;
}

HRESULT CScene_Prefab::Ready_Light_Layer(const _tchar *pLayerTag)
{
	return S_OK;
}

HRESULT CScene_Prefab::Ready_Prefab_Layer(const _tchar *pLayerTag)
{
	return S_OK;
}
