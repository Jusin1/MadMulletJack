#include "pch.h"
#include "CGameObject.h"
#include "CGui_Thumbnail.h"
#include "Engine_Define.h"
#include "Editor_Define.h"
#include "CDInputMgr.h"
#include "CGameObject.h"
#include "CTransform.h"
#include "CGui_PrefabEditorPanel.h"
#include "CObjectManager.h"
#include "CManagement.h"
#include "CGui_Log.h"
#include "CComponentMgr.h"
#include "CTexture.h"
#include "CGui_MapEditorPanel.h"
#include "CGui_Console.h"
#include "CGuiManager.h"

IMPLEMENT_SINGLETON(CGuiManager)

CGuiManager::CGuiManager()
	: m_pTarget(nullptr), m_pGraphicDevice(nullptr), m_eCategory(ObjectCategory::WALL), m_iObjectType(0), m_bCreateMode(FALSE)
{
}

CGuiManager::~CGuiManager()
{
	Free();
}

void CGuiManager::Free()
{
    for (CGui_Panel *&element : m_pPanels)
    {
        Safe_Release(element);
    }

    Safe_Release(m_pGraphicDevice);
}

HRESULT CGuiManager::Ready_CGuiManager(LPDIRECT3DDEVICE9 pGraphicDevce)
{
    if (!(m_pPanels[INSPECTOR] = CGui_MapEditorPanel::Create()))
        return E_FAIL;
    if (!(m_pPanels[CONSOLE] = CGui_Console::Create()))
        return E_FAIL;
    if (!(m_pPanels[PREFAB_INSPECTOR] = CGui_PrefabEditorPanel::Create()))
        return E_FAIL;

    m_pGraphicDevice = pGraphicDevce;
    pGraphicDevce->AddRef();
	return S_OK;
}

HRESULT CGuiManager::Initialize()
{
    ::IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ::ImGui_ImplWin32_Init(g_hWnd);
    ::ImGui_ImplDX9_Init(m_pGraphicDevice);

    return S_OK;
}

void CGuiManager::ShowEditorDockspace()
{
    _uint iCurSceneID = CManagement::GetInstance()->Get_CurrentSceneIdx();
    _uint iKey{ INSPECTOR };
    if (iCurSceneID == SCENE_PREFAB) iKey = PREFAB_INSPECTOR;       


    const ImGuiViewport *vp = ImGui::GetMainViewport();
    const ImVec2 screenPos = vp->Pos;
    const ImVec2 screenSize = vp->Size;

    constexpr float fMargin = 25.f;

    constexpr float fFull_W = 1850.f;
    constexpr float fFull_H = 980.f;
    
    constexpr float VIEW_W = 1366.0f;
    constexpr float VIEW_H = 768.0f;

    const ImVec2 areaPos = ImVec2(screenPos.x, screenPos.y);
    const ImVec2 areaSize = ImVec2(screenSize.x, screenSize.y);

    const float leftColW = VIEW_W;
    const float consoleH = (std::max)(0.0f, areaSize.y - VIEW_H);

    m_pPanelInfos[iKey].Position = ImVec2(areaPos.x + leftColW, areaPos.y);
    m_pPanelInfos[iKey].Size = ImVec2(fFull_W - VIEW_W, fFull_H);

    m_pPanelInfos[CONSOLE].Position = ImVec2(areaPos.x, areaPos.y + VIEW_H);
    m_pPanelInfos[CONSOLE].Size = ImVec2(fFull_W - m_pPanelInfos[iKey].Size.x - fMargin, fFull_H - VIEW_H);
}

void CGuiManager::ShowInspector()
{
    _uint iCurSceneID = CManagement::GetInstance()->Get_CurrentSceneIdx();
    _uint iKey{ INSPECTOR };
    if (iCurSceneID == SCENE_PREFAB) iKey = PREFAB_INSPECTOR;

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
    ImGui::SetNextWindowPos(m_pPanelInfos[iKey].Position, ImGuiCond_Once);
    ImGui::SetNextWindowSize(m_pPanelInfos[iKey].Size, ImGuiCond_Once);

    if (ImGui::Begin(m_pPanels[iKey]->GetTitle().c_str(), nullptr, flags))
    {
        m_pPanels[iKey]->Render();
    }

    ImGui::End();
}

void CGuiManager::ShowConsole()
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
    ImGui::SetNextWindowPos(m_pPanelInfos[CONSOLE].Position, ImGuiCond_Once);
    ImGui::SetNextWindowSize(m_pPanelInfos[CONSOLE].Size, ImGuiCond_Once);

    if (ImGui::Begin(m_pPanels[CONSOLE]->GetTitle().c_str(),nullptr , flags))
    {
        m_pPanels[CONSOLE]->Render();
    }

    ImGui::End();
}

HRESULT CGuiManager::AddThumbnail(const string &ThumnailName, const _tchar *CompName, CGui_Thumbnail *_pThumbnail, _uint iType)
{
    IDirect3DBaseTexture9 *pTexture = static_cast<CTexture *>(CComponentMgr::GetInstance()->Find_Component(SCENE_STATIC, CompName))->Get_Texture();
    if (!pTexture)
    {
        MSG_BOX("CGuiManager::AddThumbnail, Texture is null");
        return E_FAIL;
    }

    _pThumbnail->Add_Thumbnail(iType, ThumnailName, CompName, pTexture);
    return S_OK;
}

void CGuiManager::AddLog(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    static_cast<CGui_Log *>(GetConsole()->GetElement(0))->Add_LogV(fmt, args);

    va_end(args);
}

void CGuiManager::Render()
{
    // ;
    // z 축 -15도
    if (KEY_BUTTON_DOWN(DIK_SEMICOLON) && GetTarget())
    {
        GetTarget()->GetTransform()->RotationDegree(_vec3{ 0.f, 0.f, 1.f }, -15.f);
    }
    // '
    // z 축 15도
    else if (KEY_BUTTON_DOWN(DIK_APOSTROPHE) && GetTarget())
    {
        GetTarget()->GetTransform()->RotationDegree(_vec3{ 0.f, 0.f, 1.f }, 15.f);
    }
    // [
    // y 축 -15도
    else if (KEY_BUTTON_DOWN(DIK_LBRACKET) && GetTarget())
    {
        GetTarget()->GetTransform()->RotationDegree(_vec3{ 0.f, 1.f, 0.f }, -15.f);
    }
    // ]
    // y 축 15도
    else if (KEY_BUTTON_DOWN(DIK_RBRACKET) && GetTarget())
    {
        GetTarget()->GetTransform()->RotationDegree(_vec3{ 0.f, 1.f, 0.f }, 15.f);
    }
    // -
    // x 축 -15도
    else if (KEY_BUTTON_DOWN(DIK_MINUS) && GetTarget())
    {
        GetTarget()->GetTransform()->RotationDegree(_vec3{ 1.f, 0.f, 0.f }, -15.f);
    }
    // =
    // x 축 15도
    else if (KEY_BUTTON_DOWN(DIK_EQUALS) && GetTarget())
    {
        GetTarget()->GetTransform()->RotationDegree(_vec3{ 1.f, 0.f, 0.f }, 15.f);
    }
    ShowEditorDockspace();
    ShowInspector();
    ShowConsole();
}

void CGuiManager::SetCreateMode(_bool _b, ObjectCategory _e)
{
    _uint iCurSceneID = CManagement::GetInstance()->Get_CurrentSceneIdx();
    if (_b)
    {
        switch (_e)
        {
        case ObjectCategory::WALL:
        {
            MSG_BOX("CGuiManager::SetCreateMode, Wrong type");
        } break;
        case ObjectCategory::TILE:
        {

            if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_DummyTile", iCurSceneID, L"Dummy_Layer")))
            {
                MSG_BOX("CGuiManager::SetCreateMode, DummyTile Creat Failed");
            }
            EDITOR_CONSOLE("TILE");
        } break;
        case ObjectCategory::ENV_OBJ:
        {
            if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_DummyPlacementObject", iCurSceneID, L"Dummy_Layer")))
            {
                MSG_BOX("CGuiManager::SetCreateMode, DummyPlacementObject Creat Failed");
            }
            EDITOR_CONSOLE("ENV_OBJ");
        } break;
        case ObjectCategory::MONSTER:
        {
            if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_DummyPlacementObject", iCurSceneID, L"Dummy_Layer")))
            {
                MSG_BOX("CGuiManager::SetCreateMode, DummyPlacementObject Creat Failed");
            }
            EDITOR_CONSOLE("MONSTER");
        } break;
        case ObjectCategory::LIGHT:
        {
            EDITOR_CONSOLE("LIGHT");
        } break;
        }
    }
    else
    {
        auto list = CObjectManager::GetInstance()->Get_ObjectList(iCurSceneID, L"Dummy_Layer");
        if (list && list->size() > 0)
        {
            (*list->begin())->Set_Dead(TRUE);
        }
    }
    m_bCreateMode = _b;
}

const _tchar *CGuiManager::GetSelectedThumnailTexture()
{
    switch (CManagement::GetInstance()->Get_CurrentSceneIdx())
    {
    case SCENE_PREFAB:
        return static_cast<CGui_MapEditorPanel *>(m_pPanels[PANEL::PREFAB_INSPECTOR])->GetSelectedThumbnailTexture();
    }

    return static_cast<CGui_MapEditorPanel *>(m_pPanels[PANEL::INSPECTOR])->GetSelectedThumbnailTexture();
}
