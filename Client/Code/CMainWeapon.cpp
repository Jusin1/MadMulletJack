#include "pch.h"
#include "CMainWeapon.h"

CMainWeapon::CMainWeapon(LPDIRECT3DDEVICE9 pGraphicDev)
	:CWeapon(pGraphicDev), m_bIsEmpty(false), m_iBullet(0), m_iMaxBullet(0),
	m_iPower(0), m_iPrecision(0), m_fCoolTime(0), m_fRange(0.f)
{
}

CMainWeapon::CMainWeapon(const CMainWeapon& rhs)
	:CWeapon(rhs), m_bIsEmpty(rhs.m_bIsEmpty), m_iBullet(rhs.m_iBullet), m_iMaxBullet(rhs.m_iMaxBullet),
	m_iPower(rhs.m_iPower), m_iPrecision(rhs.m_iPrecision), m_fCoolTime(rhs.m_fCoolTime), m_fRange(rhs.m_fRange)
{
}

CMainWeapon::~CMainWeapon()
{
}

HRESULT CMainWeapon::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainWeapon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// 만약 컴포넌트가 필요하다면
	/*if (FAILED(Set_Component()))
		return E_FAIL*/

	return S_OK;
}

_int CMainWeapon::Update_GameObject(const _float& fTimeDelta)
{
	__super::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CMainWeapon::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);
	
	if (Is_PlayerState_Change()) //player가 state를 바꿨을 때
	{
		this->Set_Texture();	// texture를 바꿔라
	}
}

void CMainWeapon::Render_GameObject()
{
	__super::Render_GameObject();
}


void CMainWeapon::Free()
{
	__super::Free();
}
