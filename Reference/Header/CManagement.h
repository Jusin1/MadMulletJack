#pragma once

#include	"CBase.h"
#include	"CScene.h"

BEGIN(Engine)

class ENGINE_DLL CManagement : public CBase
{
	DECLARE_SINGLETON(CManagement)

private:
	explicit	CManagement();
	virtual		~CManagement();

public:
	HRESULT Open_Scene(unsigned int iSceneIdx, class CScene* pNewScene);
	void			Update_Scene(const _float& fTimeDelta);
	void			LateUpdate_Scene(const _float& fTimeDelta);

	CScene* GetCurrentScene() { return m_pCurrentScene; }
	_uint Get_CurrentSceneIdx() { return  m_iSceneIndex; }
	_uint Get_PrevSceneIdx() { return m_iPrevSceneIndex; }
	_uint Get_LastSceneIdx() { return m_iLastSceneIndex; }
	void Set_LastScene(_uint iIdx) { m_iLastSceneIndex = iIdx; }

private:
	CScene*					m_pCurrentScene = nullptr;
	_uint					m_iPrevSceneIndex;
	_uint					m_iSceneIndex; 
	_uint					m_iLastSceneIndex;

public:
	virtual void			Free();
};

END