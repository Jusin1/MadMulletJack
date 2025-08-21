#include "pch.h"
#include "CTextUI.h"
#include "CFontMgr.h"

// 유틸 함수
static inline float SXi(float x) { return WINCX * 0.5f + x; }
static inline float SYi(float y) { return WINCY * 0.5f - y; }
static inline float Lerp(float a, float b, float t) { return a + (b - a) * t; }
static inline float EaseOutCubic(float t) { float u = 1.f - t; return 1.f - u * u * u; }

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
	, m_bPosFix(false)
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
	, m_bPosFix(rhs.m_bPosFix)
{
}
CTextUI::~CTextUI() {}


HRESULT CTextUI::Ready_GameObject() { return __super::Ready_GameObject(); }
HRESULT CTextUI::Initialize(void* pArg) { return __super::Initialize(pArg); }

_int CTextUI::Update_GameObject(const _float& fTimeDelta)
{
	if (m_appearPlaying) {
		m_appearT += fTimeDelta;
		if (m_appearT >= m_appearDur) {
			m_appearT = m_appearDur;
			m_appearPlaying = false;
		}
	}

	return __super::Update_GameObject(fTimeDelta), NO_EVENT;
}

void CTextUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	__super::LateUpdate_GameObject(fTimeDelta);
}

void CTextUI::Render_GameObject()
{
	if (m_text.empty()) return;

	const float currScale = CurrentRenderScale();
	_vec2 pos{ SXi(m_fX), SYi(m_fY) };
	if (m_bPosFix)
	{
		pos.x = m_pTransformCom->Get_Info(INFO_POS).x;
		pos.y = m_pTransformCom->Get_Info(INFO_POS).y;
	}
	

	// 총 폭 계산 (문자 폭 + letterSpacing, 마지막 문자 뒤 spacing 제외)
	auto measureTotalWidth = [&](float scale) -> float {
		float total = 0.f;
		for (wchar_t ch : m_text)
		{
			if (ch == L' ') {
				total += 8.f * scale + m_letterSpacing;
				continue;
			}
			wchar_t buf[2] = { ch, 0 };
			_vec2 sz{};
			Engine::CFontMgr::GetInstance()->Measure_Scaled(
				m_fontTag.c_str(), buf, &sz, scale);
			total += sz.x + m_letterSpacing;
		}
		if (!m_text.empty()) total -= m_letterSpacing; // 마지막 간격 제거
		return total;
		};

	float x = pos.x;
	if (m_centered) {
		float total = measureTotalWidth(currScale);
		x -= total * 0.5f; // 가운데 정렬: 시작 x를 왼쪽으로 절반 이동
	}

	// 문자 단위 렌더 (간격 적용)
	for (wchar_t ch : m_text)
	{
		if (ch == L' ')
		{
			x += 8.f * currScale + m_letterSpacing;
			continue;
		}

		wchar_t buf[2] = { ch, 0 };
		_vec2 charPos{ x, pos.y };


		// 회전 적용을 위해서
		//if (m_bPosFix)
		//{
		//	// 회전 중심: 문장 전체의 중앙
		//	float totalWidth = measureTotalWidth(currScale);
		//	_vec2 center = { pos.x + totalWidth * 0.5f, pos.y };

		//	_vec2 rotated;
		//	rotated.x = center.x + (charPos.x - center.x) * cosf(m_fRotSum) - (charPos.y - center.y) * sinf(m_fRotSum);
		//	rotated.y = center.y + (charPos.x - center.x) * sinf(m_fRotSum) + (charPos.y - center.y) * cosf(m_fRotSum);


		//	Engine::CFontMgr::GetInstance()->Render_Font_Scaled(
		//		m_fontTag.c_str(), buf, &rotated, m_color, currScale);

		//	_vec2 sz{};
		//	Engine::CFontMgr::GetInstance()->Measure_Scaled(
		//		m_fontTag.c_str(), buf, &sz, currScale);

		//	x += sz.x + m_letterSpacing;
		//	return;
		//}
		//else
		{
			// per-char 회전 API 없으므로 스케일만 적용
			Engine::CFontMgr::GetInstance()->Render_Font_Scaled(
				m_fontTag.c_str(), buf, &charPos, m_color, currScale);

			_vec2 sz{};
			Engine::CFontMgr::GetInstance()->Measure_Scaled(
				m_fontTag.c_str(), buf, &sz, currScale);

			x += sz.x + m_letterSpacing;
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

void CTextUI::PlayAppear(float duration, float startMul, float overMul)
{
	m_appearDur = max(0.05f, duration);
	m_appearStart = startMul;
	m_appearOver = overMul;
	m_appearT = 0.f;
	m_appearPlaying = true;
}

float CTextUI::CurrentRenderScale() const
{
	if (!m_appearPlaying) return m_scale;

	float t = m_appearT / max(0.0001f, m_appearDur);
	if (t < 0.5f) {
		float k = EaseOutCubic(t / 0.5f);
		return Lerp(m_scale * m_appearStart, m_scale * m_appearOver, k);
	}
	else {
		float k = EaseOutCubic((t - 0.5f) / 0.5f);
		return Lerp(m_scale * m_appearOver, m_scale, k);
	}
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