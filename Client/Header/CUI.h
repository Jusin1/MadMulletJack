#pragma once
#include "CGameObject.h"
#include "CComponentMgr.h"
#include "Clinet_Define.h"

class CUI :
    public CGameObject
{
protected:
	explicit CUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CUI(const CUI& rhs);
	virtual ~CUI();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg) override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	VIBuffer_Color* m_pColBufferCom = nullptr;

protected:
	_matrix					m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;

protected:
	virtual HRESULT			Set_Component();

public:
	static CUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

