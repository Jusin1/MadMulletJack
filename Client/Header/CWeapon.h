#pragma once

#include "CUI.h"
#include "Clinet_Define.h"
#include "Client_Global.h"

class CWeapon : public CUI
{
public:
	// idle : 바닥에 떨어져 있을 때
	// item : 주웠을때 필요한 ui texture
	// waepon : 플레이어가 움직이는 weapon 상태
	enum WAPSTATE {IDLE, ITEM, WEAPON, WAPSTATE_END};

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

	WAPSTATE Get_WapState() { return m_eWapState; }
	void Set_WapState(WAPSTATE _eWapState) { m_eWapState = _eWapState; }

protected:
	WAPSTATE			m_eWapState; // weapon이 다양한 ui에 쓰이기 때문에 wapstate를 줌
	PlayerStateInfo		m_tInfo; // 얘의 getter,setter는 아직 안 만들었음 <- 단순히 player의 info를 받는 역할만 해주기 때문
	_bool				m_bIsAttack;

protected:
	virtual void Free() override;
};

