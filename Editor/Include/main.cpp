#include "pch.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "ImZoomSlider.h"
#include "ImCurveEdit.h"
#include "GraphEditor.h"

#include "Engine_Define.h"
#include "framework.h"
#include "CEditorApplication.h"
#include "main.h"


HWND        g_hWnd;
HINSTANCE   g_hInst;

wstring TitleName = L"Editor";
wstring WindowClassName = L"EditorWindow";

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    ::MyRegisterClass(hInstance);

    if (!::InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = ::LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EDITOR));

    MSG msg;
    msg.message = WM_NULL;

    CEditorApplication *pEditorApplication = CEditorApplication::Create();
    if (!pEditorApplication)
        return FALSE;

    while (msg.message != WM_QUIT)
    {
        if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
        }
        else
        {
            pEditorApplication->Run_EditorApplication();
        }
    }

    _ulong dwRefCnt{ 0 };

    if (dwRefCnt = Engine::Safe_Release(pEditorApplication))
    {
        MSG_BOX("EidtorApplication Release FAILED");
        return FALSE;
    }

    return (int)msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EDITOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = WindowClassName.c_str();
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return ::RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   g_hInst = hInstance;

   RECT rc{ 0, 0, WINCX, WINCY };
   ::AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

   HWND hWnd = ::CreateWindowW(WindowClassName.c_str(), TitleName.c_str(), WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, 0,
       rc.right - rc.left,
       rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   g_hWnd = hWnd;

   SetWindowPos(g_hWnd, HWND_TOP, 10, 10, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_SHOWWINDOW);

   ::ShowWindow(hWnd, nCmdShow);
   ::UpdateWindow(hWnd);

   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_EXIT:
                ::DestroyWindow(hWnd);
                break;
            default:
                return ::DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = ::BeginPaint(hWnd, &ps);
            ::EndPaint(hWnd, &ps);
        }
        break;
    case WM_DPICHANGED:
    {
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
        {
            const RECT *suggested_rect = (RECT *)lParam;
            ::SetWindowPos(hWnd, NULL, suggested_rect->left, suggested_rect->top
                , suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top
                , SWP_NOZORDER | SWP_NOACTIVATE);
        }
    } break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        break;
    default:
        return ::DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}