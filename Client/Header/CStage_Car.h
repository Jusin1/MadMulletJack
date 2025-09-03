#pragma once
#include "CScene.h"


class CStage_Car : public CScene
{
public:
	explicit CStage_Car(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStage_Car();

public:
	virtual			HRESULT		Ready_Scene();
	virtual			_int		Update_Scene(const _float &fTimeDelta);
	virtual			void		LateUpdate_Scene(const _float &fTimeDelta);
	virtual			void		Render_Scene();

private:
	HRESULT			Ready_SlideWall_Layer(const _tchar *pLayerTag);
	HRESULT			Ready_Floor_Layer(const _tchar *pLayerTag);
	HRESULT			Ready_Wall_Layer(const _tchar *pLayerTag);
	HRESULT			Ready_Ceiling_Layer(const _tchar *pLayerTag);

	HRESULT			Ready_Prefab_Layer(const _tchar *pLayerTag);
	HRESULT			Ready_EnvObj_Layer(const _tchar *pLayerTag);
	HRESULT			Ready_Tile_Layer(const _tchar *pLayerTag);
	HRESULT			Ready_Camera_Layer(const _tchar *pLayerTag);
	HRESULT			Ready_Player_Layer(const _tchar *pLayerTag);
	HRESULT			Ready_Monster_Layer(const _tchar *pLayerTag);
	HRESULT			Ready_GameLogic_Layer(const _tchar *pLayerTag);
	HRESULT			Ready_UI_Layer(const _tchar *pLayerTag);
	HRESULT			Ready_Boss_Layer(const _tchar *pLayerTag);

	void SetData(_uint _iSceneIndex);
	void InstancingObjects(const wstring &_Layer);
	void InstancingPrefabs();
public:
	static CStage_Car *Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void			Free();
};

