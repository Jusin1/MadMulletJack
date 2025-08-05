#pragma once
#include "CScene.h"


class CStage : public CScene
{
public:
	explicit CStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStage();

public:
	virtual			HRESULT		Ready_Scene();
	virtual			_int		Update_Scene(const _float& fTimeDelta);
	virtual			void		LateUpdate_Scene(const _float& fTimeDelta);
	virtual			void		Render_Scene();

private:
	HRESULT			Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_Camera_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_Player_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_Monster_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_GameLogic_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_UI_Layer(const _tchar* pLayerTag);

public:
	static CStage* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void			Free();
};

