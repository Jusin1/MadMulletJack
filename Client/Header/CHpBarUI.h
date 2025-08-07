#pragma once
#include "CUI.h"

namespace Engine
{
	class VIBuffer_Color;
}

class CHpBarUI : public CUI
{
public:
    explicit CHpBarUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CHpBarUI(const CHpBarUI& rhs);
    virtual ~CHpBarUI();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg) override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta) override;
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual			void		Render_GameObject() override;

private:
	void			Key_UI(const _float& fTimeDelta);

public:
	static  CHpBarUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

protected:
	virtual HRESULT			Set_Component();

public:
	void	Set_HpPersent(_float m_fHPPercent);

private:
    _float		m_fHPPercent; // 체력 비율
	VIBuffer_Color *m_pColBufferCom = nullptr;
};

