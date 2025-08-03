#include "CScene.h"

CScene::CScene(LPDIRECT3DDEVICE9 pGraphicDev)
    : m_pGraphicDev(pGraphicDev)
{
    m_pGraphicDev->AddRef();
}

CScene::~CScene()
{
}


HRESULT CScene::Ready_Scene()
{
    return S_OK;
}

_int CScene::Update_Scene(const _float& fTimeDelta)
{
    return S_OK;
}

void CScene::LateUpdate_Scene(const _float& fTimeDelta)
{
}

void CScene::Free()
{

    Safe_Release(m_pGraphicDev);
}
