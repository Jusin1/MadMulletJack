#pragma once
#include "CWeapon.h"
class CMainWeapon : public CWeapon
{
protected:
	explicit CMainWeapon(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMainWeapon(const CMainWeapon& rhs);
	virtual ~CMainWeapon();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg)override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

public:
	_bool Get_IsEmpty() const { return m_bIsEmpty; }
	void Set_IsEmpty(_bool _bIsEmpty) { m_bIsEmpty = _bIsEmpty; }

	_int Get_Bullet() const { return m_iBullet; }
	void Set_Bullet(_int _iBullet) { m_iBullet = _iBullet; }
	_int Get_MaxBullet() const { return m_iMaxBullet; }
	void Set_MaxBullet(_int _iMaxBullet) { m_iMaxBullet = _iMaxBullet; }
	_int Get_Power() const { return m_iPower; }
	void Set_Powery(_int _iPower) { m_iPower = _iPower; }
	_int Get_Precision() const { return m_iPrecision; }
	void Set_Precision(_int _iPrecision) { m_iPrecision = _iPrecision; }
	_int Get_Speed() const { return m_iSpeed; }
	void Set_Speed(_int _iSpeed) { m_iSpeed = _iSpeed; }

	_float Get_Range() const { return m_fRange; }
	void Set_Range(_float _fRange) { m_fRange = _fRange; }

protected:
	_bool m_bIsEmpty;	// 총알을 다썼는지 -> reload 판단시 필요
	_int m_iBullet;		// 현재 bullet 수
	_int m_iMaxBullet;	// 최대 maxBullet 수
	_int m_iPower;		// 공격도
	_int m_iPrecision;	// 정확도 <- 이거는 나중에 빼도 될듯.. 원작에 있어서 넣었어유..
	_int m_iSpeed;		// 스피드 <- 총알 속도
	_float m_fRange;	// 공격 범위

protected:
	virtual void Free() override;

};

