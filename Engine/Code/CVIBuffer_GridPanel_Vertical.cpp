#include "Engine_Define.h"
#include "CVIBuffer_GridPanel_Vertical.h"

CVIBuffer_GridPanel_Vertical::CVIBuffer_GridPanel_Vertical(LPDIRECT3DDEVICE9 pGraphicDev)
    : CVIBuffer_GridPanelBase(pGraphicDev)
{
}

CVIBuffer_GridPanel_Vertical::CVIBuffer_GridPanel_Vertical(const CVIBuffer_GridPanel_Vertical &rhs)
    : CVIBuffer_GridPanelBase(rhs)
{
}

CVIBuffer_GridPanel_Vertical::~CVIBuffer_GridPanel_Vertical()
{
}

void CVIBuffer_GridPanel_Vertical::Free()
{
    CVIBuffer_GridPanelBase::Free();
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
    return __super::Picking(pTransform, pOut);
}

_bool CVIBuffer_GridPanel_Vertical::IntersectRay(_vec3 *pOut)
{
    return __super::IntersectRay(pOut);
}

HRESULT CVIBuffer_GridPanel_Vertical::Ready_Buffer(void *pArg)
{
    if (FAILED(__super::Ready_Buffer(pArg)))
        return E_FAIL;

    if (FAILED(Set_Buffer()))
    {
        MSG_BOX("CVIBuffer_GridPanel_Vertical::Ready_Buffer, Failed");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CVIBuffer_GridPanel_Vertical::Set_Buffer()
{
    if (FAILED(__super::Set_Buffer()))
        return E_FAIL;

    m_dwVtxSize = sizeof(VTXTEX);
    m_dwVtxCnt = m_tData.dwCountZ * m_tData.dwCountY;
    m_dwTriCnt = (m_tData.dwCountZ - 1) * (m_tData.dwCountY - 1) * 2;
    m_dwFVF = FVF_TEX;

    m_dwIdxSize = sizeof(INDEX32);
    m_IdxFmt = D3DFMT_INDEX32;

    m_iColMax = m_tData.dwCountZ;
    m_iRowMax = m_tData.dwCountY;

    if (FAILED(CVIBuffer::Ready_Vertex_Buffer()))
        return E_FAIL;

    VTXTEX *pVertex = NULL;

    m_pVerticesData = new _vec3[m_iRowMax * m_iColMax];

    m_pVB->Lock(0, 0, (void **)&pVertex, 0);

    _ulong  dwIndex(0);

    for (_ulong i = 0; i < m_iRowMax; ++i)
    {
        for (_ulong j = 0; j < m_iColMax; ++j)
        {
            dwIndex = i * m_iColMax + j;

            pVertex[dwIndex].vPosition = { 0,
                                       (_float)i *m_tData.dwInterval,
                                       (_float)j *m_tData.dwInterval };

            pVertex[dwIndex].vTexUV = { (_float)j, (_float)(m_iRowMax - 1 - i) };

            m_pVerticesData[dwIndex] = pVertex[dwIndex].vPosition;
        }
    }

    m_pVB->Unlock();

    if (FAILED(CVIBuffer::Ready_Index_Buffer()))
        return E_FAIL;

    INDEX32 *pIndex = NULL;

    _ulong      dwTriCnt(0);

    m_pIB->Lock(0, 0, (void **)&pIndex, 0);

    for (_ulong i = 0; i < m_iRowMax - 1; ++i)
    {
        for (_ulong j = 0; j < m_iColMax - 1; ++j)
        {
            dwIndex = i * m_iColMax + j;

            pIndex[dwTriCnt]._0 = dwIndex + m_iColMax;
            pIndex[dwTriCnt]._1 = dwIndex + m_iColMax + 1;
            pIndex[dwTriCnt]._2 = dwIndex + 1;
            dwTriCnt++;

            pIndex[dwTriCnt]._0 = dwIndex + m_iColMax;
            pIndex[dwTriCnt]._1 = dwIndex + 1;
            pIndex[dwTriCnt]._2 = dwIndex;
            dwTriCnt++;
        }
    }

    m_pIB->Unlock();
    return S_OK;
}
