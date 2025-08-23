#include "pch.h"
#include "CEditorLoadingScene.h"
#include "CManagement.h"

#include "CEditLoader.h"
#include "CEditorScene.h"
#include "CScene_Tutorial.h"
#include "CScene_Stage_1.h"
#include "CScene_Prefab.h"

CEditorLoadingScene::CEditorLoadingScene(LPDIRECT3DDEVICE9 pGrahpicDev)
	: CScene(pGrahpicDev)
{
}

CEditorLoadingScene::~CEditorLoadingScene()
{
}

HRESULT CEditorLoadingScene::Ready_Loading(SCENE eNextScene)
{
	if (FAILED(CScene::Ready_Scene()))
		return E_FAIL;

	m_eNextScene = eNextScene;

	m_pLoader = CEditLoader::Create(m_pGraphicDev, eNextScene);
	if (!m_pLoader)
		return E_FAIL;

	return S_OK;
}

_int CEditorLoadingScene::Update_Scene(const _float &fTimeDelta)
{
	CScene::Update_Scene(fTimeDelta);

	if (m_pLoader->Get_Finish())
	{
		CScene *pNewScene = nullptr;

		switch (m_eNextScene)
		{
		case SCENE_DEV:
		{
			pNewScene = CEditorScene::Create(m_pGraphicDev);
		} break;
		case SCENE_TUTORIAL:
		{
			pNewScene = CScene_Tutorial::Create(m_pGraphicDev);
		} break;
		case SCENE_STAGE_1:
		{
			pNewScene = CScene_Stage_1::Create(m_pGraphicDev);
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
		case SCENE_PREFAB:
		{
			pNewScene = CScene_Prefab::Create(m_pGraphicDev);
		} break;
		default:
		{
			MSG_BOX("CEditorLoadingScene::Update_Scene, wrongtype");
		} break;
		}

		if (nullptr == pNewScene)
			return NO_EVENT;

		if (FAILED(CManagement::GetInstance()->Open_Scene(m_eNextScene, pNewScene)))
			return NO_EVENT;
	}

	return S_OK;
}

void CEditorLoadingScene::LateUpdate_Scene(const _float &fTimeDelta)
{
	CScene::LateUpdate_Scene(fTimeDelta);

	SetWindowText(g_hWnd, m_pLoader->Get_String());
}

CEditorLoadingScene *CEditorLoadingScene::Create(LPDIRECT3DDEVICE9 pGraphicDev, SCENE eID)
{
	CEditorLoadingScene *pLoadingScene = new CEditorLoadingScene(pGraphicDev);

	if (FAILED(pLoadingScene->Ready_Loading(eID)))
	{
		MSG_BOX("EditorLoadingScene Create Failed");
		Safe_Release(pLoadingScene);
		return nullptr;
	}

	return pLoadingScene;
}

void CEditorLoadingScene::Free()
{
	CScene::Free();
	Safe_Release(m_pLoader);
}




