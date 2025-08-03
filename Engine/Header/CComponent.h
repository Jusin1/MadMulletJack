#pragma once

#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)

// 모든 컴포넌트의 추상클래스 
class ENGINE_DLL CComponent : public CBase
{
protected:
	explicit CComponent();
	explicit CComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent();

public:
	virtual HRESULT Initialize(void* pArg); // 초기화
	virtual _int Update_Component(const _float& fTimeDelta) { return 0; }
	virtual void LateUpdate_Component() { }

protected:
	LPDIRECT3DDEVICE9			m_pGraphicDev;
	_bool						m_bClone;

public:
	virtual CComponent* Clone(void* pArg = nullptr)	PURE;		
protected:
	virtual void	Free() override;
};

END