#pragma once
#include "CScene.h"
#include "Engine_Define.h"
#include "Editor_Define.h"

class CEditLoader;

class CEditorLoadingScene : public CScene
{
private:
	explicit CEditorLoadingScene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEditorLoadingScene();

public:
	HRESULT			Ready_Loading(SCENE eID);
	virtual			_int		Update_Scene(const _float &fTimeDelta);
	virtual			void		LateUpdate_Scene(const _float &fTimeDelta);

private:
	SCENE		m_eNextScene = SCENE_END;
	CEditLoader *m_pLoader = nullptr;

public:
	static CEditorLoadingScene *Create(LPDIRECT3DDEVICE9 pGraphicDev, SCENE eID);
	virtual void	Free();
};

