#include "pch.h"
#include "CHpBarUI.h"
#include "CObjectManager.h"
#include "CMan_HpBarUI.h"
#include "CPhone_HpBarUI.h"
#include "CBlackGackGround.h"
#include "CTextUI.h"
#include "CManagement.h"

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

	m_tMoveInfo = { MV_UpDown, false, 10.f, 0.f };
	
	Set_New_TransInfo(5.f, 0.f);

	//timer 할래말래

	return S_OK;
}

_int	CHpBarUI::Update_GameObject(const _float& fTimeDelta)
{
	__super::Update_GameObject(fTimeDelta);

	// 만약 HpBar Set_UIMoveInfo가 안 바뀌면 MoveUI만 해주면 됨
	for (auto& pChild : m_vecChildren)
	{
		CUI* pCUI = dynamic_cast<CUI*>(pChild);
		if (pCUI)
		{
			pCUI->Set_UIMoveInfo(m_tMoveInfo);
			pCUI->Move_UI(fTimeDelta);
		}

		// 만약 chpbarui의 m_brenderon 값에 따라 자식들 그릴지 말지
		if (m_bRenderOn)
			pChild->Set_RenderOn(true);
		else
			pChild->Set_RenderOn(false);
	}

	//CTextUI* txt1 = dynamic_cast<CTextUI*>(this->Find_Child_ByTag(TEXT("Text")));
	//txt1->GetTransform()->Move_YUpDown(fTimeDelta,10.f,false, m_tMoveInfo.fSumRange);
		
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
	//__super::Render_GameObject();
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
	CPhone_HpBarUI* pPhoneUI = dynamic_cast<CPhone_HpBarUI*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_HpbarUI_Phone", SCENE_STATIC, L"UI_Layer"));
	if (pPhoneUI)
	{
		pPhoneUI->Set_ObjTag(L"PhoneUI");
		Add_Child(pPhoneUI); // 루트 UI에 등록
	}

	CMan_HpBarUI* pManUI = dynamic_cast<CMan_HpBarUI*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_HpbarUI_Man", SCENE_STATIC, L"UI_Layer"));
	if (pManUI)
	{
		pManUI->Set_ObjTag(L"ManUI");
		Add_Child(pManUI); // 루트 UI에 등록
	}

	if (auto* pRect = dynamic_cast<CBlackGackGround*>(
		CObjectManager::GetInstance()->Clone_GameObject(
			L"Prototype_GameObject_BlackBackground", SCENE_STATIC, L"UI_Layer"))) {
		pRect->Set_UISizeAndPos(129.f,100.f,263.f, 633.f);
		
		pRect->SetColor(D3DXCOLOR{0.f,1.f,0.f,1.f});
		pRect->SetAlpha(1.f);
		pRect->FadeTo(190,0.f,0.2f);
		
		pRect->Set_IsPosFix(false);

		pRect->Set_New_TransInfo(50.f, 7.f);
		pRect->GetTransform()->Rotation({ 0.f,0.f,1.f }, 1);

		pRect->Set_ObjTag(L"RectUI");
		Add_Child(pRect);
	}

	if (auto* txt1 = dynamic_cast<CTextUI*>(
		CObjectManager::GetInstance()->Clone_GameObject(
			L"Prototype_GameObject_TextUI", SCENE_STATIC, L"UI_Layer"))) {
		txt1->SetFontTag(L"UIFont");
		txt1->SetText(L"");
		txt1->SetColor(g_Color_White);
		txt1->SetScale(2.f);
		txt1->SetCentered(true);
		txt1->SetLetterSpacing(1.f);

		txt1->Set_UISizeAndPos(1.f, 1.f, 940.f, -200.f);
		txt1->Set_New_TransInfo(45.f, 10.f);
		txt1->GetTransform()->Rotation({ 0.f,0.f,1.f }, 1);
		txt1->Set_RotSum(D3DXToRadian(20.f));

		txt1->Set_ObjTag(L"Text");
		Add_Child(txt1);
	}

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

	m_fHpPercent = _fCurHp / _fMaxHp; // 지금은 여기 함수 안에서만 쓰여서 local 변수로 바꿔도 될듯.. 일단 남겨둠

	// percent 에 따라 색깔 (R:1-percent, G : percent , B =0)
	CBlackGackGround* pRect = dynamic_cast<CBlackGackGround*>(this->Find_Child_ByTag(TEXT("RectUI")));
	if (pRect)
	{
		pRect->SetColor(D3DXCOLOR{ 1.f- m_fHpPercent, m_fHpPercent, 0.f, 1.f });
		_float fSizeX, fSizeY;
		pRect->Get_UISize(fSizeX, fSizeY);
		//pRect->Set_UISize(fSizeX, fSizeY * m_fHpPercent); rect 사이즈 줄어들게,, but 실패
	}

	// curhp에 따라 출력 글씨 셋팅
	CTextUI* txt1 = dynamic_cast<CTextUI*>(this->Find_Child_ByTag(TEXT("Text")));
	if (txt1)
	{
		if(_fCurHp < 0)
			txt1->SetText(L"0");
		else
		{
			int iHp = static_cast<int>(std::ceil(_fCurHp));
			txt1->SetText(std::to_wstring(iHp).c_str());
		}
		
	}
}

void CHpBarUI::Free()
{
	__super::Free();
}