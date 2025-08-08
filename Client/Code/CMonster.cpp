#include "pch.h"
#include "CMonster.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CObjectManager.h"
#include "CTimerMgr.h"
#include "CPickingManager.h"
#include "CColiderManager.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCharacter(pGraphicDev)
{
}

CMonster::CMonster(const CMonster& rhs)
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


	m_pTransformCom->Set_Info(INFO_POS, _vec3(4.f, 1.f, 0.f));
	m_pTransformCom->Set_Scale(0.5f, 1.f, 1.f);
	return S_OK;
}

_int CMonster::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bDead)
		return DEAD;
	CPickingManager::GetInstance()->Remove_PickingGroup(this); // picking 그룹에서 지워줌
	CGameObject::Update_GameObject(fTimeDelta);
	CColiderManager::GetInstance()->Add_CollisionGroup(CColiderManager::COLLISION_MONSTER, this); // collider 그룹에 넣어줌
	m_pRendererCom->Add_RenderGroup(RENDER_ALPHA, this);
	return NO_EVENT;
}

void CMonster::LateUpdate_GameObject(const _float& fTimeDelta)
{
	//Key_Input(); // 테스트용 지워야 함
	Update_Position(m_pTransformCom->Get_Info(INFO_POS));
	SetUp_BillBoard();
	CPickingManager::GetInstance()->Add_PickingGroup(this); // picking 그룹에 넣어줌
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

	return S_OK;
}



_bool CMonster::Picking(_vec3* PickingPoint)
{
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