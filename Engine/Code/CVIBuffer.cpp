#include "CVIBuffer.h"

CVIBuffer::CVIBuffer()
	: m_dwVtxSize(0),m_dwVtxCnt(0), m_dwTriCnt(0), m_dwFVF(0)
	, m_pVB(nullptr), m_pIB(nullptr), m_dwIdxSize(0)
{
}

CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
	, m_dwVtxSize(0), m_dwVtxCnt(0), m_dwTriCnt(0), m_dwFVF(0)
	, m_pVB(nullptr), m_pIB(nullptr), m_dwIdxSize(0)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	:CComponent(rhs)
	, m_dwVtxSize(rhs.m_dwVtxSize), m_dwVtxCnt(rhs.m_dwVtxCnt),
	m_dwTriCnt(rhs.m_dwTriCnt), m_dwFVF(rhs.m_dwFVF)
	, m_pVB(rhs.m_pVB), m_pIB(rhs.m_pIB), m_dwIdxSize(rhs.m_dwIdxSize)
	, m_IdxFmt(rhs.m_IdxFmt)
{
	m_pVB->AddRef();
	m_pIB->AddRef();
}

CVIBuffer::~CVIBuffer()
{
}

HRESULT CVIBuffer::Ready_Buffer()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void* pArg)
{
	return S_OK;
}

void CVIBuffer::Render_Buffer()
{
	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);
	
	m_pGraphicDev->SetFVF(m_dwFVF);

	m_pGraphicDev->SetIndices(m_pIB);

	m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVtxCnt, 0, m_dwTriCnt);
}

HRESULT CVIBuffer::Ready_Vertex_Buffer()
{
	if (FAILED(m_pGraphicDev->CreateVertexBuffer(m_dwVtxCnt * m_dwVtxSize, 0, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, NULL)))
		return E_FAIL;

	return S_OK;;
}

HRESULT CVIBuffer::Ready_Index_Buffer()
{
	if (FAILED(m_pGraphicDev->CreateIndexBuffer(m_dwIdxSize * m_dwTriCnt, 0, m_IdxFmt, D3DPOOL_MANAGED, &m_pIB, NULL)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer::Free()
{
	CComponent::Free();

	m_pGraphicDev->SetStreamSource(0, nullptr, 0, 0);
	m_pGraphicDev->SetIndices(nullptr);

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
