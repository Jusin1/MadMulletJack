#include "CVIBuffer_GridPanel.h"

CVIBuffer_GridPanel::CVIBuffer_GridPanel()
{
}

CVIBuffer_GridPanel::CVIBuffer_GridPanel(LPDIRECT3DDEVICE9 pGraphicDev)
    : CVIBuffer(pGraphicDev)
{
}

CVIBuffer_GridPanel::CVIBuffer_GridPanel(const CVIBuffer_GridPanel &rhs)
    : CVIBuffer(rhs)
{
}

CVIBuffer_GridPanel::~CVIBuffer_GridPanel()
{
}

HRESULT CVIBuffer_GridPanel::Ready_Buffer(void *pArg)
{
    if (!pArg)
    {
        MSG_BOX("CVIBuffer_GridPanel::Ready_Buffer, PanelBufferData is nullptr");
        return S_OK;
    }
    else
        Set_Data(pArg);

    switch (m_tData.eType)
    {
    case PanelType::WALL_HOR:
        return Ready_HorizonWallBuffer();
    case PanelType::WALL_VER:
        return Ready_VerticalWallBuffer();
    case PanelType::INCLINE:
    case PanelType::FLOOR:
    case PanelType::CEILING:
        return Ready_PlaneBuffer();
    default:
    {
        MSG_BOX("PanelData.eType is None");
    } break;
    }

    return S_OK;
}

HRESULT CVIBuffer_GridPanel::Ready_HorizonWallBuffer()
{
    m_dwVtxSize = sizeof(VTXTEX);
    m_dwVtxCnt = m_tData.dwCountX * m_tData.dwCountY;
    m_dwTriCnt = (m_tData.dwCountX - 1) * (m_tData.dwCountY - 1) * 2;
    m_dwFVF = FVF_TEX;

    m_dwIdxSize = sizeof(INDEX32);
    m_IdxFmt = D3DFMT_INDEX32;

    if (FAILED(CVIBuffer::Ready_Vertex_Buffer()))
        return E_FAIL;

    VTXTEX *pVertex = NULL;

    m_pVB->Lock(0, 0, (void **)&pVertex, 0);

    _ulong  dwIndex(0);

    for (_ulong i = 0; i < m_tData.dwCountY; ++i)
    {
        for (_ulong j = 0; j < m_tData.dwCountX; ++j)
        {
            dwIndex = i * m_tData.dwCountX + j;

            pVertex[dwIndex].vPosition = { (_float)j * m_tData.dwInterval,
                                           (_float)i *m_tData.dwInterval,
                                            0,
                                           
            };

            pVertex[dwIndex].vTexUV = {
                                        (_float)j / (m_tData.dwCountX - 1) * (m_tData.dwCountX - 1),
                                        (_float)i / (m_tData.dwCountY - 1) * (m_tData.dwCountY - 1) };
        }
    }

    m_pVB->Unlock();

    if (FAILED(CVIBuffer::Ready_Index_Buffer()))
        return E_FAIL;

    INDEX32 *pIndex = NULL;

    _ulong      dwTriCnt(0);

    m_pIB->Lock(0, 0, (void **)&pIndex, 0);

    for (_ulong i = 0; i < m_tData.dwCountY-1; ++i)
    {
        for (_ulong j = 0; j < m_tData.dwCountX-1; ++j)
        {
            dwIndex = i * m_tData.dwCountX + j;

            // 오른쪽 위
            pIndex[dwTriCnt]._0 = dwIndex + m_tData.dwCountX;
            pIndex[dwTriCnt]._1 = dwIndex + m_tData.dwCountX + 1;
            pIndex[dwTriCnt]._2 = dwIndex + 1;
            dwTriCnt++;

            // 왼쪽 아래
            pIndex[dwTriCnt]._0 = dwIndex + m_tData.dwCountX;
            pIndex[dwTriCnt]._1 = dwIndex + 1;
            pIndex[dwTriCnt]._2 = dwIndex;
            dwTriCnt++;
        }
    }

    m_pIB->Unlock();
    return S_OK;
}

HRESULT CVIBuffer_GridPanel::Ready_VerticalWallBuffer()
{
    m_dwVtxSize = sizeof(VTXTEX);
    m_dwVtxCnt = m_tData.dwCountZ * m_tData.dwCountY;
    m_dwTriCnt = (m_tData.dwCountZ - 1) * (m_tData.dwCountY - 1) * 2;
    m_dwFVF = FVF_TEX;

    m_dwIdxSize = sizeof(INDEX32);
    m_IdxFmt = D3DFMT_INDEX32;

    if (FAILED(CVIBuffer::Ready_Vertex_Buffer()))
        return E_FAIL;

    VTXTEX *pVertex = NULL;

    m_pVB->Lock(0, 0, (void **)&pVertex, 0);

    _ulong  dwIndex(0);

    for (_ulong i = 0; i < m_tData.dwCountY; ++i)
    {
        for (_ulong j = 0; j < m_tData.dwCountZ; ++j)
        {
            dwIndex = i * m_tData.dwCountZ + j;

            pVertex[dwIndex].vPosition = { 0,
                                           (_float)i * m_tData.dwInterval,
                                           (_float)j *m_tData.dwInterval,

            };

            pVertex[dwIndex].vTexUV = {
                                        (_float)j / (m_tData.dwCountZ - 1) * (m_tData.dwCountZ - 1),
                                        (_float)i / (m_tData.dwCountY - 1) * (m_tData.dwCountY - 1) };
        }
    }

    m_pVB->Unlock();

    if (FAILED(CVIBuffer::Ready_Index_Buffer()))
        return E_FAIL;

    INDEX32 *pIndex = NULL;

    _ulong      dwTriCnt(0);

    m_pIB->Lock(0, 0, (void **)&pIndex, 0);

    for (_ulong i = 0; i < m_tData.dwCountY-1; ++i)
    {
        for (_ulong j = 0; j < m_tData.dwCountZ-1; ++j)
        {
            dwIndex = i * m_tData.dwCountZ + j;

            // 오른쪽 위
            pIndex[dwTriCnt]._0 = dwIndex + m_tData.dwCountZ;
            pIndex[dwTriCnt]._1 = dwIndex + m_tData.dwCountZ + 1;
            pIndex[dwTriCnt]._2 = dwIndex + 1;
            dwTriCnt++;

            // 왼쪽 아래
            pIndex[dwTriCnt]._0 = dwIndex + m_tData.dwCountZ;
            pIndex[dwTriCnt]._1 = dwIndex + 1;
            pIndex[dwTriCnt]._2 = dwIndex;
            dwTriCnt++;
        }
    }

    m_pIB->Unlock();
    return S_OK;
}

HRESULT CVIBuffer_GridPanel::Ready_PlaneBuffer()
{
    m_dwVtxSize = sizeof(VTXTEX);
    m_dwVtxCnt = m_tData.dwCountX * m_tData.dwCountZ;
    m_dwTriCnt = (m_tData.dwCountX - 1) * (m_tData.dwCountZ - 1) * 2;
    m_dwFVF = FVF_TEX;

    m_dwIdxSize = sizeof(INDEX32);
    m_IdxFmt = D3DFMT_INDEX32;

    if (FAILED(CVIBuffer::Ready_Vertex_Buffer()))
        return E_FAIL;

    VTXTEX *pVertex = NULL;

    m_pVB->Lock(0, 0, (void **)&pVertex, 0);

    _ulong  dwIndex(0);

    for (_ulong i = 0; i < m_tData.dwCountZ; ++i)
    {
        for (_ulong j = 0; j < m_tData.dwCountX; ++j)
        {
            dwIndex = i * m_tData.dwCountX + j;

            pVertex[dwIndex].vPosition = { (_float)j * m_tData.dwInterval,
                                           0,
                                           (_float)i * m_tData.dwInterval
            };

            pVertex[dwIndex].vTexUV = {
                                        (_float)j / (m_tData.dwCountX - 1) * (m_tData.dwCountX - 1),
                                        (_float)i / (m_tData.dwCountZ - 1) * (m_tData.dwCountZ - 1) };
        }
    }

    m_pVB->Unlock();

    if (FAILED(CVIBuffer::Ready_Index_Buffer()))
        return E_FAIL;

    INDEX32 *pIndex = NULL;

    _ulong      dwTriCnt(0);

    m_pIB->Lock(0, 0, (void **)&pIndex, 0);

    for (_ulong i = 0; i < m_tData.dwCountZ - 1; ++i)
    {
        for (_ulong j = 0; j < m_tData.dwCountX - 1; ++j)
        {
            dwIndex = i * m_tData.dwCountX + j;

            // 오른쪽 위
            pIndex[dwTriCnt]._0 = dwIndex + m_tData.dwCountX;
            pIndex[dwTriCnt]._1 = dwIndex + m_tData.dwCountX + 1;
            pIndex[dwTriCnt]._2 = dwIndex + 1;
            dwTriCnt++;

            // 왼쪽 아래
            pIndex[dwTriCnt]._0 = dwIndex + m_tData.dwCountX;
            pIndex[dwTriCnt]._1 = dwIndex + 1;
            pIndex[dwTriCnt]._2 = dwIndex;
            dwTriCnt++;
        }
    }

    m_pIB->Unlock();
    return S_OK;
}

CComponent *CVIBuffer_GridPanel::Clone(void *pArg)
{
    CVIBuffer_GridPanel *pInstance = new CVIBuffer_GridPanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("GridPanelBuffer Clone Failed");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CVIBuffer_GridPanel *CVIBuffer_GridPanel::Create(LPDIRECT3DDEVICE9 pGraphicDev, void *pArg)
{
    CVIBuffer_GridPanel *pGridPanel = new CVIBuffer_GridPanel(pGraphicDev);
    if (FAILED(pGridPanel->Ready_Buffer(pArg)))
    {
        Safe_Release(pGridPanel);
        MSG_BOX("GridPanelBuffer Create Failed");
        return nullptr;
    }

    return pGridPanel;
}

HRESULT CVIBuffer_GridPanel::Initialize(void *pArg)
{
    return Ready_Buffer(pArg);
}


void CVIBuffer_GridPanel::Free()
{
    CVIBuffer::Free();
}