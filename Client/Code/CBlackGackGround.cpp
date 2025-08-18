#include "pch.h"
#include "CBackGround.h"
#include "CBlackGackGround.h"

CBlackGackGround::CBlackGackGround(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev)
	, m_alpha(0)
	, m_start(0)
	, m_target(180)
	, m_fadeActive(false)
	, m_t(0.f)
	, m_delay(0.f)
	, m_dur(0.3f)
	, m_useHole(false)
	, m_holeX(0.f)
	, m_holeY(0.f)
	, m_holeW(0.f)
	, m_holeH(0.f)
	, m_color(D3DXCOLOR(0, 0, 0, 1))   // 기본 검정
{
}

CBlackGackGround::CBlackGackGround(const CBlackGackGround& rhs)
	: CUI(rhs)
	, m_alpha(rhs.m_alpha)
	, m_start(rhs.m_start)
	, m_target(rhs.m_target)
	, m_fadeActive(rhs.m_fadeActive)
	, m_t(rhs.m_t)
	, m_delay(rhs.m_delay)
	, m_dur(rhs.m_dur)
	, m_useHole(rhs.m_useHole)
	, m_holeX(rhs.m_holeX)
	, m_holeY(rhs.m_holeY)
	, m_holeW(rhs.m_holeW)
	, m_holeH(rhs.m_holeH)
	, m_color(rhs.m_color)   // 기본 검정
{
}

CBlackGackGround::~CBlackGackGround()
{
}

HRESULT CBlackGackGround::Initialize(void* pArg)
{
	if (FAILED(CUI::Initialize(nullptr)))
		return E_FAIL;

	Set_UIPosition(0.f, 0.f, static_cast<_float>(WINCX), static_cast<_float>(WINCY));
	return S_OK;
}

_int CBlackGackGround::Update_GameObject(const _float& fTimeDelta)
{
	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_Info(INFO_POS, _vec3(m_fX, -m_fY, 0.f));

	if (m_fadeActive)
	{
		m_t += fTimeDelta;

		if (m_t >= m_delay)
		{
			_float u = (m_t - m_delay) / max(0.001f, m_dur);
			if (u >= 1.f) { u = 1.f; m_fadeActive = false; }
			m_alpha = static_cast<BYTE>(m_start + (m_target - m_start) * u);
		}
	}

	return CUI::Update_GameObject(fTimeDelta);
}

void CBlackGackGround::Render_GameObject()
{
	LPDIRECT3DSTATEBLOCK9 pStateBlock = nullptr;
	if (SUCCEEDED(m_pGraphicDev->CreateStateBlock(D3DSBT_ALL, &pStateBlock)))
		pStateBlock->Capture();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphicDev->SetTexture(0, nullptr);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(m_alpha, (BYTE)(m_color.r * 255), (BYTE)(m_color.g * 255), (BYTE)(m_color.b * 255)));

	if (!m_useHole)
	{
		CUI::Render_GameObject();
	}
	else
	{
		const _float halfW = static_cast<_float>(WINCX) * 0.5f;
		const _float halfH = static_cast<_float>(WINCY) * 0.5f;
		const _float hx = m_holeW * 0.5f;
		const _float hy = m_holeH * 0.5f;

		const _float left = max(-halfW, m_holeX - hx);
		const _float right = min(halfW, m_holeX + hx);
		const _float top = min(halfH, m_holeY + hy);
		const _float bottom = max(-halfH, m_holeY - hy);

		if (top < halfH)
			DrawSolidQuad(0.f, (halfH + top) * 0.5f, static_cast<_float>(WINCX), halfH - top);

		if (bottom > -halfH)
			DrawSolidQuad(0.f, (bottom + (-halfH)) * 0.5f, static_cast<_float>(WINCX), bottom - (-halfH));

		if (left > -halfW)
			DrawSolidQuad((left + (-halfW)) * 0.5f, m_holeY, left - (-halfW), m_holeH);

		if (right < halfW)
			DrawSolidQuad((right + halfW) * 0.5f, m_holeY, halfW - right, m_holeH);
	}

	if (pStateBlock) { pStateBlock->Apply(); pStateBlock->Release(); }
}

void CBlackGackGround::DrawSolidQuad(_float cx, _float cy, _float w, _float h)
{
	m_pTransformCom->Set_Scale(w, h, 1.f);
	m_pTransformCom->Set_Info(INFO_POS, _vec3(cx, -cy, 0.f));
	CUI::Render_GameObject();
}

void CBlackGackGround::FadeTo(BYTE target, _float delay, _float duration)
{
	m_fadeActive = true;
	m_t = 0.f;
	m_delay = delay;
	m_dur = duration;
	m_start = m_alpha;
	m_target = target;
}

void CBlackGackGround::SetHoleRect(_float x, _float y, _float w, _float h)
{
	m_useHole = true;
	m_holeX = x;
	m_holeY = y;
	m_holeW = w;
	m_holeH = h;
}

CBlackGackGround* CBlackGackGround::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	return new CBlackGackGround(pGraphicDev);
}

CGameObject* CBlackGackGround::Clone(void* pArg)
{
	auto* pInstance = new CBlackGackGround(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CBlackGackGround::Free()
{
	CUI::Free();
}