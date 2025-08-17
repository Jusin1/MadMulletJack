#pragma once
#include "CMainWeapon.h"
class CGun : public CMainWeapon
{
protected:
	explicit CGun(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGun(const CGun& rhs);
	virtual ~CGun();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg)override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	// getter setter func
public:
	_bool Get_IsInfinite() const { return m_bIsInfinite; }
	void Set_IsInfinite(_bool _bIsInfinite) { m_bIsInfinite = _bIsInfinite; }
	_bool Get_IsZoom() const { return m_bIsZoom; }
	void Set_IsZoom(_bool _bIsZoom) { m_bIsZoom = _bIsZoom; }


protected:
	// 이거는 그냥 state로 빼서 생각해도 될듯
	_bool m_bIsInfinite;
	_bool m_bIsZoom;

protected:
	virtual void Free() override;
};