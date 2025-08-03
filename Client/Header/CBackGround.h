#pragma once

#include "CGameObject.h"
#include "CComponentMgr.h"
#include "Clinet_Define.h"

class CBackGround :   public CGameObject
{
private:
	explicit CBackGround(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBackGround(const CGameObject& rhs);
	virtual ~CBackGround();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg);
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Set_Component();

private:
	Engine::CVIBuffer_Rect*		m_pBufferCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CRenderer* m_pRenderCom;


public:
	static CBackGround* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void		Free();
};

