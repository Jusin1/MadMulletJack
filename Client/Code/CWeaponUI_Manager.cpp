#include "pch.h"
#include "CWeaponUI_Manager.h"
#include "CGlobal_Info.h"
#include "CManagement.h"
#include "CObjectManager.h"
#include "CPistol_Gun.h"
#include "CKnife_SubW.h"
#include "CPlayerUI_Manager.h"
#include "CSniper_Gun.h"
#include "CMapFactory.h"
#include "CKatana.h"

CWeaponUI_Manager::CWeaponUI_Manager(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev), m_eWeapon(WP_END), m_eWeapon2(WP2_END)
{
}

CWeaponUI_Manager::CWeaponUI_Manager(const CWeaponUI_Manager& rhs)
	: CUI(rhs), m_eWeapon(rhs.m_eWeapon), m_eWeapon2(rhs.m_eWeapon2)
{
}

CWeaponUI_Manager::~CWeaponUI_Manager()
{
}

HRESULT	CWeaponUI_Manager::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeaponUI_Manager::Initialize(void* pArg)
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

_int CWeaponUI_Manager::Update_GameObject(const _float& fTimeDelta)
{
	__super::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CWeaponUI_Manager::LateUpdate_GameObject(const _float& fTimeDelta)
{
	

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

	__super::LateUpdate_GameObject(fTimeDelta);
}

void CWeaponUI_Manager::Render_GameObject()
{
	CUIBase::Render_GameObject();
}

void CWeaponUI_Manager::Weapon_Change()
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
		TagUI_SetActive(L"KatanaUI", false);
		break;

	case WP_SNIPER:
		TagUI_SetActive(L"SniperUI", false);
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
		TagUI_SetActive(L"KatanaUI", true);
		break;

	case WP_SNIPER:
		TagUI_SetActive(L"SniperUI", true);
		break;
	}
}

void CWeaponUI_Manager::Weapon2_On()
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

void CWeaponUI_Manager::Weapon2_Off()
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

void CWeaponUI_Manager::TagUI_SetActive(const _tchar* pTag , _bool _bActive)
{
	CUIBase* pWeapon = Find_Child_ByTag(pTag);
	if (pWeapon)
	{
		pWeapon ->Set_Active(_bActive);
	}
}

HRESULT CWeaponUI_Manager::Create_Pistol(_uint _iSceneIdx)
{
	// pistol 생성 및 list에 넣기
	CPistol_Gun* pPistolUI = dynamic_cast<CPistol_Gun*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_GunPistolUI", _iSceneIdx, L"UI_Layer"));
	if (pPistolUI)
	{
		pPistolUI->Set_ObjTag(L"PistolUI");
		pPistolUI->Set_WapState(CWeapon::WAPSTATE::WEAPON); //state를 weapon으로 등록
		Add_Child(pPistolUI); // 루트 UI에 등록
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CWeaponUI_Manager::Create_Sniper(_uint _iSceneIdx)
{
	// pistol 생성 및 list에 넣기
	CSniper_Gun* pSniperUI = dynamic_cast<CSniper_Gun*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_GunSniperUI", _iSceneIdx, L"UI_Layer"));
	if (pSniperUI)
	{
		pSniperUI->Set_ObjTag(L"SniperUI");
		pSniperUI->Set_WapState(CWeapon::WAPSTATE::WEAPON); //state를 weapon으로 등록
		Add_Child(pSniperUI); // 루트 UI에 등록

		return S_OK;
	}

	return E_FAIL;
}

HRESULT CWeaponUI_Manager::Create_Katana(_uint _iSceneIdx)
{
	CKatana* pKatanaUI = dynamic_cast<CKatana*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_KatanaUI", _iSceneIdx, L"UI_Layer"));
	if (pKatanaUI)
	{
		pKatanaUI->Set_ObjTag(L"KatanaUI");
		pKatanaUI->Set_WapState(CWeapon::WAPSTATE::WEAPON); //state를 weapon으로 등록
		Add_Child(pKatanaUI); // 루트 UI에 등록
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CWeaponUI_Manager::Create_Knife(_uint _iSceneIdx)
{
	// knife 생성 및 list에 넣기
	CKnife_SubW* pKnifeUI = dynamic_cast<CKnife_SubW*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_SubWKnifeUI", _iSceneIdx, L"UI_Layer"));
	if (pKnifeUI)
	{
		pKnifeUI->Set_ObjTag(L"KnifeUI");
		pKnifeUI->Set_WapState(CWeapon::WAPSTATE::WEAPON); //state를 weapon으로 등록
		Add_Child(pKnifeUI); // 루트 UI에 등록
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CWeaponUI_Manager::Set_WeaponUI()
{
	_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex(); // stage
	_uint iCloneScene = CManagement::GetInstance()->Get_CurrentSceneIdx(); // loading

	//switch (iTargetScene)
	//{
	//case SCENE_DEV:
	//case SCENE_TUTORIAL:
	//case SCENE_STAGE_1:
	//case SCENE_STAGE_2:
	//	if (FAILED(Create_Pistol(iCloneScene)))
	//		return E_FAIL;

	//	break;

	//case SCENE_SNIPE:
	//	if (FAILED(Create_Sniper(iCloneScene)))
	//		return E_FAIL;
	//	break;

	//case SCENE_BOSS:
	//case SCENE_CAR:
	//	break;
	//}

	// test
	Create_Pistol(iCloneScene);
	Create_Sniper(iCloneScene);
	Create_Katana(iCloneScene);

	return S_OK;
}

HRESULT CWeaponUI_Manager::Set_Weapon2UI()
{
	_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex(); // stage
	_uint iCloneScene = CManagement::GetInstance()->Get_CurrentSceneIdx(); // loading

	switch (iTargetScene)
	{
	case SCENE_DEV:
	case SCENE_TUTORIAL:
	case SCENE_STAGE_1:
	case SCENE_STAGE_2:

		if (FAILED(Create_Knife(iCloneScene)))
			return E_FAIL;

		break;

	case SCENE_SNIPE:
	case SCENE_BOSS:
	case SCENE_CAR:
		break;
	}

	//CKnife_SubW* pKnifeUI = dynamic_cast<CKnife_SubW*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_SubWKnifeUI", iSceneIndex, L"UI_Layer"));
	//if (pKnifeUI)
	//{
	//	pKnifeUI->Set_ObjTag(L"KnifeUI");
	//	pKnifeUI->Set_WapState(CWeapon::WAPSTATE::WEAPON); //state를 weapon으로 등록
	//	Add_Child(pKnifeUI); // 루트 UI에 등록
	//}
	

	return S_OK;
}

CWeaponUI_Manager* CWeaponUI_Manager::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CWeaponUI_Manager* pHpBar = new CWeaponUI_Manager(pGraphicDev);

	if (FAILED(pHpBar->Ready_GameObject()))
	{
		Safe_Release(pHpBar);
		MSG_BOX("CWeaponUI_Manager Create Failed");
		return nullptr;
	}

	return pHpBar;
}

CGameObject* CWeaponUI_Manager::Clone(void* pArg)
{
	CWeaponUI_Manager* pInstance = new CWeaponUI_Manager(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CWeaponUI_Manager Clone Failed");
		return nullptr;
	}

	return pInstance;
}

void CWeaponUI_Manager::Free()
{
	__super::Free();
}
