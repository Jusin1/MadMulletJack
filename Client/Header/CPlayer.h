#pragma once

#include "CCharacter.h"
#include "Clinet_Define.h"
#include "Client_Global.h"
#include "CComponentMgr.h"
class CUIBase;

class CPlayer : public CCharacter
{
public:
	enum MOVEKEY { MOVE_NORMAL, MOVE_LR, MOVE_NON, MOVE_STOP, MOVE_END };

private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg)override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	void ChangeState(PLAYERSTATE  _e);
	void StateNormalSet();

	void StateBegin(PLAYERSTATE _e);
	void StateEnd(PLAYERSTATE _e);
	void StateUpdate(PLAYERSTATE _e, const _float& fTimeDelta);

	void IDLE_Begin();
	void IDLE_On(const _float& fTimeDelta);
	void IDLE_End();

	void JUMP_Begin();
	void JUMP_On(const _float& fTimeDelta);
	void JUMP_End();

	void DASH_ATTACK_Begin();
	void DASH_ATTACK_On(const _float& fTimeDelta);
	void DASH_ATTACK_End();

	void DASH_Begin();
	void DASH_On(const _float& fTimeDelta);
	void DASH_End();

	void SLIED_Begin();
	void SLIED_On(const _float& fTimeDelta);
	void SLIED_End();

	void KICK_Begin();
	void KICK_On(const _float& fTimeDelta);
	void KICK_End();

	void ATTACK_Begin();
	void ATTACK_On(const _float& fTimeDelta);
	void ATTACK_End();

	void ATTACK_INSTANT_Begin();
	void ATTACK_INSTANT_On(const _float& fTimeDelta);
	void ATTACK_INSTANT_End();

	void RELOAD_Begin();
	void RELOAD_On(const _float& fTimeDelta);
	void RELOAD_End();

	void HIT_Begin();
	void HIT_On(const _float& fTimeDelta);
	void HIT_End();

	void DOPING_Begin();
	void DOPING_On(const _float& fTimeDelta);
	void DOPING_End();

	void WALL_Begin();
	void WALL_On(const _float& fTimeDelta);
	void WALL_End();

	void OPENING_Begin();
	void OPENING_On(const _float& fTimeDelta);
	void OPENING_End();

	void PLAYERDEAD_Begin();
	void PLAYERDEAD_On(const _float& fTimeDelta);
	void PLAYERDEAD_End();

	void KeyInput(const _float& fTimeDelta);
	void Set_State_Idle();

	const TCHAR* StateToString(PLAYERSTATE eState); //debug
	void CountHp(const _float& fTimeDelta);

	void UIAniFinish(const _tchar* pTag); // ui의 animation이 끝나면 state = IDLE;

	// getter setter func
public:
	_vec3 Get_Pos();
	_vec3 Get_Look();
	_vec3 Get_Right();
	void Set_GroundY(float _fY) { m_fGround_Height = _fY; }

	PlayerStateInfo Get_State() { return m_tPlayerInfo; }
	void Set_State(PlayerStateInfo _tInfo) { m_tPlayerInfo = _tInfo; }
	PlayerStateInfo Get_PrevState() { return m_tPrePlayerInfo; }
	void Set_PrevState(PlayerStateInfo _tInfo) { m_tPrePlayerInfo = _tInfo; }
	MOVEKEY Get_MoveKey() { return m_eMove; }
	void Set_MoveKey(MOVEKEY _e) { m_eMove = _e; }

	_float Get_GroundHeight() { return m_fGround_Height; }
	void Set_GroundHeight(_float _fGroundHeight) {m_fGround_Height = _fGroundHeight;}

	_bool	Get_IsKeyInput() { return m_bIsKeyInput; }
	void	Set_IsKeyInput(_bool _bKeyInput) { m_bIsKeyInput = _bKeyInput; }
	_bool	Get_IsInvincible() { return m_bIsInvincible; }
	void	Set_IsInvincible(_bool _bIsInvincible) { m_bIsInvincible = _bIsInvincible; }
	_bool	Get_IsAttack() { return m_bIsAttack; }
	void	Set_IsAttack(_bool _bAttack) { m_bIsAttack = _bAttack; }
	_bool	Get_IsCountHp() { return m_bIsCountHp; }
	void	Set_IsCountHp(_bool _bCountHp) { m_bIsCountHp = _bCountHp; }

private:
	HRESULT			Set_Component();
	void			Set_Collider(void);

private:
	HRESULT Texture_Clone();
	HRESULT Change_Texture(const _tchar* componentTag);

private:
	Engine::CColider_Cube* m_pColliderCom; // 큐브 충돌
	Engine::CColider_Sphere* m_pColiderSphere; // 구 충돌
	Engine::CTexture* m_pTextureCom; // 기본 텍스쳐
	map<const _tchar*, CTexture*> m_mapTexture;

private:
	PlayerStateInfo m_tPlayerInfo;
	PlayerStateInfo m_tPrePlayerInfo;
	MOVEKEY m_eMove;

	const _tchar* m_TimerTag;
	_float m_fGround_Height;

	_bool m_bIsKeyInput;
	_bool m_bIsInvincible;
	_bool m_bIsAttack;
	_bool m_bIsCountHp;


private:
	CUIBase* m_pPlayerUI = nullptr;

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void		Free();
};
