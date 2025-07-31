#include "CGraphicDev.h"

IMPLEMENT_SINGLETON(CGraphicDev)

CGraphicDev::CGraphicDev()
	: m_pSDK(nullptr), m_pGraphicDev(nullptr)
{
	
}

CGraphicDev::~CGraphicDev()
{
	Free();
}

HRESULT CGraphicDev::Ready_GraphicDev(HWND hWnd, WINMODE eMode,
	const _uint& iSizeX, const _uint& iSizeY, CGraphicDev** ppGraphicDev)
{
	m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);

	if (nullptr == m_pSDK)
		return E_FAIL;

	D3DCAPS9	DeviceCaps;
	ZeroMemory(&DeviceCaps, sizeof(D3DCAPS9));

	// D3DADAPTER_DEFAULT : 현재 컴퓨터에 설치된 기본 그래픽 카드
	if (FAILED(m_pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps)))
		return E_FAIL;

	_ulong	dwFlag(0);

	if (DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		dwFlag |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	
	else
		dwFlag |= D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;

	D3DPRESENT_PARAMETERS		d3dpp;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	d3dpp.BackBufferWidth  = iSizeX;
	d3dpp.BackBufferHeight = iSizeY;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount  = 1;

	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;

	// D3DSWAPEFFECT_DISCARD = 1,	// 스왑 체인 방식
	// D3DSWAPEFFECT_FLIP = 2,		// 버퍼 하나를 뒤집으면서 사용
	// D3DSWAPEFFECT_COPY = 3,		// 더블 버퍼링과 흡사

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

	d3dpp.hDeviceWindow = hWnd;

	d3dpp.Windowed = eMode;		// TRUE(창 모드)

	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		
	if (FAILED(m_pSDK->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		dwFlag,
		&d3dpp,
		&m_pGraphicDev)))
	{
		return E_FAIL;
	}

	*ppGraphicDev = this;

	return S_OK;
}

// 후면 버퍼의 동작 원리

// 1. 화면을 지운다.
// 2. 다시 그릴 화면 준비
void CGraphicDev::Render_Begin(D3DXCOLOR Color)
{
	m_pGraphicDev->Clear(0,
						NULL, 
						D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
						Color,
						1.f,		// z버퍼의 초기화값
						0);

	m_pGraphicDev->BeginScene();

}


// 3. 여기까지 한 프레임을 그릴 것이라고 장치에게 통보
// 4. 전면 버퍼와 서피스 교환
void CGraphicDev::Render_End()
{
	m_pGraphicDev->EndScene();
	m_pGraphicDev->Present(NULL, NULL, NULL, NULL);

	// 1, 2 인자 : 렉트의 주소
	// 1, 2, 4 인자 : D3DSWAPEFFECT_COPY 옵션일 때만 사용 가능
	// 3인자 : 창 핸들
}


void CGraphicDev::Free()
{
	Safe_Release(m_pGraphicDev);
	Safe_Release(m_pSDK);
}
