#include "pch.h"
#include "CMonster.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CColiderManager.h"
#include "CObjectManager.h"
#include "CTimerMgr.h"
#include "CPickingManager.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCharacter(pGraphicDev)
{
}

CMonster::CMonster(const CGameObject& rhs)
	: CCharacter(rhs)
{
}

CMonster::~CMonster()
{
}

HRESULT CMonster::Ready_GameObject()
{
	if (FAILED(CGameObject::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Set_Component()))
		return E_FAIL;

	//Test
	Change_Texture(TEXT("Com_Texture_Idle"));

	m_pTransformCom->Set_Info(INFO_POS, _vec3(4.f, 1.f, 0.f));
	m_pTransformCom->Set_Scale(0.5f, 1.f, 1.f);
	return S_OK;
}

_int CMonster::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bDead)
		return DEAD;
	CPickingManager::GetInstance()->Remove_PickingGroup(this);
	CGameObject::Update_GameObject(fTimeDelta);
	CColiderManager::GetInstance()->Add_CollisionGroup(CColiderManager::COLLISION_MONSTER, this);
	m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this);
	return NO_EVENT;
}

void CMonster::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Key_Input(); // 테스트용 지워야 함
	Update_Position(m_pTransformCom->Get_Info(INFO_POS));
	SetUp_BillBoard();
	Set_Collider();
	CPickingManager::GetInstance()->Add_PickingGroup(this);
	CGameObject::LateUpdate_GameObject(fTimeDelta);

}

void CMonster::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTransformCom->Apply_WorldMatrix();

	m_pTextureCom->Set_Texture(m_pTextureCom->Get_Frame().m_iCurrentTex);
	m_pTextureCom->MoveFrame();

	// 알파 테스트 설정 추가
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pBufferCom->Render_Buffer();

	// 원상복귀
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

#ifdef _DEBUG
	if (g_ColiderRender && m_pColiderCom != nullptr)
		m_pColiderCom->Render_ColliderBox();

	if (g_ColiderRender && m_pColiderSpherCom != nullptr)
		m_pColiderSpherCom->Render_ColliderSphere();
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

	// VIBuffer
	if (FAILED(Add_Components(L"Com_Buffer", SCENE_STATIC, L"Proto_Rect_Buffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	// Texture
	if (Texture_Clone())
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
	m_pColiderCom->Set_Transform(m_pTransformCom);

	CColider_Sphere::COLLINFO CollSphereInfo;
	ZeroMemory(&CollSphereInfo, sizeof(CColider_Sphere::COLLINFO));
	CollSphereInfo.fRadius = 1.f;                    // 반지름 1
	CollSphereInfo.vOffset = _vec3(0.f, 0.f, 0.f);    // 중심 오프셋 없음
	// Colider_Sphere
	if (FAILED(Add_Components(L"Com_Collider_Sphere", SCENE_STATIC, L"Proto_Colider_Sphere", (CComponent**)&m_pColiderSpherCom, &CollSphereInfo)))
		return E_FAIL;
	m_pColiderSpherCom->Set_Transform(m_pTransformCom);
	return S_OK;
}

void CMonster::Set_Collider(void)
{
	m_pColiderCom->Update_ColliderBox();
	m_pColiderSpherCom->Update_ColliderSphere();

	if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_PLAYER, this, CColiderManager::COLLISION_CUBE, nullptr))
	{
		_vec3 vPosition = m_pTransformCom->Get_Info(INFO_POS);
	}
	if (CColiderManager::GetInstance()->CollisionGroup(CColiderManager::COLLISION_PLAYER, this, CColiderManager::COLLISION_SPHERE, nullptr))
	{
		_vec3 vPosition = m_pTransformCom->Get_Info(INFO_POS);
	}
}

void CMonster::Key_Input()
{
	if (GetAsyncKeyState('R'))
	{
		Change_Texture(TEXT("Com_Texture_AIM"));
	}
}

_bool CMonster::Picking(_vec3* PickingPoint)
{
	if (true == m_pBufferCom->Picking(m_pTransformCom, PickingPoint))
	{
		//m_vecOutPos = *PickingPoint;
		Change_Texture(TEXT("Com_Texture_AIM"));
		return true;
	}
	else
		return false;
	return true;
}

void CMonster::PickingTrue()
{
	m_bPickingTrue = true;
}

void CMonster::SetUp_BillBoard()
{
	_matrix _matView;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &_matView);
	D3DXMatrixInverse(&_matView, nullptr, &_matView);

	_vec3 vRight = *(_vec3*)&_matView.m[0][0];
	_vec3 vUp = *(_vec3*)&_matView.m[1][0];
	_vec3 vLook = *(_vec3*)&_matView.m[2][0];
	m_pTransformCom->Set_Info(INFO_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_Info(INFO_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_Info(INFO_LOOK, *D3DXVec3Normalize(&vLook, &vLook) * m_pTransformCom->Get_Scale().z);
}


HRESULT CMonster::Texture_Clone()
{
	CTexture::TEXINFO		TextureInfo;
	ZeroMemory(&TextureInfo, sizeof(CTexture::TEXINFO));

	// IDLE
	TextureInfo.m_iStart = 0;
	TextureInfo.m_iEndTex = 12;
	TextureInfo.m_fSpeed = 6;
	TextureInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_Idle", SCENE_STAGE, L"Prototype_Component_Texture_MonsterIdle", (CComponent**)&m_pTextureCom, &TextureInfo)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Idle"), m_pTextureCom));

	// AIM
	TextureInfo.m_iStart = 0;
	TextureInfo.m_iEndTex = 9;
	TextureInfo.m_fSpeed = 6;
	TextureInfo.m_bLoop = true;
	if (FAILED(Add_Components(L"Com_Texture_AIM", SCENE_STAGE, L"Prototype_Component_Texture_MonsterAim", (CComponent**)&m_pTextureCom, &TextureInfo)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_AIM"), m_pTextureCom));

	return S_OK;
}

HRESULT CMonster::Change_Texture(const _tchar* LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_Zero_Frame();

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