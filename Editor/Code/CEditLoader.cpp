#include "CEditLoader.h"
#include "CObjectManager.h"
#include "CComponentMgr.h"
#include "CEditorCamera.h"
#include "CVIBuffer_GridPanel_Editor.h"
#include "CVIBuffer_Cube_Color.h"
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
	case SCENE_EDITOR:
	{
		pLoader->Loading_Editor();
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
	lstrcpy(m_szLoading, L"텍스쳐 로딩 중");


	// 객체 생성
	lstrcpy(m_szLoading, L"객체 생성 중.");
	//// Camera_Dynamic
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Proto_Camera_Edit",
		CEditorCamera::Create(m_pGraphicDevice))))
		return E_FAIL;

	// DefaultPanel
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Proto_GameObject_DefaultPanel",
		CGridPanel::Create(m_pGraphicDevice))))
		return E_FAIL;

	// DefaultTile
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Proto_GameObject_DefaultTile",
		CTile::Create(m_pGraphicDevice))))
		return E_FAIL;

	// DummyTile
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Proto_GameObject_DummyTile",
		CDummyTile::Create(m_pGraphicDevice))))
		return E_FAIL;

	// Dummy PlacementObject
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Proto_GameObject_DummyPlacementObject",
		CDummyPlacementObject::Create(m_pGraphicDevice))))
		return E_FAIL;

	// Default PlacementObject
	if (FAILED(CObjectManager::GetInstance()->Add_Prototype(L"Proto_GameObject_DefaultPlacementObject",
		CPlacementObject::Create(m_pGraphicDevice))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("모델 로딩 중."));

	// Buffer_PanelDefault
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Component_Buffer_PanelDefault", CVIBuffer_GridPanel_Editor::Create(m_pGraphicDevice))))
		return E_FAIL;
	
	// Buffer_TileDefault
	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Component_Buffer_TileDefault", CVIBuffer_Rect::Create(m_pGraphicDevice))))
		return E_FAIL;

	if (FAILED(CComponentMgr::GetInstance()->Add_Prototype(SCENE_STATIC, L"Proto_Component_Buffer_CubeColor", CVIBuffer_Cube_Color::Create(m_pGraphicDevice))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}
