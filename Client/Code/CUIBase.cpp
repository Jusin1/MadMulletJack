#include "pch.h"
#include "CUIBase.h"
CUIBase::CUIBase(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev) ,m_bAniFinish(false), m_fRotSum(0.f)
{
}

CUIBase::CUIBase(const CUIBase& rhs)
    : CGameObject(rhs), m_bAniFinish(rhs.m_bAniFinish), m_fRotSum(rhs.m_fRotSum)
{

}

CUIBase::~CUIBase() 
{
}

HRESULT CUIBase::Ready_GameObject()
{
    return S_OK;
}

HRESULT CUIBase::Initialize(void* pArg)
{
    if (FAILED(__super::Set_Component()))
        return E_FAIL;

    return S_OK;
}

_int CUIBase::Update_GameObject(const _float& fTimeDelta) // 자식 Update돌리기
{
    if (!m_bActive)
        NO_EVENT;

    if (nullptr != m_pRendererCom)
        m_pRendererCom->Add_RenderGroup(RENDER_UI, this);

    for (auto& pChild : m_vecChildren)
    {
        if (pChild)
            pChild->Update_GameObject(fTimeDelta);
    }

    return NO_EVENT;
}

void CUIBase::LateUpdate_GameObject(const _float& fTimeDelta) // 자식 LateUpdate
{
    if (!m_bActive)
        return;

    for (auto& pChild : m_vecChildren)
    {
        if (pChild)
            pChild->LateUpdate_GameObject(fTimeDelta);
    }
}

void CUIBase::Render_GameObject() // 자식 Render
{
    if (!m_bActive)
        return;

    for (auto& pChild : m_vecChildren)
    {
        if (pChild && pChild->Get_RenderOn())
            pChild->Render_GameObject();
    }
}

HRESULT CUIBase::Set_Component()
{
    return S_OK;
}

void CUIBase::Set_Origin_Rot()
{
    // ui는 z축 기준 회전 하므로
    // -1.f로 반대로 돌려주기
    // 안에서 자기 속도 곱하게 되므로 rotsum / rotspeed 값으로 들어간다
    m_pTransformCom->Rotation({ 0.f, 0.f,-1.f }, m_fRotSum / m_pTransformCom->GetTransformInfo().fRotationSpeed);

    // rotsum 0으로 초기화
    m_fRotSum = 0.f;
}

void CUIBase::Set_New_TransInfo(_float _fSpeed, _float _fRotSpeed)
{
    CTransform::TRANSFORMINFO TransformInfo;    // 새롭게 transinfo를 저장해줌
    ZeroMemory(&TransformInfo, sizeof(CTransform::TRANSFORMINFO));

    TransformInfo.fSpeed = _fSpeed;
    TransformInfo.fRotationSpeed = D3DXToRadian(_fRotSpeed);
    TransformInfo.vStartPos = m_pTransformCom->Get_Info(INFO_POS);
    m_pTransformCom->SetTransformInfo(TransformInfo);
}

void CUIBase::Add_Child(CUIBase* pChild) // 자식 추가
{
    if (pChild)
    {
        m_vecChildren.push_back(pChild);
        pChild->Add_Ref();
    }
}

CUIBase* CUIBase::Find_Child_ByTag(const _tchar* pTag) // 자식 찾기(태그로)
{
    for (auto& pChild : m_vecChildren)
    {
        if (lstrcmp(pChild->Get_ObjTag(), pTag) == 0)
            return pChild;
    }
    return nullptr;
}

void CUIBase::Set_UIPos(_vec3 _vPos, _float _offsetX, _float _offesetY)
{
    _vPos.x += _offsetX;
    _vPos.y += _offesetY;
    m_pTransformCom->Set_Info(INFO_POS, _vPos);
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

void CUIBase::Free() // 메모리 해제
{
    for (auto& child : m_vecChildren)
        Safe_Release(child);
    m_vecChildren.clear();

    __super::Free();
}