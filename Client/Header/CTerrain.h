#pragma once

#include "CGameObject.h"
#include "CComponentMgr.h"
#include "Clinet_Define.h"

class CTerrain : public CGameObject
{
private:
	explicit CTerrain(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTerrain(const CGameObject& rhs);
	virtual ~CTerrain();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg) override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT SetComponent(void* pArg);

private:
	void SetUp_TerrainY();

private:
	Engine::CVIBuffer_Terrian* m_pBufferCom;
	Engine::CTexture*	m_pTextureCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CRenderer* m_pRendererCom;

public:
	static CTerrain* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CTerrain* Clone(void* pArg);
	virtual void		Free();
};

