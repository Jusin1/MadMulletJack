#include "pch.h"
#include "CColiderManager.h"
#include "CColider_Cube.h"
#include "CEndPoint.h"
#include "CUIManager.h"

CEndPoint::CEndPoint(LPDIRECT3DDEVICE9 pGraphicDevice)
	: CDummyBase(pGraphicDevice, EnvType::ENDPOINT)
{
}

CEndPoint::CEndPoint(const CEndPoint &rhs)
	: CDummyBase(rhs)
{
}

CEndPoint::~CEndPoint()
{
}

void CEndPoint::Free()
{
	CDummyBase::Free();
}

CEndPoint *CEndPoint::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
	CEndPoint *pInstance = new CEndPoint(pGraphicDevice);
	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CEndPoint::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject *CEndPoint::Clone(void *pArg)
{
	CEndPoint *pInstance = new CEndPoint(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CEndPoint::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

HRESULT CEndPoint::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject())) return E_FAIL;

	return S_OK;
}

HRESULT CEndPoint::Initialize(void *pArg)
{
	if (FAILED(__super::Initialize(pArg))) return E_FAIL;

	if (FAILED(Set_Component())) return E_FAIL;



	return S_OK;
}

_int CEndPoint::Update_GameObject(const _float &fTimeDelta)
{
	if (m_bDead)
		return DEAD;

	__super::Update_GameObject(fTimeDelta);
	CColiderManager::GetInstance()->Add_CollisionGroup(CColiderManager::COLLISION_DUMMY, this);

	return NO_EVENT;
}

void CEndPoint::LateUpdate_GameObject(const _float &fTimeDelta)
{
	if (m_bDead)
		return;

	Update_Collider();
	__super::LateUpdate_GameObject(fTimeDelta);
}

void CEndPoint::Render_GameObject()
{
	if (m_bDead)
		return;

	__super::Render_GameObject();
}

void CEndPoint::Update_Collider()
{
	if (m_pCollider)
		m_pCollider->Update_ColliderBox();

}

HRESULT CEndPoint::Set_Component()
{
	CColider_Cube::COLLRECTDESC tDesc;

	tDesc.bMapObject = true;
	if (FAILED(Add_Components(L"Com_Collider_Cube", SCENE_STATIC, L"Proto_Colider_Cube", (CComponent **)&m_pCollider, &tDesc)))
		return E_FAIL;

	m_pCollider->Set_Transform(m_pTransformCom);

	return S_OK;
}
