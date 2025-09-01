#pragma once
#include <dshow.h>
#include <vmr9.h>
#include <string>
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CVideo {
public:
    CVideo();
    ~CVideo();

    bool Open(LPDIRECT3DDEVICE9 pDevice, HWND hWnd, const std::wstring& filePath);
    void Update();
    void Render();   // BeginScene/EndScene 사이에서 호출
    void Stop();
    void Close();

    bool IsFinished() const { return m_bFinished; }

private:
    // DirectShow COM 인터페이스
    IGraphBuilder* m_pGraph = nullptr;
    IMediaControl* m_pControl = nullptr;
    IMediaEventEx* m_pEvent = nullptr;
    IBaseFilter* m_pVMR = nullptr;
    IVMRWindowlessControl9* m_pWinless = nullptr;

    // Direct3D
    LPDIRECT3DDEVICE9 m_pDevice = nullptr;

    // 상태
    bool m_bFinished = false;
    HWND m_hWnd = nullptr;
    bool m_bCoInit = false;

private:
    bool InitVMR9(const std::wstring& filePath);
    void ReleaseAll();
};

END