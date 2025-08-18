#include "pch.h"
#include "CBannerUI.h"

static inline float SX(float x) { return WINCX * 0.5f + x; }
static inline float SY(float y) { return WINCY * 0.5f + y; }

CBannerUI::CBannerUI(LPDIRECT3DDEVICE9 dev)
	: CUI(dev)
	, m_pTexArrow(nullptr)
	, m_pTexStripe(nullptr)
	, m_pFont(nullptr)
	, m_x(0.f), m_y(0.f), m_w(600.f), m_h(60.f)
	, m_padL(16.f), m_padR(12.f), m_padT(6.f), m_padB(6.f)
	, m_arrowSizePx(0.f), m_arrowOffX(0.f), m_arrowOffY(0.f)
	, m_text(L"FLOOR 11")
	, m_textColor(D3DCOLOR_ARGB(255, 255, 120, 60))
	, m_accentColor(D3DCOLOR_ARGB(255, 60, 255, 60))
	, m_fontHeight(28)
	, m_colorCycle(false)
	, m_hueDeg(25.f), m_hueSpeed(90.f)
	, m_textOffX(0.f), m_textOffY(0.f)
	, m_stripeAnchor(StripeAnchor::Bottom), m_stripeOffY(0.f)
	, m_scrollPx(0.f), m_speedPx(120.f)
	, m_stripes(7), m_fillRatio(0.3f), m_angleDeg(26.f)
	, m_barHeightPx(10.f), m_barHeightRate(0.4f)
	, m_tileWPerH(0.3f), m_tileWidthPx(0.f)
{
}

CBannerUI::CBannerUI(const CBannerUI& rhs)
	: CUI(rhs)
	, m_pTexArrow(rhs.m_pTexArrow)
	, m_pTexStripe(rhs.m_pTexStripe)
	, m_pFont(nullptr)
	, m_x(rhs.m_x), m_y(rhs.m_y), m_w(rhs.m_w), m_h(rhs.m_h)
	, m_padL(rhs.m_padL), m_padR(rhs.m_padR), m_padT(rhs.m_padT), m_padB(rhs.m_padB)
	, m_arrowSizePx(rhs.m_arrowSizePx), m_arrowOffX(rhs.m_arrowOffX), m_arrowOffY(rhs.m_arrowOffY)
	, m_text(rhs.m_text), m_textColor(rhs.m_textColor), m_accentColor(rhs.m_accentColor)
	, m_fontHeight(rhs.m_fontHeight), m_colorCycle(rhs.m_colorCycle)
	, m_hueDeg(rhs.m_hueDeg), m_hueSpeed(rhs.m_hueSpeed)
	, m_textOffX(rhs.m_textOffX), m_textOffY(rhs.m_textOffY)
	, m_stripeAnchor(rhs.m_stripeAnchor), m_stripeOffY(rhs.m_stripeOffY)
	, m_scrollPx(rhs.m_scrollPx), m_speedPx(rhs.m_speedPx)
	, m_stripes(rhs.m_stripes), m_fillRatio(rhs.m_fillRatio), m_angleDeg(rhs.m_angleDeg)
	, m_barHeightPx(rhs.m_barHeightPx), m_barHeightRate(rhs.m_barHeightRate)
	, m_tileWPerH(rhs.m_tileWPerH), m_tileWidthPx(rhs.m_tileWidthPx)
{
}

CBannerUI::~CBannerUI() {}

HRESULT CBannerUI::Ready_GameObject()
{
	return __super::Ready_GameObject();
}

HRESULT CBannerUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) return E_FAIL;
	if (FAILED(Texture_Clone())) return E_FAIL;

	EnsureFont();
	SetBannerRect(0.f, 0.f, 600.f, 60.f);
	return S_OK;
}

_int CBannerUI::Update_GameObject(const _float& dt)
{
	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_Info(INFO_POS, _vec3(m_fX, -m_fY, 0.f));

	m_scrollPx += m_speedPx * dt;
	if (fabsf(m_scrollPx) > 100000.f)
		m_scrollPx = fmodf(m_scrollPx, 1000.f);
	if (m_colorCycle)
	{
		m_hueDeg += m_hueSpeed * dt;
		if (m_hueDeg >= 360.f) m_hueDeg -= 360.f;
		m_textColor = HSVtoARGB(m_hueDeg, 0.85f, 1.0f, 255);
	}

	return __super::Update_GameObject(dt);
}

void CBannerUI::LateUpdate_GameObject(const _float& dt)
{
	__super::LateUpdate_GameObject(dt);
}

void CBannerUI::Render_GameObject()
{
	LPDIRECT3DSTATEBLOCK9 sb = nullptr;
	if (SUCCEEDED(m_pGraphicDev->CreateStateBlock(D3DSBT_ALL, &sb)))
		sb->Capture();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x01);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	RenderStripeBar();
	RenderArrow();
	RenderText();

	if (sb) { sb->Apply(); sb->Release(); }
}

void CBannerUI::EnsureFont()
{
	if (m_pFont) return;

	D3DXFONT_DESC fd{};
	fd.Height = m_fontHeight;
	fd.Weight = FW_BOLD;
	wcscpy_s(fd.FaceName, L"Consolas");

	D3DXCreateFontIndirect(m_pGraphicDev, &fd, &m_pFont);
}

void CBannerUI::RenderArrow()
{
	if (!m_pTexArrow) return;

	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, m_accentColor);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	const float autoSize = m_h - (m_padT + m_padB);
	const float size = (m_arrowSizePx > 0.f) ? m_arrowSizePx : autoSize;

	const float cx = (m_x - m_w * 0.5f + m_padL + size * 0.5f) + m_arrowOffX;
	const float cy = (-m_y + (m_h * 0.5f - m_padT - size * 0.5f)) + m_arrowOffY;

	D3DXMATRIX I; D3DXMatrixIdentity(&I);
	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &I);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	m_pTexArrow->Set_Texture(0);
	m_pTransformCom->Set_Scale(size, size, 1.f);
	m_pTransformCom->Set_Info(INFO_POS, _vec3(cx, cy, 0.f));
	__super::Render_GameObject();
}

void CBannerUI::RenderStripeBar()
{
	const float barH = (m_barHeightPx > 0.f) ? m_barHeightPx : max(8.f, m_h * m_barHeightRate);
	const float barW = m_w - (m_padL + m_padR);
	const float leftL = m_x - m_w * 0.5f + m_padL;

	float topBase = 0.f;
	switch (m_stripeAnchor)
	{
	case StripeAnchor::Top:    topBase = (-m_y) - m_h * 0.5f + m_padT; break;
	case StripeAnchor::Middle: topBase = (-m_y) - barH * 0.5f; break;
	default:                   topBase = (-m_y) + m_h * 0.5f - m_padB - barH; break;
	}
	const float topL = topBase + m_stripeOffY;

	RECT sc{};
	sc.left = LONG(floorf(SX(leftL)));
	sc.top = LONG(floorf(SY(topL)));
	sc.right = LONG(ceilf(SX(leftL + barW)));
	sc.bottom = LONG(ceilf(SY(topL + barH)));

	m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	m_pGraphicDev->SetScissorRect(&sc);

	m_pGraphicDev->SetTexture(0, nullptr);
	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, m_accentColor);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);

	const float period = barW / max(1, m_stripes);
	const float byAspect = barH * m_tileWPerH;
	const float byRatio = period * m_fillRatio;

	float tileW = (m_tileWidthPx > 0.f) ? m_tileWidthPx : min(byAspect, byRatio);
	tileW = min(period - 1.f, tileW);

	const float skew = tanf(D3DXToRadian(m_angleDeg)) * barH;

	float phase = fmodf(-m_scrollPx, period);
	if (phase < 0) phase += period;

	float firstX = leftL + phase;
	if (m_speedPx < 0)
	{
		if (firstX < leftL) firstX += period;
	}
	else
	{
		if (firstX > leftL + (period - tileW)) firstX -= period;
	}

	struct VTx { float x, y, z, rhw; D3DCOLOR c; };
	const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
	m_pGraphicDev->SetFVF(FVF);

	for (int i = 0; i < m_stripes; ++i)
	{
		const float x0L = firstX + i * period;
		const float x1L = x0L + tileW;

		VTx v[4] = {
			{ SX(x0L) + 0.5f,        SY(topL) + 0.5f,        0.f, 1.f, m_accentColor },
			{ SX(x1L) + 0.5f,        SY(topL) + 0.5f,        0.f, 1.f, m_accentColor },
			{ SX(x0L + skew) + 0.5f, SY(topL + barH) + 0.5f, 0.f, 1.f, m_accentColor },
			{ SX(x1L + skew) + 0.5f, SY(topL + barH) + 0.5f, 0.f, 1.f, m_accentColor },
		};
		m_pGraphicDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(VTx));
	}

	m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
}

void CBannerUI::RenderText()
{
	EnsureFont();
	if (!m_pFont) return;

	const float arrowW = (m_arrowSizePx > 0.f) ? m_arrowSizePx : (m_h - (m_padT + m_padB));

	const float leftL = m_x - m_w * 0.5f + m_padL + arrowW + 8.f + m_textOffX;
	const float rightL = m_x + m_w * 0.5f - m_padR;
	const float topL = (-m_y) - m_h * 0.5f + m_padT + m_textOffY;
	const float bottomL = (-m_y) + m_h * 0.5f - m_padB + m_textOffY;

	RECT rc{};
	rc.left = LONG(floorf(SX(leftL)));
	rc.top = LONG(floorf(SY(topL)));
	rc.right = LONG(ceilf(SX(rightL)));
	rc.bottom = LONG(ceilf(SY(bottomL)));

	m_pFont->DrawTextW(nullptr, m_text.c_str(), -1, &rc,
		DT_LEFT | DT_VCENTER | DT_SINGLELINE, m_textColor);
}

#pragma region 배너 세팅
void CBannerUI::SetBannerRect(float x, float y, float w, float h)
{
	m_x = x; m_y = y; m_w = w; m_h = h;
	Set_UIPosition(x, y, w, h);
}

void CBannerUI::SetPadding(float l, float r, float t, float b)
{
	m_padL = l; m_padR = r; m_padT = t; m_padB = b;
}

void CBannerUI::SetText(const std::wstring& s)
{
	m_text = s;
}

void CBannerUI::SetTextColor(D3DCOLOR c)
{
	m_textColor = c;
	m_colorCycle = false;
}

void CBannerUI::SetAccentColor(D3DCOLOR c)
{
	m_accentColor = c;
}

void CBannerUI::SetFontHeight(int px)
{
	m_fontHeight = px;
	if (m_pFont) { m_pFont->Release(); m_pFont = nullptr; }
	EnsureFont();
}

void CBannerUI::SetTextColorCycle(bool enable, float degPerSec)
{
	m_colorCycle = enable;
	m_hueSpeed = degPerSec;
}

void CBannerUI::SetTextOffset(float dx, float dy)
{
	m_textOffX = dx; m_textOffY = dy;
}

void CBannerUI::SetArrowSizePx(float px)
{
	m_arrowSizePx = max(0.f, px);
}

void CBannerUI::SetArrowOffset(float dx, float dy)
{
	m_arrowOffX = dx; m_arrowOffY = dy;
}

void CBannerUI::SetStripeBarAnchor(StripeAnchor a)
{
	m_stripeAnchor = a;
}

void CBannerUI::SetStripeYOffsetPx(float dy)
{
	m_stripeOffY = dy;
}

void CBannerUI::SetStripeSpeed(float pxPerSec)
{
	m_speedPx = pxPerSec;
}

void CBannerUI::SetStripeCount(int count)
{
	m_stripes = max(1, count);
}

void CBannerUI::SetStripeFillRatio(float r)
{
	m_fillRatio = max(0.05f, min(0.95f, r));
}

void CBannerUI::SetStripeAngleDeg(float d)
{
	m_angleDeg = d;
}

void CBannerUI::SetStripeBarHeightPx(float px)
{
	m_barHeightPx = (px <= 0.f ? 0.f : px);
}

void CBannerUI::SetStripeTileAspect(float wPerH)
{
	m_tileWPerH = max(0.2f, wPerH);
}

void CBannerUI::SetStripeTileWidthPx(float px)
{
	m_tileWidthPx = max(0.f, px);
}

#pragma endregion 배너 세팅


HRESULT CBannerUI::Texture_Clone()
{
	{
		CTexture::TEXINFO info{};
		info.m_iStart = 0; info.m_iEndTex = 0;
		if (FAILED(Add_Components(L"Com_Texture_ArrowSmall", SCENE_STAGE_1,
			L"Prototype_Component_Texture_ArrowUI", (CComponent**)&m_pTexArrow, &info)))
			return E_FAIL;
	}
	return S_OK;
}

D3DCOLOR CBannerUI::HSVtoARGB(float h, float s, float v, BYTE a)
{
	while (h < 0) h += 360.f;
	while (h >= 360.f) h -= 360.f;

	float c = v * s;
	float x = c * (1.f - fabsf(fmodf(h / 60.f, 2.f) - 1.f));
	float m = v - c;

	float r = 0, g = 0, b = 0;
	if (h < 60) { r = c; g = x; }
	else if (h < 120) { r = x; g = c; }
	else if (h < 180) { g = c; b = x; }
	else if (h < 240) { g = x; b = c; }
	else if (h < 300) { r = x; b = c; }
	else { r = c; b = x; }

	BYTE R = (BYTE)((r + m) * 255.f);
	BYTE G = (BYTE)((g + m) * 255.f);
	BYTE B = (BYTE)((b + m) * 255.f);

	return D3DCOLOR_ARGB(a, R, G, B);
}

CBannerUI* CBannerUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBannerUI* p = new CBannerUI(pGraphicDev);
	if (FAILED(p->Ready_GameObject()))
	{
		MSG_BOX("BannerUI Create Failed");
		Safe_Release(p);
	}
	return p;
}

CGameObject* CBannerUI::Clone(void* pArg)
{
	CBannerUI* p = new CBannerUI(*this);
	if (FAILED(p->Initialize(pArg)))
	{
		MSG_BOX("BannerUI Clone Failed");
		Safe_Release(p);
	}
	return p;
}

void CBannerUI::Free()
{
	if (m_pFont)
	{
		m_pFont->Release();
		m_pFont = nullptr;
	}
	__super::Free();
}