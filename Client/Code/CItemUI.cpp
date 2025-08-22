#include "pch.h"
#include "CItemUI.h"

CItemUI::CItemUI(LPDIRECT3DDEVICE9 dev)
    : CImageUI(dev)
    , m_bAppearing(false)
    , m_fAppearTime(0.f)
    , m_fAppearDuration(0.35f)
    , m_startW(0.f), m_startH(0.f)
    , m_targetW(0.f), m_targetH(0.f)
    , m_targetX(0.f), m_targetY(0.f)
{
}

CItemUI::CItemUI(const CItemUI& rhs)
    : CImageUI(rhs)
    , m_bAppearing(false)              
    , m_fAppearTime(0.f)
    , m_fAppearDuration(rhs.m_fAppearDuration)
    , m_startW(0.f), m_startH(0.f)
    , m_targetW(0.f), m_targetH(0.f)
    , m_targetX(0.f), m_targetY(0.f)
{
}
CItemUI::~CItemUI() {}

HRESULT CItemUI::Ready_GameObject() { return __super::Ready_GameObject(); }
HRESULT CItemUI::Initialize(void* pArg) { return __super::Initialize(pArg); }
void CItemUI::LateUpdate_GameObject(const _float& dt) { __super::LateUpdate_GameObject(dt); }
void CItemUI::Render_GameObject() { __super::Render_GameObject(); }

void CItemUI::PlayAppear(float x, float y, float w, float h, float duration)
{
    m_targetX = x;
    m_targetY = y;
    m_targetW = w;
    m_targetH = h;

    m_startW = w * 20.f;
    m_startH = h * 20.f;
    Set_UIPosition(x, y, m_startW, m_startH);

    m_fAppearTime = 0.f;
    m_fAppearDuration = duration;
    m_bAppearing = true;
}

_int CItemUI::Update_GameObject(const _float& dt)
{
    if (m_bAppearing)
    {
        m_fAppearTime += dt;
        float t = m_fAppearTime / m_fAppearDuration;
        if (t > 1.f) { t = 1.f; m_bAppearing = false; }

        float inv = 1.f - t;
        float scale = 1.f - (inv * inv * inv);

        float curW = m_startW + (m_targetW - m_startW) * scale;
        float curH = m_startH + (m_targetH - m_startH) * scale;

        Set_UIPosition(m_targetX, m_targetY, curW, curH);
    }

    return __super::Update_GameObject(dt);
}

CItemUI* CItemUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CItemUI* p = new CItemUI(pGraphicDev);
    if (FAILED(p->Ready_GameObject())) {
        MSG_BOX("CButtonUI Create Failed");
        Safe_Release(p);
    }
    return p;
}

CGameObject* CItemUI::Clone(void* pArg)
{
    CItemUI* p = new CItemUI(*this);
    if (FAILED(p->Initialize(pArg))) {
        MSG_BOX("CButtonUI Clone Failed");
        Safe_Release(p);
    }
    return p;
}

void CItemUI::Free()
{
    CImageUI::Free();
}