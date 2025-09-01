#include "pch.h"
#include "CHpBarUI.h"
#include "CObjectManager.h"
#include "CMan_HpBarUI.h"
#include "CPhone_HpBarUI.h"
#include "CBlackGackGround.h"
#include "CTextUI.h"
#include "CManagement.h"
#include "CMapFactory.h"
#include  "CImageUI.h"

CHpBarUI::CHpBarUI(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev), m_iHitCount(0), m_fHpPercent(0.f), m_eScene(SCENE_END), m_bHitChange(false),
	m_fRectY(0.f)
{
}

CHpBarUI::CHpBarUI(const CHpBarUI& rhs)
	:CUI(rhs), m_iHitCount(rhs.m_iHitCount), m_fHpPercent(rhs.m_fHpPercent), m_eScene(rhs.m_eScene), m_bHitChange(rhs.m_bHitChange),
	m_fRectY(rhs.m_fRectY)
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

	HitCount_Reset(); // hitcount <- 0 (scene 전환시 0으로 맞추기 위함)

	//timer 할래말래
	Set_UISizeAndPos(100.f, 100.f, WINCX * 0.5f - 450.f, WINCY * 0.5f);

	// 움직임 셋팅
	m_tMoveInfo = { MV_UpDown, false, 10.f, 0.f };
	// 움직임 속도 셋팅
	Set_New_TransInfo(30.f, 0.f);

	// 자식들이 따라오게
	Set_IsPosParentFix(true); // 부모 행렬 따라가게

	m_vRectOriginOffset = { 33.5f, -200.f,0.f };
	//scene에 따라 셋팅값 적용
	_uint iTargetScene = CMapFactory::GetInstance()->GetTargetSceneIndex();
	switch (iTargetScene)
	{
	case SCENE_DEV:
	case SCENE_TUTORIAL:
	case SCENE_STAGE_1:
	//case SCENE_STAGE_2: // test
		m_iSceneCase = 0;
		m_vTextSet = { -420.f, -145.f ,2.f};
		break;

	case SCENE_STAGE_2: // test
	case SCENE_SNIPE:
	case SCENE_BOSS:
	case SCENE_CAR:
		m_iSceneCase = 1;
		m_vTextSet = { -430.f, -50.f ,1.3};
		break;
	case SCENE_END:
		m_iSceneCase = -1;
		m_vRectOriginOffset = { 0.f, 0.f,0.f };
		break;
	}

	// 셋팅값을 기준으로 ui들 생성
	if (FAILED(Set_HpBarUI()))
		return E_FAIL;

	return S_OK;
}

_int	CHpBarUI::Update_GameObject(const _float& fTimeDelta)
{
	__super::Update_GameObject(fTimeDelta);

	//Move_UI(fTimeDelta);
		
	return NO_EVENT;
}

void	CHpBarUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);

	CPhone_HpBarUI* pPhone	= dynamic_cast<CPhone_HpBarUI*>(this->Find_Child_ByTag(TEXT("PhoneUI")));
	CMan_HpBarUI* pMan		= dynamic_cast<CMan_HpBarUI*>(this->Find_Child_ByTag(TEXT("ManUI")));
	CBlackGackGround* pRect =  dynamic_cast<CBlackGackGround*>(this->Find_Child_ByTag(TEXT("RectUI")));
	if (Is_Scene_Change()) // scene 이 바뀌면 
	{
		pPhone	->Set_Texture(m_eScene); // texture를 바꿔라
		pMan	->Set_Texture(m_eScene); 
	}

	if (m_bHitChange) // hitcount가 바뀌면
	{
		pMan->Set_Texture(m_iHitCount); // texture를 바꿔라
		m_bHitChange = false;
	}
}

void	CHpBarUI::Render_GameObject()
{
	CUIBase::Render_GameObject();
}

HRESULT CHpBarUI::Set_HpBarUI()
{
	// 현재 씬을 받아옴
	auto iSceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();
	_vec3 vLocalOffset;

	// phone
	CPhone_HpBarUI* pPhoneUI = dynamic_cast<CPhone_HpBarUI*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_HpbarUI_Phone", iSceneIdx, L"UI_Layer"));
	if (pPhoneUI)
	{
		pPhoneUI->Set_ObjTag(L"PhoneUI");
		Add_Child(pPhoneUI); // 루트 UI에 등록

		vLocalOffset = { 0.f, -350.f,0.f };
		pPhoneUI->Set_LocalOffset(vLocalOffset);
	}

	//rect
	if (auto* pRect = dynamic_cast<CBlackGackGround*>(
		CObjectManager::GetInstance()->Clone_GameObject(
			L"Prototype_GameObject_BlackBackground", iSceneIdx, L"UI_Layer"))) {
		pRect->Set_UISizeAndPos(128.f, 95.f, 264.f, 638.f);
		m_fRectY = 95.f;
		pRect->SetColor(D3DXCOLOR{ 0.f,1.f,0.f,1.f });
		pRect->SetAlpha(1.f);
		pRect->FadeTo(190, 0.f, 0.2f);

		pRect->Set_IsPosFix(false);

		pRect->Set_New_TransInfo(0.f, 7.2f);
		pRect->GetTransform()->Rotation({ 0.f,0.f,1.f }, 1);

		pRect->Set_ObjTag(L"RectUI");
		Add_Child(pRect);

		pRect->Set_LocalOffset(m_vRectOriginOffset);
	}

	CMan_HpBarUI* pManUI = dynamic_cast<CMan_HpBarUI*>(CObjectManager::GetInstance()->Clone_GameObject(L"Prototype_GameObject_HpbarUI_Man", iSceneIdx, L"UI_Layer"));
	if (pManUI)
	{
		pManUI->Set_ObjTag(L"ManUI");
		Add_Child(pManUI); // 루트 UI에 등록

		// offset은 클래스 내부에서
	}

	if (auto* txt1 = dynamic_cast<CTextUI*>(
		CObjectManager::GetInstance()->Clone_GameObject(
			L"Prototype_GameObject_TextUI", iSceneIdx, L"UI_Layer"))) {
		txt1->SetFontTag(L"UIFont");
		txt1->SetText(L"");
		txt1->SetColor(g_Color_White);
		txt1->SetScale(m_vTextSet.z);
		txt1->SetCentered(true);
		txt1->SetLetterSpacing(1.f);
		txt1->Set_UIPosition(m_vTextSet.x, m_vTextSet.y ,80.f , 80.f);
		txt1->SetRotation(20.f);

		txt1->Set_ObjTag(L"Text");
		Add_Child(txt1);
	}

	return S_OK;
}

void CHpBarUI::Create_CountEff(const _tchar* _tProTag)
{
	auto sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();

	CImageUI* pFx = dynamic_cast<CImageUI*>(
		CObjectManager::GetInstance()->Clone_GameObject(
			L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"));
	if (!pFx) return;

	pFx->Set_UISizeAndPos(100.f, 100.f, 0.f , -300.f);
	pFx->RegisterTexture(L"Com_Texture_HpCountEff", _tProTag, 0, 0, 1.f, false);
	pFx->ChangeTexture(L"Com_Texture_HpCountEff");


	pFx->Set_ObjTag(L"Eff_Count");

	Add_Child(pFx);

	return;
}

void CHpBarUI::Delete_CountEff()
{
	CUIBase* pEff = Find_Child_ByTag(TEXT("Eff_Count"));

	if (!pEff)
		return;

	pEff->Set_Dead(true);
	Remove_Child(pEff);

	return;
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

	if (_fCurHp < 0)
		return;

	// percent 에 따라 색깔 (R:1-percent, G : percent , B =0)
	CBlackGackGround* pRect = dynamic_cast<CBlackGackGround*>(this->Find_Child_ByTag(TEXT("RectUI")));
	if (pRect)
	{
		pRect->SetColor(D3DXCOLOR{ 1.f- m_fHpPercent, m_fHpPercent, 0.f, 1.f });
		_float fSizeX, fSizeY;
		_float fSpeed = pRect->GetTransform()->GetTransformInfo().fSpeed;
		pRect->Get_UISize(fSizeX, fSizeY);
		pRect->Set_UISize(fSizeX, m_fRectY * m_fHpPercent);// rect 사이즈 줄어들게 .. 위치 변경은 아직

		// 줄어든 사이즈
		_vec3 vNewOffset = {m_vRectOriginOffset.x ,
							m_vRectOriginOffset.y - m_fRectY * (1 - m_fHpPercent) * 0.5f,
							0.f};
		pRect->Set_LocalOffset(vNewOffset);
	}

	// curhp에 따라 출력 글씨 셋팅
	CTextUI* txt1 = dynamic_cast<CTextUI*>(this->Find_Child_ByTag(TEXT("Text")));
	if (txt1)
	{
		//if (m_iSceneCase) // 1일때
		//{
		//	int iHp = static_cast<int>(std::ceil(m_fHpPercent * 100.f));
		//	std::wstring hpText = std::to_wstring(iHp) + L"%";
		//	txt1->SetText(hpText.c_str());
		//}

		//else
		{
			int iHp = static_cast<int>(std::ceil(_fCurHp));

			//if(iHp > 3)
				Delete_CountEff();

			// 숫자 셋팅
			switch (iHp)
			{
			case 1:
				txt1->SetText(TEXT("01")); 
				Create_CountEff(L"Prototype_Component_Texture_UIHpBarPhoneEff1");
				break;
				
			case 2:
				txt1->SetText(TEXT("02")); 
				Create_CountEff(L"Prototype_Component_Texture_UIHpBarPhoneEff2");
				break;
				
			case 3:
				txt1->SetText(TEXT("03"));
				Create_CountEff(L"Prototype_Component_Texture_UIHpBarPhoneEff3");
				break;	
				
			case 4:
				txt1->SetText(TEXT("04")); break;
			case 5:
				txt1->SetText(TEXT("05")); break;
			case 6:
				txt1->SetText(TEXT("06")); break;
			case 7:
				txt1->SetText(TEXT("07")); break;
			case 8:
				txt1->SetText(TEXT("08")); break;
			case 9:
				txt1->SetText(TEXT("09")); break;
			case 10:
				txt1->SetText(TEXT("10")); break;
			default:
				txt1->SetText(TEXT("00")); break;
			}
		}
		
	}
}

HRESULT CHpBarUI::Set_Component()
{
	if (FAILED(__super::Set_Component()))
		return E_FAIL;

	return S_OK;
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

void CHpBarUI::Free()
{
	__super::Free();
	Delete_CountEff();
}