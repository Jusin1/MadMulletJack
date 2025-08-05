#pragma once

#include "CCharacter.h"
#include "Clinet_Define.h"
#include "Client_Global.h"
#include "CComponentMgr.h"

class CPlayer : public CCharacter
{
public:
	enum STATE { IDLE, WALK, PLAYER_END };
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayer(const CGameObject& rhs);
	virtual ~CPlayer();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual HRESULT Initialize(void* pArg)override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

public:
	_vec3 Get_Pos();
	_vec3 Get_Look();
	_vec3 Get_Right();
	void Set_GroundY(float _fY) { m_fGround_Height = _fY; }

private:
	HRESULT			Set_Component();
	void			Set_Collider(void);
	void			Key_Input(const _float& fTimeDelta);

private:
	HRESULT Texture_Clone();
	HRESULT Change_Texture(const _tchar* LayerTag);

private:
	Engine::CVIBuffer_Rect* m_pBufferCom;
	Engine::CRenderer* m_pRenderCom;
	Engine::CColider_Cube* m_pColliderCom; // 큐브 충돌
	Engine::CColider_Sphere* m_pColiderSphere; // 구 충돌
	Engine::CTexture* m_pTextureCom; // 기본 텍스쳐
	Engine::CCalculator* m_pCalculatorCom;
	vector<CTexture*> m_vecTexture; //애니메이션 전용 텍스쳐

private:
	STATE m_eState = IDLE;
	STATE m_ePrevState = PLAYER_END;
	const _tchar* m_TimerTag = TEXT("");
	_float m_fGround_Height = 0.f;

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void		Free();
};
