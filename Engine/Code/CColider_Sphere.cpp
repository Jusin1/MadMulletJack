
#include "CColider_Sphere.h"
#include "CTransform.h"


CColider_Sphere::CColider_Sphere(LPDIRECT3DDEVICE9 pGraphicDev)
    : CComponent(pGraphicDev), m_pGraphicDev(pGraphicDev)
{
    m_pGraphicDev->AddRef();
#ifdef _DEBUG
    D3DXCreateSphere(m_pGraphicDev, 1.f, 16, 16, &m_pSphereMesh, nullptr);
#endif
}

CColider_Sphere::CColider_Sphere(const CColider_Sphere& rhs)
    : CComponent(rhs)
    , m_SphereDesc(rhs.m_SphereDesc)
    , m_vCenter(rhs.m_vCenter)
    , m_fRadius(rhs.m_fRadius)
    , m_fBaseRadius(rhs.m_fBaseRadius)
    , m_bIsInverse(rhs.m_bIsInverse)
    , m_pGraphicDev(rhs.m_pGraphicDev)
{
    m_pGraphicDev->AddRef();
#ifdef _DEBUG
    D3DXCreateSphere(m_pGraphicDev, 1.f, 16, 16, &m_pSphereMesh, nullptr);
#endif
}

HRESULT CColider_Sphere::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CColider_Sphere::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    memcpy(&m_SphereDesc, pArg, sizeof(COLLINFO));
    m_fBaseRadius = m_SphereDesc.fRadius;
    m_vOffset = m_SphereDesc.vOffset;

#ifdef _DEBUG
    if (FAILED(D3DXCreateSphere(m_pGraphicDev, m_fBaseRadius, 20, 20, &m_pSphereMesh, nullptr)))
        return E_FAIL;
#endif

    return S_OK;
}

HRESULT CColider_Sphere::Update_ColliderSphere(const _matrix& WorldMatrix)
{
    m_SphereDesc.StateMatrix = WorldMatrix;

    _vec3 vOffsetPos;
    D3DXVec3TransformCoord(&vOffsetPos, &m_SphereDesc.vOffset, &WorldMatrix);
    m_vCenter = vOffsetPos;

    _vec3 vScale;
    vScale.x = D3DXVec3Length((_vec3*)&WorldMatrix.m[0][0]);
    vScale.y = D3DXVec3Length((_vec3*)&WorldMatrix.m[1][0]);
    vScale.z = D3DXVec3Length((_vec3*)&WorldMatrix.m[2][0]);

    float fMaxScale = max(vScale.x, max(vScale.y, vScale.z));
    m_fRadius = m_fBaseRadius * fMaxScale;

    return S_OK;
}

HRESULT CColider_Sphere::Render_ColliderSphere()
{
#ifdef _DEBUG
    if (!m_pSphereMesh || !m_pGraphicDev)
        return E_FAIL;

    _matrix matScale, matTrans, matWorld;
    D3DXMatrixScaling(&matScale, m_fRadius, m_fRadius, m_fRadius);
    D3DXMatrixTranslation(&matTrans, m_vCenter.x, m_vCenter.y, m_vCenter.z);
    matWorld = matScale * matTrans;

    DWORD oldFillMode = 0;
    m_pGraphicDev->GetRenderState(D3DRS_FILLMODE, &oldFillMode);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

    m_pSphereMesh->DrawSubset(0);

    m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, oldFillMode);
#endif

    return S_OK;
}

_bool CColider_Sphere::Collision_Check(CColider_Sphere* pTarget, _vec3* pOutDistance)
{
    if (!pTarget)
        return false;

    _vec3 vDelta = m_vCenter - pTarget->m_vCenter;
    _float fDist = D3DXVec3Length(&vDelta);
    _float fSumRadius = m_fRadius + pTarget->m_fRadius;

    if (fDist <= fSumRadius)
    {
        if (pOutDistance)
        {
            D3DXVec3Normalize(pOutDistance, &vDelta);
            *pOutDistance *= (fSumRadius - fDist);
        }
        return true;
    }

    return false;
}

CColider_Sphere* CColider_Sphere::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CColider_Sphere* pInstance = new CColider_Sphere(pGraphicDev);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("CColider_Sphere Create Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CColider_Sphere::Clone(void* pArg)
{
    CColider_Sphere* pClone = new CColider_Sphere(*this);
    if (FAILED(pClone->Initialize(pArg)))
    {
        MSG_BOX("CColider_Sphere Clone Failed");
        Safe_Release(pClone);
    }
    return pClone;
}

void CColider_Sphere::Free()
{
#ifdef _DEBUG
    Safe_Release(m_pSphereMesh);
#endif
    Safe_Release(m_pGraphicDev);
    __super::Free();
}

