#include "pch.h"
#include "CTexture.h"
#include "CHeartUI.h"

CHeartUI::CHeartUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev)
	, m_pTexHeartL(nullptr)
	, m_pTexHeartR(nullptr)
	, m_pTexLine(nullptr)
	, m_pTexPulse(nullptr)
	, m_heartSizePx(72.f)
	, m_heartPosLX(0.f)
	, m_heartPosRX(0.f)
	, m_heartPosY(0.f)
	, m_lineHeightPx(8.f)
	, m_lineOffY(0.f)
	, m_lineXL(0.f)
	, m_lineXR(0.f)
	, m_lineTint(D3DCOLOR_ARGB(255, 255, 255, 255))
	, m_pulseEnabled(true)
	, m_pulseWidthPx(160.f)
	, m_pulseHeightPx(22.f)
	, m_pulseOffY(0.f)
	, m_pulseSpeed(220.f)
	, m_pulsePhase(0.f)
	, m_pulseTint(D3DCOLOR_ARGB(255, 255, 230, 240))
	, m_beatXL(0.f)
	, m_beatXR(0.f)
	, m_beatYOverride(false)
	, m_beatY(0.f)
{
}

CHeartUI::CHeartUI(const CHeartUI& rhs)
	: CUI(rhs)
	, m_pTexHeartL(rhs.m_pTexHeartL)
	, m_pTexHeartR(rhs.m_pTexHeartR)
	, m_pTexLine(rhs.m_pTexLine)
	, m_pTexPulse(rhs.m_pTexPulse)
	, m_heartSizePx(rhs.m_heartSizePx)
	, m_heartPosLX(rhs.m_heartPosLX)
	, m_heartPosRX(rhs.m_heartPosRX)
	, m_heartPosY(rhs.m_heartPosY)
	, m_lineHeightPx(rhs.m_lineHeightPx)
	, m_lineOffY(rhs.m_lineOffY)
	, m_lineXL(rhs.m_lineXL)
	, m_lineXR(rhs.m_lineXR)
	, m_lineTint(rhs.m_lineTint)
	, m_pulseEnabled(rhs.m_pulseEnabled)
	, m_pulseWidthPx(rhs.m_pulseWidthPx)
	, m_pulseHeightPx(rhs.m_pulseHeightPx)
	, m_pulseOffY(rhs.m_pulseOffY)
	, m_pulseSpeed(rhs.m_pulseSpeed)
	, m_pulsePhase(rhs.m_pulsePhase)
	, m_pulseTint(rhs.m_pulseTint)
	, m_beatXL(rhs.m_beatXL)
	, m_beatXR(rhs.m_beatXR)
	, m_beatYOverride(rhs.m_beatYOverride)
	, m_beatY(rhs.m_beatY)
{
}

CHeartUI::~CHeartUI()
{
}

HRESULT CHeartUI::Ready_GameObject()
{
	return S_OK;
}

HRESULT CHeartUI::Initialize(void* pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Texture_Clone()))
		return E_FAIL;

	return S_OK;
}

_int CHeartUI::Update_GameObject(const _float& fTimeDelta)
{
	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_Info(INFO_POS, _vec3(m_fX, -m_fY, 0.f));

	m_pulsePhase += m_pulseSpeed * fTimeDelta;
	if (fabsf(m_pulsePhase) > 100000.f)
		m_pulsePhase = fmodf(m_pulsePhase, 2000.f);

	return CUI::Update_GameObject(fTimeDelta);
}

void CHeartUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CUI::LateUpdate_GameObject(fTimeDelta);
}

void CHeartUI::Render_GameObject()
{
	LPDIRECT3DSTATEBLOCK9 pStateBlock = nullptr;
	if (SUCCEEDED(m_pGraphicDev->CreateStateBlock(D3DSBT_ALL, &pStateBlock)))
		pStateBlock->Capture();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x01);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

	RenderBaseline();
	RenderPulse();
	RenderHearts();

	if (m_pTexHeartL) m_pTexHeartL->MoveFrame();
	if (m_pTexHeartR) m_pTexHeartR->MoveFrame();

	if (pStateBlock) { pStateBlock->Apply(); pStateBlock->Release(); }
}

void CHeartUI::RenderBaseline()
{
	if (!m_pTexLine) return;

	const _float h = max(1.f, m_lineHeightPx);
	const _float y = (-m_fY) + m_lineOffY;

	CUI::Change_Component(L"Com_Texture_Heart_LINE", (CComponent**)&m_pTextureCom);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, m_lineTint);

	m_pTextureCom->Set_Texture(0);
	m_pTransformCom->Set_Scale(m_lineXR - m_lineXL, h, 1.f);
	m_pTransformCom->Set_Info(INFO_POS, _vec3(m_fX + (m_lineXL + m_lineXR) * 0.5f, y, 0.f));
	CUI::Render_GameObject();
}

void CHeartUI::RenderPulse()
{
	if (!m_pulseEnabled || !m_pTexPulse) return;

	const _float pw = max(4.f, m_pulseWidthPx);
	const _float ph = max(2.f, m_pulseHeightPx);
	const _float trackY = (-m_fY) + (m_beatYOverride ? m_beatY : m_pulseOffY);
	const _float W = m_beatXR - m_beatXL;
	const _float travel = W + pw;

	_float p = fmodf((m_pulseSpeed >= 0.f ? m_pulsePhase : (travel + fmodf(m_pulsePhase, travel))), travel);
	_float cx = m_fX + m_beatXL + p - pw * 0.5f;

	CUI::Change_Component(L"Com_Texture_Heart_PULSE", (CComponent**)&m_pTextureCom);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, m_pulseTint);

	m_pTextureCom->Set_Texture(0);
	m_pTransformCom->Set_Scale(pw, ph, 1.f);
	m_pTransformCom->Set_Info(INFO_POS, _vec3(cx, trackY, 0.f));
	CUI::Render_GameObject();
}

void CHeartUI::RenderHearts()
{
	if (!m_pTexHeartL) return;

	const _float s = max(4.f, m_heartSizePx);
	const _float cy = (-m_fY) + m_heartPosY;

	m_pTexHeartL->Set_Texture(m_pTexHeartL->Get_Frame().m_iCurrentTex);
	m_pTransformCom->Set_Scale(s, s, 1.f);
	m_pTransformCom->Set_Info(INFO_POS, _vec3(m_fX + m_heartPosLX, cy, 0.f));
	CUI::Render_GameObject();

	CTexture *pR = (m_pTexHeartR ? m_pTexHeartR : m_pTexHeartL);
	pR->Set_Texture(pR->Get_Frame().m_iCurrentTex);
	m_pTransformCom->Set_Info(INFO_POS, _vec3(m_fX + m_heartPosRX, cy, 0.f));
	CUI::Render_GameObject();
}


#pragma region 배치 함수
void CHeartUI::SetHeartSizePx(_float px) { m_heartSizePx = max(4.f, px); }
void CHeartUI::SetHeartManual(_float xLeft, _float xRight, _float y) { m_heartPosLX = xLeft; m_heartPosRX = xRight; m_heartPosY = y; }
void CHeartUI::SetLineHeightPx(_float px) { m_lineHeightPx = max(1.f, px); }
void CHeartUI::SetLineYOffset(_float py) { m_lineOffY = py; }
void CHeartUI::SetLineRangePx(_float xL, _float xR) { m_lineXL = xL; m_lineXR = xR; }
void CHeartUI::SetLineTint(D3DCOLOR tint) { m_lineTint = tint; }
void CHeartUI::SetPulseStyle(_float w, _float h, _float yOff, D3DCOLOR tint) { m_pulseWidthPx = max(4.f, w); m_pulseHeightPx = max(2.f, h); m_pulseOffY = yOff; m_pulseTint = tint; }
void CHeartUI::SetPulseSpeed(_float v) { m_pulseSpeed = v; }
void CHeartUI::SetPulseStartOffsetPx(_float px) { m_pulsePhase = px; }
void CHeartUI::SetBeatTrackPx(_float xL, _float xR) { m_beatXL = xL; m_beatXR = xR; }
void CHeartUI::SetBeatYOffset(_float py) { m_beatY = py; m_beatYOverride = true; }
#pragma endregion 배치 함수

HRESULT CHeartUI::Texture_Clone()
{
	CTexture::TEXINFO iL{ 0, 5, 3.f, true };
	if (FAILED(Add_Components(L"Com_Texture_Heart_L", SCENE_STATIC, L"Prototype_Component_Texture_HeartUI", (CComponent**)&m_pTexHeartL, &iL)))
		return E_FAIL;

	CTexture::TEXINFO iR{ 0, 5, 6.f, true };
	if (FAILED(Add_Components(L"Com_Texture_Heart_R", SCENE_STATIC, L"Prototype_Component_Texture_HeartUI_B", (CComponent**)&m_pTexHeartR, &iR)))
		m_pTexHeartR = nullptr;

	CTexture::TEXINFO iLine{ 0, 0, 0.f, true };
	if (FAILED(Add_Components(L"Com_Texture_Heart_LINE", SCENE_STATIC, L"Prototype_Component_Texture_HeartUI_LINE", (CComponent**)&m_pTexLine, &iLine)))
		m_pTexLine = nullptr;

	CTexture::TEXINFO iPulse{ 0, 0, 0.f, true };
	if (FAILED(Add_Components(L"Com_Texture_Heart_PULSE", SCENE_STATIC, L"Prototype_Component_Texture_HeartUI_BEAT", (CComponent**)&m_pTexPulse, &iPulse)))
		m_pTexPulse = nullptr;

	return S_OK;
}

CHeartUI* CHeartUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CHeartUI* pInstance = new CHeartUI(pGraphicDev);
	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CHeartUI Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHeartUI::Clone(void* pArg)
{
	CHeartUI* pInstance = new CHeartUI(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CHeartUI Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHeartUI::Free()
{
	CUI::Free();
}