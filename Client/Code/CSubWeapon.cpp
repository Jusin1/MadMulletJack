#include "pch.h"
#include "CSubWeapon.h"

CSubWeapon::CSubWeapon(LPDIRECT3DDEVICE9 pGraphicDev)
	:CWeapon(pGraphicDev)
{
}

CSubWeapon::CSubWeapon(const CSubWeapon& rhs)
	:CWeapon(rhs)
{
}

CSubWeapon::~CSubWeapon()
{
}

HRESULT CSubWeapon::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSubWeapon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// 만약 컴포넌트가 필요하다면
	/*if (FAILED(Set_Component()))
		return E_FAIL*/

	return S_OK;
}

_int CSubWeapon::Update_GameObject(const _float& fTimeDelta)
{
	__super::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CSubWeapon::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);

	// 내 wapstate가 weapon이고 player가 state를 change 했을 때
	if (m_eWapState == WEAPON && Is_PlayerState_Change())
	{
		// player의 state가 attack instant일때만 바꿔줘라
		if (m_tInfo.ePlayerState == ATTACK_INSTANT)
			Set_Texture();	// texture를 바꿔라

		else
			m_bRenderOn = false;
	}
}

void CSubWeapon::Render_GameObject()
{
	__super::Render_GameObject();
}


void CSubWeapon::Free()
{
	__super::Free();
}