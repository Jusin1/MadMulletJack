#include "pch.h"
#include "CVideo.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mmreg.h>      // WAVEFORMATEX
#include <mmsystem.h>   // waveOut
#include <algorithm>
#include <cmath>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "winmm.lib") 

#ifndef Safe_Release
#define Safe_Release(p) do { if(p){ (p)->Release(); (p)=nullptr; } } while(0)
#endif

long CVideo::s_mfRef = 0;
long CVideo::s_comRef = 0;

CVideo::CVideo(LPDIRECT3DDEVICE9 pGraphicDev) : CUI(pGraphicDev) {}
CVideo::CVideo(const CVideo& rhs) : CUI(rhs) {}
CVideo::~CVideo() { Free(); }

HRESULT CVideo::Ready_GameObject()
{
    // CUI가 버퍼/트랜스폼/렌더러 준비
    return S_OK;
}

HRESULT CVideo::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg))) return E_FAIL;

    const wchar_t* path = reinterpret_cast<const wchar_t*>(pArg);
    if (!path) return E_FAIL;

    // COM
    if (s_comRef++ == 0) {
        HRESULT hrCI = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (hrCI != S_OK && hrCI != S_FALSE && hrCI != RPC_E_CHANGED_MODE) {
            --s_comRef; return hrCI;
        }
    }

    // MF
    if (s_mfRef++ == 0) {
        HRESULT hr = MFStartup(MF_VERSION, MFSTARTUP_FULL);
        if (FAILED(hr)) { --s_mfRef; return hr; }
    }

    // Reader 열기(비디오 RGB32, 오디오는 PCM 요청)
    HRESULT hr = openReader(path);
    if (FAILED(hr)) return hr;

    // 오디오 초기화(실패해도 영상만 재생 가능)
    initAudio();

    // UI 크기 미지정이면 원본 영상 크기 사용
    if (m_fSizeX <= 0.f || m_fSizeY <= 0.f)
        Set_UISize((float)m_texW, (float)m_texH);

    Play();
    return S_OK;
}

_int CVideo::Update_GameObject(const _float& fTimeDelta)
{
    if (m_playing && !m_eos)
    {
        // 누적 시간
        m_accumSec += (double)fTimeDelta;

        // 프레임 지속시간만큼 누적될 때만 업데이트
        while (m_accumSec >= m_frameDurSec)
        {
            m_playPosSec += m_frameDurSec;
            m_accumSec -= m_frameDurSec;

            decodeToTime(m_playPosSec);
        }

        pumpAudio();
    }

    return __super::Update_GameObject(fTimeDelta);
}

void CVideo::LateUpdate_GameObject(const _float& fTimeDelta)
{
    __super::LateUpdate_GameObject(fTimeDelta);
}

void CVideo::Render_GameObject()
{
    if (!m_bActive || m_bDead || !m_tex) return;

    m_pTransformCom->Apply_WorldMatrix();

    _matrix View; D3DXMatrixIdentity(&View);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetTransform(D3DTS_VIEW, &View);
    m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

    if (m_keepAspect) applyLetterboxScale();

    m_pGraphicDev->SetTexture(0, m_tex);
    if (m_pVIBufferCom) m_pVIBufferCom->Render_Buffer();
    m_pGraphicDev->SetTexture(0, nullptr);

    if (m_keepAspect) restoreOriginalScale();

    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    // 자식 UI
    for (auto& c : m_vecChildren) {
        if (c && c->Is_Active() && !c->Get_Dead() && c->Get_RenderOn())
            c->Render_GameObject();
    }
}

// ===== Control =====
void CVideo::Play()
{
    m_eos = false;
    seekToStart();
    m_playPosSec = 0.0;
    m_lastVidSec = 0.0;
    m_accumSec = 0.0;   // ? 초기화

    m_playing = true;
    startAudio();
}

void CVideo::Pause(bool pause)
{
    m_playing = !pause;
    if (pause) stopAudio(); else startAudio();
}

void CVideo::Stop()
{
    m_playing = false;
    m_eos = true;
    stopAudio();
}

// ===== MF Reader =====
HRESULT CVideo::openReader(const wchar_t* filePath)
{
    if (GetFileAttributesW(filePath) == INVALID_FILE_ATTRIBUTES)
        return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

    IMFAttributes* attr = nullptr;
    MFCreateAttributes(&attr, 2);
    attr->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, TRUE);
    attr->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);

    HRESULT hr = MFCreateSourceReaderFromURL(filePath, attr, &m_reader);
    Safe_Release(attr);
    if (FAILED(hr)) return hr;

    // 관심 스트림만
    m_reader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, FALSE);
    m_reader->SetStreamSelection(MF_SOURCE_READER_FIRST_VIDEO_STREAM, TRUE);
    m_reader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);

    // 비디오: RGB32
    {
        IMFMediaType* out = nullptr;
        hr = MFCreateMediaType(&out); if (FAILED(hr)) return hr;
        out->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
        out->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
        hr = m_reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, out);
        Safe_Release(out);
        if (FAILED(hr)) return hr;

        IMFMediaType* cur = nullptr;
        hr = m_reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, &cur);
        if (FAILED(hr)) return hr;

        UINT32 w = 0, h = 0; MFGetAttributeSize(cur, MF_MT_FRAME_SIZE, &w, &h);
        m_texW = w; m_texH = h;

        UINT32 frNum = 0, frDen = 0;
        if (SUCCEEDED(MFGetAttributeRatio(cur, MF_MT_FRAME_RATE, &frNum, &frDen)) && frNum && frDen)
            m_frameDurSec = (double)frDen / (double)frNum;
        else
            m_frameDurSec = 1.0 / 30.0;

        Safe_Release(cur);

        if (FAILED(createTexture(m_texW, m_texH))) return E_FAIL;
    }

    // 오디오: PCM 요청(실패해도 무시)
    {
        IMFMediaType* aout = nullptr;
        if (SUCCEEDED(MFCreateMediaType(&aout))) {
            aout->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
            aout->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
            m_reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, aout);
            Safe_Release(aout);
        }
    }

    return S_OK;
}

HRESULT CVideo::seekToStart()
{
    PROPVARIANT var; PropVariantInit(&var);
    var.vt = VT_I8; var.hVal.QuadPart = 0;
    HRESULT hr = m_reader->SetCurrentPosition(GUID_NULL, var);
    PropVariantClear(&var);

    resetAudioQueue();
    m_lastAudEndSec = 0.0;
    m_lastVidSec = 0.0;

    // waveOut 포지션 0이 가리키는 기준을 현재 재생 시각으로
    m_audioBaseSec = m_playPosSec;

    return hr;
}

// ===== Video =====
HRESULT CVideo::createTexture(UINT w, UINT h)
{
    Safe_Release(m_tex);
    return m_pGraphicDev->CreateTexture(
        w, h, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_tex, nullptr);
}

HRESULT CVideo::decodeToTime(double targetSec)
{
    const int kMaxDrain = 12;
    IMFSample* lastSample = nullptr;
    LONGLONG   lastTS100 = 0;

    for (int i = 0; i < kMaxDrain; ++i) {
        DWORD flags = 0, stream = 0; LONGLONG ts = 0;
        IMFSample* sample = nullptr;
        HRESULT hr = m_reader->ReadSample(
            MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &stream, &flags, &ts, &sample);
        if (FAILED(hr)) { Safe_Release(lastSample); return hr; }

        if (flags & MF_SOURCE_READERF_ENDOFSTREAM) {
            Safe_Release(sample);
            m_eos = true; m_playing = false;
            if (m_loop) { seekToStart(); m_eos = false; m_playing = true; }
            break;
        }

        if (!sample) break;

        // 프레임 지속시간 갱신(가끔 유효)
        LONGLONG dur100 = 0;
        if (SUCCEEDED(sample->GetSampleDuration(&dur100)) && dur100 > 0)
            m_frameDurSec = (double)dur100 / 10000000.0;

        const double ssec = (double)ts / 10000000.0;
        if (ssec + (m_frameDurSec * 0.5) < targetSec) {
            Safe_Release(sample);
            continue; // 과거 프레임 드랍
        }

        Safe_Release(lastSample);
        lastSample = sample;
        lastTS100 = ts;

        if (ssec >= (targetSec - m_frameDurSec * 0.25))
            break;
    }

    if (lastSample) {
        HRESULT hr = uploadSampleToTexture(lastSample);
        Safe_Release(lastSample);
        if (SUCCEEDED(hr)) m_lastVidSec = (double)lastTS100 / 10000000.0;
        return hr;
    }
    return S_OK;
}

HRESULT CVideo::uploadSampleToTexture(IMFSample* sample)
{
    IMFMediaBuffer* buffer = nullptr;
    HRESULT hr = sample->ConvertToContiguousBuffer(&buffer);
    if (FAILED(hr)) return hr;

    BYTE* pData = nullptr; DWORD maxLen = 0, curLen = 0;
    hr = buffer->Lock(&pData, &maxLen, &curLen);
    if (FAILED(hr) || !pData) { Safe_Release(buffer); return hr; }

    D3DLOCKED_RECT lr{};
    if (SUCCEEDED(m_tex->LockRect(0, &lr, nullptr, D3DLOCK_DISCARD))) {
        const int srcPitch = (int)m_texW * 4;
        const int rows = (int)m_texH;
        const int pitch = (std::min)(srcPitch, (int)lr.Pitch);

        BYTE* dst = (BYTE*)lr.pBits;
        const BYTE* src = pData;
        for (int y = 0; y < rows; ++y) {
            memcpy(dst, src, pitch);
            dst += lr.Pitch;
            src += srcPitch;
        }
        m_tex->UnlockRect(0);
    }

    buffer->Unlock();
    Safe_Release(buffer);
    return S_OK;
}

// ===== Audio (waveOut) =====
HRESULT CVideo::initAudio()
{
    IMFMediaType* cur = nullptr;
    if (FAILED(m_reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &cur)))
        return E_FAIL;

    UINT32 ch = 0, rate = 0, bps = 0;
    cur->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &ch);
    cur->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &rate);
    cur->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, &bps);
    Safe_Release(cur);

    if (!ch || !rate || !bps) return E_FAIL;

    m_aCh = ch;
    m_aRate = rate;
    m_aBits = bps;
    m_aBlock = (UINT32)(ch * (bps / 8));
    m_aBytes = (UINT32)(rate * m_aBlock);

    WAVEFORMATEX wfx{};
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = (WORD)m_aCh;
    wfx.nSamplesPerSec = m_aRate;
    wfx.wBitsPerSample = (WORD)m_aBits;
    wfx.nBlockAlign = (WORD)m_aBlock;
    wfx.nAvgBytesPerSec = m_aBytes;

    HWAVEOUT hwo = nullptr;
    if (waveOutOpen(&hwo, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR)
        return E_FAIL;

    m_hWaveOut = hwo;

    // 약 50ms 블록 × 4
    m_blocks.clear();
    m_blocks.resize(kNumBlocks);
    m_blockBytes = (std::max)(m_aBytes / 20, m_aBlock); // ~50ms, blockAlign 보정

    for (int i = 0; i < kNumBlocks; ++i) {
        m_blocks[i].data.resize(m_blockBytes, 0);
        m_blocks[i].hdr = new WAVEHDR{};
        auto* hdr = reinterpret_cast<WAVEHDR*>(m_blocks[i].hdr);
        hdr->lpData = reinterpret_cast<LPSTR>(m_blocks[i].data.data());
        hdr->dwBufferLength = m_blockBytes;
        waveOutPrepareHeader(hwo, hdr, sizeof(WAVEHDR));
        m_blocks[i].prepared = true;
    }
    m_blockIndex = 0;

    return S_OK;
}

void CVideo::startAudio()
{
    if (!m_hWaveOut) return;
    waveOutRestart(reinterpret_cast<HWAVEOUT>(m_hWaveOut));
}

void CVideo::stopAudio()
{
    if (!m_hWaveOut) return;
    waveOutPause(reinterpret_cast<HWAVEOUT>(m_hWaveOut));
}

void CVideo::resetAudioQueue()
{
    m_audioPending.clear();
    if (m_hWaveOut) waveOutReset(reinterpret_cast<HWAVEOUT>(m_hWaveOut));
    // waveOut 포지션 0의 기준을 현재 영상 시각으로 재설정
    m_audioBaseSec = m_playPosSec;
}

void CVideo::pumpAudio()
{
    if (!m_hWaveOut || !m_playing) return;
    auto* hwo = reinterpret_cast<HWAVEOUT>(m_hWaveOut);

    // ---- A) 실제 오디오 재생 시각(audioPlayedSec) 산출 ----
    double audioPlayedSec = 0.0;
    {
        MMTIME mm{}; mm.wType = TIME_BYTES;
        if (waveOutGetPosition(hwo, &mm, sizeof(mm)) == MMSYSERR_NOERROR) {
            if (mm.wType == TIME_BYTES && m_aBytes > 0)
                audioPlayedSec = (double)mm.u.cb / (double)m_aBytes;
            else if (mm.wType == TIME_MS)
                audioPlayedSec = (double)mm.u.ms / 1000.0;
        }
        audioPlayedSec += m_audioBaseSec; // 기준 보정
    }

    // ---- B) 오디오가 영상보다 많이(>200ms) 뒤지면: 큐 리셋 + 타임스탬프 드랍 ----
    {
        const double target = m_playPosSec;
        const double margin = 0.03;   // 30ms
        const double behindTH = 0.20;   // 200ms

        if (audioPlayedSec + behindTH < target) {
            waveOutReset(hwo);
            m_audioPending.clear();

            // target 근처까지 오디오 샘플 과감히 드랍
            for (int guard = 0; guard < 512; ++guard) {
                DWORD flags = 0, stream = 0; LONGLONG ts = 0;
                IMFSample* sample = nullptr;
                HRESULT hr = m_reader->ReadSample(
                    MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &stream, &flags, &ts, &sample);
                if (FAILED(hr)) break;

                if (flags & MF_SOURCE_READERF_ENDOFSTREAM) {
                    Safe_Release(sample);
                    if (m_loop) { seekToStart(); }
                    break;
                }
                if (!sample) break;

                LONGLONG dur100 = 0; sample->GetSampleDuration(&dur100);
                const double ssec = (double)ts / 10000000.0;
                const double esec = (double)(ts + dur100) / 10000000.0;

                if (esec < (target - margin)) {
                    Safe_Release(sample);
                    continue; // 과거 샘플 통째로 버림
                }

                // target에 도달: 이 샘플부터 pending 적재
                IMFMediaBuffer* buf = nullptr;
                if (SUCCEEDED(sample->ConvertToContiguousBuffer(&buf))) {
                    BYTE* p = nullptr; DWORD mx = 0, cur = 0;
                    if (SUCCEEDED(buf->Lock(&p, &mx, &cur)) && p && cur > 0) {
                        m_audioPending.insert(m_audioPending.end(), p, p + cur);
                        buf->Unlock();
                    }
                    Safe_Release(buf);
                }
                Safe_Release(sample);

                // waveOut 포지션 0 기준을 target으로 재세팅
                m_audioBaseSec = target;
                break;
            }

            // 기준 업데이트 이후 audioPlayedSec 다시 근사
            audioPlayedSec = m_audioBaseSec;
        }
    }

    // ---- C) 필요 시 pending 채우는 람다 ----
    auto fillPendingFromReader = [&]() -> bool {
        DWORD flags = 0, stream = 0; LONGLONG ts = 0;
        IMFSample* sample = nullptr;
        HRESULT hr = m_reader->ReadSample(
            MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &stream, &flags, &ts, &sample);
        if (FAILED(hr)) return false;

        if (flags & MF_SOURCE_READERF_ENDOFSTREAM) {
            if (m_loop) { seekToStart(); Safe_Release(sample); return true; }
            Safe_Release(sample);
            return false;
        }
        if (!sample) return true; // 아직 준비 안 됨

        LONGLONG dur100 = 0; sample->GetSampleDuration(&dur100);
        const double esec = (double)(ts + dur100) / 10000000.0;

        IMFMediaBuffer* buf = nullptr;
        if (SUCCEEDED(sample->ConvertToContiguousBuffer(&buf))) {
            BYTE* p = nullptr; DWORD mx = 0, cur = 0;
            if (SUCCEEDED(buf->Lock(&p, &mx, &cur)) && p && cur > 0) {
                m_audioPending.insert(m_audioPending.end(), p, p + cur);
                buf->Unlock();
            }
            Safe_Release(buf);
        }
        Safe_Release(sample);

        m_lastAudEndSec = max(m_lastAudEndSec, esec);
        return true;
        };

    // ---- D) 과도한 큐잉 제한: INQUEUE 블록 수가 많으면 일단 대기 ----
    int inq = 0;
    for (auto& b : m_blocks) {
        auto* hdr = reinterpret_cast<WAVEHDR*>(b.hdr);
        if (hdr && (hdr->dwFlags & WHDR_INQUEUE)) ++inq;
    }
    const int kMaxQueueBlocks = 3; // 한 번에 너무 많이 쌓지 않기
    if (inq >= kMaxQueueBlocks) return;

    // ---- E) 빈 블록에 pending 데이터를 채워 전송 ----
    for (int iter = 0; iter < kNumBlocks; ++iter)
    {
        auto& blk = m_blocks[m_blockIndex];
        auto* hdr = reinterpret_cast<WAVEHDR*>(blk.hdr);
        if (hdr->dwFlags & WHDR_INQUEUE) {
            m_blockIndex = (m_blockIndex + 1) % kNumBlocks;
            continue;
        }

        const unsigned want = hdr->dwBufferLength;
        unsigned have = (unsigned)m_audioPending.size();

        int guard = 0;
        while (have < want && guard++ < 3) {
            if (!fillPendingFromReader()) break;
            have = (unsigned)m_audioPending.size();
        }
        if (have == 0) break;

        const unsigned n = (std::min)(want, have);
        memcpy(blk.data.data(), m_audioPending.data(), n);
        if (n < want) memset(blk.data.data() + n, 0, want - n);
        m_audioPending.erase(m_audioPending.begin(), m_audioPending.begin() + n);

        if (!blk.prepared) {
            waveOutPrepareHeader(hwo, hdr, sizeof(WAVEHDR));
            blk.prepared = true;
        }
        waveOutWrite(hwo, hdr, sizeof(WAVEHDR));

        m_blockIndex = (m_blockIndex + 1) % kNumBlocks;

        // 한 번에 너무 많이 쏘면 지연 증가 → 1~2개만 채우고 종료
        if (++inq >= kMaxQueueBlocks) break;
    }
}

// ===== Letterbox =====
void CVideo::applyLetterboxScale()
{
    m_savedSizeX = m_fSizeX; m_savedSizeY = m_fSizeY;
    if (!m_texW || !m_texH || m_fSizeX <= 0.f || m_fSizeY <= 0.f) return;

    const float videoAR = (float)m_texW / (float)m_texH;
    const float targetAR = m_fSizeX / m_fSizeY;
    float w = m_fSizeX, h = m_fSizeY;

    if (videoAR > targetAR) h = w / videoAR; // 위/아래 레터박스
    else                    w = h * videoAR; // 좌/우 레터박스

    m_pTransformCom->Set_Scale(w, h, 1.f);
}

void CVideo::restoreOriginalScale()
{
    if (m_savedSizeX > 0.f && m_savedSizeY > 0.f)
        m_pTransformCom->Set_Scale(m_savedSizeX, m_savedSizeY, 1.f);
    m_savedSizeX = m_savedSizeY = 0.f;
}

// ===== Factory/Clone/Free =====
CVideo* CVideo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    auto* p = new CVideo(pGraphicDev);
    if (FAILED(p->Ready_GameObject())) { Safe_Release(p); return nullptr; }
    return p;
}

CGameObject* CVideo::Clone(void* pArg)
{
    auto* p = new CVideo(*this);
    if (FAILED(p->Initialize(pArg))) { Safe_Release(p); return nullptr; }
    return p;
}

void CVideo::Free()
{
    // 오디오 종료/정리
    if (m_hWaveOut) {
        auto hwo = reinterpret_cast<HWAVEOUT>(m_hWaveOut);
        waveOutReset(hwo);
        for (auto& blk : m_blocks) {
            auto* hdr = reinterpret_cast<WAVEHDR*>(blk.hdr);
            if (hdr) {
                waveOutUnprepareHeader(hwo, hdr, sizeof(WAVEHDR));
                delete hdr;
                blk.hdr = nullptr;
            }
        }
        waveOutClose(hwo);
        m_hWaveOut = nullptr;
        m_blocks.clear();
        m_audioPending.clear();
    }

    // 비디오/MF
    Safe_Release(m_tex);
    Safe_Release(m_reader);

    if (s_mfRef > 0 && --s_mfRef == 0) MFShutdown();
    if (s_comRef > 0 && --s_comRef == 0) CoUninitialize();

    __super::Free();
}