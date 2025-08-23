#pragma once
#include "CScene.h"
#include "Clinet_Define.h"

class CLogo :  public CScene
{
public:
	explicit CLogo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLogo();

public:
	virtual			HRESULT		Ready_Scene();
	virtual			_int		Update_Scene(const _float& fTimeDelta);
	virtual			void		LateUpdate_Scene(const _float& fTimeDelta);
	virtual			void		Render_Scene();

private:
	HRESULT			Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_GameLogic_Layer(const _tchar* pLayerTag) { return 0; }
	HRESULT			Ready_UI_Layer(const _tchar* pLayerTag);
	

public:
	static CLogo* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	bool m_bOpenScene;

protected:
	virtual void			Free();
};

