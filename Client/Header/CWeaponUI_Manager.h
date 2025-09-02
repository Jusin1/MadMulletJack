#pragma once
#include "CUI.h"

// player의 wepaon 상태에 따라 알맞게 객체 생성

class CWeaponUI_Manager : public CUI
{
private:
	explicit CWeaponUI_Manager(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CWeaponUI_Manager(const CWeaponUI_Manager& rhs);
	virtual ~CWeaponUI_Manager();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg) override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta) override;
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual			void		Render_GameObject() override;

private:
	HRESULT Set_WeaponUI();
	HRESULT Set_Weapon2UI();
	void	Weapon_Change();
	void	Weapon2_Change();

	void	TagUI_SetActive(const _tchar* pTag, _bool _bActive);

	// weapon create func
	HRESULT Create_Pistol(_uint _iSceneIdx);
	HRESULT Create_Sniper(_uint _iSceneIdx);
	HRESULT Create_Katana(_uint _iSceneIdx);
	HRESULT Create_ShotGun(_uint _iSceneIdx);
	HRESULT Create_MiniGun(_uint _iSceneIdx);
	
	// weapon create func
	HRESULT Create_Knife(_uint _iSceneIdx);

	// effect
	HRESULT Create_AimUI(const _tchar* pProtoTag, _float _fsizeX, _float _fSizeY);
	void	Delete_AimUI(const _tchar* pTag); // 그냥 delete

	void SpawnInsEff();
	void DeleteEff(const _tchar* pTag); // ani가 끝나면 delete

	//getter setter func
public:
	WEAPON Get_Weapon() const { return m_eWeapon; }
	void Set_Weapon(WEAPON _eWeapon) { m_eWeapon = _eWeapon; }

private:
	WEAPON m_eWeapon;
	WEAPON2 m_eWeapon2;

public:
	static  CWeaponUI_Manager* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

