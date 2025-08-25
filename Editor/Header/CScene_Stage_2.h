#pragma once
#include "CScene.h"

class CScene_Stage_2 : public CScene
{
private:
	explicit CScene_Stage_2(LPDIRECT3DDEVICE9 pGraphicDevice);
	virtual ~CScene_Stage_2();

	virtual void Free() override;
public:
	static CScene_Stage_2 *Create(LPDIRECT3DDEVICE9 pGraphicDevice);

	virtual			HRESULT		Ready_Scene() override;
	virtual			_int		Update_Scene(const _float &fTimeDelta) override;
	virtual			void		LateUpdate_Scene(const _float &fTimeDelta) override;
	virtual			void		Render_Scene();

	virtual HRESULT SaveData() override;
private:
	HRESULT			Ready_Camera_Layer(const _tchar *pLayerTag);
	HRESULT			Ready_Dummy_Layer(const _tchar *pLayerTag);
	HRESULT			Ready_EditLogic_Layer(const _tchar *pLayerTag);
	HRESULT			Ready_Wall_Layer(const _tchar *pLayerTag);
	HRESULT			Ready_Tile_Layer(const _tchar *pLayerTag);
	HRESULT			Ready_EnvObj_Layer(const _tchar *pLayerTag);
	HRESULT			Ready_Monster_Layer(const _tchar *pLayerTag);
	HRESULT			Ready_Light_Layer(const _tchar *pLayerTag);
};

