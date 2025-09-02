#pragma once

#include <xaudio2.h>
#include "Engine_Define.h"
#include "CBase.h"

BEGIN(Engine)

class ENGINE_DLL SoundManager : public CBase
{
private:
    SoundManager();
    ~SoundManager();
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    static SoundManager* Instance;

public:
    static SoundManager* Get_Instance()
    {
        if (!Instance)
            Instance = new SoundManager;
        return Instance;
    }
    static void Destroy_Instance()
    {
        delete Instance;
        Instance = nullptr;
    }

public:
    bool Initialize();
    void Shutdown();

    // 사운드 파일(.wav) 로드. name은 식별자.
    bool LoadSound(const std::string& name, const std::wstring& filePath);

    // 사운드 재생. loop가 true면 무한 반복
    void Play_Sound(const std::string& name, float _volume = 1.f, bool loop = false);

    // 같은 이름의 사운드 하나만 재생
    void Play_Sound_Unique(const std::string& name, float _volume = 1.f, bool loop = false);

    // 특정 이름의 모든 재생 중인 사운드 중지
    void StopSound(const std::string& name);

    // 특정 이름의 사운드 볼륨 조절
    void SetVolume(const std::string& name, float volume);

    void SetMasterVolume(float volume);
    float GetMasterVolume() const;

    // 전체 사운드 정지
    void StopAll();

private:
    struct SoundData
    {
        WAVEFORMATEX waveFormat = {};      // 사운드 형식 정보
        BYTE* audioData = nullptr;         // 사운드 데이터
        DWORD dataSize = 0;                // 사운드 데이터 크기
    };

    struct VoiceInstance
    {
        IXAudio2SourceVoice* sourceVoice = nullptr;
        const SoundData* sound = nullptr;
    };

    bool LoadWAVFile(const std::wstring& filePath, SoundData& outSound);

private:
    IXAudio2* m_xAudio2 = nullptr;
    IXAudio2MasteringVoice* m_masterVoice = nullptr;

    std::map<std::string, SoundData> m_soundMap;
    std::vector<VoiceInstance> m_activeVoices;

    void Free() override final;
};

END