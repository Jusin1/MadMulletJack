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
#include "CImageUI.h"
#include "CShot_Gun.h"
#include "CUIManager.h"
#include "CMini_Gun.h"

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
	Weapon2_Change();

	return S_OK;
}

_int CWeaponUI_Manager::Update_GameObject(const _float& fTimeDelta)
{
	PlayerStateInfo tPlayerInfo = CGlobal_Info::Get_Instance()->Get_PlayerInfo();

	// 만약 player의 weapon이 바뀌면 
	if (m_eWeapon != tPlayerInfo.eWeapon)
	{
		// 무기를 바꿔 줌
		Weapon_Change();
	}

	// player가 attack instant이면
	if (m_eWeapon2 != tPlayerInfo.eWeapon2)
	{
		Weapon2_Change();
	}

	if (tPlayerInfo.ePlayerState == ATTACK_INSTANT)
	{
		SpawnInsEff();
	}
	else
	{
		DeleteEff(TEXT("Ins_Eff"));
	}

	__super::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CWeaponUI_Manager::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);
}

void CWeaponUI_Manager::Render_GameObject()
{
	CUIBase::Render_GameObject();
}

void CWeaponUI_Manager::Weapon_Change()
{
	Delete_AimUI(L"Com_Aim");

	// 이전 waepon은 active false
	switch (m_eWeapon)
	{
	case WP_NON:
		break;

	case WP_PISTOL:
		TagUI_SetActive(L"PistolUI", false);
		break;

	case WP_SHOTGUN:
		TagUI_SetActive(L"ShotGunUI", false);
		break;

	case WP_MINIGUN:
		TagUI_SetActive(L"MiniGunUI", false);
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
		Create_AimUI(L"Prototype_Component_Texture_WapPistol_AimEff",50.f,50.f);
		break;

	case WP_SHOTGUN:
		TagUI_SetActive(L"ShotGunUI", true);
		Create_AimUI(L"Prototype_Component_Texture_WapShot_AimEff",30.f,30.f);
		break;

	case WP_MINIGUN:
		TagUI_SetActive(L"MiniGunUI", true);
		Create_AimUI(L"Prototype_Component_Texture_WapPistol_AimEff", 50.f, 50.f);
		break;

	case WP_KATANA:
		TagUI_SetActive(L"KatanaUI", true);
		CUIManager::GetInstance()->DestroyReloadUI();
		break;

	case WP_SNIPER:
		TagUI_SetActive(L"SniperUI", true);
		Create_AimUI(L"Prototype_Component_Texture_WapSniper_AimEff",10.f,10.f);
		break;
	}
}

void CWeaponUI_Manager::Weapon2_Change()
{
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

	// effect 생성
	//SpawnInsEff();
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

HRESULT CWeaponUI_Manager::Create_ShotGun(_uint _iSceneIdx)
{
	CShot_Gun* pShotGunUI = dynamic_cast<CShot_Gun*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_GunShotGUI", _iSceneIdx, L"UI_Layer"));
	if (pShotGunUI)
	{
		pShotGunUI->Set_ObjTag(L"ShotGunUI");
		pShotGunUI->Set_WapState(CWeapon::WAPSTATE::WEAPON); //state를 weapon으로 등록
		Add_Child(pShotGunUI); // 루트 UI에 등록
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CWeaponUI_Manager::Create_MiniGun(_uint _iSceneIdx)
{
	CMini_Gun* pMiniGunUI = dynamic_cast<CMini_Gun*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_GunMiniGUI", _iSceneIdx, L"UI_Layer"));
	if (pMiniGunUI)
	{
		pMiniGunUI->Set_ObjTag(L"MiniGunUI");
		pMiniGunUI->Set_WapState(CWeapon::WAPSTATE::WEAPON); //state를 weapon으로 등록
		Add_Child(pMiniGunUI); // 루트 UI에 등록
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

HRESULT CWeaponUI_Manager::Create_AimUI(const _tchar* pProtoTag, _float _fsizeX, _float _fSizeY)
{
	auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();

	CImageUI* pAimEff = dynamic_cast<CImageUI*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"));
	if (pAimEff)
	{
		pAimEff->Set_UISizeAndPos(_fsizeX, _fSizeY, 0.f, 0.f);

		pAimEff->RegisterTexture(L"Com_Texture_Aim", pProtoTag, 0, 0, 0.f, true);

		// tag
		pAimEff->Set_ObjTag(L"Com_Aim");
		Add_Child(pAimEff); // 루트 UI에 등록

		pAimEff->ChangeTexture(L"Com_Texture_Aim");
		return S_OK;
	}

	return E_FAIL;
}

void CWeaponUI_Manager::Delete_AimUI(const _tchar* pTag)
{
	CImageUI* pEff = dynamic_cast<CImageUI*> (Find_Child_ByTag((pTag)));
	if (!pEff)
		return;

	pEff->Set_Dead(true);
	Remove_Child(pEff);
}

void CWeaponUI_Manager::SpawnInsEff()
{
	// 중복 생성 방지
	if (Find_Child_ByTag(L"Ins_Eff"))
		return;

	auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();

	CImageUI* pFx = dynamic_cast<CImageUI*>(
		CObjectManager::GetInstance()->Clone_GameObject(
			L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"));
	if (!pFx) return;


	float fxW = 800.f, fxH = 800.f;

	pFx->Set_UISizeAndPos(fxW, fxH, 150.f, -15.f);
	pFx->RegisterTexture(L"Com_Texture_KnifeEff", L"Prototype_Component_Texture_SubWKnife_Eff", 0, 18, 10.f, false);
	pFx->ChangeTexture(L"Com_Texture_KnifeEff");

	pFx->Set_ObjTag(L"Ins_Eff");

	Add_Child(pFx);
}

void CWeaponUI_Manager::DeleteEff(const _tchar* pTag)
{
	CImageUI* pEff = dynamic_cast<CImageUI*> (Find_Child_ByTag(pTag));
	if (pEff && pEff->GetTextureCom()->Is_AnimFinished())
	{
		pEff->Set_Dead(true);
		Remove_Child(pEff);
	}
}

HRESULT CWeaponUI_Manager::Set_WeaponUI()
{
	_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex(); // stage
	_uint iCloneScene = CManagement::GetInstance()->Get_CurrentSceneIdx(); // loading
	//Create_MiniGun
	switch (iTargetScene)
	{
	case SCENE_DEV:
		if (FAILED(Create_Pistol(iCloneScene)))
			return E_FAIL;
		if (FAILED(Create_Katana(iCloneScene)))
			return E_FAIL;
		if (FAILED(Create_ShotGun(iCloneScene)))
			return E_FAIL;
		if (FAILED(Create_Sniper(iCloneScene)))
			return E_FAIL;
		if (FAILED(Create_MiniGun(iCloneScene)))
			return E_FAIL;
		break;

	case SCENE_TUTORIAL:
		if (FAILED(Create_Pistol(iCloneScene)))
			return E_FAIL;
		break;

	case SCENE_STAGE_1:
		if (FAILED(Create_ShotGun(iCloneScene)))
			return E_FAIL;
		break;

	case SCENE_STAGE_2:
		if (FAILED(Create_Katana(iCloneScene)))
			return E_FAIL;
		break;

	case SCENE_BOSS:
		if (FAILED(Create_Pistol(iCloneScene)))
			return E_FAIL;
		if (FAILED(Create_ShotGun(iCloneScene)))
			return E_FAIL;
		if (FAILED(Create_Katana(iCloneScene)))
			return E_FAIL;
		break;

	case SCENE_SNIPE:
		if (FAILED(Create_Sniper(iCloneScene)))
			return E_FAIL;
		break;

	case SCENE_CAR:
		if (FAILED(Create_MiniGun(iCloneScene)))
			return E_FAIL;
		break;
	}

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
