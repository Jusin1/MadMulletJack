#include "pch.h"
#include "CEffectUI.h"
#include "CFontMgr.h"
#include "CCamera.h"
#include "CTransform.h"

CEffectUI* CEffectUI::s_pMain = nullptr;

CEffectUI::CEffectUI(LPDIRECT3DDEVICE9 pGraphicDev)
    : CUI(pGraphicDev)
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
    , m_iconW(rhs.m_iconW), m_iconH(rhs.m_iconH), m_iconOff(rhs.m_iconOff), m_hasIcon(rhs.m_hasIcon)
    , m_fadeOutDur(rhs.m_fadeOutDur), m_blinkSpeed(rhs.m_blinkSpeed)
    , m_bannerExtraW(rhs.m_bannerExtraW), m_bannerAngleDeg(rhs.m_bannerAngleDeg)
    , m_pBgBufferCom(nullptr)
    , m_mapTextures(rhs.m_mapTextures)
    , m_pAnchorTr(rhs.m_pAnchorTr)
    , m_lastWorld(rhs.m_lastWorld)
    , m_worldYOffset(rhs.m_worldYOffset)
    , m_risePixels(rhs.m_risePixels)
    , m_followSpeed(rhs.m_followSpeed)
    , m_screenStartY(rhs.m_screenStartY)
    , m_emDigits(rhs.m_emDigits)
    , m_emScale(rhs.m_emScale)
    , m_RightText(rhs.m_RightText)
    , m_textAngleDeg(rhs.m_textAngleDeg)
    , m_textOffX(rhs.m_textOffX), m_textOffY(rhs.m_textOffY)
    , m_bannerDownSpeed(rhs.m_bannerDownSpeed)
    , m_bannerLifeAdd(rhs.m_bannerLifeAdd)
    , m_bannerShowIcon(rhs.m_bannerShowIcon)
    , m_labelPopEnabled(rhs.m_labelPopEnabled)
    , m_labelPopStart(rhs.m_labelPopStart)
    , m_labelPopDur(rhs.m_labelPopDur)
    , m_rightFixedScale(rhs.m_rightFixedScale)
    , m_rightUseFixedScale(rhs.m_rightUseFixedScale)
    , m_linkTextAngleToBanner(rhs.m_linkTextAngleToBanner)
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

    const float denom = (m_totalLife > 1e-4f) ? m_totalLife : 1e-4f;
    float t = 1.f - (m_fRemain / denom);
    t = (t < 0.f) ? 0.f : (t > 1.f) ? 1.f : t;
    m_progress = EaseExpoOut(t);

    if (m_mode == SCREEN_FOLLOW)
    {
        _vec3 w = m_lastWorld;
        if (m_pAnchorTr) {
            w = m_pAnchorTr->Get_Info(INFO_POS);
            m_lastWorld = w;
        }
        w.y += m_worldYOffset;

        float sx = m_cx, syTmp = m_cy;
        ProjectWorldToScreen(w, sx, syTmp);

        float targetX = sx;
        float targetY = m_screenStartY - (m_risePixels * m_progress);

        D3DVIEWPORT9 vp{}; m_pGraphicDev->GetViewport(&vp);
        const float marginX = (m_boxW > 1.f ? m_boxW * 0.5f : 40.f) + 8.f;
        const float marginY = (m_boxH > 1.f ? m_boxH * 0.5f : 20.f) + 8.f;
        targetX = min(max(targetX, vp.X + marginX), vp.X + vp.Width - marginX);
        targetY = min(max(targetY, vp.Y + marginY), vp.Y + vp.Height - marginY);

        const float k = 1.f - expf(-m_followSpeed * dt);
        m_cx += (targetX - m_cx) * k;
        m_cy += (targetY - m_cy) * k;

        if (m_cy > targetY) m_cy = targetY;
    }
    else if (m_mode == FLOAT_MOVE)
    {
        m_cx = m_startX + (m_endX - m_startX) * m_progress;
        m_cy = m_startY + (m_endY - m_startY) * m_progress;
    }
    else if (m_mode == BANNER_FIXED)
    {
        m_cy += m_bannerDownSpeed * dt;
    }

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

        CFontMgr* fm = CFontMgr::GetInstance();
        D3DXCOLOR col = m_FontColor; col.a *= aMul;

        const float baseScale = m_scaleStart + (m_scaleEnd - m_scaleStart) * m_progress;

        auto isEm = [&](wchar_t ch)->bool {
            return (!m_emDigits.empty() && m_emDigits.find(ch) != std::wstring::npos);
            };
        auto mchar = [&](wchar_t ch, float sc)->_vec2 {
            wchar_t one[2] = { ch,0 };
            _vec2 zs{ 0,0 }, zb{ 0,0 };
            if (fm->Measure_Scaled(m_FontTag.c_str(), one, &zs, sc)) return zs;
            if (fm->Measure(m_FontTag.c_str(), one, &zb)) return _vec2(zb.x * sc, zb.y * sc);
            float h = m_boxH * 0.6f * sc; return _vec2(h * 0.6f, h);
            };
        auto strW = [&](const std::wstring& s, float sc, bool applyEm)->float {
            float w = 0.f;
            for (wchar_t ch : s) {
                float sc2 = sc * ((applyEm && isEm(ch)) ? m_emScale : 1.f);
                w += mchar(ch, sc2).x;
            }
            return w;
            };

        float labelScale = baseScale;
        if (m_labelPopEnabled) {
            float elapsed = m_totalLife - m_fRemain;                    
            float tpop = min(1.f, elapsed / max(0.001f, m_labelPopDur));
            float popK = 1.f + (m_labelPopStart - 1.f) * (1.f - EaseExpoOut(tpop));
            labelScale *= popK;
        }

        const float leftEdge = cx - m_boxW * 0.5f;
        const float drawCy = cy + m_textOffY;

        float labelW = strW(m_Text, labelScale, true);
        {
            float x = leftEdge + m_padL + m_textOffX;
            for (wchar_t ch : m_Text) {
                float sc = labelScale * (isEm(ch) ? m_emScale : 1.f);
                _vec2 sz = mchar(ch, sc);
                wchar_t one[2] = { ch,0 };
                _vec2 center{ floorf(x + sz.x * 0.5f + 0.5f), drawCy };
                fm->Render_Font_ScaledCenteredRot(m_FontTag.c_str(), one, &center, col, sc, m_textAngleDeg);
                x += sz.x;
            }
        }

        float afterX = leftEdge + m_padL + labelW;
        if (m_bannerShowIcon && m_pTextureCom && m_pVIBufferCom)
        {
            const float baseX = (labelW + m_gap);
            const float keepX = m_iconOff.x;
            const float keepY = m_iconOff.y;

            m_iconOff.x = baseX + keepX;
            m_iconOff.y = keepY; 

            RenderIcon(cx, cy, aMul, 1.0f);
            m_iconOff.x = keepX;
            m_iconOff.y = keepY;

            afterX += (m_gap + m_iconW);
        }
        if (!m_RightText.empty()) {
            // 오른쪽 텍스트에도 팝업 효과(labelScale) 적용
            const float rScale = m_rightUseFixedScale ? m_rightFixedScale : labelScale;

            float x = afterX + m_gap + m_textOffX;
            for (wchar_t ch : m_RightText) {
                float sc = rScale * (isEm(ch) ? m_emScale : 1.f);
                _vec2 sz = mchar(ch, sc);
                wchar_t one[2] = { ch,0 };
                _vec2 center{ floorf(x + sz.x * 0.5f + 0.5f), drawCy };
                fm->Render_Font_ScaledCenteredRot(m_FontTag.c_str(), one, &center, col, sc, m_textAngleDeg);
                x += sz.x;
            }
        }

        if (pSB) { pSB->Apply(); pSB->Release(); }
        return;
    }
    else
    {
        if (boxW > 0.01f && boxH > 0.01f && m_bgAlpha > 0.f)
            RenderBox(m_cx, m_cy, boxW, boxH, m_bgAlpha * aMul, 0.f);

        if (m_hasIcon) RenderIcon(m_cx, m_cy, aMul, 1.0f);

        const float leftXBase = m_cx - boxW * 0.4f + m_padL + (m_hasIcon ? (m_iconW + m_gap) : 0.f);
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
    D3DXMatrixRotationZ(&R, D3DXToRadian(angleDeg));
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

    auto isEm = [&](wchar_t ch)->bool {
        return (!m_emDigits.empty() && m_emDigits.find(ch) != std::wstring::npos);
        };
    auto measureChar = [&](wchar_t ch, float scale)->_vec2 {
        wchar_t one[2] = { ch, 0 };
        _vec2 szScaled{ 0.f,0.f }, szBase{ 0.f,0.f };
        if (fm->Measure_Scaled(m_FontTag.c_str(), one, &szScaled, scale)) return szScaled;
        if (fm->Measure(m_FontTag.c_str(), one, &szBase)) return _vec2(szBase.x * scale, szBase.y * scale);
        float h = m_boxH * 0.6f * scale; return _vec2(h * 0.6f, h);
        };

    if (m_mode == BANNER_FIXED) return; 

    const float baseScale = m_scaleStart + (m_scaleEnd - m_scaleStart) * m_progress;

    _vec2 szBase{ 0.f,0.f }, szScaled{ 0.f,0.f };
    float lineH = 0.f;
    if (fm->Measure_Scaled(m_FontTag.c_str(), m_Text.c_str(), &szScaled, baseScale)) lineH = szScaled.y;
    else if (fm->Measure(m_FontTag.c_str(), m_Text.c_str(), &szBase)) lineH = szBase.y * baseScale;
    else lineH = m_boxH * 0.6f * baseScale;

    float ty = centerY - (lineH * 0.5f);

    if (m_boxW <= 0.01f) {
        float totalW = 0.f;
        for (wchar_t ch : m_Text) {
            float sc = baseScale * (isEm(ch) ? m_emScale : 1.f);
            totalW += measureChar(ch, sc).x;
        }
        leftX -= totalW * 0.5f;
    }

    float x = leftX;
    for (wchar_t ch : m_Text) {
        float sc = baseScale * (isEm(ch) ? m_emScale : 1.f);
        _vec2 sz = measureChar(ch, sc);
        wchar_t one[2] = { ch, 0 };
        _vec2 pos{ x, ty };
        fm->Render_Font_Scaled(m_FontTag.c_str(), one, &pos, col, sc);
        x += sz.x;
    }
}

void CEffectUI::Show(const wchar_t* text,
    const wchar_t* texTag,
    float seconds,
    float centerX, float centerY,
    float /*angleDeg*/, float bgAlpha,
    const wchar_t* fontTag, D3DXCOLOR fontColor)
{
    m_mode = FLOAT_MOVE;

    m_Text = text ? text : L"";
    m_FontTag = fontTag ? fontTag : L"Font_UI_ROUGH";
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

    m_pAnchorTr = nullptr;
    m_screenStartY = m_cy;

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
    m_FontTag = fontTag ? fontTag : L"Font_UI_ROUGH";
    m_FontColor = fontColor;
    m_bgAlpha = bgAlpha;

    m_bannerAngleDeg = angleDeg;
    if (m_linkTextAngleToBanner) m_textAngleDeg = angleDeg;

    _vec2 lsz{ 0,0 };
    float baseTextW = 120.f, baseTextH = 26.f;
    if (CFontMgr::GetInstance()->Measure(m_FontTag.c_str(), m_Text.c_str(), &lsz)) {
        baseTextW = lsz.x; baseTextH = max(20.f, lsz.y);
    }

    _vec2 rsz{ 0,0 }; float rightW = 0.f, rightH = 0.f;
    if (!m_RightText.empty() &&
        CFontMgr::GetInstance()->Measure(m_FontTag.c_str(), m_RightText.c_str(), &rsz)) {
        float rScale = (m_rightUseFixedScale ? m_rightFixedScale : 1.f);
        rightW = rsz.x * rScale * (m_emDigits.empty() ? 1.f : m_emScale);
        rightH = rsz.y * rScale;
    }

    const bool hasIcon = (m_bannerShowIcon && m_pTextureCom && m_pVIBufferCom);

    const float padY = 6.f;
    m_boxH = max(baseTextH * m_labelPopStart, max(rightH, hasIcon ? m_iconH : 0.f)) + padY * 2.f;

    float iconBlockW = hasIcon ? (m_iconW + m_gap) : 0.f;
    m_boxW = m_padL + (baseTextW * m_labelPopStart)
        + ((!m_RightText.empty()) ? (m_gap + iconBlockW + rightW) : 0.f)
        + m_padL + m_bannerExtraW;

    m_totalLife = max(0.25f, seconds + m_bannerLifeAdd);
    m_fRemain = m_totalLife;
    m_fadeOutDur = min(0.7f, m_totalLife * 0.5f);

    m_progress = 0.f;
    m_scaleStart = scaleStart;
    m_scaleEnd = scaleEnd;

    m_cx = x; m_cy = y;

    m_pAnchorTr = nullptr;
    m_screenStartY = m_cy;

    m_bVisible = true; m_bActive = true;
}

void CEffectUI::ShowFollowTransform(const wchar_t* text,
    const wchar_t* texTag,
    float seconds,
    CTransform* pAnchorTr,
    float worldYOffset,
    float risePixels,
    float followSpeed,
    float bgAlpha,
    const wchar_t* fontTag,
    D3DXCOLOR fontColor)
{
    m_mode = SCREEN_FOLLOW;

    m_Text = text ? text : L"";
    m_FontTag = fontTag ? fontTag : L"Font_UI_ROUGH";
    m_FontColor = fontColor;
    m_bgAlpha = bgAlpha;

    if (texTag && texTag[0]) { Change_Texture(texTag); m_hasIcon = true; }
    else { m_pTextureCom = nullptr; m_hasIcon = false; }

    m_pAnchorTr = pAnchorTr;
    m_worldYOffset = worldYOffset;
    m_risePixels = risePixels;
    m_followSpeed = followSpeed;

    if (m_pAnchorTr) {
        _vec3 w = m_pAnchorTr->Get_Info(INFO_POS);
        m_lastWorld = w;
        w.y += m_worldYOffset;
        ProjectWorldToScreen(w, m_cx, m_cy);
    }
    m_screenStartY = m_cy;

    m_totalLife = max(0.25f, seconds);
    m_fRemain = m_totalLife;
    m_fadeOutDur = min(0.6f, m_totalLife * 0.5f);

    m_progress = 0.f;
    m_scaleStart = 0.70f;
    m_scaleEnd = 1.00f;

    m_bVisible = true; m_bActive = true;
}

void CEffectUI::Hide()
{
    m_bVisible = false;
    m_bActive = false;
    m_Text.clear(); m_RightText.clear();
    m_hasIcon = false;
    m_pAnchorTr = nullptr;
}

HRESULT CEffectUI::PreloadTexture(const _tchar* mapTag, const _tchar* protoTag)
{
    if (!mapTag || !protoTag) return E_FAIL;

    for (auto& kv : m_mapTextures)
        if (!lstrcmp(kv.first, mapTag)) return S_OK;

    CTexture* tex = nullptr;
    if (FAILED(Add_Components(mapTag, SCENE_STATIC, protoTag, (CComponent**)&tex)))
        return E_FAIL;

    m_mapTextures.insert({ mapTag, tex });
    return S_OK;
}

HRESULT CEffectUI::Change_Texture(const _tchar* tag)
{
    if (!tag) { m_pTextureCom = nullptr; return S_OK; }

    for (auto& kv : m_mapTextures)
        if (0 == lstrcmp(kv.first, tag)) { m_pTextureCom = kv.second; return S_OK; }

    if (SUCCEEDED(PreloadTexture(tag, tag))) {
        m_pTextureCom = m_mapTextures[tag];
        return S_OK;
    }
    return E_FAIL;
}

bool CEffectUI::ProjectWorldToScreen(const _vec3& world, float& outSX, float& outSY) const
{
    const _matrix& view = CCamera::GetView();
    const _matrix& proj = CCamera::GetProj();

    D3DVIEWPORT9 vp{}; m_pGraphicDev->GetViewport(&vp);

    D3DXVECTOR4 p(world.x, world.y, world.z, 1.f);
    D3DXVec4Transform(&p, &p, &view);
    D3DXVec4Transform(&p, &p, &proj);

    bool behind = (p.w <= 0.f);
    if (fabsf(p.w) < 1e-6f) p.w = 1e-6f;

    float ndcX = p.x / p.w;
    float ndcY = p.y / p.w;

    if (behind) { ndcX = -ndcX; ndcY = -ndcY; }

    float sx = (ndcX * 0.5f + 0.5f) * vp.Width + vp.X;
    float sy = (1.f - (ndcY * 0.5f + 0.5f)) * vp.Height + vp.Y;

    const float marginX = (m_boxW > 1.f ? m_boxW * 0.5f : 40.f) + 8.f;
    const float marginY = (m_boxH > 1.f ? m_boxH * 0.5f : 20.f) + 8.f;
    sx = min(max(sx, vp.X + marginX), vp.X + vp.Width - marginX);
    sy = min(max(sy, vp.Y + marginY), vp.Y + vp.Height - marginY);

    outSX = sx; outSY = sy;
    return true;
}

void CEffectUI::SetNumberEmphasis(const wchar_t* digits, float scale)
{
    m_emDigits = digits ? digits : L"";
    m_emScale = (scale > 0.f) ? scale : 1.f;
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