#pragma once

#include "CGameObject.h"
#include "CProtoMgr.h"

class CMonster : public CGameObject
{
private:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMonster(const CGameObject& rhs);
	virtual ~CMonster();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg) override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT SetComponent(void* pArg = nullptr);

private:
	Engine::CTransform* m_pTransformCom;


public:
	static CMonster* Create(LPDIRECT3DDEVICE9 pGrahpicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

