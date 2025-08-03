#pragma once
#include "CScene.h"
#include "Engine_Define.h"
#include "Clinet_Define.h"

// 로딩 씬
class CLoading_Scene :  public Engine::CScene
{

private:
	explicit CLoading_Scene(LPDIRECT3DDEVICE9 pGraphicDev); 
	virtual ~CLoading_Scene();

public:
	HRESULT			Ready_Loading(SCENE eID); // 로딩 준비
	virtual			_int		Update_Scene(const _float& fTimeDelta); 
	virtual			void		LateUpdate_Scene(const _float& fTimeDelta);

private:
	SCENE		m_eNextScene = SCENE_END; // 로딩이 끝난 후 이동할 씬
	class CLoader* m_pLoader = nullptr; // Loader객체

public:
	static CLoading_Scene* Create(LPDIRECT3DDEVICE9 pGraphicDev, SCENE eID);
	virtual void	Free();
};

