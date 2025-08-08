#pragma once

#include "CCharacter.h"
#include "CComponentMgr.h"
#include "Clinet_Define.h"
#include "Client_Global.h"

class CMonster : public CCharacter
{
protected:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMonster(const CMonster& rhs);
	virtual ~CMonster();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg);
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	HRESULT	Set_Component();

protected: 
	virtual _bool Picking(_vec3* PickingPoint) override;
	virtual void PickingTrue() override;

protected:
	void SetUp_BillBoard();

protected:
	virtual HRESULT Texture_Clone();
	virtual HRESULT Change_Texture(const _tchar* LayerTag);
	
protected:
	Engine::CColider_Sphere* m_pColiderCom;
	Engine::CTexture* m_pTextureCom; // 기본 텍스쳐
	map<const _tchar*, CTexture*> m_mapTexture;

public:
	static CMonster* Create(LPDIRECT3DDEVICE9 pGrahpicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
