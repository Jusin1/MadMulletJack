#include "CTexture.h"
#include "CTimerMgr.h"
CTexture::CTexture(LPDIRECT3DDEVICE9 pGraphicDev)
    : CComponent(pGraphicDev)
    , m_iNumTextures(0)
    , m_vecTexture{}
    , m_TextureInfo{}
    , m_fTimeAcc(0.f)
    , m_bStopAnim(false)
{
}


CTexture::CTexture(const CTexture& rhs)
    : CComponent(rhs)
    , m_iNumTextures(rhs.m_iNumTextures)
    , m_vecTexture(rhs.m_vecTexture)
    , m_TextureInfo(rhs.m_TextureInfo)
    , m_fTimeAcc(rhs.m_fTimeAcc)
    , m_bStopAnim(rhs.m_bStopAnim)
{
    // 텍스쳐 참조 카운트 증가
    for (auto& pTex : m_vecTexture)
    {
        if (pTex)
            pTex->AddRef();
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
        HRESULT hr = eType == TEX_NORMAL ? D3DXCreateTextureFromFileEx(
            m_pGraphicDev,
            szFullPath,
            D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2,
            1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
            D3DX_FILTER_NONE, D3DX_FILTER_NONE,
            0, NULL, NULL, (LPDIRECT3DTEXTURE9*)&pTexture) : D3DXCreateCubeTextureFromFile(m_pGraphicDev, szFullPath, (LPDIRECT3DCUBETEXTURE9*)&pTexture);


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
    _uint clampedIndex = iIndex;
    if (clampedIndex >= m_vecTexture.size()) {
        clampedIndex = static_cast<_uint>(m_vecTexture.size() - 1);
    }

    m_pGraphicDev->SetTexture(0, m_vecTexture[clampedIndex]);
}

// 애니메이션 프레임 이동
void CTexture::MoveFrame()
{
    if (m_bStopAnim)
        return;

    m_fTimeAcc += CTimerMgr::GetInstance()->Get_TimeDelta(L"Timer_60");

    if (m_fTimeAcc > 1.f / m_TextureInfo.m_fSpeed)
    {
        m_fTimeAcc = 0.f;

        if (!m_TextureInfo.m_bLoop)
        {
            if (m_TextureInfo.m_iCurrentTex < m_TextureInfo.m_iEndTex)
            {
                ++m_TextureInfo.m_iCurrentTex;
            }
            // EndTex 넘으면 멈추도록 아무것도 하지 않음
        }
        else
        {
            ++m_TextureInfo.m_iCurrentTex;

            if (m_TextureInfo.m_iCurrentTex > m_TextureInfo.m_iEndTex)
                m_TextureInfo.m_iCurrentTex = m_TextureInfo.m_iStart;
        }
    }
}

// 애니메이션 프레임 설정
void CTexture::Set_Frame(int iStart, int iEnd, int iSpeed, _bool bLoop)
{
    m_TextureInfo.m_iStart = iStart;
    m_TextureInfo.m_iEndTex = iEnd;
    m_TextureInfo.m_fSpeed = iSpeed;
    m_TextureInfo.m_iCurrentTex = m_TextureInfo.m_iStart;
    m_TextureInfo.m_bLoop = bLoop;
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
