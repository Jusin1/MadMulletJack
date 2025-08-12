#pragma once

#include "CUI.h"
#include "Clinet_Define.h"
#include "Client_Global.h"

class CWeapon : public CUI
{
protected:
	explicit CWeapon(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CWeapon(const CWeapon& rhs);
	virtual ~CWeapon();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg)override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	virtual HRESULT Texture_Clone() PURE;
	virtual HRESULT Change_Texture(const _tchar* componentTag) PURE;

public:
	void Set_IsAttack(_bool _bIsAttack) { m_bIsAttack = _bIsAttack; }
	_bool Get_IsAttack() const { return m_bIsAttack; }
	_bool Is_PlayerState_Change() // 플레이어의 상태가 바뀌었는지 + 변화값 받음
	{
		if (m_tInfo != CGlobal_Info::Get_Instance()->Get_PlayerInfo())
		{
			m_tInfo = CGlobal_Info::Get_Instance()->Get_PlayerInfo();
			return true;
		}

		return false;
	}

protected:
	PlayerStateInfo		m_tInfo;
	_bool				m_bIsAttack;

protected:
	virtual void Free() override;
};

