#include "pch.h"
#include "CMonster.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CMonster::CMonster(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CMonster::~CMonster()
{
}

HRESULT CMonster::Ready_GameObject()
{
	if(FAILED(CGameObject::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if(FAILED(SetComponent()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(5.f, 1.f, 5.f);

	return S_OK;
}

_int CMonster::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bDead)
		return DEAD;


	CGameObject::Update_GameObject(fTimeDelta);

	Update_Position(m_pTransformCom->Get_Info(INFO_POS));
	return NO_EVENT;
}

void CMonster::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);


}

void CMonster::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMonster::SetComponent(void* pArg)
{
	return S_OK;
}


CMonster* CMonster::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMonster* pInstance = new CMonster(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("pMonster Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster::Clone(void* pArg)
{
	CMonster* pInstance = new CMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("pMonster Clone Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster::Free()
{
	Engine::CGameObject::Free();
}
