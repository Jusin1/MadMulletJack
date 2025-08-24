#pragma once
#include "Clinet_Define.h"
#include "CBase.h"
#include "Engine_Define.h"


// 로딩을 위한 클래스
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
	float Get_Progress() const;
	void  PhaseBegin(float base, float weight, const wchar_t* name);
	void  PhaseStep(float local01, const wchar_t* msg = nullptr); 
	void  PhaseDone(const wchar_t* msgDone = L"로딩이 완료되었습니다.");

public:
	HRESULT			Ready_Loading(SCENE eNextScene); // 로딩 준비
	HRESULT			Loading_UI();
	HRESULT			Loading_Logo(); // 로고 씬 로딩
	HRESULT			Loading_Dev();	// Dev 씬 로딩
	HRESULT			Loading_Tutorial();	// Dev 씬 로딩
	HRESULT			Loading_Stage_1();	// Stage_1 로딩
	HRESULT			Loading_Stage_2();	// Stage_2 로딩
	HRESULT			Loading_Stage_3();	// Stage_3 로딩
	HRESULT			Loading_Snipe();	// Snipe 씬 로딩
	HRESULT			Loading_Rooftop();	// 옥상 보스 씬 로딩
	HRESULT			Loading_Road();	// 자동차 보스 씬 로딩
	HRESULT			Loading_MapObjectTexture_Src();
private:
	SCENE					m_eNextScene = SCENE_END;
	_bool					m_isFinished = false;
	_tchar					m_szLoading[MAX_PATH] = TEXT("");
	LPDIRECT3DDEVICE9		m_pGraphicDev = nullptr;

private:
	HANDLE					m_hThread = 0;
	CRITICAL_SECTION		m_Crt;

private:
	// 진행률 상태
	float m_progress = 0.f;      
	float m_phaseBase = 0.f;        
	float m_phaseWeight = 1.f;       

public:
	static CLoader* Create(LPDIRECT3DDEVICE9 pGrahpicDev, SCENE eNextScene);
	virtual void Free();	
};

