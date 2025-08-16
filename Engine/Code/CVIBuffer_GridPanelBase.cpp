#include "CTransform.h"
#include "CPicking.h"
#include "CVIBuffer_GridPanelBase.h"

CVIBuffer_GridPanelBase::CVIBuffer_GridPanelBase(LPDIRECT3DDEVICE9 pGraphicDev)
    : CVIBuffer(pGraphicDev)
{
}

CVIBuffer_GridPanelBase::CVIBuffer_GridPanelBase(const CVIBuffer_GridPanelBase &rhs)
    : CVIBuffer(rhs)
{
}

CVIBuffer_GridPanelBase::~CVIBuffer_GridPanelBase()
{
}

void CVIBuffer_GridPanelBase::Free()
{
    CVIBuffer::Free();
    if (m_pVerticesData)
        delete[] m_pVerticesData;

    m_pVerticesData = nullptr;
}

HRESULT CVIBuffer_GridPanelBase::Initialize(void *pArg)
{
    return S_OK;
}

_bool CVIBuffer_GridPanelBase::Picking(CTransform *pTransform, _vec3 *pOut)
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

HRESULT CVIBuffer_GridPanelBase::Ready_Buffer(void *pArg)
{
    if (pArg)
    {
        if (PANELDATA *pData = reinterpret_cast<PANELDATA *>(pArg))
        {
            m_tData = *pData;
        }
    }

    return S_OK;
}

HRESULT CVIBuffer_GridPanelBase::Set_Buffer()
{
    return S_OK;
}

_bool CVIBuffer_GridPanelBase::IntersectRay(_vec3 *pOut)
{
    CPicking *pPickingSystem = CPicking::GetInstance();
    pPickingSystem->Add_Ref();

    _ulong  dwIndex{ 0 };
    _ulong iLeftTop{ 0 };
    _ulong iRightTop{ 0 };
    _ulong iRightBottom{ 0 };
    _ulong iLeftBottom{ 0 };

    for (_ulong iRow = 0; iRow < m_iRowMax - 1; ++iRow)
    {
        for (_ulong iCol = 0; iCol < m_iColMax - 1; ++iCol)
        {
            dwIndex = iRow * m_iColMax + iCol;
            iLeftTop = dwIndex + m_iColMax;
            iRightTop = dwIndex + m_iColMax + 1;
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
