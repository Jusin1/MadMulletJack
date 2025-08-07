#pragma once
#include "CScene.h"

class CEditorScene : public CScene
{
private:
	explicit CEditorScene(LPDIRECT3DDEVICE9 pGraphicDevice);
	virtual ~CEditorScene();

	virtual void Free() override;
public:
	static CEditorScene *Create(LPDIRECT3DDEVICE9 pGraphicDevice);

	virtual			HRESULT		Ready_Scene() override;
	virtual			_int		Update_Scene(const _float &fTimeDelta) override;
	virtual			void		LateUpdate_Scene(const _float &fTimeDelta) override;
	virtual			void		Render_Scene();

private:
	HRESULT			Ready_Camera_Layer(const _tchar *pLayerTag);
	HRESULT			Ready_EditLogic_Layer(const _tchar *pLayerTag);
};

