#pragma once

#include	"CBase.h"
#include	"CScene.h"

BEGIN(Engine)

// 전체 게임 씬을 관리
class ENGINE_DLL CManagement : public CBase
{
	DECLARE_SINGLETON(CManagement)

private:
	explicit	CManagement();
	virtual		~CManagement();

public:
	// 새로운 씬을 열고 기존에 있던 씬 메모리 해제
	HRESULT Open_Scene(unsigned int iSceneIdx, class CScene* pNewScene);
	void			Update_Scene(const _float& fTimeDelta);
	void			LateUpdate_Scene(const _float& fTimeDelta);

	CScene* GetCurrentScene() { return m_pCurrentScene; }
	_uint Get_CurrentSceneIdx() { return  m_iSceneIndex; }
	_uint Get_PrevSceneIdx() { return m_iPrevSceneIndex; }
	_uint Get_LastSceneIdx() { return m_iLastSceneIndex; }
	void Set_LastScene(_uint iIdx) { m_iLastSceneIndex = iIdx; }

private:
	CScene*					m_pCurrentScene;// 현재 씬
	_uint					m_iPrevSceneIndex; // 이전 씬엔덱스
	_uint					m_iSceneIndex;  // 현재 씬 인덱스
	_uint					m_iLastSceneIndex; // 마지막 씬 인덱스(전환 예약 용)

public:
	virtual void			Free();
};

END