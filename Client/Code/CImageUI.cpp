#include "pch.h"
#include "CImageUI.h"

CImageUI::CImageUI(LPDIRECT3DDEVICE9 dev) : CUI(dev), m_bPosFix(true){}
CImageUI::CImageUI(const CImageUI& rhs)
    : CUI(rhs)
    , m_tint(rhs.m_tint), m_additive(rhs.m_additive)
    , m_repX(rhs.m_repX), m_repY(rhs.m_repY)
    , m_play(rhs.m_play), m_colorMode(rhs.m_colorMode)
    , m_curTag(rhs.m_curTag), m_bPosFix(rhs.m_bPosFix) {
}
CImageUI::~CImageUI() {}

HRESULT CImageUI::Ready_GameObject() { return __super::Ready_GameObject(); }

HRESULT CImageUI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg))) return E_FAIL;
    ChangeTexture(TEXT("Com_Texture_Image"));


    return S_OK;
}

_int CImageUI::Update_GameObject(const _float& dt)
{
    m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
    if (m_bPosFix)
    {
        m_pTransformCom->Set_Info(INFO_POS, _vec3(m_fX, -m_fY, 0.f));
    }

    else
        Update_Position(m_pTransformCom->Get_Info(INFO_POS));
    
    if (m_play && m_pTextureCom) m_pTextureCom->MoveFrame();
    __super::Update_GameObject(dt);
    return NO_EVENT;
}

void CImageUI::LateUpdate_GameObject(const _float& dt) { __super::LateUpdate_GameObject(dt); }

void CImageUI::Render_GameObject()
{
    if (!m_pTextureCom) return;

    LPDIRECT3DSTATEBLOCK9 sb = nullptr;
    if (SUCCEEDED(m_pGraphicDev->CreateStateBlock(D3DSBT_ALL, &sb)))
        sb->Capture();

    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, m_additive ? D3DBLEND_ONE : D3DBLEND_INVSRCALPHA);

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x01);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

    switch (m_colorMode)
    {
    case ColorMode::TintMultiply: 
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
        break;
    case ColorMode::TintAdd:   
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
        break;
    case ColorMode::TextureOnly:
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        break;
    case ColorMode::TintOnly:    
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
        break;
    }

    if (m_colorMode == ColorMode::TintOnly) {
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
    }
    else {
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    }
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, m_tint);

    if (m_repX == 1.f && m_repY == 1.f) {
        m_pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        m_pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
        D3DXMATRIX I; D3DXMatrixIdentity(&I);
        m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &I);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
    }
    else {
        m_pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
        m_pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
        D3DXMATRIX S; D3DXMatrixScaling(&S, m_repX, m_repY, 1.f);
        m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &S);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
    }

    m_pTextureCom->Set_Texture(m_pTextureCom->Get_Frame().m_iCurrentTex);
    __super::Render_GameObject(); 
    if (sb) { sb->Apply(); sb->Release(); }
}


HRESULT CImageUI::RegisterTexture(const _tchar* tag, const _tchar* proto,
    int start, int end, float fps, bool loop)
{
    if (!tag || !proto) return E_FAIL;
    CTexture::TEXINFO ti{}; ti.m_iStart = start; ti.m_iEndTex = end; ti.m_fSpeed = fps; ti.m_bLoop = loop;
    CTexture* p = nullptr;
    if (FAILED(Add_Components(tag, SCENE_STATIC, proto, (CComponent**)&p, &ti))) return E_FAIL;
    return S_OK;
}
HRESULT CImageUI::ChangeTexture(const _tchar* tag)
{
    if (!tag) return E_FAIL;
    if (FAILED(__super::Change_Component(tag, (CComponent**)&m_pTextureCom))) return E_FAIL;
    m_pTextureCom->Set_Zero_Frame();
    m_curTag = tag;
    return S_OK;
}

void CImageUI::SetTint(D3DCOLOR tint) { m_tint = tint; }
void CImageUI::SetTintRGBA(BYTE r, BYTE g, BYTE b, BYTE a) { m_tint = D3DCOLOR_ARGB(a, r, g, b); }
void CImageUI::SetAlpha(BYTE a)
{
    BYTE r = (BYTE)((m_tint >> 16) & 0xFF);
    BYTE g = (BYTE)((m_tint >> 8) & 0xFF);
    BYTE b = (BYTE)((m_tint) & 0xFF);
    m_tint = D3DCOLOR_ARGB(a, r, g, b);
}
void CImageUI::SetAdditive(bool on) { m_additive = on; }
void CImageUI::SetUVRepeat(float rx, float ry) { m_repX = rx; m_repY = ry; }
void CImageUI::SetColorMode(ColorMode m) { m_colorMode = m; }

void CImageUI::Play(bool on) { m_play = on; }
void CImageUI::ResetToFirstFrame() { if (m_pTextureCom) m_pTextureCom->Set_Zero_Frame(); }
// factory
CImageUI* CImageUI::Create(LPDIRECT3DDEVICE9 dev) { auto* p = new CImageUI(dev); if (FAILED(p->Ready_GameObject())) { MSG_BOX("CImageUI Create Failed"); Safe_Release(p); } return p; }
CGameObject* CImageUI::Clone(void* pArg) { auto* p = new CImageUI(*this); if (FAILED(p->Initialize(pArg))) { MSG_BOX("CImageUI Clone Failed"); Safe_Release(p); } return p; }
void CImageUI::Free() { __super::Free(); }