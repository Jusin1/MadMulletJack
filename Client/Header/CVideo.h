#pragma once
#include "CUI.h"
#include <vector>

// 전방 선언(구체 헤더 의존 최소화)
struct IMFSourceReader;

class CVideo : public CUI
{
public:
    explicit CVideo(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CVideo(const CVideo& rhs);
    virtual ~CVideo();

public:
    // pArg: const wchar_t* (영상 경로)
    virtual HRESULT Ready_GameObject() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual _int    Update_GameObject(const _float& fTimeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void    Render_GameObject() override;

    // 제어
    void Play();
    void Pause(bool pause);
    void Stop();
    bool IsPlaying() const { return m_playing; }

    // 옵션
    void SetLoop(bool loop) { m_loop = loop; }
    void SetKeepAspect(bool keep) { m_keepAspect = keep; } // 레터박스

public:
    static CVideo* Create(LPDIRECT3DDEVICE9 pGraphicDev);      // 프로토타입
    virtual CGameObject* Clone(void* pArg = nullptr) override; // 경로 전달

protected:
    virtual void Free() override;

private:
    // ===== Media Foundation =====
    HRESULT openReader(const wchar_t* filePath);
    HRESULT seekToStart();

    // ===== Video =====
    HRESULT createTexture(UINT w, UINT h);
    HRESULT decodeToTime(double targetSec);      // 목표 시각까지 스킵하며 최신 프레임 1장만 업로드
    HRESULT uploadSampleToTexture(struct IMFSample* sample);
    void    applyLetterboxScale();
    void    restoreOriginalScale();

    // ===== Audio (winmm waveOut) =====
    HRESULT initAudio();     // PCM 포맷 확인 및 waveOut 준비(실패해도 영상 재생됨)
    void    startAudio();
    void    stopAudio();
    void    resetAudioQueue();
    void    pumpAudio();     // 오디오 버퍼 공급

private:
    // D3D9
    IDirect3DTexture9* m_tex = nullptr;
    UINT               m_texW = 0, m_texH = 0;

    // Media Foundation
    IMFSourceReader* m_reader = nullptr;
    static long        s_mfRef;     // MFStartup/MFShutdown ref
    static long        s_comRef;    // CoInitialize/CoUninitialize ref

    // 상태
    bool   m_playing = false;
    bool   m_eos = false;
    bool   m_loop = true;
    bool   m_keepAspect = false;

    // 타이밍
    double m_frameDurSec = 1.0 / 30.0;  // 기본 30fps
    double m_playPosSec = 0.0;         // 재생 시계(초)
    double m_lastVidSec = 0.0;         // 마지막 업로드된 비디오 시각(초)

    // 오디오 동기화용
    double  m_audioBaseSec = 0.0;       // waveOut 포지션 0이 가리키는 ‘절대 재생 시각’
    double  m_lastAudEndSec = 0.0;      // 마지막으로 수집한 오디오 샘플 끝 시각(모니터링용)

    // 레터박스 전/후 스케일 저장
    float  m_savedSizeX = 0.f, m_savedSizeY = 0.f;

    // ===== 오디오(waveOut) 관련 =====
    void* m_hWaveOut = nullptr;   // HWAVEOUT (void*로 보관)
    unsigned  m_aCh = 0;            // 채널수
    unsigned  m_aRate = 0;            // 샘플레이트
    unsigned  m_aBits = 0;            // 비트수(보통 16)
    unsigned  m_aBlock = 0;            // nBlockAlign
    unsigned  m_aBytes = 0;            // nAvgBytesPerSec
    unsigned  m_blockBytes = 0;       // 한 블록 바이트 크기(?50ms)

    struct AudioBlock {
        void* hdr = nullptr;                         // WAVEHDR*
        std::vector<unsigned char> data;             // 전송 버퍼
        bool prepared = false;
    };
    static constexpr int kNumBlocks = 4;
    std::vector<AudioBlock> m_blocks;
    size_t   m_blockIndex = 0;

    std::vector<unsigned char> m_audioPending;       // Reader에서 뽑은 PCM 누적 버퍼
};