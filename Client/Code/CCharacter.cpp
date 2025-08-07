#include "pch.h"
#include "CCharacter.h"
#include "CObjectManager.h"

CCharacter::CCharacter(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev), m_eObjID(OBJ_END), m_bPickingTrue(false),
	m_fHp(0.f), m_fAttack(0.f), m_fVelocity(0.f), 
	m_bJumping(false), m_fJumpTime(0.f)
{
}



CCharacter::CCharacter(const CCharacter& rhs)
	: CGameObject(rhs), m_eObjID(rhs.m_eObjID), m_bPickingTrue(rhs.m_bPickingTrue),
	m_fHp(rhs.m_fHp), m_fAttack(rhs.m_fAttack), m_fVelocity(rhs.m_fVelocity),
	m_bJumping(rhs.m_bJumping), m_fJumpTime(rhs.m_fJumpTime)
{
}

CCharacter::~CCharacter()
{

}


HRESULT CCharacter::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCharacter::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Set_Component()))
		return E_FAIL;

	return S_OK;
}

_int CCharacter::Update_GameObject(const _float& fTimeDelta)
{
	__super::Update_GameObject(fTimeDelta);
	Set_OnTerrain();

	return NO_EVENT;
}

void CCharacter::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);
}

void CCharacter::Render_GameObject()
{
	__super::Render_GameObject();
}

HRESULT	CCharacter::Set_Component()
{
	// CCalculator
	if (FAILED(Add_Components(L"Com_Calculator", SCENE_STATIC, L"Proto_Calculator", (CComponent**)&m_pCalculatorCom)))
		return E_FAIL;

	return S_OK;
}

void CCharacter::Set_OnTerrain()
{
	_vec3 vPos = m_pTransformCom->Get_Info(INFO_POS);

	Engine::CVIBuffer_Terrian* pTerrainBufferCom =dynamic_cast<Engine::CVIBuffer_Terrian*>
		(CObjectManager::GetInstance()->Get_Component(SCENE_STAGE, L"Environment_Layer", L"Com_VIBuffer", 0));
	_float fHeight = m_pCalculatorCom->
		Compute_HeightOnTerrain(&vPos, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);
	
	if (m_bJumping)
	{
		vPos.y = vPos.y -  m_fVelocity * m_fJumpTime - (9.8f * m_fJumpTime * m_fJumpTime) * 0.5f;
		m_fJumpTime += 0.2f;

		if (vPos.y <= fHeight)
		{
			m_bJumping = false;
			m_fJumpTime = 0.f;
		}
	}

	m_pTransformCom->Set_Info(INFO_POS, { vPos.x , fHeight , vPos.z });
}

void CCharacter::Free()
{
	__super::Free();
}
