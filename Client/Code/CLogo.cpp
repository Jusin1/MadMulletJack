#include "pch.h"
#include "CLogo.h"
#include "CBackGround.h"
#include "CPlayer.h"
#include "CMonster.h"
#include "CStage.h"
#include "CManagement.h"
#include "CFontMgr.h"
#include "CLoading_Scene.h"
#include "CObjectManager.h"
#include "CImageUI.h"
#include "CDynamicCamera.h"
#include "CButtonUI.h"
#include "CTextUI.h"
#include "Sound_Manager.h"
#include "CVideo.h"

CLogo::CLogo(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CScene(pGraphicDev),
    m_bOpenScene(false)
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

    if (FAILED(Ready_UI_Layer(L"UI_Layer")))
        return E_FAIL;
    return S_OK;
}

_int CLogo::Update_Scene(const _float& fTimeDelta)
{
    if (m_bOpenScene)
    {
        m_bOpenScene = false;
        if (FAILED(CManagement::GetInstance()->Open_Scene(
            SCENE_LOADING,
            CLoading_Scene::Create(m_pGraphicDev, SCENE_DEV))))
            return E_FAIL;

        return 0;
    }
    CScene::Update_Scene(fTimeDelta);
    return 0;
}

void CLogo::LateUpdate_Scene(const _float& fTimeDelta)
{
    CScene::LateUpdate_Scene(fTimeDelta);
    SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
    // 테스트용 UI 띄우기
    if (GetAsyncKeyState('P') & 0x0001) // P 한번만
    {
        CSound_Manager::GetInstance()->PlayBGM((TCHAR*)TEXT("mx_stage_01.wav"), 1.0f);
    }
    if (GetAsyncKeyState('F') & 0x0001) // F 한번만
    {
        CSound_Manager::GetInstance()->StopAll();
    }
}
void CLogo::Render_Scene()
{
}

HRESULT CLogo::Ready_Environment_Layer(const _tchar* pLayerTag)
{


    return S_OK;
}


HRESULT CLogo::Ready_UI_Layer(const _tchar* pLayerTag)
{
    // 1) 클론하면서 파일 경로 넘김
    auto* video = dynamic_cast<CVideo*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_VideoUI",
            SCENE_LOGO, pLayerTag,
            (void*)L"../Bin/Resource/Video/df.mp4"  
        ));
    if (!video) return E_FAIL;
    video->Set_UISizeAndPos((float)WINCX, (float)WINCY, WINCX * 0.5f, WINCY * 0.5f);
    video->SetKeepAspect(false);     // 레터박스
    video->Set_Active(true);
    video->Set_RenderOn(true);



    if (auto* img2 = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", SCENE_LOGO, pLayerTag)))
    {
        img2->Set_Active(true);
        img2->Set_RenderOn(true);
        img2->Set_UIPosition(0.f, -250.f, 400.f, 200.f); // 화면 프레임
        img2->RegisterTexture(L"Com_Texture_Logo", L"Prototype_Component_Texture_Logo", 0, 20, 10.f, true);
        img2->ChangeTexture(L"Com_Texture_Logo");
        img2->Play(true);
    }


    auto* btnA = dynamic_cast<CButtonUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIButton", SCENE_LOGO, L"UI_Layer"));

    btnA->Set_ButtonRect(-560.f, -100.f, 250.f, 100.f);
    btnA->SetSolidMode(false);
    btnA->SetHoverScale(1.14f);
    btnA->SetPressScale(1.04f);
    btnA->SetLerpSpeeds(22.f, 14.f);
    btnA->RegisterTexture(L"Com_Btn_Idle", L"Prototype_Component_Texture_LogoButton", 0, 0, 0.f, false);
    btnA->SetStateTextures(L"Com_Btn_Idle");

    btnA->SetTextureTints(
        D3DXCOLOR(1.f, 0.3f, 0.7f, 1.f), // Normal: 분홍
        D3DXCOLOR(0.45f, 0.8f, 1.f, 1.f), // Hover : 하늘색
        D3DXCOLOR(0.6f, 0.9f, 1.f, 1.f), // Pressed
        D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f)  // Disabled
    );
    btnA->ChangeTexture(L"Com_Btn_Idle");

    CTextUI* text = dynamic_cast<CTextUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_TextUI", SCENE_LOGO, L"UI_Layer"));
    if (text)
    {
        text->SetFontTag(L"Font_UI_Bold");
        text->SetText(L"게임 시작");
        text->SetScale(0.75f);
        text->SetCentered(true);
        text->SetLetterSpacing(1.f);
        text->Set_UIPosition(-560.f, 120.f, 120.f, 26.f);
        btnA->Add_Child(text);
    }
    btnA->SetOnClick([this, btnA, text]() {
        CSound_Manager::GetInstance()->PlaySoundW(L"../Bin/Resource/Sounds/menu.selected-002", SOUND_UI, 2.f, false);
        m_bOpenScene = true;
        });

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
