#include "pch.h"
#include "CMainWeapon.h"
#include "CManagement.h"
#include "CObjectManager.h"
#include "CPlayer.h"

CMainWeapon::CMainWeapon(LPDIRECT3DDEVICE9 pGraphicDev)
	:CWeapon(pGraphicDev), m_bIsEmpty(false), m_iBullet(0), m_iMaxBullet(0),
	m_iPower(0), m_iPrecision(0), m_iSpeed(0), m_fRange(0.f)
{
}

CMainWeapon::CMainWeapon(const CMainWeapon& rhs)
	:CWeapon(rhs), m_bIsEmpty(rhs.m_bIsEmpty), m_iBullet(rhs.m_iBullet), m_iMaxBullet(rhs.m_iMaxBullet),
	m_iPower(rhs.m_iPower), m_iPrecision(rhs.m_iPrecision), m_iSpeed(rhs.m_iSpeed), m_fRange(rhs.m_fRange)
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

	// 만약 탄창이 비웠다면 attack = false
	if (m_bIsEmpty)
	{
		m_bIsAttack = false;

		// 플레이어의 m_bIsAttack = false; 해주기

		// 현재 씬을 가져옴
		_uint eCurScene = CManagement::GetInstance()->Get_CurrentSceneIdx();

		// 그 씬에 있는 플레이어를 가져옴
		CGameObject* pPlayer = CObjectManager::GetInstance()->Find_Object(eCurScene,L"Player_Layer", 0);

		// 플레이어 함수 사용
		dynamic_cast<CPlayer*>(pPlayer)->Set_IsAttack(false);
	}
		

	return NO_EVENT;
}

void CMainWeapon::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);

	// 만약 bullet이 0개이면 isempty = true
	if (m_iBullet == 0)
	{
		m_bIsEmpty = true;
		// reload effect 추가
	}
	else
	{
		m_bIsEmpty = false;
	}
		
	if (Is_PlayerState_Change()) //player가 state를 바꿨을 때
	{
		Set_Texture();	// texture를 바꿔라
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
