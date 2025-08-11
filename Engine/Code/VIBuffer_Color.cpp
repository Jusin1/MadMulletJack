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

    // 정점 좌표 세팅
    VTXCOL* pVertex = nullptr;
    m_pVB->Lock(0, 0, (void**)&pVertex, 0);
    pVertex[0].vPosition = { -1.f,  1.f, 0.f };
    pVertex[1].vPosition = { 1.f,  1.f, 0.f };
    pVertex[2].vPosition = { 1.f, -1.f, 0.f };
    pVertex[3].vPosition = { -1.f, -1.f, 0.f };
    // 색은 아래 ApplyColorToVB에서 한 번에 반영
    m_pVB->Unlock();

    if (FAILED(CVIBuffer::Ready_Index_Buffer()))
        return E_FAIL;

    INDEX32* pIndex = nullptr;
    m_pIB->Lock(0, 0, (void**)&pIndex, 0);
    pIndex[0]._0 = 0; pIndex[0]._1 = 1; pIndex[0]._2 = 2;
    pIndex[1]._0 = 0; pIndex[1]._1 = 2; pIndex[1]._2 = 3;
    m_pIB->Unlock();

    // 기본 색 적용
    return ApplyColorToVB();
}

HRESULT VIBuffer_Color::Initialize(void* pArg)
{
    // pArg로 시작 색을 넘기고 싶다면 D3DXCOLOR* 사용
    if (pArg) {
        D3DXCOLOR* pCol = reinterpret_cast<D3DXCOLOR*>(pArg);
        m_Color = *pCol;
        return ApplyColorToVB();
    }
    return S_OK;
}

HRESULT VIBuffer_Color::ApplyColorToVB()
{
    if (!m_pVB) return E_FAIL;
    VTXCOL* pVertex = nullptr;
    if (FAILED(m_pVB->Lock(0, 0, (void**)&pVertex, 0)))
        return E_FAIL;

    DWORD dw = m_Color; // D3DXCOLOR → DWORD 변환자 오버로드 존재
    pVertex[0].dwColor = dw;
    pVertex[1].dwColor = dw;
    pVertex[2].dwColor = dw;
    pVertex[3].dwColor = dw;

    m_pVB->Unlock();
    return S_OK;
}

void VIBuffer_Color::SetColor(const D3DXCOLOR& color)
{
    m_Color = color;
    ApplyColorToVB();
}

void VIBuffer_Color::SetAlpha(float a)
{
    m_Color.a = a;
    ApplyColorToVB();
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