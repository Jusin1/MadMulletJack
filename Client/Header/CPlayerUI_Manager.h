#pragma once
#include "CUI.h"

class CPlayerUI_Manager : public CUI
{
private:
	explicit CPlayerUI_Manager(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayerUI_Manager(const CPlayerUI_Manager& rhs);
	virtual ~CPlayerUI_Manager();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg) override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta) override;
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual			void		Render_GameObject() override;

private:
	HRESULT Set_PlayerUI();

	HRESULT	PlayerUI_OnOff();
	HRESULT	TagUI_SetActive(const _tchar* pTag, _bool _bActive);

	//getter setter func
public:
	PlayerStateInfo Get_Info() const { return m_tInfo; }
	void Set_Info(PlayerStateInfo _tPlayerStateInfo) { m_tInfo = _tPlayerStateInfo; }

private:
	PlayerStateInfo m_tInfo;

public:
	static  CPlayerUI_Manager* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

