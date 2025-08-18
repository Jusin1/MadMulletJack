#include "pch.h"
#include "CHpBarUI.h"
#include "CObjectManager.h"
#include "CMan_HpBarUI.h"
#include "CPhone_HpBarUI.h"
#include "CManagement.h"
#include "CColRect_HpBarUI.h"

CHpBarUI::CHpBarUI(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev), m_iHitCount(0.f), m_fHpPercent(0.f), m_eScene(SCENE_END), m_bHitChange(false)
{
}

CHpBarUI::CHpBarUI(const CHpBarUI& rhs)
	:CUI(rhs), m_iHitCount(rhs.m_iHitCount), m_fHpPercent(rhs.m_fHpPercent), m_eScene(rhs.m_eScene), m_bHitChange(rhs.m_bHitChange)
{
}

CHpBarUI::~CHpBarUI()
{

}

HRESULT	CHpBarUI::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

HRESULT	CHpBarUI::Initialize(void* pArg)
{
	// 상위 객체들 initial(set compnent 까지 해줌)
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Set_HpBarUI()))
		return E_FAIL;

	HitCount_Reset(); // hitcount <- 0 (scene 전환시 0으로 맞추기 위함)

	m_fRange = 10.f;
	
	Set_New_TransInfo(5.f, 0.f);

	//timer 할래말래

	return S_OK;
}

_int	CHpBarUI::Update_GameObject(const _float& fTimeDelta)
{
	__super::Update_GameObject(fTimeDelta);

	for (auto& pChild : m_vecChildren)
	{
		pChild->GetTransform()->Move_YUpDown(fTimeDelta, m_fRange);
	}

	return NO_EVENT;
}

void	CHpBarUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);

	CPhone_HpBarUI* pPhone = dynamic_cast<CPhone_HpBarUI*>(this->Find_Child_ByTag(TEXT("PhoneUI")));
	CMan_HpBarUI* pMan = dynamic_cast<CMan_HpBarUI*>(this->Find_Child_ByTag(TEXT("ManUI")));

	if (Is_Scene_Change()) // scene 이 바뀌면 
	{
		pPhone->Set_Texture(m_eScene); // texture를 바꿔라
		pMan->Set_Texture(m_eScene); 
	}

	if (m_bHitChange) // hitcount가 바뀌면
	{
		pMan->Set_Texture(m_iHitCount); // texture를 바꿔라
		m_bHitChange = false;
	}
}

void	CHpBarUI::Render_GameObject()
{
	__super::Render_GameObject();
}

CHpBarUI* CHpBarUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CHpBarUI* pHpBar = new CHpBarUI(pGraphicDev);

	if (FAILED(pHpBar->Ready_GameObject()))
	{
		Safe_Release(pHpBar);
		MSG_BOX("CHpBarUI Create Failed");
		return nullptr;
	}

	return pHpBar;
}

CGameObject* CHpBarUI::Clone(void* pArg)
{
	CHpBarUI* pInstance = new CHpBarUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CHpBarUI Clone Failed");
		return nullptr;
	}

	return pInstance;
}

HRESULT CHpBarUI::Set_Component()
{
	if (FAILED(__super::Set_Component()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHpBarUI::Set_HpBarUI()
{
	CPhone_HpBarUI* pPhoneUI = dynamic_cast<CPhone_HpBarUI*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_HpbarUI_Phone", SCENE_STAGE_1, L"UI_Layer"));
	if (pPhoneUI)
	{
		pPhoneUI->Set_ObjTag(L"PhoneUI");
		Add_Child(pPhoneUI); // 루트 UI에 등록
	}

	CMan_HpBarUI* pManUI = dynamic_cast<CMan_HpBarUI*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_HpbarUI_Man", SCENE_STAGE_1, L"UI_Layer"));
	if (pManUI)
	{
		pManUI->Set_ObjTag(L"ManUI");
		Add_Child(pManUI); // 루트 UI에 등록
	}

	//CColRect_HpBarUI* pColRectUI = dynamic_cast<CColRect_HpBarUI*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_HpbarUI_ColRect", SCENE_STAGE_1, L"UI_Layer"));
	//if (pColRectUI)
	//{
	//	pColRectUI->Set_ObjTag(L"ColRectUI");
	//	Add_Child(pColRectUI); // 루트 UI에 등록
	//}

	return S_OK;
}

_bool CHpBarUI::Is_Scene_Change()
{
	if (m_eScene != (SCENE)CManagement::GetInstance()->Get_CurrentSceneIdx())
	{
		m_eScene = (SCENE)CManagement::GetInstance()->Get_CurrentSceneIdx();
		return true;
	}
	
	return false;
}

// 체력 상호작용 받아오기
void CHpBarUI::Set_Hp(_float _fMaxHp, _float _fCurHp)
{
	// 내가 그리는 y값의 percent 만큼 그리기

	m_fHpPercent = _fCurHp / _fMaxHp;

	// percent 에 따라 색깔 (R:1-percent, G : percent , B =0)
	CColRect_HpBarUI* pColRectUI = dynamic_cast<CColRect_HpBarUI*>(this->Find_Child_ByTag(TEXT("ColRectUI")));
	if (pColRectUI)
	{
		pColRectUI->Set_HpBarColor(m_fHpPercent);
	}

	// curhp에 따라 출력 글씨 셋팅
}

void CHpBarUI::Free()
{
	__super::Free();
}