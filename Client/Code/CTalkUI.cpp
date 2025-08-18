#include "pch.h"
#include "CTalkUI.h"
#include "CImageUI.h"
#include "CObjectManager.h"
#include "CFontMgr.h"
#include "CUIManager.h"
#include "CLisaUI.h"

CTalkUI::CTalkUI(LPDIRECT3DDEVICE9 dev)
	: CUI(dev)
	, m_iCurrentIndex(0)
	, m_pFrame(nullptr)
	, m_fCharInterval(0.05f)
	, m_fAccTime(0.f)
	, m_bTypingDone(false)
	, m_vTextOffset{ 20.f, 20.f }
	, m_fTextScale(1.f)
	, m_fFrameX(100.f)
	, m_fFrameY(WINCY - 200.f)
	, m_fFrameSizeX(WINCX - 200.f)
	, m_fFrameSizeY(150.f)
{
}

CTalkUI::CTalkUI(const CTalkUI& rhs)
	: CUI(rhs)
	, m_vecDialogues(rhs.m_vecDialogues)
	, m_iCurrentIndex(rhs.m_iCurrentIndex)
	, m_CurrentText(rhs.m_CurrentText)
	, m_DisplayText(rhs.m_DisplayText)
	, m_fCharInterval(rhs.m_fCharInterval)
	, m_fAccTime(0.f)
	, m_bTypingDone(rhs.m_bTypingDone)
	, m_pFrame(nullptr)
	, m_vTextOffset(rhs.m_vTextOffset)
	, m_fTextScale(rhs.m_fTextScale)
	, m_fFrameX(rhs.m_fFrameX)
	, m_fFrameY(rhs.m_fFrameY)
	, m_fFrameSizeX(rhs.m_fFrameSizeX)
	, m_fFrameSizeY(rhs.m_fFrameSizeY)
{
}

CTalkUI::~CTalkUI() {}


HRESULT CTalkUI::Ready_GameObject()
{
	if (FAILED(__super::Ready_GameObject()))
		return E_FAIL;
	return S_OK;
}

HRESULT CTalkUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pFrame = dynamic_cast<CImageUI*>(
		CObjectManager::GetInstance()->Clone_GameObject(
			L"Prototype_GameObject_UIImage", SCENE_STAGE_1, L"UI_Layer"));

	if (!m_pFrame)
		return E_FAIL;

	m_pFrame->Set_UIPosition(60.f, -300.f, 640.f, 100.f);
	if (FAILED(m_pFrame->RegisterTexture(
		L"TalkFrame", L"Prototype_Component_Texture_Talk")))
		return E_FAIL;
	if (FAILED(m_pFrame->ChangeTexture(L"TalkFrame")))
		return E_FAIL;

	Add_Child(m_pFrame);

	return S_OK;
}

_int CTalkUI::Update_GameObject(const _float& dt)
{
	// 테스트로 다음대사 출력
	if (GetAsyncKeyState(VK_RETURN) & 0x0001)
		NextDialogue();

	if (!m_bTypingDone && !m_CurrentText.empty())
	{
		m_fAccTime += dt;
		int charCount = (int)(m_fAccTime / m_fCharInterval);

		if (charCount > (int)m_DisplayText.size())
		{
			int nextChars = min(charCount, (int)m_CurrentText.size());
			m_DisplayText = m_CurrentText.substr(0, nextChars);

			if (m_DisplayText.size() == m_CurrentText.size())
				m_bTypingDone = true;
		}
	}

	return NO_EVENT;
}

void CTalkUI::LateUpdate_GameObject(const _float& dt)
{

	if (m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(RENDER_UI, this);
}

void CTalkUI::Render_GameObject()
{
	if (!Is_Active() || Get_Dead() || m_DisplayText.empty()) return;

	LPDIRECT3DSTATEBLOCK9 sb = nullptr;
	if (SUCCEEDED(m_pGraphicDev->CreateStateBlock(D3DSBT_ALL, &sb)))
		sb->Capture();

	m_pGraphicDev->SetTexture(0, nullptr);
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


	_vec2 pos = { m_fFrameX + m_vTextOffset.x, m_fFrameY + m_vTextOffset.y };
	if (m_fTextScale != 1.f)
		CFontMgr::GetInstance()->Render_Font_Scaled(L"TalkFont", m_DisplayText.c_str(), &pos,
			D3DXCOLOR(1, 1, 1, 1), m_fTextScale);
	else
		CFontMgr::GetInstance()->Render_Font(L"TalkFont", m_DisplayText.c_str(), &pos,
			D3DXCOLOR(1, 1, 1, 1));

	if (sb) { sb->Apply(); sb->Release(); }
}


void CTalkUI::LoadDialogues(const vector<wstring>& dialogues)
{
	m_vecDialogues = dialogues;
	m_iCurrentIndex = -1;  

	m_CurrentText.clear();
	m_DisplayText.clear();
	m_fAccTime = 0.f;
	m_bTypingDone = true;
}

void CTalkUI::NextDialogue()
{
	m_CurrentText.clear();
	m_DisplayText.clear();

	const int size = (int)m_vecDialogues.size();
	if (size <= 0) {
		Set_Active(false);
		CUIManager::GetInstance()->DestroyEnterUI();
		return;
	}

	const int last = size - 1;

	if (m_iCurrentIndex < last)
	{
		++m_iCurrentIndex;
		m_CurrentText = m_vecDialogues[m_iCurrentIndex];

		if (m_iCurrentIndex == last) {
			m_pLisa->SetState(CLisaUI::AnimState::Bye);
		}
		else if (m_iCurrentIndex == 2) {
			CUIManager::GetInstance()->CreatePhoneUI();
		}

		m_fAccTime = 0.f;
		m_bTypingDone = false;
		return;
	}

	if (m_pFrame) m_pFrame->Set_Active(false);
	Set_Active(false);
	CUIManager::GetInstance()->DestroyEnterUI();
}

void CTalkUI::Set_FramePos(float x, float y, float sizeX, float sizeY)
{
	m_fFrameX = x;
	m_fFrameY = y;
	m_fFrameSizeX = sizeX;
	m_fFrameSizeY = sizeY;

	if (m_pFrame)
		m_pFrame->Set_UIPosition(x, y, sizeX, sizeY);
}


CTalkUI* CTalkUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTalkUI* p = new CTalkUI(pGraphicDev);
	if (FAILED(p->Ready_GameObject()))
	{
		MSG_BOX("CTalkUI Create Failed");
		Safe_Release(p);
	}
	return p;
}

CGameObject* CTalkUI::Clone(void* pArg)
{
	CTalkUI* p = new CTalkUI(*this);
	if (FAILED(p->Initialize(pArg)))
	{
		MSG_BOX("CTalkUI Clone Failed");
		Safe_Release(p);
	}
	return p;
}

void CTalkUI::Free()
{
	CUIBase::Free();
}