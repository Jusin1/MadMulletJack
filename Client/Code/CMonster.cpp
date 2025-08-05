#include "pch.h"
#include "CMonster.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CColiderManager.h"
#include "CObjectManager.h"

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

	if(FAILED(Set_Component()))
		return E_FAIL;

	Set_CollisionMatrix();

	
	m_pTransformCom->Set_Info(INFO_POS, _vec3(4.f, 1.f, 0.f));
	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);

	return S_OK;
}

_int CMonster::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bDead)
		return DEAD;

	list<CGameObject*>* objList = CObjectManager::GetInstance()->Get_ObjectList(SCENE_STAGE, L"GameLogic_Layer");
	CTransform* pPlayer = dynamic_cast<CTransform*>(CObjectManager::GetInstance()->Get_Component(SCENE_STAGE, L"GameLogic_Layer", L"Com_Transform", 0));


	
	CGameObject::Update_GameObject(fTimeDelta);
	CColiderManager::GetInstance()->Add_CollisionGroup(CColiderManager::COLLISION_MONSTER, this);
	m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this);
	return NO_EVENT;
}

void CMonster::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Update_Position(m_pTransformCom->Get_Info(INFO_POS));
	Set_Collider();

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMonster::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTransformCom->Apply_WorldMatrix();
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	
#ifdef _DEBUG
	if (g_ColiderRender && m_pColiderCom != nullptr)
	{
		m_pColiderCom->Render_ColliderBox(); // 충돌체 디버그 렌더
	}
#endif
}

HRESULT CMonster::Set_Component(void* pArg)
{
	CTransform::TRANSFORMINFO TransformInfo;
	ZeroMemory(&TransformInfo, sizeof(CTransform::TRANSFORMINFO));

	TransformInfo.fSpeed = 5.f;
	TransformInfo.fRotationSpeed = D3DXToRadian(90.f);
	TransformInfo.vStartPos = _vec3(0.f, 0.f, 0.f);

	if (FAILED(Add_Components(L"Com_Transform", SCENE_STATIC, L"Proto_Transform", (CComponent**)&m_pTransformCom, &TransformInfo)))
		return E_FAIL;

	// Render
	if (FAILED(Add_Components(L"Com_Renderer", SCENE_STATIC, L"Proto_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CColider_Cube::COLLRECTDESC CollCubeDesc;
	ZeroMemory(&CollCubeDesc, sizeof(CColider_Cube::COLLRECTDESC));
	CollCubeDesc.fRadiusY = 1.f;
	CollCubeDesc.fRadiusX = 1.f;
	CollCubeDesc.fRadiusZ = 1.f;
	CollCubeDesc.fOffSetX = 0.f;
	CollCubeDesc.fOffSetY = 0.25f;
	CollCubeDesc.fOffsetZ = 0.f;

	// Colider
	if (FAILED(Add_Components(L"Com_Collider_Cube", SCENE_STATIC, L"Proto_Colider_Cube", (CComponent**)&m_pColiderCom, &CollCubeDesc)))
		return E_FAIL;

	return S_OK;
}

void CMonster::Set_Collider(void)
{
	Set_CollisionMatrix();


	if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_PLAYER, this, CColiderManager::COLLISION_CUBE, nullptr))
	{
		_vec3 vPosition = m_pTransformCom->Get_Info(INFO_POS);
	}
}

void CMonster::Set_CollisionMatrix()
{
	D3DXMatrixIdentity(&m_CollisionMatrix);
	memcpy(*(_vec3*)&m_CollisionMatrix.m[3][0], m_pTransformCom->Get_Info(INFO_POS), sizeof(_vec3));
	m_pColiderCom->Update_ColliderBox(m_CollisionMatrix);
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
