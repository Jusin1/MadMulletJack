#include "pch.h"
#include "CPlayerUI_Manager.h"
#include "CManagement.h"
#include "CObjectManager.h"
#include "CPlayer_Arm.h"
#include "CPlayer_Foot.h"
#include "CPlayer_HandR.h"
#include "CPlayer_HandL.h"

CPlayerUI_Manager::CPlayerUI_Manager(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev), m_tInfo({ PLAYER_END, PMV_END, WP_END, WP2_END })
{
}

CPlayerUI_Manager::CPlayerUI_Manager(const CPlayerUI_Manager& rhs)
	: CUI(rhs), m_tInfo(rhs.m_tInfo)
{
}

CPlayerUI_Manager::~CPlayerUI_Manager()
{
}

HRESULT	CPlayerUI_Manager::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerUI_Manager::Initialize(void* pArg)
{
	// 상위 객체들 initial(set compnent 까지 해줌)
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Set_PlayerUI()))
		return E_FAIL;

	return S_OK;
}

_int CPlayerUI_Manager::Update_GameObject(const _float& fTimeDelta)
{
	__super::Update_GameObject(fTimeDelta);

	return NO_EVENT();
}

void CPlayerUI_Manager::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);

	// player의 info가 바뀌었다면
	// ui들 active on off 결정후 set texture
	if (m_tInfo != CGlobal_Info::Get_Instance()->Get_PlayerInfo())
	{
		// info를 바꿔 주고
		m_tInfo = CGlobal_Info::Get_Instance()->Get_PlayerInfo();

		// 바뀐 info를 기준으로 onoff 결정
		PlayerUI_OnOff();
	}
}

void CPlayerUI_Manager::Render_GameObject()
{
	CUIBase::Render_GameObject();
}

HRESULT CPlayerUI_Manager::PlayerUI_OnOff()
{
	// active 를 다 꺼줌
	for (auto& pChild : m_vecChildren)
	{
		if (pChild)
			pChild->Set_Active(false);
	}

	// handR은 거의 켜주기 때문에 active true
	if (FAILED(TagUI_SetActive(L"HandRUI", true)))
		return E_FAIL;

	if (m_tInfo.eWeapon == WEAPON::WP_KATANA)
		TagUI_SetActive(L"HandRUI", false);

	// state에 따라 onoff
	switch (m_tInfo.ePlayerState)
	{
	case IDLE:
	case JUMP:
		break;

	case KICK:
		TagUI_SetActive(L"FootUI", true);
		break;

	case ATTACK:
		break;

	case ATTACK_INSTANT:
		if (FAILED(TagUI_SetActive(L"HandLUI", true)))
			return E_FAIL;
		if (FAILED(TagUI_SetActive(L"FootUI", true)))
			return E_FAIL;
		if (FAILED(TagUI_SetActive(L"ArmUI", true)))
			return E_FAIL;
		break;

	case ZOOMING:
		break;

	case ZOOM:
		break;

	case RELOAD:
		if (FAILED(TagUI_SetActive(L"HandLUI", true)))
			return E_FAIL;
		break;

	case DOPING:
		if (FAILED(TagUI_SetActive(L"HandLUI", true)))
			return E_FAIL;
		break;

	case OPENING:
		if (FAILED(TagUI_SetActive(L"HandLUI", true)))
			return E_FAIL;
		if (FAILED(TagUI_SetActive(L"ArmUI", true)))
			return E_FAIL;
		break;

	case PLAYERDEAD:
		break;

	case CLEAR:
		m_bActive = false;
		break;
	}

	// move에 따라 onoff
	switch (m_tInfo.ePlayerMove)
	{
	case PMV_NORMAL:
		break;

	case PMV_DASHATT:
		break;

	case PMV_DASH:
		break;

	case PMV_SLIDE:
		if (FAILED(TagUI_SetActive(L"FootUI", true)))
			return E_FAIL;
		break;

	case PMV_WALL:
		break;

	}
}

HRESULT CPlayerUI_Manager::TagUI_SetActive(const _tchar* pTag, _bool _bActive)
{
	CUIBase* pChild = Find_Child_ByTag(pTag);
	if (pChild)
	{
		pChild->Set_Active(_bActive);
		dynamic_cast<CUI*>(pChild)->Set_Texture();
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CPlayerUI_Manager::Set_PlayerUI()
{
	_uint iSceneIndex = CManagement::GetInstance()->Get_CurrentSceneIdx();

	// foot UI 생성
	CPlayer_Foot* pFootUI = dynamic_cast<CPlayer_Foot*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_PlayerFootUI", iSceneIndex, L"UI_Layer"));
	if (pFootUI)
	{
		pFootUI->Set_ObjTag(L"FootUI");
		Add_Child(pFootUI); // 루트 UI에 등록
	}

	// habdR UI 생성
	CPlayer_HandR* pHandRUI = dynamic_cast<CPlayer_HandR*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_PlayerHandRUI", iSceneIndex, L"UI_Layer"));
	if (pHandRUI)
	{
		pHandRUI->Set_ObjTag(L"HandRUI");
		Add_Child(pHandRUI); // 루트 UI에 등록
	}

	// handL UI 생성
	CPlayer_HandL* pHandLUI = dynamic_cast<CPlayer_HandL*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_PlayerHandLUI", iSceneIndex, L"UI_Layer"));
	if (pHandLUI)
	{
		pHandLUI->Set_ObjTag(L"HandLUI");
		Add_Child(pHandLUI); // 루트 UI에 등록
	}

	// arm UI 생성
	CPlayer_Arm* pArmUI = dynamic_cast<CPlayer_Arm*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_PlayerArmUI", iSceneIndex, L"UI_Layer"));
	if (pArmUI)
	{
		pArmUI->Set_ObjTag(L"ArmUI");
		Add_Child(pArmUI); // 루트 UI에 등록
	}

	return S_OK;
}

CPlayerUI_Manager* CPlayerUI_Manager::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayerUI_Manager* pHpBar = new CPlayerUI_Manager(pGraphicDev);

	if (FAILED(pHpBar->Ready_GameObject()))
	{
		Safe_Release(pHpBar);
		MSG_BOX("CPlayerUI_Manager Create Failed");
		return nullptr;
	}

	return pHpBar;
}

CGameObject* CPlayerUI_Manager::Clone(void* pArg)
{
	CPlayerUI_Manager* pInstance = new CPlayerUI_Manager(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CPlayerUI_Manager Clone Failed");
		return nullptr;
	}

	return pInstance;
}

void CPlayerUI_Manager::Free()
{
	__super::Free();
}