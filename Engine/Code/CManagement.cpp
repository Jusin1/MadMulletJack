#include "CManagement.h"
#include "CRenderer.h"
#include "CComponentMgr.h"
#include "CObjectManager.h"

IMPLEMENT_SINGLETON(CManagement)

CManagement::CManagement()
	: m_pCurrentScene(nullptr)
	, m_iPrevSceneIndex(0)
	, m_iSceneIndex(0)
	, m_iLastSceneIndex(0)
{
}

CManagement::~CManagement()
{
    Free();
}

// 씬을 새로 교체
// 기존 씬이 존재하면 객체들 제거
HRESULT CManagement::Open_Scene(unsigned int iSceneIdx, CScene* pNewScene)
{
	if (nullptr != m_pCurrentScene)
	{
		m_iPrevSceneIndex = m_iSceneIndex;
		CComponentMgr::GetInstance()->Clear(m_iSceneIndex);
		CObjectManager::GetInstance()->Clear(m_iSceneIndex);
	}


	Safe_Release(m_pCurrentScene);
	m_pCurrentScene = pNewScene;

	m_iSceneIndex = iSceneIdx;
	return S_OK;
}

void CManagement::Update_Scene(const _float& fTimeDelta)
{
    if (nullptr == m_pCurrentScene)
        return;

    m_pCurrentScene->Update_Scene(fTimeDelta);
}

void CManagement::LateUpdate_Scene(const _float& fTimeDelta)
{
	if (nullptr == m_pCurrentScene)
		return;
    m_pCurrentScene->LateUpdate_Scene(fTimeDelta);
}

void CManagement::SaveData()
{
	m_pCurrentScene->SaveData();
}

void CManagement::Free()
{
    Safe_Release(m_pCurrentScene);
}
