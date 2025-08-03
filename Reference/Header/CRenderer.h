#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CGameObject.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer :    public CComponent
{

private:
	explicit CRenderer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CRenderer();

public:
	virtual HRESULT Ready_Render();
	virtual HRESULT Initialize(void* pArg);
public:
	HRESULT		Add_RenderGroup(RENDERID eType, CGameObject* pGameObject);
	HRESULT		Render_GameObject();

private:
	HRESULT		Render_Priority();
	HRESULT		Render_NonAlpha();
	HRESULT		Render_Alpha();
	HRESULT		Render_UI();

private:
	list<CGameObject*>			m_RenderGroup[RENDER_END];
	typedef list<class CGameObject*>		GAMEOBJECTS;

public:
	static CRenderer* Create(LPDIRECT3DDEVICE9 pGrahpicDev);
	virtual CComponent* Clone(void* pArg = nullptr)override;
	virtual void Free();
};
END
