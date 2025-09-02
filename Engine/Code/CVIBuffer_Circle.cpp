#include "CVIBuffer_Circle.h"

CVIBuffer_Circle::CVIBuffer_Circle(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CVIBuffer_Circle::CVIBuffer_Circle(const CVIBuffer_Circle &rhs)
	: CVIBuffer(rhs)
{
}

CVIBuffer_Circle::~CVIBuffer_Circle()
{
}

void CVIBuffer_Circle::Free()
{
	CVIBuffer::Free();
}

CComponent *CVIBuffer_Circle::Clone(void *pArg)
{
	CVIBuffer_Circle *pInstance = new CVIBuffer_Circle(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CVIBuffer_Circle Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CVIBuffer_Circle *CVIBuffer_Circle::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CVIBuffer_Circle *pInstance = new CVIBuffer_Circle(pGraphicDev);
	if (FAILED(pInstance->Ready_Buffer()))
	{
		MSG_BOX("CVIBuffer_Circle Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

HRESULT CVIBuffer_Circle::Ready_Buffer()
{
	m_iSegments = (std::max<_uint>)(m_iSegments, 3u);

	m_dwVtxSize = sizeof(VTXCOL);
	m_dwVtxCnt = m_iSegments + 1;
	m_dwTriCnt = m_iSegments;
	m_dwFVF = FVF_COL;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	if (FAILED(CVIBuffer::Ready_Vertex_Buffer()))
		return E_FAIL;
	if (FAILED(CVIBuffer::Ready_Index_Buffer()))
		return E_FAIL;

	if (FAILED(Fill_Positions_ToVB())) return E_FAIL;
	if (FAILED(Fill_Indice_ToIB()))   return E_FAIL;
	if (FAILED(ApplyWhiteToVB()))    return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Circle::Initialize(void *pArg)
{
	if (pArg)
	{
		if (CircleBufferData *p = reinterpret_cast<CircleBufferData *>(pArg))
		{
			const _bool needRebuild = (p->iSegments != m_iSegments);
			m_iSegments = std::max<_uint>(p->iSegments, 3u);
			m_fRadius = std::max<_float>(p->fRadius, 0.001f);
			Set_Tint(p->Tint); // m_Tint/m_dwTint ¼¼ÆÃ

			if (needRebuild)
			{
				if (FAILED(Recreate_Buffers()))
					return E_FAIL;
			}
			if (FAILED(Fill_Positions_ToVB())) return E_FAIL;
		}
	}
	return S_OK;
}

void CVIBuffer_Circle::Render_Buffer()
{
	m_pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &m_prevValue);
	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, m_dwTint);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	CVIBuffer::Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, m_prevValue);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
}

HRESULT CVIBuffer_Circle::Recreate_Buffers()
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	m_dwVtxSize = sizeof(VTXCOL);
	m_dwVtxCnt = m_iSegments + 1;
	m_dwTriCnt = m_iSegments;
	m_dwFVF = FVF_COL;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	if (FAILED(CVIBuffer::Ready_Vertex_Buffer()))
		return E_FAIL;
	if (FAILED(CVIBuffer::Ready_Index_Buffer()))
		return E_FAIL;

	if (FAILED(Fill_Positions_ToVB())) return E_FAIL;
	if (FAILED(Fill_Indice_ToIB()))   return E_FAIL;
	if (FAILED(ApplyWhiteToVB()))    return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Circle::Fill_Positions_ToVB()
{
	if (!m_pVB)
		return E_FAIL;

	VTXCOL *pVertices = nullptr;

	if (FAILED(m_pVB->Lock(0, 0, (void **)&pVertices, 0)))
		return E_FAIL;

	pVertices[0].vPosition = { 0.f, 0.f, 0.f };

	const _float fStep = (2.f * D3DX_PI) / (_float)m_iSegments;
	for (_uint i = 0; i < m_iSegments; ++i)
	{
		_float fAngle = fStep * (_float)i;
		_float fX = m_fRadius * std::cos(fAngle);
		_float fY = m_fRadius * std::sin(fAngle);
		pVertices[1 + i].vPosition = { fX, fY, 0.f };
	}

	m_pVB->Unlock();
	return S_OK;
}

HRESULT CVIBuffer_Circle::Fill_Indice_ToIB()
{
	if (!m_pIB)
		return E_FAIL;
	INDEX32 *pIndices = nullptr;
	if (FAILED(m_pIB->Lock(0, 0, (void **)&pIndices, 0)))
		return E_FAIL;

	for (_uint i = 0; i < m_iSegments; ++i)
	{
		_uint i0 = 0;
		_uint i1 = 1 + i;
		_uint i2 = 1 + ((i + 1) % m_iSegments);
		pIndices[i]._0 = i0; pIndices[i]._1 = i1; pIndices[i]._2 = i2;
	}

	m_pIB->Unlock();
	return S_OK;
}

HRESULT CVIBuffer_Circle::ApplyWhiteToVB()
{
	if (!m_pVB)
		return E_FAIL;

	VTXCOL *pV = nullptr;

	if (FAILED(m_pVB->Lock(0, 0, (void **)&pV, 0)))
		return E_FAIL;

	const DWORD white = D3DCOLOR_ARGB(255, 255, 255, 255);
	for (_uint i = 0; i < m_dwVtxCnt; ++i)
		pV[i].dwColor = white;

	m_pVB->Unlock();
	return S_OK;
}

void CVIBuffer_Circle::Set_Tint(const D3DXCOLOR &_c)
{
	m_Tint = _c;
	m_dwTint = m_Tint;
}

void CVIBuffer_Circle::Set_Radius(_float fRadius)
{
	m_fRadius = (std::max<_float>)(fRadius, 0.001f);
	Fill_Positions_ToVB();
}

void CVIBuffer_Circle::Set_Segments(_uint _i)
{
	_i = std::max<_uint>(_i, 3u);
	if (_i == m_iSegments)
		return;
	m_iSegments = _i;
	Recreate_Buffers();
}
