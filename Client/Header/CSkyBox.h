#pragma once

#include "CGameObject.h"
#include "CComponentMgr.h"
#include "Clinet_Define.h"

class CSkyBox : public CGameObject
{
private:
	explicit CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSkyBox(const CGameObject& rhs);
	virtual ~CSkyBox();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg) override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Set_Component();

private:
	Engine::VIBuffer_Cube* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;

public:
	static CSkyBox* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

