#pragma once
#include "Clinet_Define.h"
#include "CBase.h"
#include "Engine_Define.h"

// 다음 씬 데이터 설정
class CLoader : public CBase
{
private:
	explicit CLoader(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoader();

public:
	CRITICAL_SECTION* Get_Crt() { return &m_Crt; }
	SCENE		Get_NextSceneID() const { return m_eNextScene; }
	_bool			Get_Finish() const { return m_isFinished; }
	const _tchar* Get_String() const { return m_szLoading; }
public:
	HRESULT			Ready_Loading(SCENE eNextScene);
	HRESULT			Loading_Logo();
	HRESULT			Loading_ForStage();

private:
	SCENE					m_eNextScene = SCENE_END;
	_bool					m_isFinished = false;
	_tchar					m_szLoading[MAX_PATH] = TEXT("");
	LPDIRECT3DDEVICE9		m_pGraphicDev = nullptr;

private:
	HANDLE					m_hThread = 0;
	CRITICAL_SECTION		m_Crt;

public:
	static CLoader* Create(LPDIRECT3DDEVICE9 pGrahpicDev, SCENE eNextScene);
	virtual void Free();	
};

