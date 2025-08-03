#pragma once
#include "CScene.h"
#include "Engine_Define.h"
#include "Clinet_Define.h"

class CLoading_Scene :  public Engine::CScene
{

private:
	explicit CLoading_Scene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoading_Scene();

public:
	HRESULT			Ready_Loading(SCENE eID);
	virtual			_int		Update_Scene(const _float& fTimeDelta);
	virtual			void		LateUpdate_Scene(const _float& fTimeDelta);

private:
	SCENE		m_eNextScene = SCENE_END;
	class CLoader* m_pLoader = nullptr;

public:
	static CLoading_Scene* Create(LPDIRECT3DDEVICE9 pGraphicDev, SCENE eID);
	virtual void	Free();
};

