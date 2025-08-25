#pragma once
#include "CUI.h"

// player의 wepaon 상태에 따라 알맞게 객체 생성

class CUIManager_Weapon : public CUI
{
private:
	explicit CUIManager_Weapon(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CUIManager_Weapon(const CUIManager_Weapon& rhs);
	virtual ~CUIManager_Weapon();

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
	void	Weapon2_On();
	void	Weapon2_Off();
	void	TagUI_SetActive(const _tchar* pTag, _bool _bActive);

	//getter setter func
public:
	WEAPON Get_Weapon() const { return m_eWeapon; }
	void Set_Weapon(WEAPON _eWeapon) { m_eWeapon = _eWeapon; }

private:
	WEAPON m_eWeapon;
	WEAPON2 m_eWeapon2;

public:
	static  CUIManager_Weapon* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

