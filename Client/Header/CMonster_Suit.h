#pragma once
#include "CMonster.h"
class CMonster_Suit : public CMonster
{
private:
	explicit CMonster_Suit(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMonster_Suit(const CMonster_Suit& rhs);
	virtual ~CMonster_Suit();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg) override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	virtual HRESULT Set_Component(void* pArg = nullptr);
	virtual void			Set_Collider(void);

protected:
	virtual _bool Picking(_vec3* PickingPoint) override;

protected:
	void SetUp_BillBoard();

protected:
	virtual HRESULT Texture_Clone();

protected:
	Engine::CVIBuffer_Rect* m_pBufferCom;
	Engine::CRenderer* m_pRendererCom;
	Engine::CColider_Sphere* m_pColiderHead;
	Engine::CColider_Sphere* m_pColiderBody;
	Engine::CColider_Sphere* m_pColiderBall;
	Engine::CColider_Sphere* m_pColiderLeg;
	Engine::CTexture* m_pTextureCom; // 기본 텍스쳐
	map<const _tchar*, CTexture*> m_mapTexture;

public:
	static CMonster_Suit* Create(LPDIRECT3DDEVICE9 pGrahpicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

