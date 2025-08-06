#pragma once
#include "CGameObject.h"
#include "Clinet_Define.h"
class CCharacter :
    public CGameObject
{
protected:
	explicit CCharacter(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCharacter(const CGameObject& rhs);
	virtual ~CCharacter();


public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg) override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

public:
	OBJID	Get_ObjID(void) { return m_eObjID; }
protected:
	OBJID m_eObjID;
	_bool m_bPickingTrue;

	virtual void Free() override;
};
