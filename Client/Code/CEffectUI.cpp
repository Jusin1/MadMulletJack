#include "pch.h"
#include "CEffectUI.h"
#include "CFontMgr.h"

CEffectUI* CEffectUI::s_pMain = nullptr;

CEffectUI::CEffectUI(LPDIRECT3DDEVICE9 pGraphicDev)
    : CUI(pGraphicDev)
    , m_mode(FLOAT_MOVE)
    , m_bVisible(false)
    , m_time(0.f)
    , m_totalLife(0.f)
    , m_fRemain(0.f)
    , m_cx(WINCX * 0.5f), m_cy(WINCY * 0.33f)
    , m_startX(0.f), m_startY(0.f), m_endX(0.f), m_endY(0.f)
    , m_progress(0.f)
    , m_scaleStart(0.70f), m_scaleEnd(1.00f)
    , m_dirX(-1)
    , m_liftMin(120.f), m_liftMax(220.f)
    , m_targetLeftX(100.f), m_targetRightX(1200.f)
    , m_moveSpeed(0.f), m_useSpeed(false)
    , m_bgAlpha(0.85f)
    , m_boxW(100.f), m_boxH(30.f)
    , m_padL(10.f), m_gap(6.f)
    , m_Text(), m_FontTag(L"DefaultFont"), m_FontColor(D3DXCOLOR(1, 1, 1, 1))
    , m_iconW(18.f), m_iconH(18.f)
    , m_iconOff{ 0.f, 0.f }
    , m_hasIcon(false)
    , m_fadeOutDur(0.35f), m_blinkSpeed(18.f)
    , m_bannerExtraW(120.f)
    , m_bannerAngleDeg(-10.f)
    , m_pBgBufferCom(nullptr)
    , m_mapTextures()
{
}

CEffectUI::CEffectUI(const CEffectUI& rhs)
    : CUI(rhs)
    , m_mode(rhs.m_mode)
    , m_bVisible(rhs.m_bVisible)
    , m_time(rhs.m_time)
    , m_totalLife(rhs.m_totalLife)
    , m_fRemain(rhs.m_fRemain)
    , m_cx(rhs.m_cx), m_cy(rhs.m_cy)
    , m_startX(rhs.m_startX), m_startY(rhs.m_startY)
    , m_endX(rhs.m_endX), m_endY(rhs.m_endY)
    , m_progress(rhs.m_progress)
    , m_scaleStart(rhs.m_scaleStart), m_scaleEnd(rhs.m_scaleEnd)
    , m_dirX(rhs.m_dirX)
    , m_liftMin(rhs.m_liftMin), m_liftMax(rhs.m_liftMax)
    , m_targetLeftX(rhs.m_targetLeftX), m_targetRightX(rhs.m_targetRightX)
    , m_moveSpeed(rhs.m_moveSpeed), m_useSpeed(rhs.m_useSpeed)
    , m_bgAlpha(rhs.m_bgAlpha)
    , m_boxW(rhs.m_boxW), m_boxH(rhs.m_boxH)
    , m_padL(rhs.m_padL), m_gap(rhs.m_gap)
    , m_Text(rhs.m_Text), m_FontTag(rhs.m_FontTag), m_FontColor(rhs.m_FontColor)
    , m_iconW(rhs.m_iconW), m_iconH(rhs.m_iconH)
    , m_iconOff(rhs.m_iconOff)
    , m_hasIcon(rhs.m_hasIcon)
    , m_fadeOutDur(rhs.m_fadeOutDur), m_blinkSpeed(rhs.m_blinkSpeed)
    , m_bannerExtraW(rhs.m_bannerExtraW)
    , m_bannerAngleDeg(rhs.m_bannerAngleDeg)
    , m_pBgBufferCom(nullptr)
    , m_mapTextures(rhs.m_mapTextures)
{
}

CEffectUI::~CEffectUI() {}

HRESULT CEffectUI::Ready_GameObject()
{
    if (FAILED(__super::Ready_GameObject())) return E_FAIL;
    return S_OK;
}

HRESULT CEffectUI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg))) return E_FAIL;

    if (FAILED(Add_Components(L"Com_BackGroundBuffer", SCENE_STATIC, L"Proto_Color_Buffer",
        (CComponent**)&m_pBgBufferCom)))
        return E_FAIL;

    PreloadTexture(L"Com_Tex_Heal", L"Prototype_Component_Texture_HealEffect");
    m_targetRightX = min(1200.f, WINCX - 20.f);

    s_pMain = this;
    return S_OK;
}

_int CEffectUI::Update_GameObject(const _float& dt)
{
    __super::Update_GameObject(dt);
    if (!m_bVisible) return NO_EVENT;

    m_time += dt;
    m_fRemain -= dt;
    if (m_fRemain <= 0.f) { Hide(); return NO_EVENT; }

    const float denom = (m_totalLife > 0.0001f) ? m_totalLife : 0.0001f;
    float t = 1.f - (m_fRemain / denom);
    t = (t < 0.f) ? 0.f : (t > 1.f) ? 1.f : t;

    m_progress = EaseExpoOut(t);

    m_cx = m_startX + (m_endX - m_startX) * m_progress;
    m_cy = m_startY + (m_endY - m_startY) * m_progress;

    return NO_EVENT;
}

void CEffectUI::LateUpdate_GameObject(const _float& dt)
{
    __super::LateUpdate_GameObject(dt);
}

void CEffectUI::Render_GameObject()
{
    if (!m_bVisible) return;

    IDirect3DStateBlock9* pSB = nullptr;
    if (SUCCEEDED(m_pGraphicDev->CreateStateBlock(D3DSBT_ALL, &pSB))) pSB->Capture();

    _matrix V; D3DXMatrixIdentity(&V);
    m_pGraphicDev->SetTransform(D3DTS_VIEW, &V);
    m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

    float fade = 1.f;
    if (m_fRemain < m_fadeOutDur) fade = max(0.f, m_fRemain / m_fadeOutDur);
    const float blink = (m_fRemain < m_fadeOutDur)
        ? (0.4f + 0.6f * 0.5f * (1.f + sinf(m_time * m_blinkSpeed)))
        : 1.f;
    const float aMul = fade * blink;

    const float boxW = m_boxW;
    const float boxH = m_boxH;

    if (m_mode == BANNER_FIXED)
    {
        const float cx = floorf(m_cx + 0.5f);
        const float cy = floorf(m_cy + 0.5f);

        if (boxW > 0.01f && boxH > 0.01f && m_bgAlpha > 0.f)
            RenderBox(cx, cy, boxW, boxH, m_bgAlpha * aMul, m_bannerAngleDeg);

        if (m_hasIcon) RenderIcon(cx, cy, aMul, 1.0f);

        RenderText(0.f, cy, aMul); 
    }
    else
    {
        if (boxW > 0.01f && boxH > 0.01f && m_bgAlpha > 0.f)
            RenderBox(m_cx, m_cy, boxW, boxH, m_bgAlpha * aMul, 0.f);

        if (m_hasIcon) RenderIcon(m_cx, m_cy, aMul, 1.0f);

        const float leftXBase = m_cx - boxW * 0.5f + m_padL + (m_hasIcon ? (m_iconW + m_gap) : 0.f);
        RenderText(leftXBase, m_cy, aMul);
    }

    if (pSB) { pSB->Apply(); pSB->Release(); }
}

void CEffectUI::RenderBox(float cx, float cy, float w, float h, float alpha, float angleDeg)
{
    if (!m_pBgBufferCom) return;

    m_pBgBufferCom->SetColor(D3DXCOLOR(0.f, 0.f, 0.f, alpha));
    const float wx = -WINCX * 0.5f + cx - 0.5f;
    const float wy = WINCY * 0.5f - cy + 0.5f;

    _matrix S, R, T, W;
    D3DXMatrixScaling(&S, w * 0.5f, h * 0.5f, 1.f);
    D3DXMatrixRotationZ(&R, D3DXToRadian(angleDeg)); // 배경은 화면 각도를 그대로 사용
    D3DXMatrixTranslation(&T, wx, wy, 0.f);

    W = S * R * T;
    m_pGraphicDev->SetTransform(D3DTS_WORLD, &W);

    m_pGraphicDev->SetTexture(0, nullptr);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_pBgBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CEffectUI::RenderIcon(float cx, float cy, float aMul, float /*scale*/)
{
    if (!m_pVIBufferCom || !m_pTextureCom) { m_hasIcon = false; return; }
    m_hasIcon = true;

    const float iconW = m_iconW;
    const float iconH = m_iconH;
    const float padL = m_padL;
    const float boxW = m_boxW;

    const float iconCx = cx - boxW * 0.5f + padL + iconW * 0.5f + m_iconOff.x;
    const float iconCy = cy + m_iconOff.y;

    const float wx = -WINCX * 0.5f + iconCx;
    const float wy = WINCY * 0.5f - iconCy;

    _matrix S, T, W;
    D3DXMatrixScaling(&S, iconW, iconH, 1.f);
    D3DXMatrixTranslation(&T, wx, wy, 0.f);
    W = S * T;
    m_pGraphicDev->SetTransform(D3DTS_WORLD, &W);

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

    const BYTE a = (BYTE)(max(0.f, min(1.f, aMul)) * 255.f);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(a, 255, 255, 255));

    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_pTextureCom->Set_Texture(0);
    m_pVIBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CEffectUI::RenderText(float leftX, float centerY, float aMul)
{
    if (m_Text.empty()) return;

    CFontMgr* fm = CFontMgr::GetInstance();
    D3DXCOLOR col = m_FontColor; col.a *= aMul;

    if (m_mode == BANNER_FIXED)
    {
        float s = m_progress;                       // 0..1
        float tri = (s <= 0.5f) ? (s * 2.f) : (2.f - s * 2.f);
        tri = max(0.f, min(1.f, tri));
        const float fontScale = m_scaleEnd + (m_scaleStart - m_scaleEnd) * tri;

        _vec2 center{ floorf(m_cx + 0.5f), floorf(centerY + 0.5f) };
        fm->Render_Font_ScaledCenteredRot(
            m_FontTag.c_str(), m_Text.c_str(), &center, col, fontScale, -m_bannerAngleDeg);
        return;
    }

    // FLOAT_MOVE: 좌상단 기준 선형 스케일
    const float fontScale = m_scaleStart + (m_scaleEnd - m_scaleStart) * m_progress;

    _vec2 szBase{ 0.f, 0.f }, szScaled{ 0.f, 0.f };
    float textH = 0.f;
    if (fm->Measure_Scaled(m_FontTag.c_str(), m_Text.c_str(), &szScaled, fontScale))
        textH = szScaled.y;
    else if (fm->Measure(m_FontTag.c_str(), m_Text.c_str(), &szBase))
        textH = szBase.y * fontScale;
    else
        textH = m_boxH * 0.6f * fontScale;

    float ty = centerY - (textH * 0.5f);

    if (m_boxW <= 0.01f) {
        float textW = (szScaled.x > 0.f) ? szScaled.x : (szBase.x * fontScale);
        leftX -= textW * 0.5f;
    }

    _vec2 pos{ leftX, ty };
    fm->Render_Font_Scaled(m_FontTag.c_str(), m_Text.c_str(), &pos, col, fontScale);
}

void CEffectUI::Show(const wchar_t* text,
    const wchar_t* texTag,
    float seconds,
    float centerX, float centerY,
    float angleDeg, float bgAlpha,
    const wchar_t* fontTag, D3DXCOLOR fontColor)
{
    m_mode = FLOAT_MOVE;

    m_Text = text ? text : L"";
    m_FontTag = fontTag ? fontTag : L"DefaultFont";
    m_FontColor = fontColor;
    m_bgAlpha = bgAlpha;

    if (texTag && texTag[0]) { Change_Texture(texTag); m_hasIcon = true; }
    else { m_pTextureCom = nullptr; m_hasIcon = false; }

    m_startX = centerX;  m_startY = centerY;
    m_dirX = m_hasIcon ? ((rand() & 1) ? +1 : -1) : -1;

    const float targetX = (m_dirX > 0) ? m_targetRightX : max(m_targetLeftX, 20.f);

    auto frand = [](float a, float b) { return a + (b - a) * (float)(rand() % 1000) * 0.001f; };
    const float lift = frand(m_liftMin, m_liftMax);

    m_endX = targetX;
    m_endY = m_startY - lift;

    const float dx = (m_endX - m_startX);
    const float dy = (m_endY - m_startY);
    const float dist = sqrtf(dx * dx + dy * dy);

    m_totalLife = (m_useSpeed && m_moveSpeed > 1.f) ? max(0.15f, dist / m_moveSpeed)
        : max(0.25f, seconds);
    m_fRemain = m_totalLife;
    m_fadeOutDur = min(0.6f, m_totalLife * 0.6f);

    m_progress = 0.f;
    m_scaleStart = 0.70f;
    m_scaleEnd = 1.00f;

    m_cx = m_startX; m_cy = m_startY;
    m_bVisible = true; m_bActive = true;
}

void CEffectUI::ShowBanner(const wchar_t* text,
    float seconds,
    float x, float y,
    float scaleStart, float scaleEnd,
    const wchar_t* fontTag, D3DXCOLOR fontColor,
    float bgAlpha,
    float angleDeg)
{
    m_mode = BANNER_FIXED;

    m_Text = text ? text : L"";
    m_FontTag = fontTag ? fontTag : L"DefaultFont";
    m_FontColor = fontColor;
    m_bgAlpha = bgAlpha;

    m_hasIcon = false;

    m_bannerAngleDeg = angleDeg; 

    _vec2 sz{ 0.f, 0.f };
    float textW = 120.f, textH = 26.f;
    if (CFontMgr::GetInstance()->Measure(m_FontTag.c_str(), m_Text.c_str(), &sz)) {
        textW = sz.x; textH = max(20.f, sz.y);
    }
    const float padY = 6.f;
    m_boxH = textH + padY * 2.f;
    m_boxW = textW + m_padL * 2.f + m_bannerExtraW;

    m_startX = x; m_startY = y;
    m_endX = x; m_endY = y;

    m_totalLife = max(0.25f, seconds);
    m_fRemain = m_totalLife;
    m_fadeOutDur = min(0.6f, m_totalLife * 0.5f);

    m_progress = 0.f;

    m_scaleStart = scaleStart;
    m_scaleEnd = scaleEnd;   

    m_cx = m_startX; m_cy = m_startY;
    m_bVisible = true; m_bActive = true;
}

void CEffectUI::Hide()
{
    m_bVisible = false;
    m_bActive = false;
    m_Text.clear();
    m_hasIcon = false;
}

HRESULT CEffectUI::PreloadTexture(const _tchar* mapTag, const _tchar* protoTag)
{
    if (!mapTag || !protoTag) return E_FAIL;

    for (auto& kv : m_mapTextures)
        if (!lstrcmp(kv.first, mapTag)) return S_OK;

    CTexture* tex = nullptr;
    if (FAILED(Add_Components(mapTag, SCENE_STAGE, protoTag, (CComponent**)&tex)))
        return E_FAIL;

    m_mapTextures.insert({ mapTag, tex });
    return S_OK;
}

HRESULT CEffectUI::Change_Texture(const _tchar* tag)
{
    if (!tag) { m_pTextureCom = nullptr; return S_OK; }

    for (auto& kv : m_mapTextures)
        if (0 == lstrcmp(kv.first, tag)) { m_pTextureCom = kv.second; return S_OK; }

    if (SUCCEEDED(PreloadTexture(tag, tag))) { m_pTextureCom = m_mapTextures[tag]; return S_OK; }
    return E_FAIL;
}

CEffectUI* CEffectUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CEffectUI* p = new CEffectUI(pGraphicDev);
    if (FAILED(p->Ready_GameObject())) { MSG_BOX("CEffectUI Create Failed"); Safe_Release(p); }
    return p;
}

CGameObject* CEffectUI::Clone(void* pArg)
{
    CEffectUI* p = new CEffectUI(*this);
    if (FAILED(p->Initialize(pArg))) { MSG_BOX("CEffectUI Clone Failed"); Safe_Release(p); }
    return p;
}

void CEffectUI::Free()
{
    __super::Free();
}