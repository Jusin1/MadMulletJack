#pragma once
#include "CScene.h"

class CMonster_Suit;
class CImageUI;
class CStage_Snipe : public CScene
{
public:
	explicit CStage_Snipe(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStage_Snipe();

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

	void SetData(_uint _iSceneIndex);
	void InstancingObjects(const wstring &_Layer);
	void InstancingPrefabs();
public:
	static CStage_Snipe *Create(LPDIRECT3DDEVICE9 pGraphicDev);


private:
	// 몬스터 스폰 관련 ㅎ마수
	void SetMonsterActive();
	void ActivateNext(int n = 1);
	void TickDeathsAndProgress();
	void  SpawnKillIconAtIndex(int idx);
	void  ClearKillIcons();
	void  SetLayoutIcon();

private:
	// 몬스터 스폰 관련 변수
	int  m_iKillCount;
	bool m_bSpawned;
	std::vector<CMonster_Suit*> m_vMonsters;
	std::vector<bool>           m_vDeathMarked;
	std::vector<_vec3>          m_vSavedPos;
	int  m_iNextActivate;
	int  m_iInitialActivate;
	int  m_iTargetKills;
	std::vector<CImageUI*> m_vKillIcons;
	const int   m_iKillMax;
	// 시작 위치/간격/사이즈
	float m_killUIStartX;
	float m_killUIStartY;
	float m_killUISpacing;
	float m_killUISize;

protected:
	virtual void			Free();
};

