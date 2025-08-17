#include "pch.h"
#include "CTextUI.h"
#include "CFontMgr.h"

static inline float SXi(float x) { return WINCX * 0.5f + x; }
static inline float SYi(float y) { return WINCY * 0.5f - y; }

CTextUI::CTextUI(LPDIRECT3DDEVICE9 dev)
	: CUI(dev)
	, m_text(L"")
	, m_fontTag(L"DefaultFont")
	, m_color(1.f, 1.f, 1.f, 1.f)
	, m_scale(1.f)
	, m_angle(0.f)
	, m_centered(false)
	, m_letterSpacing(0.f)
	, m_autoSize(false)
	, m_dirtyMeasure(true)
{
}
CTextUI::CTextUI(const CTextUI& rhs)
	: CUI(rhs)
	, m_text(rhs.m_text)
	, m_fontTag(rhs.m_fontTag)
	, m_color(rhs.m_color)
	, m_scale(rhs.m_scale)
	, m_angle(rhs.m_angle)
	, m_centered(rhs.m_centered)
	, m_letterSpacing(rhs.m_letterSpacing)
	, m_autoSize(rhs.m_autoSize)
	, m_dirtyMeasure(true)
{
}
CTextUI::~CTextUI() {}


HRESULT CTextUI::Ready_GameObject() { return __super::Ready_GameObject(); }
HRESULT CTextUI::Initialize(void* pArg) { return __super::Initialize(pArg); }

_int CTextUI::Update_GameObject(const _float& fTimeDelta)
{
	if (m_autoSize && m_dirtyMeasure && !m_text.empty())
	{
		_vec2 size{};
		if (Engine::CFontMgr::GetInstance()->Measure_Scaled(
			m_fontTag.c_str(), m_text.c_str(), &size, m_scale))
		{
			m_fSizeX = size.x + (float)(max(0, (int)m_text.size() - 1)) * m_letterSpacing;
			m_fSizeY = size.y;
		}
		m_dirtyMeasure = false;
	}
	__super::Update_GameObject(fTimeDelta);
	return NO_EVENT;
}

void CTextUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);
}

void CTextUI::Render_GameObject()
{
	if (m_text.empty()) return;

	_vec2 pos{ SXi(m_fX), SYi(m_fY) };

	if (m_centered)
	{
		Engine::CFontMgr::GetInstance()->Render_Font_ScaledCenteredRot(
			m_fontTag.c_str(), m_text.c_str(), &pos,
			m_color, m_scale, m_angle);
	}
	else
	{
		float x = pos.x;
		for (wchar_t ch : m_text)
		{
			if (ch == L' ')
			{
				float spaceWidth = 8.f * m_scale;
				x += spaceWidth + m_letterSpacing;
				continue;
			}

			wchar_t buf[2] = { ch, 0 };
			_vec2 charPos{ x, pos.y };

			Engine::CFontMgr::GetInstance()->Render_Font_Scaled(
				m_fontTag.c_str(), buf, &charPos, m_color, m_scale);

			_vec2 size{};
			Engine::CFontMgr::GetInstance()->Measure_Scaled(
				m_fontTag.c_str(), buf, &size, m_scale);

			x += size.x + m_letterSpacing;
		}
	}
}


void CTextUI::SetText(const std::wstring& ws) { m_text = ws; m_dirtyMeasure = true; }
void CTextUI::AppendText(const std::wstring& ws) { m_text += ws; m_dirtyMeasure = true; }
void CTextUI::SetFontTag(const std::wstring& tag) { m_fontTag = tag; m_dirtyMeasure = true; }
void CTextUI::SetColor(D3DXCOLOR c) { m_color = c; }
void CTextUI::SetScale(float s) { m_scale = s; m_dirtyMeasure = true; }
void CTextUI::SetRotation(float deg) { m_angle = deg; }
void CTextUI::SetCentered(bool on) { m_centered = on; }
void CTextUI::SetLetterSpacing(float px) { m_letterSpacing = px; m_dirtyMeasure = true; }
void CTextUI::SetAutoSize(bool on) { m_autoSize = on; m_dirtyMeasure = true; }

void CTextUI::FitToText()
{
	if (m_text.empty()) return;
	_vec2 size{};
	if (Engine::CFontMgr::GetInstance()->Measure_Scaled(
		m_fontTag.c_str(), m_text.c_str(), &size, m_scale))
	{
		m_fSizeX = size.x + (float)(max(0, (int)m_text.size() - 1)) * m_letterSpacing;
		m_fSizeY = size.y;
	}
	m_dirtyMeasure = false;
}


CTextUI* CTextUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTextUI* p = new CTextUI(pGraphicDev);
	if (FAILED(p->Ready_GameObject()))
	{
		MSG_BOX("CTextUI Create Failed");
		Safe_Release(p);
	}
	return p;
}

CGameObject* CTextUI::Clone(void* pArg)
{
	CTextUI* p = new CTextUI(*this);
	if (FAILED(p->Initialize(pArg)))
	{
		MSG_BOX("CTextUI Clone Failed");
		Safe_Release(p);
	}
	return p;
}

void CTextUI::Free()
{
	__super::Free();
}