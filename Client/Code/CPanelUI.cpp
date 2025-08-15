#include "pch.h"
#include "CPanelUI.h"


CPanelUI::CPanelUI(LPDIRECT3DDEVICE9 dev)
    : CUI(dev)
    , m_mapTextures()
    , m_BaseTag(nullptr)
    , m_OverlayTag(nullptr)
    , m_BaseTint(D3DCOLOR_ARGB(255, 255, 255, 255))
    , m_OverlayTint(D3DCOLOR_ARGB(0, 255, 255, 255))
    , m_BaseAdd(false)
    , m_OverlayAdd(true)
    , m_OverlayScale(1.f)
    , m_BaseRepeatX(1.f), m_BaseRepeatY(1.f)
    , m_OvRepeatX(1.f), m_OvRepeatY(1.f)
    , m_useStroke(false)
    , m_strokePx(8.f)
    , m_strokeCol(D3DCOLOR_ARGB(255, 90, 255, 90))
    , m_pixelSnap(true)
{
}

CPanelUI::CPanelUI(const CPanelUI& rhs)
    : CUI(rhs)
    , m_mapTextures(rhs.m_mapTextures)
    , m_BaseTag(rhs.m_BaseTag), m_OverlayTag(rhs.m_OverlayTag)
    , m_BaseTint(rhs.m_BaseTint), m_OverlayTint(rhs.m_OverlayTint)
    , m_BaseAdd(rhs.m_BaseAdd), m_OverlayAdd(rhs.m_OverlayAdd)
    , m_OverlayScale(rhs.m_OverlayScale)
    , m_BaseRepeatX(rhs.m_BaseRepeatX), m_BaseRepeatY(rhs.m_BaseRepeatY)
    , m_OvRepeatX(rhs.m_OvRepeatX), m_OvRepeatY(rhs.m_OvRepeatY)
    , m_useStroke(rhs.m_useStroke), m_strokePx(rhs.m_strokePx), m_strokeCol(rhs.m_strokeCol)
    , m_pixelSnap(rhs.m_pixelSnap)
{
}

CPanelUI::~CPanelUI() {}

HRESULT CPanelUI::Ready_GameObject()
{
    return __super::Ready_GameObject();
}

HRESULT CPanelUI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg))) return E_FAIL;
    if (FAILED(Texture_Clone())) return E_FAIL;

    SetStyle(TEXT("Com_Texture_Panel_FileGrid"), D3DCOLOR_ARGB(200, 255, 255, 255), false);
    SetUVRepeat(1.f, 1.f);
    return S_OK;
}

_int CPanelUI::Update_GameObject(const _float& dt)
{
    m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_Info(INFO_POS, _vec3(m_fX, -m_fY, 0.f));
    __super::Update_GameObject(dt);
    return NO_EVENT;
}

void CPanelUI::LateUpdate_GameObject(const _float& dt)
{
    __super::LateUpdate_GameObject(dt);
}

void CPanelUI::Render_GameObject()
{
    LPDIRECT3DSTATEBLOCK9 sb = nullptr;
    if (SUCCEEDED(m_pGraphicDev->CreateStateBlock(D3DSBT_ALL, &sb))) sb->Capture();

    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x01);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

    if (m_useStroke) {
        RenderStrokeOnly();
    }
    else {
        if (m_OverlayTag && ((m_OverlayTint & 0xFF000000u) != 0u))
            RenderLayer(m_OverlayTag, m_OverlayTint, m_OverlayAdd, m_OverlayScale, m_OvRepeatX, m_OvRepeatY);
        if (m_BaseTag)
            RenderLayer(m_BaseTag, m_BaseTint, m_BaseAdd, 1.f, m_BaseRepeatX, m_BaseRepeatY);
    }

    if (sb) { sb->Apply(); sb->Release(); }
}

void CPanelUI::RenderLayer(const _tchar* tag, D3DCOLOR tint, bool additive,
    float scaleMul, float repX, float repY)
{
    if (FAILED(__super::Change_Component(tag, (CComponent**)&m_pTextureCom))) return;

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, tint);

    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, additive ? D3DBLEND_ONE : D3DBLEND_INVSRCALPHA);

    if (repX == 1.f && repY == 1.f) {
        m_pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        m_pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
        D3DXMATRIX I; D3DXMatrixIdentity(&I);
        m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &I);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
    }
    else {
        m_pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
        m_pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
        D3DXMATRIX S; D3DXMatrixScaling(&S, repX, repY, 1.f);
        m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &S);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
    }

    m_pTextureCom->Set_Texture(0);
    m_pTransformCom->Set_Scale(m_fSizeX * scaleMul, m_fSizeY * scaleMul, 1.f);
    __super::Render_GameObject();

    D3DXMATRIX I; D3DXMatrixIdentity(&I);
    m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &I);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

void CPanelUI::EnableStroke(float thicknessPx, D3DCOLOR color)
{
    m_useStroke = true;
    m_strokePx = max(1.f, thicknessPx);
    m_strokeCol = color;
}

void CPanelUI::RenderStrokeOnly()
{
    const float W = m_fSizeX;
    const float H = m_fSizeY;
    const float t = m_strokePx;

    const float leftX = m_fX - W * 0.5f + t * 0.5f;
    const float rightX = m_fX + W * 0.5f - t * 0.5f;
    const float topY = -m_fY + H * 0.5f - t * 0.5f;
    const float bottomY = -m_fY - H * 0.5f + t * 0.5f;

    m_pGraphicDev->SetTexture(0, nullptr);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);

    DrawSolid(leftX, (topY + bottomY) * 0.5f, t, H, m_strokeCol);
    DrawSolid(rightX, (topY + bottomY) * 0.5f, t, H, m_strokeCol);
    DrawSolid(m_fX, topY, W, t, m_strokeCol);
    DrawSolid(m_fX, bottomY, W, t, m_strokeCol);
}

void CPanelUI::DrawSolid(float cx, float cy, float w, float h, D3DCOLOR c)
{
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, c);
    m_pTransformCom->Set_Scale(w, h, 1.f);
    m_pTransformCom->Set_Info(INFO_POS, _vec3(cx, cy, 0.f));
    __super::Render_GameObject();
}

void CPanelUI::SetStyle(const _tchar* baseTag, D3DCOLOR baseTint, bool baseAdditive)
{
    m_BaseTag = baseTag;
    m_BaseTint = baseTint;
    m_BaseAdd = baseAdditive;
}

void CPanelUI::SetOverlay(const _tchar* overlayTag, D3DCOLOR overlayTint,
    bool overlayAdditive, float overlayScale)
{
    m_OverlayTag = overlayTag;
    m_OverlayTint = overlayTint;
    m_OverlayAdd = overlayAdditive;
    m_OverlayScale = overlayScale;
}

void CPanelUI::SetPanelSize(float w, float h) { Set_UISize(w, h); }
void CPanelUI::SetPanelPos(float x, float y) { m_fX = x; m_fY = y; }
void CPanelUI::SetUVRepeat(float x, float y) { m_BaseRepeatX = x; m_BaseRepeatY = y; }
void CPanelUI::SetOverlayUVRepeat(float x, float y) { m_OvRepeatX = x; m_OvRepeatY = y; }

void CPanelUI::UseGreenFramePreset(float x, float y, float w, float h,
    float borderOrThickness, bool useStroke)
{
    SetPanelPos(x, y);
    SetPanelSize(w, h);

    if (useStroke) {
        EnableStroke(borderOrThickness, D3DCOLOR_ARGB(255, 90, 255, 90));
        return;
    }

    SetStyle(L"Com_Texture_Panel_UIFrame", D3DCOLOR_ARGB(255, 90, 255, 90), false);
    SetUVRepeat(1.f, 1.f);
    SetOverlay(nullptr, D3DCOLOR_ARGB(0, 0, 0, 0), true, 1.f);
    SetOverlayUVRepeat(1.f, 1.f);
}

HRESULT CPanelUI::Texture_Clone()
{
    auto addTex = [&](const _tchar* tag, const _tchar* proto)->HRESULT {
        CTexture::TEXINFO info{};
        info.m_iStart = 0; info.m_iEndTex = 0; info.m_fSpeed = 0.f; info.m_bLoop = false;
        CTexture* p = nullptr;
        if (FAILED(Add_Components(tag, SCENE_STAGE, proto, (CComponent**)&p, &info))) return E_FAIL;
        m_mapTextures.insert({ tag, p });
        return S_OK;
        };

    addTex(TEXT("Com_Texture_Panel_FileGrid"), TEXT("Prototype_Component_Texture_FileGridUI"));
    addTex(TEXT("Com_Texture_Panel_UIFrame"), TEXT("Prototype_Component_Texture_FrameUI"));

    __super::Change_Component(TEXT("Com_Texture_Panel_FileGrid"), (CComponent**)&m_pTextureCom);
    return S_OK;
}

CPanelUI* CPanelUI::Create(LPDIRECT3DDEVICE9 dev)
{
    CPanelUI* p = new CPanelUI(dev);
    if (FAILED(p->Ready_GameObject()))
    {
        MSG_BOX("CPanelUI Create Failed");
        Safe_Release(p); 
    }
    return p;
}

CGameObject* CPanelUI::Clone(void* pArg)
{
    CPanelUI* p = new CPanelUI(*this);
    if (FAILED(p->Initialize(pArg))) { MSG_BOX("CPanelUI Clone Failed"); Safe_Release(p); }
    return p;
}

void CPanelUI::Free()
{
    __super::Free();
}