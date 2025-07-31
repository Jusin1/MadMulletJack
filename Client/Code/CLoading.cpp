#include "pch.h"
#include "CLoading.h"
#include "CProtoMgr.h"

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev), m_bFinish(false), m_eLoadingID(LOADING_END)
{
	m_pGraphicDev->AddRef();
	ZeroMemory(m_szLoading, sizeof(m_szLoading));
}

CLoading::~CLoading()
{
}

HRESULT CLoading::Ready_Loading(LOADINGID eID)
{
	InitializeCriticalSection(&m_Crt);

	int		iHp(0);

	m_hThread = (HANDLE)_beginthreadex(NULL, // 보안 속성(기본 값) : 핸들의 상속 여부, null이면 상속에서 제외
										0,	 // 디폴트 스택 사이즈(1바이트)
										Thread_Main,	 // 쓰레드를 이용하여 동작시킬 함수(__stdcall 규약)
										this, // 쓰레드 함수의 매개 변수로 전달할 데이터 주소
										0,	  // 쓰레드의 생성 및 실행을 조절하기 위한 flag
										NULL); // 쓰레드 ID 반환

	m_eLoadingID = eID;

	return S_OK;
}

_uint CLoading::Loading_ForStage()
{
	lstrcpy(m_szLoading, L"Buffer Component Loading.....................");

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TriCol", Engine::CTriCol::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RcCol", Engine::CRcCol::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainTex", Engine::CTerrainTex::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_CubeTex", Engine::CCubeTex::Create(m_pGraphicDev))))
		return E_FAIL;
	
	lstrcpy(m_szLoading, L"Texture Component Loading.....................");

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PlayerTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Player%d.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Terrain/Grass_%d.tga", TEX_NORMAL, 2))))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SkyBoxTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/SkyBox/burger%d.dds", TEX_CUBE, 4))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Etc Component Loading.....................");

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Transform", Engine::CTransform::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Calculator", Engine::CCalculator::Create(m_pGraphicDev))))
		return E_FAIL;


	m_bFinish = true;

	lstrcpy(m_szLoading, L"Loading Complete");

	return 0;
}

unsigned int __stdcall CLoading::Thread_Main(void* pArg)
{
	CLoading* pLoading = reinterpret_cast<CLoading*>(pArg);

	_uint iFlag(0);

	EnterCriticalSection(pLoading->Get_Crt());

	switch (pLoading->Get_LoadingID())
	{
	case LOADING_STAGE:
		iFlag = pLoading->Loading_ForStage();
		break;

	case LOADING_BOSS:
		break;
	}

	LeaveCriticalSection(pLoading->Get_Crt());

	// _endthreadex(0);

	return iFlag;
}

CLoading* CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID)
{
	CLoading* pLoading = new CLoading(pGraphicDev);

	if (FAILED(pLoading->Ready_Loading(eID)))
	{
		Safe_Release(pLoading);
		MSG_BOX("Loading Create Failed");
		return nullptr;
	}

	return pLoading;
}

void CLoading::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	//WaitForMultipleObjects(3, /*총 3개의 커널 오브젝트를 관찰한다는 의미.*/
	//						hThread, /*쓰레드 핸들을 가지고 있는 배열의 첫 주소*/
	//						TRUE, /*모든 쓰레드가 종료(소멸)가 되기를*/
	//						INFINITE); /*영원히 기다린다*/
	//						// 3번인자가 FALSE인 경우에는 하나라도 종료되기를 기다린다는 의미.

	// for (int i = 0; i < 3; ++i)
	// {
	// 	// 해당 쓰레드가 실행한 함수가 종료될 때의 반환 값을 얻어온다.
	// 	GetExitCodeThread(hThread[i], &dwResult);
	// }


	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_Crt);

	Safe_Release(m_pGraphicDev);
}
