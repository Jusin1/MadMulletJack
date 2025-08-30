#include "pch.h"
#include "CGun.h"
#include "CObjectManager.h"
#include "CManagement.h"
#include "CPlayer.h"
#include "CUIManager.h"
#include "CImageUI.h"

CGun::CGun(LPDIRECT3DDEVICE9 pGraphicDev)
	:CMainWeapon(pGraphicDev), m_bIsInfinite(false), m_bIsZoom(false)
{
}

CGun::CGun(const CGun& rhs)
	:CMainWeapon(rhs), m_bIsInfinite(rhs.m_bIsInfinite), m_bIsZoom(rhs.m_bIsZoom)
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

	// 만약 탄창이 비웠다면 attack = false
	if (m_bIsEmpty)
	{
		m_bIsAttack = false;

		// 플레이어의 m_bIsAttack = false; 해주기

		// 현재 씬을 가져옴
		_uint eCurScene = CManagement::GetInstance()->Get_CurrentSceneIdx();

		// 그 씬에 있는 플레이어를 가져옴
		CGameObject* pPlayer = CObjectManager::GetInstance()->Find_Object(eCurScene, L"Player_Layer", 0);

		// 플레이어 함수 사용
		dynamic_cast<CPlayer*>(pPlayer)->Set_IsAttack(false);

		// reload ui 생성
		CUIManager::GetInstance()->CreateReloadUI();
	}

	else
	{
		// reloa ui 삭제
		CUIManager::GetInstance()->DestroyReloadUI();
	}

	if (m_tInfo.ePlayerState == CLEAR)
	{
		// reloa ui 삭제
		CUIManager::GetInstance()->DestroyReloadUI();
	}

	return NO_EVENT;
}

void CGun::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);

	// 만약 무한 모드이면 -> m_bIsEmpty는 항상 falsse
	if(m_bIsInfinite)
	{
		m_bIsEmpty = false;
		return;
	}
		
	// 만약 bullet이 0개이면 isempty = true
	if (m_iBullet == 0)
	{
		m_bIsEmpty = true;
	}
	else
	{
		m_bIsEmpty = false;
	}
}

void CGun::Render_GameObject()
{
	__super::Render_GameObject();
}


void CGun::Free()
{
	__super::Free();
}
