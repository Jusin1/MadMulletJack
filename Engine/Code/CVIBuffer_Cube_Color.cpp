#include "CPicking.h"
#include "CTransform.h"
#include "CVIBuffer_Cube_Color.h"


CVIBuffer_Cube_Color::CVIBuffer_Cube_Color(LPDIRECT3DDEVICE9 pGraphicDev)
    : CVIBuffer(pGraphicDev)
{
    m_dwColor = { 0.f, 0.f, 0.f, 1.f };
}

CVIBuffer_Cube_Color::CVIBuffer_Cube_Color(const CVIBuffer_Cube_Color &rhs)
    : CVIBuffer(rhs), m_dwColor(rhs.m_dwColor)
{
    for (int i = 0; i < 8; ++i)
    {
        ::memcpy(&m_vVerticesLocal[i], &rhs.m_vVerticesLocal[i], sizeof(_vec3));
    }
}

CVIBuffer_Cube_Color::~CVIBuffer_Cube_Color()
{
}

HRESULT CVIBuffer_Cube_Color::Ready_Buffer()
{
    m_dwVtxSize = sizeof(VTXCUBECOLOR);
    m_dwVtxCnt = 8;
    m_dwTriCnt = 12;
    m_dwFVF = FVF_CUBE;

    m_dwIdxSize = sizeof(INDEX32);
    m_IdxFmt = D3DFMT_INDEX32;

    if (FAILED(CVIBuffer::Ready_Vertex_Buffer()))
        return E_FAIL;

    VTXCUBECOLOR *pVertex = NULL;

    // 3인자 : 버텍스 버퍼에 저장된 정점들 중 첫 번째 정점의 주소를 얻어옴
    m_pVB->Lock(0, 0, (void **)&pVertex, 0);

    // 전면
    pVertex[0].vPosition = { -0.5f, 0.5f, -0.5f };
    pVertex[1].vPosition = { 0.5f, 0.5f, -0.5f };
    pVertex[2].vPosition = { 0.5f, -0.5f, -0.5f };
    pVertex[3].vPosition = { -0.5f, -0.5f, -0.5f };

    // 후면
    pVertex[4].vPosition = { -0.5f, 0.5f, 0.5f };
    pVertex[5].vPosition = { 0.5f, 0.5f, 0.5f };
    pVertex[6].vPosition = { 0.5f, -0.5f, 0.5f };
    pVertex[7].vPosition = { -0.5f, -0.5f, 0.5f };

    for (int i = 0; i < 8; ++i)
    {
        ::memcpy(&m_vVerticesLocal[i], &pVertex[i].vPosition, sizeof(_vec3));
    }

    m_pVB->Unlock();

    if (FAILED(CVIBuffer::Ready_Index_Buffer()))
        return E_FAIL;

    INDEX32 *pIndex = NULL;

    m_pIB->Lock(0, 0, (void **)&pIndex, 0);

    // X+
    pIndex[0]._0 = 1;
    pIndex[0]._1 = 5;
    pIndex[0]._2 = 6;

    pIndex[1]._0 = 1;
    pIndex[1]._1 = 6;
    pIndex[1]._2 = 2;

    // X-
    pIndex[2]._0 = 4;
    pIndex[2]._1 = 0;
    pIndex[2]._2 = 3;

    pIndex[3]._0 = 4;
    pIndex[3]._1 = 3;
    pIndex[3]._2 = 7;

    // Y+
    pIndex[4]._0 = 4;
    pIndex[4]._1 = 5;
    pIndex[4]._2 = 1;

    pIndex[5]._0 = 4;
    pIndex[5]._1 = 1;
    pIndex[5]._2 = 0;

    // Y-
    pIndex[6]._0 = 3;
    pIndex[6]._1 = 2;
    pIndex[6]._2 = 6;

    pIndex[7]._0 = 3;
    pIndex[7]._1 = 6;
    pIndex[7]._2 = 7;

    // Z+
    pIndex[8]._0 = 7;
    pIndex[8]._1 = 6;
    pIndex[8]._2 = 5;

    pIndex[9]._0 = 7;
    pIndex[9]._1 = 5;
    pIndex[9]._2 = 4;

    // Z-
    pIndex[10]._0 = 0;
    pIndex[10]._1 = 1;
    pIndex[10]._2 = 2;

    pIndex[11]._0 = 0;
    pIndex[11]._1 = 2;
    pIndex[11]._2 = 3;

    m_pIB->Unlock();

    return ApplyColorToVB();
}

HRESULT CVIBuffer_Cube_Color::Initialize(void *pArg)
{
    if (pArg) {
        D3DXCOLOR *pCol = reinterpret_cast<D3DXCOLOR *>(pArg);
        m_dwColor = *pCol;
        return ApplyColorToVB();
    }
    return S_OK;
}

_bool CVIBuffer_Cube_Color::Picking(CTransform *pTransform, _vec3 *pOut)
{
    CPicking *pPicking = CPicking::GetInstance();

    pPicking->Add_Ref();


    _matrix   WorldMatrix = *pTransform->Get_World();
    _matrix	WorldMatrixInverse;
    D3DXMatrixInverse(&WorldMatrixInverse, nullptr, &WorldMatrix);

    pPicking->TransformRayToLocalSpace(WorldMatrixInverse);

#pragma region nogada
    if (pPicking->IntersectRayWithTriangleInLocal(m_vVerticesLocal[0], m_vVerticesLocal[1], m_vVerticesLocal[2], pOut) || 
        pPicking->IntersectRayWithTriangleInLocal(m_vVerticesLocal[4], m_vVerticesLocal[5], m_vVerticesLocal[1], pOut) ||
        pPicking->IntersectRayWithTriangleInLocal(m_vVerticesLocal[3], m_vVerticesLocal[2], m_vVerticesLocal[6], pOut) ||
        pPicking->IntersectRayWithTriangleInLocal(m_vVerticesLocal[1], m_vVerticesLocal[5], m_vVerticesLocal[6], pOut) ||
        pPicking->IntersectRayWithTriangleInLocal(m_vVerticesLocal[4], m_vVerticesLocal[0], m_vVerticesLocal[3], pOut) ||
        pPicking->IntersectRayWithTriangleInLocal(m_vVerticesLocal[7], m_vVerticesLocal[6], m_vVerticesLocal[5], pOut))
        goto Coll;
    else if (pPicking->IntersectRayWithTriangleInLocal(m_vVerticesLocal[0], m_vVerticesLocal[2], m_vVerticesLocal[3], pOut) ||
        pPicking->IntersectRayWithTriangleInLocal(m_vVerticesLocal[4], m_vVerticesLocal[1], m_vVerticesLocal[0], pOut) ||
        pPicking->IntersectRayWithTriangleInLocal(m_vVerticesLocal[3], m_vVerticesLocal[6], m_vVerticesLocal[7], pOut) ||
        pPicking->IntersectRayWithTriangleInLocal(m_vVerticesLocal[7], m_vVerticesLocal[5], m_vVerticesLocal[4], pOut) ||
        pPicking->IntersectRayWithTriangleInLocal(m_vVerticesLocal[4], m_vVerticesLocal[3], m_vVerticesLocal[7], pOut) || 
        pPicking->IntersectRayWithTriangleInLocal(m_vVerticesLocal[1], m_vVerticesLocal[6], m_vVerticesLocal[2], pOut))
        goto Coll;
#pragma endregion

    Safe_Release(pPicking);
    return false;

Coll:
    D3DXVec3TransformCoord(pOut, pOut, &WorldMatrix);

    Safe_Release(pPicking);

    return true;
}

CVIBuffer_Cube_Color *CVIBuffer_Cube_Color::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CVIBuffer_Cube_Color *pCubeTex = new CVIBuffer_Cube_Color(pGraphicDev);

    if (FAILED(pCubeTex->Ready_Buffer()))
    {
        Safe_Release(pCubeTex);
        MSG_BOX("pCube Create Failed");
        return nullptr;
    }

    return pCubeTex;
}


CComponent *CVIBuffer_Cube_Color::Clone(void *pArg)
{
    CVIBuffer_Cube_Color *pInstance = new CVIBuffer_Cube_Color(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("pCube Create Failed");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Cube_Color::Free()
{
    CVIBuffer::Free();
}

HRESULT CVIBuffer_Cube_Color::ApplyColorToVB()
{
    if (!m_pVB) return E_FAIL;

    VTXCOL *pVertex = nullptr;
    
    if (FAILED(m_pVB->Lock(0, 0, (void **)&pVertex, 0)))
        return E_FAIL;

    DWORD dw = m_dwColor; // D3DXCOLOR → DWORD 변환자 오버로드 존재
    pVertex[0].dwColor = dw;
    pVertex[1].dwColor = dw;
    pVertex[2].dwColor = dw;
    pVertex[3].dwColor = dw;
    pVertex[4].dwColor = dw;
    pVertex[5].dwColor = dw;
    pVertex[6].dwColor = dw;
    pVertex[7].dwColor = dw;

    m_pVB->Unlock();
    return S_OK;
}
