#pragma once

#include "CCharacter.h"
#include "CComponentMgr.h"
#include "Clinet_Define.h"
#include "Client_Global.h"

class CMonster : public CCharacter
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
	HRESULT Set_Component(void* pArg = nullptr);
	void			Set_Collider(void);
	void			Key_Input(); // 테스트용 지워야함

private: 
	virtual _bool Picking(_vec3* PickingPoint) override;
	virtual void PickingTrue() override;

private:
	void SetUp_BillBoard();

private:
	HRESULT Texture_Clone();
	HRESULT Change_Texture(const _tchar* LayerTag);

private:
	Engine::CVIBuffer_Rect* m_pBufferCom;
	Engine::CRenderer* m_pRendererCom;
	Engine::CColider_Cube* m_pColiderCom;
	Engine::CColider_Sphere* m_pColiderSpherCom;
	Engine::CTexture* m_pTextureCom; // 기본 텍스쳐
	map<const _tchar*, CTexture*> m_mapTexture;
	_vec3 m_vecOutPos; 

public:
	static CMonster* Create(LPDIRECT3DDEVICE9 pGrahpicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
