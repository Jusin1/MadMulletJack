#include "pch.h"
#include "CCharacter.h"
#include "CGridPanel.h"
#include "CGrounding.h"
#include "CGameDataManager.h"
#include "CVIBuffer_GridPanel_Normal.h"
#include "CManagement.h"
#include "CObjectManager.h"

CCharacter::CCharacter(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev), m_eObjID(OBJ_END), m_bPickingTrue(false),
	m_fHp(0.f), m_fAttack(0.f), m_fVelocity(0.f), 
	m_bJumping(false), m_fJumpTime(0.f), m_fMaxHp(10.f)
{
}



CCharacter::CCharacter(const CCharacter& rhs)
	: CGameObject(rhs), m_eObjID(rhs.m_eObjID), m_bPickingTrue(rhs.m_bPickingTrue),
	m_fHp(rhs.m_fHp), m_fAttack(rhs.m_fAttack), m_fVelocity(rhs.m_fVelocity),
	m_bJumping(rhs.m_bJumping), m_fJumpTime(rhs.m_fJumpTime), m_fMaxHp(rhs.m_fMaxHp)
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
	if (FAILED(Add_Components(L"Com_Calculator", SCENE_STATIC, L"Proto_Calculator", (CComponent**)&m_pCalculatorCom)))
		return E_FAIL;

	if (FAILED(Add_Components(L"Com_Buffer", SCENE_STATIC, L"Proto_Rect_Buffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	if (FAILED(Add_Components(L"Com_Grounding", SCENE_STATIC, L"Proto_Grounding", (CComponent **)&m_pGroundingCom)))
		return E_FAIL;

	return S_OK;
}

// TODO - 여러개의 terrain 해결
// 추후에 여러개의 GridPanel이 들어왔을 때 연산량이 많아지는 문제가 발생한다.
// 이를 방지하기 위해 MapLayer를 파싱할때 z값을 기준으로 Goal을 만들고, z정렬을 실시하여 저장한다.
// z값에 기반하여 순차적으로 데이터를 불러와 인스턴싱을 하고, 순회를 할 때 선형적인 타임어택 게임에 아이디어를 가져온다.
// 지나온 terrain은 다시는 밟을 일이 없으니 currentindex를 가지고 다음 terrain을 밟을 시 currentindex를 갱신시켜 연산량을 줄여나간다.
// 이렇게 하면 땅을 밟고 있는 상황에선 항상 하나의 Terrain만 검사하게 될것이다.
void CCharacter::Set_OnTerrain(const _float &fTimeDelta)
{
	_vec3 vPos = m_pTransformCom->Get_Info(INFO_POS);

	_float fHeight;
	if (m_pGroundingCom->GetHeight(CGameDataManager::GetInstance()->Get_SortedFloorEntries(), vPos.x, vPos.z, &fHeight))
	{
		if (m_bJumping)
		{
			vPos.y = vPos.y + m_fVelocity * fTimeDelta;

			if (vPos.y - m_pTransformCom->Get_Scale().y * 0.5f <= fHeight)
			{
				m_bJumping = false;
				vPos.y = fHeight + m_pTransformCom->Get_Scale().y * 0.5f;
			}
		}
		else
		{
			vPos.y = fHeight + m_pTransformCom->Get_Scale().y * 0.5f;
			_float f = m_pTransformCom->Get_Scale().y;
		}
	}
	else
		vPos.y = vPos.y + m_fVelocity * fTimeDelta;

	

	m_pTransformCom->Set_Info(INFO_POS, { vPos.x , vPos.y , vPos.z });
}

_bool CCharacter::Is_OnTerrain()
{
	_vec3 vPos = m_pTransformCom->Get_Info(INFO_POS);

	_float fHeight;
	if (m_pGroundingCom->GetHeight(CGameDataManager::GetInstance()->Get_SortedFloorEntries(), vPos.x, vPos.z, &fHeight))
	{
		// 만약 내가 pos가 terrain위가 아니라면
		if (vPos.y > fHeight + m_pTransformCom->Get_Scale().y * 0.5f)
			return false; // false 반환

		else
			return true; // 아니라면  true 반환
	}
}

void CCharacter::Gravity(const _float &fDeltaTime)
{
	_float fVel = Get_Velocity();
	fVel -= 9.8f * fDeltaTime * 0.5f * 3.f/*Mess*/;
	Set_Velocity(fVel);
}

WallType CCharacter::GetGroundedFloorType()
{
	return (*CGameDataManager::GetInstance()->Get_SortedFloorEntries())[m_pGroundingCom->GetCurrentIndex()].eType;
}

void CCharacter::Free()
{
	__super::Free();
}
