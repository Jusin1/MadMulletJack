#include "CPicking.h"
#include "CTransform.h"
#include "Engine_Define.h"
#include "CVIBuffer_GridPanel_Vertical.h"

CVIBuffer_GridPanel_Vertical::CVIBuffer_GridPanel_Vertical()
    : m_pVerticesData(nullptr)
{
}

CVIBuffer_GridPanel_Vertical::CVIBuffer_GridPanel_Vertical(LPDIRECT3DDEVICE9 pGraphicDev)
    : CVIBuffer(pGraphicDev), m_pVerticesData(nullptr)
{
}

CVIBuffer_GridPanel_Vertical::CVIBuffer_GridPanel_Vertical(const CVIBuffer_GridPanel_Vertical &rhs)
    : CVIBuffer(rhs), m_tData(rhs.m_tData), m_pVerticesData(nullptr)
{
}

CVIBuffer_GridPanel_Vertical::~CVIBuffer_GridPanel_Vertical()
{
}

void CVIBuffer_GridPanel_Vertical::Free()
{
    CVIBuffer::Free();
    if (m_pVerticesData)
        delete[] m_pVerticesData;

    m_pVerticesData = nullptr;
}

HRESULT CVIBuffer_GridPanel_Vertical::Initialize(void *pArg)
{
    return Ready_Buffer(pArg);
}

CVIBuffer_GridPanel_Vertical *CVIBuffer_GridPanel_Vertical::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CVIBuffer_GridPanel_Vertical *pGridPanel = new CVIBuffer_GridPanel_Vertical(pGraphicDev);

    if (FAILED(pGridPanel->Ready_Buffer(nullptr)))
    {
        Safe_Release(pGridPanel);
        MSG_BOX("CVIBuffer_GridPanel_Vertical::Create, Failed");
        return nullptr;
    }

    return pGridPanel;
}

CComponent *CVIBuffer_GridPanel_Vertical::Clone(void *pArg)
{
    CVIBuffer_GridPanel_Vertical *pInstance = new CVIBuffer_GridPanel_Vertical(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CVIBuffer_GridPanel_Vertical::Clone, Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

_bool CVIBuffer_GridPanel_Vertical::Picking(CTransform *pTransform, _vec3 *pOut)
{
    CPicking *pPickingSystem = CPicking::GetInstance();
    pPickingSystem->Add_Ref();

    const _matrix *pMatWorld = pTransform->Get_World();
    _matrix matInvWorld;
    D3DXMatrixInverse(&matInvWorld, nullptr, pMatWorld);

    pPickingSystem->TransformRayToLocalSpace(matInvWorld);

    if (IntersectRay(pOut))
    {
        ::D3DXVec3TransformCoord(pOut, pOut, pMatWorld);
        Safe_Release(pPickingSystem);
        return TRUE;
    }

    Safe_Release(pPickingSystem);
    return FALSE;
}

_bool CVIBuffer_GridPanel_Vertical::IntersectRay(_vec3 *pOut)
{
    CPicking *pPickingSystem = CPicking::GetInstance();
    pPickingSystem->Add_Ref();

    _int iColMax{ m_tData.dwCountZ };
    _int iRowMax{ m_tData.dwCountY };

    _ulong  dwIndex{ 0 };
    _int iLeftTop{ 0 };
    _int iRightTop{ 0 };
    _int iRightBottom{ 0 };
    _int iLeftBottom{ 0 };

    for (_int iRow = 0; iRow < iRowMax - 1; ++iRow)
    {
        for (_int iCol = 0; iCol < iColMax - 1; ++iCol)
        {
            dwIndex = iRow * iColMax + iCol;
            iLeftTop = dwIndex + iColMax;
            iRightTop = dwIndex + iColMax + 1;
            iRightBottom = dwIndex + 1;
            iLeftBottom = dwIndex;
            if (pPickingSystem->IntersectRayWithTriangleInLocal(m_pVerticesData[iLeftTop],
                m_pVerticesData[iRightTop],
                m_pVerticesData[iRightBottom],
                pOut)
                ||
                pPickingSystem->IntersectRayWithTriangleInLocal(m_pVerticesData[iLeftTop],
                    m_pVerticesData[iRightBottom],
                    m_pVerticesData[iLeftBottom],
                    pOut))
            {
                Safe_Release(pPickingSystem);
                return TRUE;
            }
        }
    }

    Safe_Release(pPickingSystem);
    return FALSE;
}

HRESULT CVIBuffer_GridPanel_Vertical::Ready_Buffer(void *pArg)
{
    if (pArg)
    {
        if (PANELDATA *pData = reinterpret_cast<PANELDATA *>(pArg))
        {
            m_tData = *pData;
        }
    }

    if (FAILED(Set_Buffer()))
    {
        MSG_BOX("CVIBuffer_GridPanel_Vertical::Ready_Buffer, Failed");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CVIBuffer_GridPanel_Vertical::Set_Buffer()
{
    m_dwVtxSize = sizeof(VTXTEX);
    m_dwVtxCnt = m_tData.dwCountZ * m_tData.dwCountY;
    m_dwTriCnt = (m_tData.dwCountZ - 1) * (m_tData.dwCountY - 1) * 2;
    m_dwFVF = FVF_TEX;

    m_dwIdxSize = sizeof(INDEX32);
    m_IdxFmt = D3DFMT_INDEX32;

    _ulong iColMax{ m_tData.dwCountZ };
    _ulong iRowMax{ m_tData.dwCountY };

    if (FAILED(CVIBuffer::Ready_Vertex_Buffer()))
        return E_FAIL;

    VTXTEX *pVertex = NULL;

    m_pVerticesData = new _vec3[iRowMax * iColMax];

    m_pVB->Lock(0, 0, (void **)&pVertex, 0);

    _ulong  dwIndex(0);

    for (_ulong i = 0; i < iRowMax; ++i)
    {
        for (_ulong j = 0; j < iColMax; ++j)
        {
            dwIndex = i * iColMax + j;

            pVertex[dwIndex].vPosition = { (_float)j * m_tData.dwInterval,
                                       0,
                                       (_float)i * m_tData.dwInterval };

            pVertex[dwIndex].vTexUV = { (_float)j, (_float)(iRowMax - 1 - i) };

            m_pVerticesData[dwIndex] = pVertex[dwIndex].vPosition;
        }
    }

    m_pVB->Unlock();

    if (FAILED(CVIBuffer::Ready_Index_Buffer()))
        return E_FAIL;

    INDEX32 *pIndex = NULL;

    _ulong      dwTriCnt(0);

    m_pIB->Lock(0, 0, (void **)&pIndex, 0);

    for (_ulong i = 0; i < iRowMax - 1; ++i)
    {
        for (_ulong j = 0; j < iColMax - 1; ++j)
        {
            dwIndex = i * iColMax + j;

            pIndex[dwTriCnt]._0 = dwIndex + iColMax;
            pIndex[dwTriCnt]._1 = dwIndex + iColMax + 1;
            pIndex[dwTriCnt]._2 = dwIndex + 1;
            dwTriCnt++;

            pIndex[dwTriCnt]._0 = dwIndex + iColMax;
            pIndex[dwTriCnt]._1 = dwIndex + 1;
            pIndex[dwTriCnt]._2 = dwIndex;
            dwTriCnt++;
        }
    }

    m_pIB->Unlock();
    return S_OK;
}
