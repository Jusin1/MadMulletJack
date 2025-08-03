#include "CTexture.h"
#include "CTimerMgr.h"
CTexture::CTexture(LPDIRECT3DDEVICE9 pGraphicDev)
    : CComponent(pGraphicDev)
{
}

CTexture::CTexture(const CTexture& rhs)
    : CComponent(rhs)
{
    size_t   iContainerSize = rhs.m_vecTexture.size();

    m_vecTexture.reserve(iContainerSize);

    m_vecTexture = rhs.m_vecTexture;

    // 모든 텍스쳐 참조 증가
    for (size_t i = 0; i < iContainerSize; ++i)
    {
        m_vecTexture[i]->AddRef();
    }
}

CTexture::~CTexture()
{

}

// 텍스쳐 로딩 함수
HRESULT CTexture::Ready_Texture(TEXTUREID eType,
                                const _tchar* pPath,
                                const _uint& iCnt)
{
    m_iNumTextures = iCnt;

    _tchar	szFullPath[MAX_PATH] = TEXT("");

    for (_uint i = 0; i < m_iNumTextures; ++i)
    {
        IDirect3DBaseTexture9* pTexture = nullptr;

        wsprintf(szFullPath, pPath, i); // 경로 내 인덱스 적용

        // 2D or Cube 텍스쳐 생성
        HRESULT hr = eType == TEX_NORMAL ? D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, (LPDIRECT3DTEXTURE9*)&pTexture) : D3DXCreateCubeTextureFromFile(m_pGraphicDev, szFullPath, (LPDIRECT3DCUBETEXTURE9*)&pTexture);

        if (FAILED(hr))
            return E_FAIL;

        m_vecTexture.push_back(pTexture);
    }

    return S_OK;
}

// 텍스쳐 초기화(프레임 정보 적용)
HRESULT CTexture::Initialize(void* pArg)
{
    if (pArg != nullptr)
    {
        memcpy(&m_TextureInfo, pArg, sizeof(TEXINFO));
        m_TextureInfo.m_iCurrentTex = m_TextureInfo.m_iStart;
    }

    return S_OK;
}

// 텍스쳐 바인딩
void CTexture::Set_Texture(const _uint& iIndex)
{
    if (m_vecTexture.size() < iIndex)
        return;

    m_pGraphicDev->SetTexture(0, m_vecTexture[iIndex]);
}

// 애니메이션 프레임 이동
void CTexture::MoveFrame(const _tchar* timeTag)
{
    if (m_bStopAnim)
        return;

    m_fTimeAcc += CTimerMgr::GetInstance()->Get_TimeDelta(L"Timer_60");

    if (m_fTimeAcc > 1.f / m_TextureInfo.m_fSpeed)
    {
        m_TextureInfo.m_iCurrentTex++;

        if (m_TextureInfo.m_iCurrentTex >= m_TextureInfo.m_iEndTex)
            m_TextureInfo.m_iCurrentTex = m_TextureInfo.m_iStart;

        m_fTimeAcc = 0.f;
    }
}

// 애니메이션 프레임 설정
void CTexture::Set_Frame(int iStart, int iEnd, int iSpeed)
{
    m_TextureInfo.m_iStart = iStart;
    m_TextureInfo.m_iEndTex = iEnd;
    m_TextureInfo.m_fSpeed = iSpeed;
    m_TextureInfo.m_iCurrentTex = m_TextureInfo.m_iStart;
}


CTexture* CTexture::Create(LPDIRECT3DDEVICE9 pGraphicDev, TEXTUREID eType, const _tchar* pPath, const _uint& iCnt)
{
    CTexture* pTexture = new CTexture(pGraphicDev);

    if (FAILED(pTexture->Ready_Texture(eType, pPath, iCnt)))
    {
        Safe_Release(pTexture);
        MSG_BOX("pTexture Create Failed");
        return nullptr;
    }

    return pTexture;
}

CComponent* CTexture::Clone(void* pArg)
{
    CTexture* pInstance = new CTexture(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("pTexture Clone Failed");
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CTexture::Free()
{
    for (size_t i = 0; i < m_vecTexture.size(); ++i)
    {
        Safe_Release(m_vecTexture[i]);
    }   

    m_vecTexture.clear();

    CComponent::Free();
}
