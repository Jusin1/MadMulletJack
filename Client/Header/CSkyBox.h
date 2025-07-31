#pragma once

#include "CGameObject.h"
#include "CProtoMgr.h"

class CSkyBox : public CGameObject
{
private:
	explicit CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSkyBox(const CGameObject& rhs);
	virtual ~CSkyBox();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

private:
	Engine::CCubeTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CTransform* m_pTransformCom;

public:
	static CSkyBox* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

