#pragma once

#include "CCharacter.h"
#include "Clinet_Define.h"
#include "Client_Global.h"
#include "CComponentMgr.h"
class CUIBase;

class CPlayer : public CCharacter
{
public:
	enum MOVEKEY { MVKEY_NON, MVKEY_NORMAL, MVKEY_LR, MVKEY_STOP, MVKEY_END };

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
	void Change_State(PLAYERSTATE _eState); // 이전 state 정리 후 state 전환
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

	void OPENING_Begin();
	void OPENING_On(const _float& fTimeDelta);
	void OPENING_End();

	void PLAYERDEAD_Begin();
	void PLAYERDEAD_On(const _float& fTimeDelta);
	void PLAYERDEAD_End();

	void Clear_Begin();

	void KeyInput(const _float& fTimeDelta);	// 모든 keyinput 여기서 처리
	void Set_State_Normal();					// state를 idle로 move를 normal로 바꿈

	const TCHAR* StateToString(PLAYERSTATE eState );	//debug
	const TCHAR* MoveToString(PLAYERMOVE eMove);		// debug

	void CountTime(const _float& fTimeDelta); // 시간 카운트 해줌 : hptime, dashcool time

	_bool StateTime_IsEnd(const _float& fTimeDelta, _float fAddTime =1.f); // state 시간 누적하면서 끝났는지 bool값으로 반환

	void Move(const _float& fTimeDelta); // move state를 이용해서 움직임 부여

	void Change_Move(PLAYERMOVE ePlayerMove, _bool bYFix = false); // move state 전환. : 속도 값 세팅

	// playermove에 따라 어떻게 움직일지
	void Move_Normal(const _float& fTimeDelta);
	void Move_Dash(const _float& fTimeDelta);
	void Move_Slide(const _float& fTimeDelta);
	void Move_Wall(const _float& fTimeDelta);
	void Move_JumpDash(const _float& fTimeDelta);
	void Move_Fall(const _float& fTimeDelta);

	void Change_Weapon(WEAPON _eWeapon);
	void Change_Weapon2(WEAPON2 _eWeapon2);

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
	MOVEKEY Get_MoveKey() const { return m_eMoveKey; }
	void Set_MoveKey(MOVEKEY _e) { m_eMoveKey = _e; }

	_float Get_GroundHeight()const { return m_fGround_Height; }
	void Set_GroundHeight(_float _fGroundHeight) {m_fGround_Height = _fGroundHeight;}
	_float Get_MaxHp() const { return m_fMaxHp; }
	void Set_MaxHp(_float _fMaxHp) { m_fMaxHp = _fMaxHp; }
	_float Get_FixY() const { return m_fFixY; }
	void Set_FixY(_float _fFixY) { m_fFixY = _fFixY; }

	_bool	Get_IsKeyInput()const { return m_bIsKeyInput; }
	void	Set_IsKeyInput(_bool _bKeyInput) { m_bIsKeyInput = _bKeyInput; }
	_bool	Get_IsInvincible() const { return m_bIsInvincible; }
	void	Set_IsInvincible(_bool _bIsInvincible) { m_bIsInvincible = _bIsInvincible; }
	_bool	Get_IsAttack() const { return m_bIsAttack; }
	void	Set_IsAttack(_bool _bAttack) { m_bIsAttack = _bAttack; }
	_bool	Get_IsCountHp() const { return m_bIsCountHp; }
	void	Set_IsCountHp(_bool _bCountHp) { m_bIsCountHp = _bCountHp; }

	// collider func
private:
	HRESULT			Set_Component();
	void			Set_Collider(const _float& fTimeDelta);

	// collider func
	void			Set_Collider_With_Clear();
	void			Set_Collider_With_Wall();
	void			Set_Collider_With_Door();
	void			Set_Colllider_With_Monster(const _float& fTimeDelta);
	_bool			Set_Collider_With_SlideWall();
	void			Set_Collider_With_Item();			// item과 충돌시 -> test : 자판기랑 collision
	_bool			Set_Collider_With_SpecialTile();	// 발차기 이벤트 나가는 tile || wall 충돌 검사

	void			HitFromObject(const _float& fTimeDelta, _float fHit); // hit만큼 목숨 깍아줌

	// setting func
private:
	HRESULT			Texture_Clone();
	HRESULT			Change_Texture(const _tchar* componentTag);
	HRESULT			Set_UI();

	// util func
private:
	_float			CosRadian(_vec3 v1, _vec3 v2); // 두 벡터를 정규화 후 내적값 반환
	void			PushBack(_vec3 vDistance); // vDistance 만큼 위치 조정

private:
	Engine::CColider_Cube* m_pColliderCom; // 큐브 충돌
	Engine::CColider_Sphere* m_pColiderSphere; // 구 충돌
	Engine::CTexture* m_pTextureCom; // 기본 텍스쳐 -> 지금은 필요없긴 한디 우선 남겨두겠습니다
	map<const _tchar*, CTexture*> m_mapTexture;

private:
	PlayerStateInfo m_tPlayerInfo;
	PlayerStateInfo m_tPrePlayerInfo;
	MOVEKEY m_eMoveKey;

	const _tchar* m_TimerTag;
	_float m_fGround_Height; // 안쓰는데 일단은 살려는 드릴게

	_bool m_bIsKeyInput; // 상태 변화를 위한 키 값 받을래 말래
	_bool m_bIsInvincible; // 무적 상태일래 말래
	_bool m_bIsAttack;		// 공격 할래 말래
	_bool m_bIsCountHp; // hp 깎을래 말래
	_bool m_bIsFixY; // Y고정 할래말래
	

	_float m_fAddTime; // state 누적 시간
	_float m_fStateTime; // state 지속할 시간
	_float m_fHitTime; // hit 시간
	_float m_fFixY; // 고정 y값
	
	_float m_fNormalSpeed;
	_float m_fDashCoolTime;

private:
	CUIBase* m_pPlayerUI = nullptr;
	CUIBase* m_pHpBarUI = nullptr;
	CUIBase* m_pWeaponUI = nullptr;

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void		Free();
};
