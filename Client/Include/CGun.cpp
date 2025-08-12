#include "pch.h"
#include "CGun.h"

CGun::CGun(LPDIRECT3DDEVICE9 pGraphicDev)
	:CMainWeapon(pGraphicDev)
{
}

CGun::CGun(const CGun& rhs)
	:CMainWeapon(rhs)
{
}

CGun::~CGun()
{
}

HRESULT CGun::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGun::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// 만약 컴포넌트가 필요하다면
	/*if (FAILED(Set_Component()))
		return E_FAIL*/

	return S_OK;
}

_int CGun::Update_GameObject(const _float& fTimeDelta)
{
	__super::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CGun::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);

	// 만약 무한 모드이면 -> m_bIsEmpty는 항상 falsse
	if(m_bIsInfinite)
		m_bIsEmpty = false;
}

void CGun::Render_GameObject()
{
	__super::Render_GameObject();
}


void CGun::Free()
{
	__super::Free();
}
