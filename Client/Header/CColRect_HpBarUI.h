#pragma once
#include "CUI.h"

class CColRect_HpBarUI : public CUI
{
private:
	explicit CColRect_HpBarUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CColRect_HpBarUI(const CColRect_HpBarUI& rhs);
	virtual ~CColRect_HpBarUI();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg) override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta) override;
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual			void		Render_GameObject() override;

public:
	virtual HRESULT			Set_Component() override;
	void Set_HpBarColor(_float _fPercent);

private:
	VIBuffer_Color* m_pVIColBufferCom = nullptr;

public:
	static  CColRect_HpBarUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

