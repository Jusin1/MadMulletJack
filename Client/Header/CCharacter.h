#pragma once
#include "CGameObject.h"
#include "Clinet_Define.h"
#include "CComponentMgr.h"

namespace Engine
{
	class CGrounding;
}

class CCharacter :
	public CGameObject
{
protected:
	explicit CCharacter(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCharacter(const CCharacter& rhs);
	virtual ~CCharacter();

	virtual void Free() override;
public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Initialize(void* pArg) override;
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	HRESULT	Set_Component();
	void	Set_OnTerrain(const _float &fTimeDelta);

	// getter setter func
public:
	OBJID	Get_ObjID(void) { return m_eObjID; }

	void Set_Hp(_float _fHp) { m_fHp = _fHp; }
	_float Get_Hp() const { return m_fHp; }

	void Set_Attack(_float _fAttack) { m_fAttack = _fAttack; }
	_float Get_Attack() const { return m_fAttack; }

	void Set_Velocity(_float _fVelocity) { m_fVelocity = _fVelocity; }
	_float Get_Velocity() const { return m_fVelocity; }

	void Set_Jumping(_bool _bJumping) { m_bJumping = _bJumping; }
	_bool Get_Jumping() { return m_bJumping; }

	void Set_JumpTime(_float _fJumpTime) { m_fJumpTime = _fJumpTime; }
	_float Get_JumpTime() const { return m_fJumpTime; }

	void Gravity(const _float &fDeltaTime);

	WallType GetGroundedFloorType();
protected:
	Engine::CCalculator* m_pCalculatorCom; // terrain충돌 위함
	Engine::CVIBuffer_Rect* m_pBufferCom;
	Engine::CGrounding *m_pGroundingCom;
protected:
	OBJID m_eObjID; //OBJ_END
	_bool m_bPickingTrue;

	_float m_fHp;		// hp
	_float m_fAttack;	// 공격치
	_float m_fVelocity; // 중력

	_bool m_bJumping;
	_float m_fJumpTime;
};
