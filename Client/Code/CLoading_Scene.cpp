#include "pch.h"
#include "CLoading_Scene.h"
#include "CManagement.h"
#include "CLoader.h"

// 다음 씬들
#include "CLogo.h"
#include "CDev.h"
#include "CTutorial.h"
#include "CStage.h"
#include "CStage_2.h"
#include "CStage_Boss.h"
#include "CStage_Snipe.h"
#include "CStage_Car.h"

// UI
#include "CObjectManager.h"
#include "CImageUI.h"
#include "CButtonUI.h"
#include "CTextUI.h"

CLoading_Scene::CLoading_Scene(LPDIRECT3DDEVICE9 pGD)
	: CScene(pGD)
	, m_eNextScene(SCENE_END)
	, m_pLoader(nullptr)
	, m_bg(nullptr)
	, m_barBg(nullptr)
	, m_barFill(nullptr)
	, m_txtPct(nullptr)
	, m_barWMax(500.f)
	, m_barH(24.f)
	, m_barX(300.f)
	, m_barY(0.f)
	, m_visProg(0.f)
	, m_switchRequested(false) // <-- 기본은 false, 필요 시 코드에서 true로 전환
	, m_skipVisual(false)
{
}

CLoading_Scene::~CLoading_Scene() {}

HRESULT CLoading_Scene::Ready_Loading(SCENE eNextScene)
{
	if (FAILED(CScene::Ready_Scene()))
		return E_FAIL;

	m_eNextScene = eNextScene;

	// 로더 시작
	m_pLoader = CLoader::Create(m_pGraphicDev, eNextScene);
	if (!m_pLoader) return E_FAIL;

	// 로고 씬은 로딩 스킵
	m_skipVisual = (eNextScene == SCENE_LOGO);

	// 로딩 화면 UI 구성
	if (!m_skipVisual) {
		if (FAILED(ready_ui()))
			return E_FAIL;
	}

	return S_OK;
}

_int CLoading_Scene::Update_Scene(const _float& dt)
{
	CScene::Update_Scene(dt);


	if (m_skipVisual) {
		if (m_pLoader && m_pLoader->Get_Finish()) {
			CScene* pNew = nullptr;
			switch (m_eNextScene) {
			case SCENE_LOGO:     pNew = CLogo::Create(m_pGraphicDev);     break;
			case SCENE_DEV:      pNew = CDev::Create(m_pGraphicDev);      break;
			case SCENE_TUTORIAL: pNew = CTutorial::Create(m_pGraphicDev); break;
			case SCENE_STAGE_1: pNew = CStage::Create(m_pGraphicDev); break;
			case SCENE_STAGE_2: pNew = CStage_2::Create(m_pGraphicDev); break;
			case SCENE_SNIPE: pNew = CStage_Snipe::Create(m_pGraphicDev); break;
			case SCENE_BOSS: pNew = CStage_Boss::Create(m_pGraphicDev); break;
			case SCENE_CAR: pNew = CStage_Car::Create(m_pGraphicDev); break;
			default: break;
			}
			if (pNew) CManagement::GetInstance()->Open_Scene(m_eNextScene, pNew);
		}
		return S_OK;
	}


	float target = 0.f;
	bool  done = false;
	if (m_pLoader) {
		target = m_pLoader->Get_Progress();
		done = m_pLoader->Get_Finish();
	}
	if (done) target = 1.f;

	const float spd = done ? 3.0f : 1.2f;
	m_visProg += (target - m_visProg) * (1.f - expf(-spd * dt));
	if (m_visProg > 1.f) m_visProg = 1.f;

	update_progress_ui(m_visProg);

	const float kEps = 0.995f;
	if (done && m_visProg >= kEps)
		m_switchRequested = true;

	if (m_switchRequested) {
		CScene* pNew = nullptr;
		switch (m_eNextScene) {
		case SCENE_LOGO:     pNew = CLogo::Create(m_pGraphicDev);     break;
		case SCENE_DEV:      pNew = CDev::Create(m_pGraphicDev);      break;
		case SCENE_TUTORIAL: pNew = CTutorial::Create(m_pGraphicDev); break;
		case SCENE_STAGE_1: pNew = CStage::Create(m_pGraphicDev); break;
		case SCENE_STAGE_2: pNew = CStage_2::Create(m_pGraphicDev); break;
		case SCENE_SNIPE: pNew = CStage_Snipe::Create(m_pGraphicDev); break;
		case SCENE_BOSS: pNew = CStage_Boss::Create(m_pGraphicDev); break;
		case SCENE_CAR: pNew = CStage_Car::Create(m_pGraphicDev); break;
		default: break;
		}
		if (pNew) CManagement::GetInstance()->Open_Scene(m_eNextScene, pNew);
		m_switchRequested = false;
	}

	return S_OK;
}

void CLoading_Scene::LateUpdate_Scene(const _float& dt)
{
	CScene::LateUpdate_Scene(dt);
	if (m_pLoader) SetWindowText(g_hWnd, m_pLoader->Get_String());
}

CLoading_Scene* CLoading_Scene::Create(LPDIRECT3DDEVICE9 pGraphicDev, SCENE eID)
{
	auto* p = new CLoading_Scene(pGraphicDev);
	if (FAILED(p->Ready_Loading(eID))) {
		MSG_BOX("CLoading_Scene Create Failed");
		Safe_Release(p);
		return nullptr;
	}
	return p;
}

void CLoading_Scene::Free()
{
	CScene::Free();
	Safe_Release(m_pLoader);
}


HRESULT CLoading_Scene::ready_ui()
{
	if (auto* bg = dynamic_cast<CImageUI*>(
		CObjectManager::GetInstance()->Clone_GameObject(
			L"Prototype_GameObject_UIImage", SCENE_LOADING, L"UI_Layer")))
	{
		bg->Set_UIPosition(0.f, 0.f, WINCX, WINCY);
		bg->RegisterTexture(L"Com_Tex_LoadingBG", L"Prototype_Component_Texture_LoadingBG",
			0, 2, 1.f, true);
		bg->ChangeTexture(L"Com_Tex_LoadingBG");
		bg->Play(true);
		m_bg = bg;
	}

	m_barX = 450.f;
	m_barY = 250.f;
	m_barWMax = 300.f;


	if (auto* barBg = dynamic_cast<CButtonUI*>(
		CObjectManager::GetInstance()->Clone_GameObject(
			L"Prototype_GameObject_UIButton", SCENE_LOADING, L"UI_Layer")))
	{
		barBg->Set_ButtonRect(m_barX, m_barY, m_barWMax, m_barH);
		barBg->SetSolidMode(true);
		barBg->SetSolidColors(
			D3DXCOLOR(0.08f, 0.08f, 0.08f, 0.85f),
			D3DXCOLOR(0.08f, 0.08f, 0.08f, 0.85f),
			D3DXCOLOR(0.08f, 0.08f, 0.08f, 0.85f),
			D3DXCOLOR(0.08f, 0.08f, 0.08f, 0.85f));
		barBg->SetInteractable(false);
		m_barBg = barBg;
	}

	if (auto* barFill = dynamic_cast<CButtonUI*>(
		CObjectManager::GetInstance()->Clone_GameObject(
			L"Prototype_GameObject_UIButton", SCENE_LOADING, L"UI_Layer")))
	{
		barFill->Set_ButtonRect(m_barX, m_barY, 1.f, m_barH); 
		barFill->SetSolidMode(true);
		barFill->SetSolidColors(
			D3DXCOLOR(1.f, 0.15f, 0.15f, 0.95f),
			D3DXCOLOR(1.f, 0.15f, 0.15f, 0.95f),
			D3DXCOLOR(1.f, 0.15f, 0.15f, 0.95f),
			D3DXCOLOR(1.f, 0.15f, 0.15f, 0.95f));
		barFill->SetInteractable(false);
		m_barFill = barFill;
	}

	if (auto* t = dynamic_cast<CTextUI*>(
		CObjectManager::GetInstance()->Clone_GameObject(
			L"Prototype_GameObject_TextUI", SCENE_LOADING, L"UI_Layer")))
	{
		t->SetFontTag(L"Font_UI_Bold");
		t->SetText(L"0%");
		t->SetScale(0.8f);
		t->SetCentered(true);
		t->Set_UIPosition(m_barX, m_barY - 430.f, 140.f, 28.f);
		m_txtPct = t;
	}
	return S_OK;
}

void CLoading_Scene::update_progress_ui(float p)
{
	p = std::clamp(p, 0.f, 1.f);

	if (m_barFill) {
		float w = max(1.f, m_barWMax * p);
		float cx = m_barX - m_barWMax * 0.5f + w * 0.5f;
		m_barFill->Set_ButtonRect(cx, m_barY, w, m_barH);
	}

	if (m_txtPct) {
		wchar_t buf[32];
		swprintf(buf, 32, L"%d%%", (int)(p * 100.f + 0.5f));
		m_txtPct->SetText(buf);
	}
}