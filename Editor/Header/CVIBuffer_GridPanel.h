#pragma once
#include "Editor_Define.h"
#include "CVIBuffer.h"

class CVIBuffer_GridPanel : public CVIBuffer
{
private:
	explicit CVIBuffer_GridPanel();
	explicit CVIBuffer_GridPanel(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer_GridPanel(const CVIBuffer_GridPanel &rhs);
	virtual ~CVIBuffer_GridPanel();

	virtual void		Free();
	virtual HRESULT Initialize(void *pArg) override;
public:
	virtual CComponent *Clone(void *pArg) override;
	static CVIBuffer_GridPanel *Create(LPDIRECT3DDEVICE9 pGraphicDev, void *pArg = nullptr);
public:
	const PANELDATA *Get_Data() { return &m_tData; }
	void Set_Data(void *pData) { ::memcpy(&m_tData, pData, sizeof(PANELDATA)); }
private:
	HRESULT	Ready_Buffer(void *pArg);
	HRESULT Ready_HorizonWallBuffer();
	HRESULT Ready_VerticalWallBuffer();
	HRESULT Ready_PlaneBuffer();

private:
	PANELDATA m_tData;
};

//HRESULT CVIBuffer_GridPanel::Ready_PlaneBuffer()
//{
//    m_dwVtxSize = sizeof(VTXTEX);
//    m_dwVtxCnt = m_tData.dwCountX * m_tData.dwCountZ;
//    m_dwTriCnt = (m_tData.dwCountX - 1) * (m_tData.dwCountZ - 1) * 2;
//    m_dwFVF = FVF_TEX;
//
//    m_dwIdxSize = sizeof(INDEX32);
//    m_IdxFmt = D3DFMT_INDEX32;
//
//    if (FAILED(CVIBuffer::Ready_Vertex_Buffer()))
//        return E_FAIL;
//
//    VTXTEX *pVertex = NULL;
//
//    m_pVB->Lock(0, 0, (void **)&pVertex, 0);
//
//    _ulong  dwIndex(0);
//
//    for (_ulong i = 0; i < m_tData.dwCountZ; ++i)
//    {
//        for (_ulong j = 0; j < m_tData.dwCountX; ++j)
//        {
//            dwIndex = i * m_tData.dwCountX + j;
//
//            pVertex[dwIndex].vPosition = { (_float)j * m_tData.dwInterval,
//                                           0,
//                                           (_float)i * m_tData.dwInterval
//            };
//
//            pVertex[dwIndex].vTexUV = { (_float)j,(_float)i };
//        }
//    }
//
//    m_pVB->Unlock();
//
//    if (FAILED(CVIBuffer::Ready_Index_Buffer()))
//        return E_FAIL;
//
//    INDEX32 *pIndex = NULL;
//
//    _ulong      dwTriCnt(0);
//
//    m_pIB->Lock(0, 0, (void **)&pIndex, 0);
//
//    for (_ulong i = 0; i < m_tData.dwCountZ - 1; ++i)
//    {
//        for (_ulong j = 0; j < m_tData.dwCountX - 1; ++j)
//        {
//            dwIndex = i * m_tData.dwCountX + j;
//
//            // 오른쪽 위
//            pIndex[dwTriCnt]._0 = dwIndex + m_tData.dwCountX;
//            pIndex[dwTriCnt]._1 = dwIndex + m_tData.dwCountX + 1;
//            pIndex[dwTriCnt]._2 = dwIndex + 1;
//            dwTriCnt++;
//
//            // 왼쪽 아래
//            pIndex[dwTriCnt]._0 = dwIndex + m_tData.dwCountX;
//            pIndex[dwTriCnt]._1 = dwIndex + 1;
//            pIndex[dwTriCnt]._2 = dwIndex;
//            dwTriCnt++;
//        }
//    }
//
//    m_pIB->Unlock();
//    return S_OK;
//}