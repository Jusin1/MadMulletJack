#include "pch.h"
#include "CGameObject.h"
#include "CGui_Thumbnail.h"
#include "Engine_Define.h"
#include "Editor_Define.h"
#include "CGameObject.h"
#include "CObjectManager.h"
#include "CGui_Log.h"
#include "CComponentMgr.h"
#include "CTexture.h"
#include "CGui_MapEditorPanel.h"
#include "CGui_Console.h"
#include "CGuiManager.h"

IMPLEMENT_SINGLETON(CGuiManager)

CGuiManager::CGuiManager()
	: m_pTarget(nullptr), m_pGraphicDevice(nullptr), m_eCategory(MapEditorObjectCategory::WALL), m_iObjectType(0), m_bCreateMode(FALSE)
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

    m_pPanelInfos[INSPECTOR].Position = ImVec2(areaPos.x + leftColW, areaPos.y);
    m_pPanelInfos[INSPECTOR].Size = ImVec2(fFull_W - VIEW_W, fFull_H);

    m_pPanelInfos[CONSOLE].Position = ImVec2(areaPos.x, areaPos.y + VIEW_H);
    m_pPanelInfos[CONSOLE].Size = ImVec2(fFull_W - m_pPanelInfos[INSPECTOR].Size.x - fMargin, fFull_H - VIEW_H);
}

void CGuiManager::ShowInspector()
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
    ImGui::SetNextWindowPos(m_pPanelInfos[INSPECTOR].Position, ImGuiCond_Once);
    ImGui::SetNextWindowSize(m_pPanelInfos[INSPECTOR].Size, ImGuiCond_Once);

    if (ImGui::Begin(m_pPanels[INSPECTOR]->GetTitle().c_str(), nullptr, flags))
    {
        m_pPanels[INSPECTOR]->Render();
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
    ShowEditorDockspace();
    ShowInspector();
    ShowConsole();
}

void CGuiManager::SetCreateMode(_bool _b, MapEditorObjectCategory _e)
{
    if (_b)
    {
        switch (_e)
        {
        case MapEditorObjectCategory::WALL:
        {
            MSG_BOX("CGuiManager::SetCreateMode, Wrong type");
        } break;
        case MapEditorObjectCategory::TILE:
        {
            if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_DummyTile", SCENE_EDITOR, L"Dummy_Layer")))
            {
                MSG_BOX("CGuiManager::SetCreateMode, DummyTile Creat Failed");
            }
            EDITOR_CONSOLE("TILE");
        } break;
        case MapEditorObjectCategory::ENV_OBJ:
        {
            if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_DummyPlacementObject", SCENE_EDITOR, L"Dummy_Layer")))
            {
                MSG_BOX("CGuiManager::SetCreateMode, DummyPlacementObject Creat Failed");
            }
            EDITOR_CONSOLE("ENV_OBJ");
        } break;
        case MapEditorObjectCategory::MONSTER:
        {
            if (FAILED(CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_DummyPlacementObject", SCENE_EDITOR, L"Dummy_Layer")))
            {
                MSG_BOX("CGuiManager::SetCreateMode, DummyPlacementObject Creat Failed");
            }
            EDITOR_CONSOLE("MONSTER");
        } break;
        case MapEditorObjectCategory::LIGHT:
        {
            EDITOR_CONSOLE("LIGHT");
        } break;
        }
    }
    else
    {
        auto list = CObjectManager::GetInstance()->Get_ObjectList(SCENE_EDITOR, L"Dummy_Layer");
        if (list && list->size() > 0)
        {
            (*list->begin())->Set_Dead(TRUE);
        }
    }
    m_bCreateMode = _b;
}
