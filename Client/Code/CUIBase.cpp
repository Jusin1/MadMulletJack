#include "pch.h"
#include "CUIBase.h"
CUIBase::CUIBase(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev) , m_fRotSum(0.f)
{
}

CUIBase::CUIBase(const CUIBase& rhs)
    : CGameObject(rhs), m_fRotSum(rhs.m_fRotSum)
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
    if (!m_bActive || m_bDead)
        return NO_EVENT;                  

    if (m_pRendererCom)
        m_pRendererCom->Add_RenderGroup(RENDER_UI, this);

    for (auto& pChild : m_vecChildren)
    {
        if (pChild && pChild->Is_Active())
            pChild->Update_GameObject(fTimeDelta);
    }

    return NO_EVENT;
}

void CUIBase::LateUpdate_GameObject(const _float& fTimeDelta) // 자식 LateUpdate
{
    if (!m_bActive || m_bDead) return;
    for (auto& pChild : m_vecChildren)
    {
        if (pChild && pChild->Is_Active())
            pChild->LateUpdate_GameObject(fTimeDelta);
    }
}

void CUIBase::Render_GameObject() // 자식 Render
{
    if (!m_bActive || m_bDead) return;

    for (auto& pChild : m_vecChildren) {
        if (!pChild) continue;
        if (!pChild->Is_Active() || pChild->Get_Dead()) continue; 
        if (pChild->Get_RenderOn())
            pChild->Render_GameObject();
    }
}

HRESULT CUIBase::Set_Component()
{
    return S_OK;
}

void CUIBase::Set_Origin_Rot()
{
    // rotSum이 0이면 rotation을 하지 않음
    if (m_fRotSum == 0)
        return;

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
    if (!pChild) return;
    m_vecChildren.push_back(pChild);
    pChild->m_pParent = this;   
    pChild->Add_Ref();         
}

void CUIBase::Add_ChildFront(CUIBase* pChild)
{
    if (!pChild) return;

    // 같은 부모인 경우: 목록 내 위치만 맨 앞으로 이동
    if (pChild->m_pParent == this) {
        auto it = std::find(m_vecChildren.begin(), m_vecChildren.end(), pChild);
        if (it != m_vecChildren.end() && it != m_vecChildren.begin()) {
            m_vecChildren.erase(it);
            m_vecChildren.insert(m_vecChildren.begin(), pChild);
        }
        else if (it == m_vecChildren.end()) {
            // 이 부모로 되어있는데 리스트엔 없으면 비정상 상황?앞에 꽂아줌
            m_vecChildren.insert(m_vecChildren.begin(), pChild);
            pChild->Add_Ref();
        }
        return;
    }

    // 다른 부모에 붙어있으면 먼저 떼기
    if (pChild->m_pParent) {
        pChild->m_pParent->Remove_Child(pChild); // 프로젝트에 이미 있는 함수
    }

    // 맨 앞에 추가
    pChild->m_pParent = this;
    m_vecChildren.insert(m_vecChildren.begin(), pChild);
    pChild->Add_Ref();
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

void CUIBase::Remove_Child(CUIBase* pChild)
{
    if (!pChild) return;
    auto it = std::find(m_vecChildren.begin(), m_vecChildren.end(), pChild);
    if (it != m_vecChildren.end()) {
        (*it)->m_pParent = nullptr;
        Safe_Release(*it);              
        m_vecChildren.erase(it);
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

void CUIBase::Free() // 메모리 해제
{
    for (auto& child : m_vecChildren)
        Safe_Release(child);
    m_vecChildren.clear();

    __super::Free();
}