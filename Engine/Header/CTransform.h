#pragma once
#include "CComponent.h"

BEGIN(Engine)

// Transform : 위치, 회전, 스케일 정보 관리(이동/ 회전 기능)
class ENGINE_DLL CTransform : public CComponent
{
public:
	typedef struct tagTransformInfo
	{
		_vec3 vStartPos = { 0, 0, 0 };
		_float fSpeed;
		_float fRotationSpeed;
	}TRANSFORMINFO;
private:
	explicit CTransform();
	explicit CTransform(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform();

public:
	HRESULT		 Ready_Transform(); // 프로토 타입 초기화
	virtual HRESULT Initialize(void* pArg); // 클론 객체 초기화

public:

	_vec3		Get_Info(INFO eType) { return *(_vec3*)&m_matWorld.m[eType][0]; }
	void		Set_Info(INFO eType, _vec3 vState) { memcpy(&m_matWorld.m[eType][0], &vState, sizeof(_vec3)); }
	_vec3		Get_Scale();
	void		Set_Scale(_float x, _float y, _float z);
	
	const	_matrix* Get_World() const { return &m_matWorld; }
	void	Get_World(_matrix* pWorld) const{*pWorld = m_matWorld;}
	void	SetTransformInfo(TRANSFORMINFO TransformInfo) { m_TransformInfo = TransformInfo; }
	TRANSFORMINFO& GetTransformInfo() { return m_TransformInfo; }

	void Set_Dir(_float _fDir) { m_fDir = _fDir; }
	_float Get_Dir() { return m_fDir; }

public:
	// 로컬 방향 기준 이동 함수
	void Move_Forward(_float fTimeDelta, _float fHeight = NONE_HEIGHT);
	void Move_Backward(_float fTimeDelta, _float fHeight = NONE_HEIGHT);
	void Move_Left(_float fTimeDelta, _float fHeight = NONE_HEIGHT);
	void Move_Right(_float fTimeDelta, _float fHeight = NONE_HEIGHT);
	void Rotation(_vec3 vAxis, _float fTimeDelta);
	void RotationDegree(const _vec3& axis, float degrees);
	void SetDegreeForEditor(const _vec3 &axis, float degrees);


public:
	// 월드 좌표 기준 이동
	void Move_PosUp(_float fTimeDelta);
	void Move_PosDown(_float fTimeDelta);
	void Move_PosLeft(_float fTimeDelta);
	void Move_PosRight(_float fTimeDelta);
	void Move_PosTarget(_float fTimeDelta, _vec3 TargetPos, _vec3 distance);
	void Move_PosDir(_float fTimeDelta, _vec3 _vDir);

	// UIMove용 func
	void Move_YUp(_float fTimeDelta);
	void Move_YDown(_float fTimeDelta);
	void Move_RL(_float fTimeDelta, _float  fRange);
	void Move_YUpDown(_float fTimeDelta, _float  fRange);

public:
	void LookAt(_vec3 _targetPos);
	void ChaseTarget(_vec3 TargetPos, _vec3 distance);

public:
	HRESULT Apply_WorldMatrix();
	void SetTarget(_vec3 target) { m_vTarget = target; }
private:
	_matrix			m_matWorld;
	TRANSFORMINFO	m_TransformInfo;
	_vec3			m_vTarget;
	_float			m_fDir; // Move_RL 시 필요

public:
	virtual CComponent* Clone(void* pArg = nullptr) override;
	static CTransform* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free();
};
END
