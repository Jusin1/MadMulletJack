#pragma once
#include "Editor_Define.h"
#include "CBase.h"
#include "Engine_Define.h"

class CEditLoader : public CBase
{
private:
	explicit CEditLoader(LPDIRECT3DDEVICE9 pGraphicDevice);
	virtual ~CEditLoader();

	virtual void Free();
public:
	static CEditLoader *Create(LPDIRECT3DDEVICE9 pGrahpicDev, SCENE eNextScene);

	HRESULT			Ready_Loading(SCENE eNextScene);
	HRESULT			Loading_Editor();

public:
	CRITICAL_SECTION *Get_Crt() { return &m_Crt; }
	SCENE		Get_NextSceneID() const { return m_eNextScene; }
	_bool			Get_Finish() const { return m_isFinished; }
	const _tchar *Get_String() const { return m_szLoading; }

private:
	SCENE					m_eNextScene = SCENE_END;
	_bool					m_isFinished = false;
	_tchar					m_szLoading[MAX_PATH] = TEXT("");
	LPDIRECT3DDEVICE9		m_pGraphicDevice = nullptr;

private:
	HANDLE					m_hThread = 0;
	CRITICAL_SECTION		m_Crt;
};