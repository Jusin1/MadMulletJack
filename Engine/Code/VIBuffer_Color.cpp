#include "VIBuffer_Color.h"

VIBuffer_Color::VIBuffer_Color(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

VIBuffer_Color::VIBuffer_Color(const VIBuffer_Color& rhs)
	: CVIBuffer(rhs)
{
}

VIBuffer_Color::~VIBuffer_Color()
{
}

HRESULT VIBuffer_Color::Ready_Buffer()
{
    m_dwVtxSize = sizeof(VTXCOL);
    m_dwVtxCnt = 4;
    m_dwTriCnt = 2;
    m_dwFVF = FVF_COL;

    m_dwIdxSize = sizeof(INDEX32);
    m_IdxFmt = D3DFMT_INDEX32;

    if (FAILED(CVIBuffer::Ready_Vertex_Buffer()))
        return E_FAIL;

    VTXCOL* pVertex = NULL;

    // 3인자 : 버텍스 버퍼에 저장된 정점들 중 첫 번째 정점의 주소를 얻어옴
    m_pVB->Lock(0, 0, (void**)&pVertex, 0);

    // 오른쪽 위
    pVertex[0].vPosition = { -1.f, 1.f, 0.f };
    pVertex[0].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

    pVertex[1].vPosition = { 1.f, 1.f, 0.f };
    pVertex[1].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

    pVertex[2].vPosition = { 1.f, -1.f, 0.f };
    pVertex[2].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

    pVertex[3].vPosition = { -1.f, -1.f, 0.f };
    pVertex[3].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

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

HRESULT VIBuffer_Color::Initialize(void* pArg)
{
	return S_OK;
}

CComponent* VIBuffer_Color::Clone(void* pArg)
{
    VIBuffer_Color* pInstance = new VIBuffer_Color(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("VIBuffer_Color Clone Failed");
        Safe_Release(pInstance);
    }

    return pInstance;
}

VIBuffer_Color* VIBuffer_Color::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    VIBuffer_Color* pRcCol = new VIBuffer_Color(pGraphicDev);

    if (FAILED(pRcCol->Ready_Buffer()))
    {
        Safe_Release(pRcCol);
        MSG_BOX("VIBuffer_Color Create Failed");
        return nullptr;
    }

    return pRcCol;
}

void	VIBuffer_Color::Free()
{
    CVIBuffer::Free();
}