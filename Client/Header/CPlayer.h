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

public:
	void Add_Hp(_float _fAddHp); // 체력의 증감을 다룸 -> maxHp 보정 + state:PLAYERDEAD 처리

	// state func
private:
	// 이전state end -> statenormalset -> 현재 state begin
	void ChangeState(PLAYERSTATE  _e);	//현재 state와 이전 state가 바뀌었으면 바꿔줌
	void StateNormalSet();				// state 변화전 변수들 초기화

	void StateBegin(PLAYERSTATE _e); // state의 begin 함수를 실행
	void StateEnd(PLAYERSTATE _e);	// state의 end 함수를 실행
	void StateUpdate(PLAYERSTATE _e, const _float& fTimeDelta);	// state의 on 하수를 실행

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

	void ZOOMING_Begin();
	void ZOOMING_On(const _float& fTimeDelta);
	void ZOOMING_End();

	void ZOOM_Begin();
	void ZOOM_On(const _float& fTimeDelta);
	void ZOOM_End();

	void RELOAD_Begin();
	void RELOAD_On(const _float& fTimeDelta);
	void RELOAD_End();

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

	void Clear_Begin();

	void KeyInput(const _float& fTimeDelta);
	void Set_State_Idle(); // state를 idle로 바꿈

	const TCHAR* StateToString(PLAYERSTATE eState); //debug
	void CountHp(const _float& fTimeDelta);

	_bool StateTime_IsEnd(const _float& fTimeDelta, _float fAddTime =1.f);

	// getter setter func
public:
	_vec3 Get_Pos();
	_vec3 Get_Look();
	_vec3 Get_Right();
	void Set_GroundY(float _fY) { m_fGround_Height = _fY; }

	PlayerStateInfo Get_State()const { return m_tPlayerInfo; }
	void Set_State(PlayerStateInfo _tInfo) { m_tPlayerInfo = _tInfo; }
	PlayerStateInfo Get_PrevState()const { return m_tPrePlayerInfo; }
	void Set_PrevState(PlayerStateInfo _tInfo) { m_tPrePlayerInfo = _tInfo; }
	MOVEKEY Get_MoveKey() const { return m_eMove; }
	void Set_MoveKey(MOVEKEY _e) { m_eMove = _e; }

	_float Get_GroundHeight()const { return m_fGround_Height; }
	void Set_GroundHeight(_float _fGroundHeight) {m_fGround_Height = _fGroundHeight;}
	_float Get_MaxHp() const { return m_fMaxHp; }
	void Set_MaxHp(_float _fMaxHp) { m_fMaxHp = _fMaxHp; }

	_bool	Get_IsKeyInput()const { return m_bIsKeyInput; }
	void	Set_IsKeyInput(_bool _bKeyInput) { m_bIsKeyInput = _bKeyInput; }
	_bool	Get_IsInvincible() const { return m_bIsInvincible; }
	void	Set_IsInvincible(_bool _bIsInvincible) { m_bIsInvincible = _bIsInvincible; }
	_bool	Get_IsAttack() const { return m_bIsAttack; }
	void	Set_IsAttack(_bool _bAttack) { m_bIsAttack = _bAttack; }
	_bool	Get_IsCountHp() const { return m_bIsCountHp; }
	void	Set_IsCountHp(_bool _bCountHp) { m_bIsCountHp = _bCountHp; }

private:
	HRESULT			Set_Component();
	void			Set_Collider(const _float& fTimeDelta);
	_float			CosRadian(_vec3 v1, _vec3 v2);
	void			HitFromObject(const _float& fTimeDelta,_float fHit);
	void			Set_Collider_With_Wall();
	void			Set_Collider_With_Door();
private:
	HRESULT Texture_Clone();
	HRESULT Change_Texture(const _tchar* componentTag);
	HRESULT Set_PlayerUI();
	HRESULT Set_HpBarUI();

private:
	Engine::CColider_Cube* m_pColliderCom; // 큐브 충돌
	Engine::CColider_Sphere* m_pColiderSphere; // 구 충돌
	Engine::CTexture* m_pTextureCom; // 기본 텍스쳐 -> 지금은 필요없긴 한디 우선 남겨두겠습니다
	map<const _tchar*, CTexture*> m_mapTexture;

private:
	PlayerStateInfo m_tPlayerInfo;
	PlayerStateInfo m_tPrePlayerInfo;
	MOVEKEY m_eMove;

	const _tchar* m_TimerTag;
	_float m_fGround_Height;

	_bool m_bIsKeyInput; // 상태 변화를 위한 키 값 받을래 말래
	_bool m_bIsInvincible; // 무적 상태일래 말래
	_bool m_bIsAttack;		// 공격 할래 말래
	_bool m_bIsCountHp; // hp 깎을래 말래

	_float m_fAddTime; // state 누적 시간
	_float m_fStateTime; // state 지속할 시간
	_float m_fHitTime; // hit 시간

private:
	CUIBase* m_pPlayerUI = nullptr;
	CUIBase* m_pHpBarUI = nullptr;

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void		Free();
};
