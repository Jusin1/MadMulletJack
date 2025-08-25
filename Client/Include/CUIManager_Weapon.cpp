#include "pch.h"
#include "CUIManager_Weapon.h"
#include "CGlobal_Info.h"
#include "CManagement.h"
#include "CObjectManager.h"
#include "CPistol_Gun.h"
#include "CKnife_SubW.h"

CUIManager_Weapon::CUIManager_Weapon(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev), m_eWeapon(WP_END), m_eWeapon2(WP2_END)
{
}

CUIManager_Weapon::CUIManager_Weapon(const CUIManager_Weapon& rhs)
	: CUI(rhs), m_eWeapon(rhs.m_eWeapon), m_eWeapon2(rhs.m_eWeapon2)
{
}

CUIManager_Weapon::~CUIManager_Weapon()
{
}

HRESULT	CUIManager_Weapon::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIManager_Weapon::Initialize(void* pArg)
{
	// 상위 객체들 initial(set compnent 까지 해줌)
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if(FAILED(Set_WeaponUI()))
		return E_FAIL;
	if (FAILED(Set_Weapon2UI()))
		return E_FAIL;

	Weapon_Change();
	Weapon2_Off();

	return S_OK;
}

_int CUIManager_Weapon::Update_GameObject(const _float& fTimeDelta)
{
	__super::Update_GameObject(fTimeDelta);

	return NO_EVENT();
}

void CUIManager_Weapon::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);

	PlayerStateInfo tPlayerInfo = CGlobal_Info::Get_Instance()->Get_PlayerInfo();

	// 만약 player의 weapon이 바뀌면 
	if (m_eWeapon != tPlayerInfo.eWeapon)
	{
		// 무기를 바꿔 줌
		Weapon_Change();
	}

	// player가 attack instant이면
	if (ATTACK_INSTANT == tPlayerInfo.ePlayerState)
	{
		// 무기2 on
		Weapon2_On();
	}
	// attack instant 가 아니라면
	else
	{
		// 무기2 off
		Weapon2_Off();
	}
}

void CUIManager_Weapon::Render_GameObject()
{
	CUIBase::Render_GameObject();
}

void CUIManager_Weapon::Weapon_Change()
{
	// 이전 waepon은 active false
	switch (m_eWeapon)
	{
	case WP_NON:
		break;

	case WP_PISTOL:
		TagUI_SetActive(L"PistolUI", false);
		break;

	case WP_SHOTGUN:
		break;

	case WP_RIFLE:
		break;

	case WP_KATANA:
		break;

	case WP_SNIPER:
		break;
	}

	// weapon update
	m_eWeapon = CGlobal_Info::Get_Instance()->Get_PlayerInfo().eWeapon;

	// 이번 weapon은 active true
	switch (m_eWeapon)
	{
	case WP_NON:
		break;

	case WP_PISTOL:
		TagUI_SetActive(L"PistolUI", true);
		break;

	case WP_SHOTGUN:
		break;

	case WP_RIFLE:
		break;

	case WP_KATANA:
		break;

	case WP_SNIPER:
		break;
	}
}

void CUIManager_Weapon::Weapon2_On()
{
	// weapon update
	m_eWeapon2 = CGlobal_Info::Get_Instance()->Get_PlayerInfo().eWeapon2;

	switch (m_eWeapon2)
	{
	case WP_KICK:
		break;

	case WP_KNIFE:
		TagUI_SetActive(L"KnifeUI", true);
		break;

	case WP_BOOK:
		break;
	}

	
}

void CUIManager_Weapon::Weapon2_Off()
{
	// weapon update
	m_eWeapon2 = CGlobal_Info::Get_Instance()->Get_PlayerInfo().eWeapon2;

	// 이번 weapon은 active true
	switch (m_eWeapon2)
	{
	case WP_KICK:
		break;

	case WP_KNIFE:
		TagUI_SetActive(L"KnifeUI", false);
		break;

	case WP_BOOK:
		break;
	}
}

void CUIManager_Weapon::TagUI_SetActive(const _tchar* pTag , _bool _bActive)
{
	Find_Child_ByTag(pTag)->Set_Active(_bActive);
}

HRESULT CUIManager_Weapon::Set_WeaponUI()
{
	_uint iSceneIndex = CManagement::GetInstance()->Get_CurrentSceneIdx();

	// pistol 생성 및 list에 넣기
	CPistol_Gun* pPistolUI = dynamic_cast<CPistol_Gun*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_GunPistolUI", iSceneIndex, L"UI_Layer"));
	if (pPistolUI)
	{
		pPistolUI->Set_ObjTag(L"PistolUI");
		pPistolUI->Set_WapState(CWeapon::WAPSTATE::WEAPON); //state를 weapon으로 등록
		Add_Child(pPistolUI); // 루트 UI에 등록
	}

	return S_OK;
}

HRESULT CUIManager_Weapon::Set_Weapon2UI()
{
	_uint iSceneIndex = CManagement::GetInstance()->Get_CurrentSceneIdx();

	// knife 생성 및 list에 넣기
	CKnife_SubW* pKnifeUI = dynamic_cast<CKnife_SubW*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_SubWKnifeUI", iSceneIndex, L"UI_Layer"));
	if (pKnifeUI)
	{
		pKnifeUI->Set_ObjTag(L"KnifeUI");
		pKnifeUI->Set_WapState(CWeapon::WAPSTATE::WEAPON); //state를 weapon으로 등록
		Add_Child(pKnifeUI); // 루트 UI에 등록
	}

	return S_OK;
}

CUIManager_Weapon* CUIManager_Weapon::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CUIManager_Weapon* pHpBar = new CUIManager_Weapon(pGraphicDev);

	if (FAILED(pHpBar->Ready_GameObject()))
	{
		Safe_Release(pHpBar);
		MSG_BOX("CUIManager_Weapon Create Failed");
		return nullptr;
	}

	return pHpBar;
}

CGameObject* CUIManager_Weapon::Clone(void* pArg)
{
	CUIManager_Weapon* pInstance = new CUIManager_Weapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CUIManager_Weapon Clone Failed");
		return nullptr;
	}

	return pInstance;
}

void CUIManager_Weapon::Free()
{
	__super::Free();
}
