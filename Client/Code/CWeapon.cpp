#include "pch.h"
#include "CWeapon.h"

CWeapon::CWeapon(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev), m_bIsAttack(false) , m_eWapState(WAPSTATE_END)
{
}

CWeapon::CWeapon(const CWeapon& rhs)
	: CUI(rhs), m_bIsAttack(rhs.m_bIsAttack), m_eWapState(rhs.m_eWapState)
{
}

CWeapon::~CWeapon()
{
}

HRESULT CWeapon::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// 만약 컴포넌트가 필요하다면
	/*if (FAILED(Set_Component()))
		return E_FAIL*/

	m_bActive = false; // 손에서 active 여부 부여

	return S_OK;
}

_int CWeapon::Update_GameObject(const _float& fTimeDelta)
{

	return NO_EVENT;
}

void CWeapon::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CWeapon::Render_GameObject()
{
	__super::Render_GameObject();
}


void CWeapon::Free()
{
	__super::Free();
}