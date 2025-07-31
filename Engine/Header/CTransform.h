#pragma once
#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CTransform : public CComponent
{
private:
	explicit CTransform();
	explicit CTransform(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform();

public:
	void		Get_Info(INFO eType, _vec3* pInfo)
	{
		memcpy(pInfo, &m_matWorld.m[eType][0], sizeof(_vec3));
	}

	void		Move_Pos(const _vec3* pDir, const _float& fSpeed, const _float& fTimeDelta)
	{
		m_vInfo[INFO_POS] += *pDir * fSpeed * fTimeDelta;
	}

	void		Rotation(ROTATION eType, const _float& fAngle)
	{
		*(((_float*)&m_vAngle) + eType) += fAngle;
	}

	const	_matrix* Get_World() const { return &m_matWorld; }
	void	Get_World(_matrix* pWorld) const
	{
		*pWorld = m_matWorld;
	}	
		
	void		Set_Pos(_float _fX, _float _fY, _float _fZ)
	{
		m_vInfo[INFO_POS] = { _fX, _fY, _fZ };
	}

public:
	HRESULT		 Ready_Transform();
	virtual _int Update_Component(const _float& fTimeDelta);
	virtual void LateUpdate_Component();

	void		Chase_Target(const _vec3* pTargetPos, const _float& fSpeed, const _float& fTimeDelta);

	_matrix* Compute_Lookattarget(const _vec3* pTargetPos);

public:
	_vec3			m_vInfo[INFO_END];
	_vec3			m_vScale;
	_vec3			m_vAngle;

	_matrix			m_matWorld;

public:
	virtual CComponent* Clone();
	static CTransform* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free();

};

END
