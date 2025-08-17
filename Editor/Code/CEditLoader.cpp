#include "CEditLoader.h"
#include "CObjectManager.h"
#include "CComponentMgr.h"
#include "CEditorCamera.h"
#include "CDataManager.h"
#include "CFileManager.h"
#include "CMapFactory.h"
#include "CVIBuffer_GridPanel_Editor.h"
#include "CVIBuffer_Cube_Color.h"
#include "CManagement.h"
#include "CDummyPlacementObject.h"
#include "CPlacementObject.h"
#include "CDummyTile.h"
#include "CTile.h"
#include "CGridPanel.h"
#include <process.h>

CEditLoader::CEditLoader(LPDIRECT3DDEVICE9 pGraphicDevice)
	: m_pGraphicDevice(pGraphicDevice)
{
	m_pGraphicDevice->AddRef();
	::ZeroMemory(m_szLoading, sizeof(m_szLoading));
}

CEditLoader::~CEditLoader()
{
}

unsigned int APIENTRY Editor_Thread_Main(void *pArg)
{
	CEditLoader *pLoader = (CEditLoader*)pArg;

	EnterCriticalSection(pLoader->Get_Crt());

	switch (pLoader->Get_NextSceneID())
	{
	case SCENE_DEV:
	{
		pLoader->Loading_Editor();
	} break;
	case SCENE_TUTORIAL:
	{
		pLoader->Loading_Tutorial();
	} break;
	case SCENE_STAGE_1:
	{
		pLoader->Loading_Stage_1();
	} break;
	case SCENE_STAGE_2:
	{
		// TODO
	} break;
	case SCENE_STAGE_3:
	{
		// TODO
	} break;
	case SCENE_SNIPE:
	{
		// TODO
	} break;
	case SCENE_BOSS:
	{
		// TODO
	} break;
	case SCENE_CAR:
	{
		// TODO
	} break;
	}
	LeaveCriticalSection(pLoader->Get_Crt());
	return 0;
}

void CEditLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_Crt);

	Safe_Release(m_pGraphicDevice);
}

CEditLoader *CEditLoader::Create(LPDIRECT3DDEVICE9 pGrahpicDev, SCENE eNextScene)
{
	CEditLoader *pInstance = new CEditLoader(pGrahpicDev);

	if (FAILED(pInstance->Ready_Loading(eNextScene)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

HRESULT CEditLoader::Ready_Loading(SCENE eNextScene)
{
	m_eNextScene = eNextScene;

	InitializeCriticalSection(&m_Crt);

	m_hThread = (HANDLE)_beginthreadex
	(
		NULL,
		0,
		Editor_Thread_Main,
		this,
		0,
		NULL);

	if (m_hThread == 0)
		return E_FAIL;

	return S_OK;
}

HRESULT CEditLoader::Loading_Editor()
{
	SetData(SCENE_DEV);

	lstrcpy(m_szLoading, L"텍스쳐 로딩 중");

	// 객체 생성
	lstrcpy(m_szLoading, L"객체 생성 중.");
	InstancingObjects(L"Wall_Layer");
	InstancingObjects(L"Tile_Layer");
	InstancingObjects(L"Env_Layer");
	InstancingObjects(L"Monster_Layer");
	lstrcpy(m_szLoading, TEXT("모델 로딩 중."));

	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CEditLoader::Loading_Tutorial()
{
	SetData(SCENE_TUTORIAL);

	lstrcpy(m_szLoading, L"텍스쳐 로딩 중");

	// 객체 생성
	lstrcpy(m_szLoading, L"객체 생성 중.");
	InstancingObjects(L"Wall_Layer");
	InstancingObjects(L"Tile_Layer");
	InstancingObjects(L"Env_Layer");
	InstancingObjects(L"Monster_Layer");
	lstrcpy(m_szLoading, TEXT("모델 로딩 중."));

	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;
	return S_OK;
}

HRESULT CEditLoader::Loading_Stage_1()
{
	SetData(SCENE_STAGE_1);

	lstrcpy(m_szLoading, L"텍스쳐 로딩 중");
	
	// 객체 생성
	lstrcpy(m_szLoading, L"객체 생성 중.");
	InstancingObjects(L"Wall_Layer");
	InstancingObjects(L"Tile_Layer");
	InstancingObjects(L"Env_Layer");
	InstancingObjects(L"Monster_Layer");

	lstrcpy(m_szLoading, TEXT("모델 로딩 중."));

	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CEditLoader::Loading_Stage_2()
{
	SetData(SCENE_STAGE_2);

	return S_OK;
}

HRESULT CEditLoader::Loading_Stage_3()
{
	SetData(SCENE_STAGE_3);

	return S_OK;
}

HRESULT CEditLoader::Loading_Snipe()
{
	SetData(SCENE_SNIPE);

	return S_OK;
}

HRESULT CEditLoader::Loading_Rooftop()
{
	SetData(SCENE_BOSS);

	return S_OK;
}

HRESULT CEditLoader::Loading_Road()
{
	SetData(SCENE_CAR);

	return S_OK;
}

void CEditLoader::SetData(_uint _iSceneIndex)
{
	CDataManager::GetInstance()->Clear();
	CFileManager::GetInstance()->LoadDataFile(_iSceneIndex, L"Wall_Layer");
	CFileManager::GetInstance()->LoadDataFile(_iSceneIndex, L"Tile_Layer");
	CFileManager::GetInstance()->LoadDataFile(_iSceneIndex, L"Env_Layer");
	CFileManager::GetInstance()->LoadDataFile(_iSceneIndex, L"Monster_Layer");

	CMapFactory::GetInstance()->SetTargetSceneIndex(_iSceneIndex);
}

void CEditLoader::InstancingObjects(const wstring &_Layer)
{
	if (vector<MAPOBJECTDATA> *pVecData = CDataManager::GetInstance()->FindData(_Layer))
	{
		for (MAPOBJECTDATA &element : *pVecData)
		{
			CMapFactory::GetInstance()->Create(element.eCategory, element.iType, &element);
		}
	}
}
