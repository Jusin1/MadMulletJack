#include "pch.h"
#include "CLogo.h"
#include "CBackGround.h"
#include "CProtoMgr.h"
#include "CPlayer.h"
#include "CMonster.h"
#include "CTerrain.h"
#include "CStage.h"
#include "CManagement.h"
#include "CFontMgr.h"
#include "CLoading_Scene.h"
#include "CObjectManager.h"

CLogo::CLogo(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CScene(pGraphicDev)
{
}

CLogo::~CLogo()
{
}

HRESULT CLogo::Ready_Scene()
{
    if (FAILED(CScene::Ready_Scene()))
        return E_FAIL;

    if(FAILED(Ready_Environment_Layer(L"Layer_BackGround")))
        return E_FAIL;


    return S_OK;
}

_int CLogo::Update_Scene(const _float& fTimeDelta)
{
    CScene::Update_Scene(fTimeDelta);

    if (GetAsyncKeyState(VK_SPACE) & 0X8000)
    {
        if (FAILED(CManagement::GetInstance()->Open_Scene(SCENE_LOADING, CLoading_Scene::Create(m_pGraphicDev, SCENE_STAGE))))
            return E_FAIL;
    }
    
}

void CLogo::LateUpdate_Scene(const _float& fTimeDelta)
{
    CScene::LateUpdate_Scene(fTimeDelta);
    SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
}

void CLogo::Render_Scene()
{
    //// debug용 렌더
    //_vec2       vPos{ 100.f, 100.f };

    //CFontMgr::GetInstance()->Render_Font(L"Font_Default", m_pLoading->Get_String(), &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

}

HRESULT CLogo::Ready_Environment_Layer(const _tchar* pLayerTag)
{
    // BackGround
    if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Prototype_GameObject_BackGround", SCENE_LOGO, pLayerTag, nullptr)))
        return E_FAIL;

    return S_OK;
}

CLogo* CLogo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CLogo* pLogo = new CLogo(pGraphicDev);
    if (FAILED(pLogo->Ready_Scene()))
    {
        MSG_BOX("Logo Create Failed");
        Safe_Release(pLogo);
        return nullptr;
    }
    return pLogo;
}

void CLogo::Free()
{
    Engine::CScene::Free();
}
