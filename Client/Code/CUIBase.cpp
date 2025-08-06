#include "pch.h"
#include "CUIBase.h"
CUIBase::CUIBase(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CUIBase::CUIBase(const CUIBase& rhs)
    : CGameObject(rhs)
{

}

CUIBase::~CUIBase()
{
    for (auto& pChild : m_vecChildren)
        Safe_Release(pChild);
    m_vecChildren.clear();
}

HRESULT CUIBase::Ready_GameObject()
{
    return S_OK;
}

HRESULT CUIBase::Initialize(void* pArg)
{
    return S_OK;
}

_int CUIBase::Update_GameObject(const _float& fTimeDelta)
{
    for (auto& pChild : m_vecChildren)
    {
        if (pChild)
            pChild->Update_GameObject(fTimeDelta);
    }

    return NO_EVENT;
}

void CUIBase::LateUpdate_GameObject(const _float& fTimeDelta)
{
    for (auto& pChild : m_vecChildren)
    {
        if (pChild)
            pChild->LateUpdate_GameObject(fTimeDelta);
    }
}

void CUIBase::Render_GameObject()
{
    for (auto& pChild : m_vecChildren)
    {
        if (pChild)
            pChild->Render_GameObject();
    }
}

void CUIBase::Add_Child(CUIBase* pChild)
{
    if (pChild)
    {
        m_vecChildren.push_back(pChild);
        pChild->Add_Ref();
    }
}

CUIBase* CUIBase::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CUIBase* pInstance = new CUIBase(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        MSG_BOX("CUIBase Create Failed");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUIBase::Clone(void* pArg)
{
    CUIBase* pInstance = new CUIBase(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CUIBase Clone Failed");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUIBase::Free()
{
    for (auto& child : m_vecChildren)
        Safe_Release(child);
    m_vecChildren.clear();

    __super::Free();
}