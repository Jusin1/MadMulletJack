#include "pch.h"
#include "CCharacter.h"

CCharacter::CCharacter(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev), m_bPickingTrue(false)
{
}



CCharacter::CCharacter(const CGameObject& rhs)
	: CGameObject(rhs)
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

void CCharacter::Free()
{
	__super::Free();
}
