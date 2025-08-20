#include "CVIBuffer_Rect.h"
#include "CPicking.h"
#include "CTransform.h"

CVIBuffer_Rect::CVIBuffer_Rect(LPDIRECT3DDEVICE9 pGraphicDev)
    : CVIBuffer(pGraphicDev)
{
}

CVIBuffer_Rect::CVIBuffer_Rect(const CVIBuffer_Rect& rhs)
    : CVIBuffer(rhs)
{
    for (int i = 0; i < 4; ++i)
        m_vVerticesLocal[i] = rhs.m_vVerticesLocal[i];
}

CVIBuffer_Rect::~CVIBuffer_Rect()
{

}

HRESULT CVIBuffer_Rect::Ready_Buffer()
{
    m_dwVtxSize = sizeof(VTXTEX);
    m_dwVtxCnt = 4;
    m_dwTriCnt = 2;
    m_dwFVF = FVF_TEX;

    m_dwIdxSize = sizeof(INDEX32);
    m_IdxFmt = D3DFMT_INDEX32;



    if (FAILED(CVIBuffer::Ready_Vertex_Buffer()))
        return E_FAIL;

    VTXTEX* pVertex = NULL;

    // 3인자 : 버텍스 버퍼에 저장된 정점들 중 첫 번째 정점의 주소를 얻어옴
    m_pVB->Lock(0, 0, (void**)&pVertex, 0);

    // 오른쪽 위
    pVertex[0].vPosition = { -0.5f, 0.5f, 0.f };
    pVertex[0].vTexUV = { 0.f, 0.f };

    pVertex[1].vPosition = { 0.5f, 0.5f, 0.f };
    pVertex[1].vTexUV = { 1.f, 0.f };

    pVertex[2].vPosition = { 0.5f, -0.5f, 0.f };
    pVertex[2].vTexUV = { 1.f, 1.f };

    pVertex[3].vPosition = { -0.5f, -0.5f, 0.f };
    pVertex[3].vTexUV = { 0.f, 1.f };

    // 로컬 정점 위치 저장
    m_vVerticesLocal[0] = pVertex[0].vPosition;
    m_vVerticesLocal[1] = pVertex[1].vPosition;
    m_vVerticesLocal[2] = pVertex[2].vPosition;
    m_vVerticesLocal[3] = pVertex[3].vPosition;

    m_pVB->Unlock();


    if (FAILED(CVIBuffer::Ready_Index_Buffer()))
        return E_FAIL;

    INDEX32* pIndex = nullptr;

    m_pIB->Lock(0, 0, (void**)&pIndex, 0);

    // 오른쪽 위
    pIndex[0]._0 = 0;
    pIndex[0]._1 = 1;
    pIndex[0]._2 = 2;

    // 왼쪽 아래
    pIndex[1]._0 = 0;
    pIndex[1]._1 = 2;
    pIndex[1]._2 = 3;

    m_pIB->Unlock();

    return S_OK;
}

HRESULT CVIBuffer_Rect::Initialize(void* pArg)
{
    return S_OK;
}

_bool CVIBuffer_Rect::Picking(CTransform* pTransform, _vec3* pOut)
{
    CPicking* pPicking = CPicking::GetInstance();

    pPicking->Add_Ref();


    _matrix   WorldMatrix = *pTransform->Get_World();
    _matrix	WorldMatrixInverse;
    D3DXMatrixInverse(&WorldMatrixInverse, nullptr, &WorldMatrix);

    pPicking->TransformRayToLocalSpace(WorldMatrixInverse);

    if (true == pPicking->IntersectRayWithTriangleInLocal(m_vVerticesLocal[0], m_vVerticesLocal[1], m_vVerticesLocal[2], pOut))
        goto Coll;

    else if (true == pPicking->IntersectRayWithTriangleInLocal(m_vVerticesLocal[0], m_vVerticesLocal[2], m_vVerticesLocal[3], pOut))
        goto Coll;

    Safe_Release(pPicking);
    return false;

Coll:
    D3DXVec3TransformCoord(pOut, pOut, &WorldMatrix);

    Safe_Release(pPicking);

    return true;
}

CVIBuffer_Rect* CVIBuffer_Rect::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CVIBuffer_Rect* pRcTex = new CVIBuffer_Rect(pGraphicDev);

    if (FAILED(pRcTex->Ready_Buffer()))
    {
        Safe_Release(pRcTex);
        MSG_BOX("pRcTex Create Failed");
        return nullptr;
    }

    return pRcTex;
}

CComponent* CVIBuffer_Rect::Clone(void* pArg)
{
    CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("pRcTex Clone Failed");
        Safe_Release(pInstance);
    }

     return pInstance;
}

void CVIBuffer_Rect::Free()
{
    CVIBuffer::Free();
}
