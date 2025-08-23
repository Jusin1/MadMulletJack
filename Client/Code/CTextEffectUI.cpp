#include "pch.h"
#include "CTextEffectUI.h"
#include "CImageUI.h"
#include "CTextUI.h"
#include "CObjectManager.h"
#include "CManagement.h"

static inline D3DCOLOR ToD3DCOLOR(const D3DXCOLOR& c)
{
    return D3DCOLOR_COLORVALUE(c.r, c.g, c.b, c.a);
}

// ===== ctor/dtor (이니셜라이저 리스트) =====
CTextEffectUI::CTextEffectUI(LPDIRECT3DDEVICE9 dev)
    : CUI(dev)
    , m_imgs()
    , m_label(nullptr)
    , m_rainbowOn(false)
    , m_interval(0.2f)
    , m_timer(0.f)
    , m_idx(0)
    , m_moveOn(false)
    , m_moveT(0.f)
    , m_moveDur(0.40f)
    , m_moveStartX(0.f), m_moveStartY(0.f)
    , m_moveEndX(0.f), m_moveEndY(0.f)
    , m_textBaseScale(1.00f)
    , m_autoKill(true)    
    , m_lifeSec(5.f)    
    , m_lifeT(0.f)
{
}

CTextEffectUI::CTextEffectUI(const CTextEffectUI& rhs)
    : CUI(rhs)
    , m_imgs()
    , m_label(nullptr)
    , m_rainbowOn(rhs.m_rainbowOn)
    , m_interval(rhs.m_interval)
    , m_timer(0.f)
    , m_idx(rhs.m_idx)
    , m_moveOn(false)
    , m_moveT(0.f)
    , m_moveDur(rhs.m_moveDur)
    , m_moveStartX(0.f), m_moveStartY(0.f)
    , m_moveEndX(0.f), m_moveEndY(0.f)
    , m_textBaseScale(rhs.m_textBaseScale)
    , m_autoKill(rhs.m_autoKill)  
    , m_lifeSec(rhs.m_lifeSec) 
    , m_lifeT(0.f)             
{
}

CTextEffectUI::~CTextEffectUI() {}

HRESULT CTextEffectUI::Ready_GameObject() { return S_OK; }

CTextEffectUI* CTextEffectUI::Create(LPDIRECT3DDEVICE9 dev)
{
    auto* p = new CTextEffectUI(dev);
    if (FAILED(p->Ready_GameObject())) { MSG_BOX("CTextEffectUI Create Failed"); Safe_Release(p); }
    return p;
}

CGameObject* CTextEffectUI::Clone(void* pArg)
{
    auto* p = new CTextEffectUI(*this);
    if (FAILED(p->Initialize(pArg))) { MSG_BOX("CTextEffectUI Clone Failed"); Safe_Release(p); }
    return p;
}

void CTextEffectUI::Free()
{
    m_imgs.clear();
    m_label = nullptr; 
    __super::Free();
}

HRESULT CTextEffectUI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg))) return E_FAIL;
    Set_Active(true);
    Set_RenderOn(true);
    return S_OK;
}

_int CTextEffectUI::Update_GameObject(const _float& dt)
{
    if (!Is_Active()) return NO_EVENT;

    if (m_autoKill) {
        m_lifeT += dt;
        if (m_lifeT >= m_lifeSec) {
            Set_DeadRecursive(true);

            if (auto* p = GetParent())   
                p->Set_DeadRecursive(true);

            return NO_EVENT;
        }
    }

    if (m_rainbowOn) {
        m_timer += dt;
        if (m_timer >= m_interval) {
            m_timer = 0.f;
            m_idx = (m_idx + 1) % 7;
            applyColorStep();
        }
    }

    if (m_moveOn && m_label)
    {
        m_moveT += dt;
        float t = (m_moveDur > 0.f) ? (m_moveT / m_moveDur) : 1.f;
        if (t > 1.f) t = 1.f;


        float u = 1.f - t;
        float e = 1.f - u * u * u;

        float px = m_moveStartX + (m_moveEndX - m_moveStartX) * e;
        float py = m_moveStartY + (m_moveEndY - m_moveStartY) * e;
        m_label->Set_UIPosition(px, py, 1300.f, 80.f);

        if (t >= 1.f) {
            m_moveOn = false;
            m_label->Set_UIPosition(m_moveEndX, m_moveEndY, 1300.f, 80.f);
            m_label->SetScale(m_textBaseScale); 
        }
    }

    return __super::Update_GameObject(dt);
}


void CTextEffectUI::AddImage(float localX, float localY, D3DCOLOR color,
    float width, float height)
{
    const int sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();

    auto* img = dynamic_cast<CImageUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_UIImage", sceneIdx, L"UI_Layer"));
    if (!img) return;

    
    img->RegisterTexture(L"UI_DiamondWhite", L"Prototype_Component_Texture_UIDiamondWhite",
        0, 0, 0.f, false);
    img->ChangeTexture(L"UI_DiamondWhite");

    img->Set_UIPosition(localX, localY, width, height);
    img->SetTint(color);
    img->SetAdditive(false);
    img->SetColorMode(CImageUI::ColorMode::TintMultiply);
    img->Set_Active(true);
    img->Set_RenderOn(true);

    m_imgs.push_back(img);
    Add_Child(img);
}

void CTextEffectUI::SetupText(const std::wstring& text, float localX, float localY)
{
    const int sceneIdx = CManagement::GetInstance()->Get_CurrentSceneIdx();

    m_label = dynamic_cast<CTextUI*>(
        CObjectManager::GetInstance()->Clone_GameObject(
            L"Prototype_GameObject_TextUI", sceneIdx, L"UI_Layer"));
    if (!m_label) return;

    m_label->SetFontTag(L"Font_UI_Bold");
    m_label->SetText(text.c_str());
    m_label->SetColor(D3DXCOLOR(1.f, 1.f, 0.f, 1.f));
    m_label->SetCentered(true);

   
    m_moveEndX = localX; m_moveEndY = localY;
    m_label->SetScale(m_textBaseScale);
    m_label->Set_UIPosition(localX, localY, 1300.f, 80.f);

    m_label->Set_Active(true);
    m_label->Set_RenderOn(true);
    Add_Child(m_label);
}

void CTextEffectUI::SetTextBaseScale(float s)
{
    m_textBaseScale = (s > 0.f ? s : 1.f);
    if (m_label && !m_moveOn)
        m_label->SetScale(m_textBaseScale);
}

void CTextEffectUI::SetAutoKill(float seconds)
{
    m_autoKill = true;
    m_lifeSec = (seconds > 0.f ? seconds : 5.f);
    m_lifeT = 0.f;
}

void CTextEffectUI::CancelAutoKill()
{
    m_autoKill = false;
    m_lifeT = 0.f;
}

void CTextEffectUI::PlayTextOvershootMove(float totalDur,
    float overMul,
    float offsetX, float offsetY)
{
    
    m_moveDur = totalDur;
    m_moveStartX = m_moveEndX + offsetX;
    m_moveStartY = m_moveEndY + offsetY;
    m_moveT = 0.f;
    m_moveOn = true;

    if (m_label)
    {
       
        m_label->Set_UIPosition(m_moveStartX, m_moveStartY, 1300.f, 80.f);
        m_label->SetScale(m_textBaseScale);

        
        m_label->PlayAppear(totalDur,overMul,overMul);
    }
}

void CTextEffectUI::applyColorStep()
{
    const D3DCOLOR col = ToD3DCOLOR(m_rainbow[m_idx]);
    for (auto* img : m_imgs)
        if (img) img->SetTint(col);
}