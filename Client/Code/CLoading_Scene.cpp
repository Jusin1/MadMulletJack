#include "pch.h"
#include "CLoading_Scene.h"
#include "CManagement.h"

#include "CLoader.h"
#include "CLogo.h"
#include "CStage.h"

CLoading_Scene::CLoading_Scene(LPDIRECT3DDEVICE9 pGrahpicDev) : CScene(pGrahpicDev)
{
}

CLoading_Scene::~CLoading_Scene()
{
}

HRESULT CLoading_Scene::Ready_Loading(SCENE eNextScene)
{
	if (FAILED(CScene::Ready_Scene()))
		return E_FAIL;

	m_eNextScene = eNextScene;

	m_pLoader = CLoader::Create(m_pGraphicDev, eNextScene);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

_int CLoading_Scene::Update_Scene(const _float& fTimeDelta)
{
	CScene::Update_Scene(fTimeDelta);

	if (true == m_pLoader->Get_Finish())
	{
		if (GetKeyState(VK_RETURN) & 0x8000)
		{
			CScene* pNewScene = nullptr;

			switch (m_eNextScene)
			{
			case SCENE_LOGO:
				pNewScene = CLogo::Create(m_pGraphicDev);
				break;
			case SCENE_STAGE:
				pNewScene = CStage::Create(m_pGraphicDev);
				break;
			}

			if (nullptr == pNewScene)
				return NO_EVENT;

			if (FAILED(CManagement::GetInstance()->Open_Scene(m_eNextScene, pNewScene)))
				return NO_EVENT;

		}
	}
	return S_OK;
}

void CLoading_Scene::LateUpdate_Scene(const _float& fTimeDelta)
{
	CScene::LateUpdate_Scene(fTimeDelta);

	SetWindowText(g_hWnd, m_pLoader->Get_String());
}

CLoading_Scene* CLoading_Scene::Create(LPDIRECT3DDEVICE9 pGraphicDev, SCENE eID)
{
	CLoading_Scene* pLoadingScene = new CLoading_Scene(pGraphicDev);

	if (FAILED(pLoadingScene->Ready_Loading(eID)))
	{
		MSG_BOX("Logo Create Failed");
		Safe_Release(pLoadingScene);
		return nullptr;
	}

	return pLoadingScene;
}

void CLoading_Scene::Free()
{
	CScene::Free();
	Safe_Release(m_pLoader);
}




