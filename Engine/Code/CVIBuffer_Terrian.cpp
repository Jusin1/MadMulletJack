#include "CVIBuffer_Terrian.h"

CVIBuffer_Terrian::CVIBuffer_Terrian()
{
}

CVIBuffer_Terrian::CVIBuffer_Terrian(LPDIRECT3DDEVICE9 pGraphicDev)
    : CVIBuffer(pGraphicDev), m_hFile(nullptr), m_pPos(nullptr)
{
}

CVIBuffer_Terrian::CVIBuffer_Terrian(const CVIBuffer_Terrian& rhs)
    : CVIBuffer(rhs), m_hFile(rhs.m_hFile), m_fh(rhs.m_fh), m_ih(rhs.m_ih)
    , m_pPos(rhs.m_pPos)
{
}

CVIBuffer_Terrian::~CVIBuffer_Terrian()
{
}

HRESULT CVIBuffer_Terrian::Ready_Buffer(const _ulong& dwCntX,
    const _ulong& dwCntZ,
    const _ulong& dwVtxItv)
{

    m_dwVtxSize = sizeof(VTXTEX);
    m_dwVtxCnt = dwCntX * dwCntZ;
    m_dwTriCnt = (dwCntX - 1) * (dwCntZ - 1) * 2;
    m_dwFVF = FVF_TEX;

    m_pPos = new _vec3[m_dwVtxCnt];

    m_dwIdxSize = sizeof(INDEX32);
    m_IdxFmt = D3DFMT_INDEX32;

    if (FAILED(CVIBuffer::Ready_Vertex_Buffer()))
        return E_FAIL;


    m_hFile = CreateFile(L"../Bin/Resource/Texture/Terrain/Height.bmp", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == m_hFile)
        return E_FAIL;

    _ulong  dwByte(0);

    ReadFile(m_hFile, &m_fh, sizeof(BITMAPFILEHEADER), &dwByte, NULL);
    ReadFile(m_hFile, &m_ih, sizeof(BITMAPINFOHEADER), &dwByte, NULL);

    _ulong* pPixel = new _ulong[m_ih.biWidth * m_ih.biHeight];

    ReadFile(m_hFile, pPixel, sizeof(_ulong) * m_ih.biWidth * m_ih.biHeight, &dwByte, NULL);

    CloseHandle(m_hFile);


    VTXTEX* pVertex = NULL;

    // 3인자 : 버텍스 버퍼에 저장된 정점들 중 첫 번째 정점의 주소를 얻어옴
    m_pVB->Lock(0, 0, (void**)&pVertex, 0);

    _ulong  dwIndex(0);

    for (_ulong i = 0; i < dwCntZ; ++i)
    {
        for (_ulong j = 0; j < dwCntX; ++j)
        {
            dwIndex = i * dwCntX + j;

            pVertex[dwIndex].vPosition = { (_float)j * dwVtxItv,
                                           _float(pPixel[dwIndex] & 0x000000ff) / 20.f,
                                           (_float)i * dwVtxItv 
                                          };

            pVertex[dwIndex].vTexUV = { 
                                        (_float)j / (dwCntX - 1) * 20.f, 
                                        (_float)i / (dwCntZ - 1) * 20.f
                                      };

            m_pPos[dwIndex] = pVertex[dwIndex].vPosition;

        }
    }

    Safe_Delete_Array(pPixel);


    m_pVB->Unlock();

    if (FAILED(CVIBuffer::Ready_Index_Buffer()))
        return E_FAIL;

    INDEX32* pIndex = NULL;

    _ulong      dwTriCnt(0);

    m_pIB->Lock(0, 0, (void**)&pIndex, 0);

    for (_ulong i = 0; i < dwCntZ - 1; ++i)
    {
        for (_ulong j = 0; j < dwCntX - 1; ++j)
        {
            dwIndex = i * dwCntX + j;

            // 오른쪽 위
            pIndex[dwTriCnt]._0 = dwIndex + dwCntX;
            pIndex[dwTriCnt]._1 = dwIndex + dwCntX + 1;
            pIndex[dwTriCnt]._2 = dwIndex + 1;
            dwTriCnt++;

            // 왼쪽 아래
            pIndex[dwTriCnt]._0 = dwIndex + dwCntX;
            pIndex[dwTriCnt]._1 = dwIndex + 1;
            pIndex[dwTriCnt]._2 = dwIndex;
            dwTriCnt++;
        }
    }


   

    m_pIB->Unlock();


    return S_OK;
}

HRESULT CVIBuffer_Terrian::Initialize(void* pArg)
{
    return S_OK;
}

CVIBuffer_Terrian* CVIBuffer_Terrian::Create(LPDIRECT3DDEVICE9 pGraphicDev,
    const _ulong& dwCntX,
    const _ulong& dwCntZ,
    const _ulong& dwVtxItv)
{
    CVIBuffer_Terrian* pTerrainTex = new CVIBuffer_Terrian(pGraphicDev);

    if (FAILED(pTerrainTex->Ready_Buffer(dwCntX, dwCntZ, dwVtxItv)))
    {
        Safe_Release(pTerrainTex);
        MSG_BOX("pTerrainTex Create Failed");
        return nullptr;
    }

    return pTerrainTex;
}


CComponent* CVIBuffer_Terrian::Clone(void* pArg)
{
    CVIBuffer_Terrian* pInstance = new CVIBuffer_Terrian(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("pTerrainTex Clone Failed");
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CVIBuffer_Terrian::Free()
{
    if (false == m_bClone)
    {
        Safe_Delete_Array(m_pPos);
    }

    CVIBuffer::Free();
}
