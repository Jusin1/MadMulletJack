#include "CTexture.h"
#include "CTimerMgr.h"

#include <process.h>
#include <atomic>

// 간단 파일 읽기
static bool ReadAllBytes(const wchar_t* path, std::vector<BYTE>& out)
{
    HANDLE h = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER sz{};
    if (!GetFileSizeEx(h, &sz) || sz.QuadPart <= 0) { CloseHandle(h); return false; }

    out.resize((size_t)sz.QuadPart);
    DWORD done = 0, total = 0;
    while (total < sz.QuadPart) {
        DWORD toRead = (DWORD)min<ULONGLONG>(sz.QuadPart - total, 1 << 20); // 1MB 청크
        DWORD rd = 0;
        if (!ReadFile(h, out.data() + total, toRead, &rd, nullptr)) { CloseHandle(h); return false; }
        if (rd == 0) break;
        total += rd;
    }
    CloseHandle(h);
    return total == sz.QuadPart;
}
CTexture::CTexture(LPDIRECT3DDEVICE9 pGraphicDev)
    : CComponent(pGraphicDev)
    , m_iNumTextures(0)
    , m_vecTexture{}
    , m_TextureInfo{}
    , m_fTimeAcc(0.f)
    , m_bStopAnim(false)
    , m_OriginComponentName(L"")
{
}


CTexture::CTexture(const CTexture& rhs)
    : CComponent(rhs)
    , m_iNumTextures(rhs.m_iNumTextures)
    , m_vecTexture(rhs.m_vecTexture)
    , m_TextureInfo(rhs.m_TextureInfo)
    , m_fTimeAcc(rhs.m_fTimeAcc)
    , m_bStopAnim(rhs.m_bStopAnim)
    , m_OriginComponentName(rhs.m_OriginComponentName)
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
    // 큐브/기타는 기존대로
    if (eType != TEX_NORMAL) {
        m_iNumTextures = iCnt;
        m_vecTexture.clear(); m_vecTexture.reserve(iCnt);
        _tchar full[MAX_PATH] = TEXT("");
        for (_uint i = 0; i < iCnt; ++i) {
            IDirect3DBaseTexture9* pTex = nullptr;
            wsprintf(full, pPath, i);
            if (FAILED(D3DXCreateCubeTextureFromFile(m_pGraphicDev, full, (LPDIRECT3DCUBETEXTURE9*)&pTex)))
                return E_FAIL;
            m_vecTexture.push_back(pTex);
        }
        return S_OK;
    }

    // ===== PNG 애니: 완전 병렬 로딩(파일 읽기 + 텍스처 생성까지 워커에서) =====
    m_iNumTextures = iCnt;
    m_vecTexture.clear();
    m_vecTexture.resize(iCnt, nullptr);  // ★ 인덱스별로 채운다(push_back 금지)

    SYSTEM_INFO si; GetSystemInfo(&si);
    unsigned cores = si.dwNumberOfProcessors ? si.dwNumberOfProcessors : 4;
    unsigned workers = (cores > 1) ? cores - 1 : 1;   // 코어-1 권장
    if (workers > 8) workers = 8;                     // 과도 생성 방지

    std::atomic<unsigned> next(0);

    struct Ctx {
        CTexture* self;
        const _tchar* fmt;
        std::atomic<unsigned>* pNext;
    } ctx{ this, pPath, &next };

    auto __stdcall Worker = [](void* p)->unsigned {
        Ctx* c = (Ctx*)p;
        _tchar path[MAX_PATH];

        for (;;) {
            unsigned i = c->pNext->fetch_add(1);
            if (i >= c->self->m_iNumTextures) break;

            wsprintf(path, c->fmt, i);

            // 1) 파일 읽기
            std::vector<BYTE> bytes;
            if (!ReadAllBytes(path, bytes)) return 0; // 실패 시 그냥 빠짐(아래서 체크)

            // 2) 텍스처 생성 (병렬, Device는 MULTITHREADED 필수)
            IDirect3DBaseTexture9* pTex = nullptr;
            HRESULT hr = D3DXCreateTextureFromFileInMemoryEx(
                c->self->m_pGraphicDev,
                bytes.data(), (UINT)bytes.size(),
                D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2,
                1, 0,
                D3DFMT_A8R8G8B8,            // 변환 비용 최소화
                D3DPOOL_MANAGED,
                D3DX_FILTER_NONE, D3DX_FILTER_NONE,
                0, nullptr, nullptr,
                (LPDIRECT3DTEXTURE9*)&pTex);

            if (SUCCEEDED(hr)) {
                c->self->m_vecTexture[i] = pTex;   // 자기 인덱스에 저장
            }
        }
        return 0;
        };

    std::vector<HANDLE> th; th.reserve(workers);
    for (unsigned t = 0; t < workers; ++t) {
        uintptr_t h = _beginthreadex(nullptr, 0, Worker, &ctx, 0, nullptr);
        if (h) { SetThreadPriority((HANDLE)h, THREAD_PRIORITY_BELOW_NORMAL); th.push_back((HANDLE)h); }
    }
    if (!th.empty()) {
        WaitForMultipleObjects((DWORD)th.size(), th.data(), TRUE, INFINITE);
        for (HANDLE h : th) CloseHandle(h);
    }

    // 실패한 슬롯이 있으면 에러
    for (auto* tex : m_vecTexture) if (!tex) return E_FAIL;

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

bool CTexture::GetFrameSize(UINT index, UINT& w, UINT& h) const
{
    if (index >= m_vecTexture.size())
        return false;

    LPDIRECT3DTEXTURE9 pTex = nullptr;
    if (FAILED(m_vecTexture[index]->QueryInterface(IID_IDirect3DTexture9, (void**)&pTex)) || !pTex)
        return false;

    D3DSURFACE_DESC desc{};
    pTex->GetLevelDesc(0, &desc);
    Safe_Release(pTex);

    w = desc.Width;
    h = desc.Height;
    return true;
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
