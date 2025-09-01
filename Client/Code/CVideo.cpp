#include "pch.h"
#include "CVideo.h"
#include <atlbase.h> // CComPtr

CVideo::CVideo() {}
CVideo::~CVideo() { Close(); }

bool CVideo::Open(LPDIRECT3DDEVICE9 pDevice, HWND hWnd, const std::wstring& filePath) {
    m_pDevice = pDevice;
    m_hWnd = hWnd;
    m_bFinished = false;

    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(hr)) m_bCoInit = true;

    return InitVMR9(filePath);
}

bool CVideo::InitVMR9(const std::wstring& filePath) {
    HRESULT hr = S_OK;

    // 필터 그래프 생성
    hr = CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER,
        IID_IGraphBuilder, (void**)&m_pGraph);
    if (FAILED(hr)) return false;

    // VMR9 생성
    hr = CoCreateInstance(CLSID_VideoMixingRenderer9, nullptr, CLSCTX_INPROC_SERVER,
        IID_IBaseFilter, (void**)&m_pVMR);
    if (FAILED(hr)) return false;

    // 필터 그래프에 추가
    m_pGraph->AddFilter(m_pVMR, L"Video Renderer");

    // Windowless 모드 설정
    CComPtr<IVMRFilterConfig9> pConfig;
    hr = m_pVMR->QueryInterface(IID_IVMRFilterConfig9, (void**)&pConfig);
    if (FAILED(hr)) return false;

    hr = pConfig->SetRenderingMode(VMR9Mode_Windowless);
    if (FAILED(hr)) return false;

    hr = m_pVMR->QueryInterface(IID_IVMRWindowlessControl9, (void**)&m_pWinless);
    if (FAILED(hr)) return false;

    // 출력할 윈도우 설정
    hr = m_pWinless->SetVideoClippingWindow(m_hWnd);
    if (FAILED(hr)) return false;

    // 윈도우 크기에 맞춰 비디오 위치 설정
    RECT rc;
    GetClientRect(m_hWnd, &rc);
    m_pWinless->SetVideoPosition(nullptr, &rc);

    // 파일 로드
    hr = m_pGraph->RenderFile(filePath.c_str(), nullptr);
    if (FAILED(hr)) return false;

    // Media Control / Event 가져오기
    m_pGraph->QueryInterface(IID_IMediaControl, (void**)&m_pControl);
    m_pGraph->QueryInterface(IID_IMediaEventEx, (void**)&m_pEvent);

    // 재생 시작
    if (m_pControl) m_pControl->Run();

    return true;
}

void CVideo::Update() {
    if (!m_pEvent) return;

    long evCode;
    LONG_PTR p1, p2;
    while (SUCCEEDED(m_pEvent->GetEvent(&evCode, &p1, &p2, 0))) {
        m_pEvent->FreeEventParams(evCode, p1, p2);
        if (evCode == EC_COMPLETE) {
            m_bFinished = true;
            Stop();
        }
    }
}

void CVideo::Render() {
    if (m_pWinless && m_pDevice) {
        RECT rc;
        GetClientRect(m_hWnd, &rc);
        m_pWinless->RepaintVideo(m_hWnd, nullptr);
    }
}

void CVideo::Stop() {
    if (m_pControl) m_pControl->Stop();
}

void CVideo::Close() {
    Stop();
    ReleaseAll();
    if (m_bCoInit) {
        CoUninitialize();
        m_bCoInit = false;
    }
}

void CVideo::ReleaseAll() {
    if (m_pControl) { m_pControl->Release(); m_pControl = nullptr; }
    if (m_pEvent) { m_pEvent->Release();   m_pEvent = nullptr; }
    if (m_pWinless) { m_pWinless->Release(); m_pWinless = nullptr; }
    if (m_pVMR) { m_pVMR->Release();     m_pVMR = nullptr; }
    if (m_pGraph) { m_pGraph->Release();   m_pGraph = nullptr; }
}