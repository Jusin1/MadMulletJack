#include "SoundManager.h"
#include <fstream>


//////////////////////////////////////////////////////// 싱글톤 ////////////////////////////////////////////////////////im
SoundManager* SoundManager::Instance = nullptr;
/******************************************************* 싱글톤 *******************************************************/


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
    Shutdown();
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 이니셜라이즈 ////////////////////////////////////////////////////////
bool SoundManager::Initialize()
{
    if (FAILED(XAudio2Create(&m_xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)))
        return false;

    if (FAILED(m_xAudio2->CreateMasteringVoice(&m_masterVoice)))
        return false;

    return true;
}
/******************************************************* 이니셜라이즈 *******************************************************/



//////////////////////////////////////////////////////// 릴리즈 ////////////////////////////////////////////////////////
void SoundManager::Shutdown()
{
    StopAll();

    // 메모리 해제
    for (auto& pair : m_soundMap)
    {
        delete[] pair.second.audioData;
    }

    if (m_masterVoice)
        m_masterVoice->DestroyVoice();
    if (m_xAudio2)
        m_xAudio2->Release();

    m_soundMap.clear();
    m_activeVoices.clear();
}
/******************************************************* 릴리즈 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너에 사운드 저장 ////////////////////////////////////////////////////////
bool SoundManager::LoadSound(const std::string& name, const std::wstring& filePath)
{
    if (m_soundMap.find(name) != m_soundMap.end())
        return true; // 이미 로드됨

    SoundData sound;
    if (!LoadWAVFile(filePath, sound))
    {
        MessageBox(NULL, filePath.c_str(), L"없음", 0);
        MessageBox(NULL, L"해당 경로의 Wav 파일이 없습니다", L"없음", 0);
        return false;
    }

    m_soundMap[name] = sound;
    return true;
}
/******************************************************* 컨테이너에 사운드 저장 *******************************************************/



//////////////////////////////////////////////////////// 사운드 재생 ////////////////////////////////////////////////////////
void SoundManager::Play_Sound(const std::string& name, float _volume, bool loop)
{
    auto it = m_soundMap.find(name);
    if (it == m_soundMap.end())
        return;

    const SoundData& sound = it->second;

    IXAudio2SourceVoice* sourceVoice = nullptr;
    if (FAILED(m_xAudio2->CreateSourceVoice(&sourceVoice, &sound.waveFormat)))
        return;

    XAUDIO2_BUFFER buffer = {};
    buffer.AudioBytes = sound.dataSize;
    buffer.pAudioData = sound.audioData;
    buffer.Flags = XAUDIO2_END_OF_STREAM;
    if (loop)
        buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

    if (FAILED(sourceVoice->SubmitSourceBuffer(&buffer)))
    {
        sourceVoice->DestroyVoice();
        return;
    }

    sourceVoice->SetVolume(_volume);

    if (FAILED(sourceVoice->Start()))
    {
        sourceVoice->DestroyVoice();
        return;
    }

    // 추적용으로 리스트에 보관
    m_activeVoices.push_back({ sourceVoice, &sound });
}
/******************************************************* 사운드 재생 *******************************************************/



//////////////////////////////////////////////////////// 같은 사운드 하나만 재생 ////////////////////////////////////////////////////////
void SoundManager::Play_Sound_Unique(const std::string& name, float _volume, bool loop)
{
    auto it = m_soundMap.find(name);
    if (it == m_soundMap.end())
        return;

    StopSound(name);

    const SoundData& sound = it->second;

    IXAudio2SourceVoice* sourceVoice = nullptr;
    if (FAILED(m_xAudio2->CreateSourceVoice(&sourceVoice, &sound.waveFormat)))
        return;

    XAUDIO2_BUFFER buffer = {};
    buffer.AudioBytes = sound.dataSize;
    buffer.pAudioData = sound.audioData;
    buffer.Flags = XAUDIO2_END_OF_STREAM;
    if (loop)
        buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

    if (FAILED(sourceVoice->SubmitSourceBuffer(&buffer)))
    {
        sourceVoice->DestroyVoice();
        return;
    }

    sourceVoice->SetVolume(_volume);

    if (FAILED(sourceVoice->Start()))
    {
        sourceVoice->DestroyVoice();
        return;
    }

    // 추적용으로 리스트에 보관
    m_activeVoices.push_back({ sourceVoice, &sound });
}
/******************************************************* 같은 사운드 하나만 재생 *******************************************************/



//////////////////////////////////////////////////////// 해당 사운드 OFF ////////////////////////////////////////////////////////
void SoundManager::StopSound(const std::string& name)
{
    auto it = m_soundMap.find(name);
    if (it == m_soundMap.end())
        return;

    const SoundData* targetSound = &it->second;

    // 해당 사운드를 참조 중인 모든 Voice 제거
    auto itVoice = m_activeVoices.begin();
    while (itVoice != m_activeVoices.end())
    {
        if (itVoice->sound == targetSound)
        {
            itVoice->sourceVoice->Stop();
            itVoice->sourceVoice->FlushSourceBuffers();
            itVoice->sourceVoice->DestroyVoice();
            itVoice = m_activeVoices.erase(itVoice);
        }
        else
        {
            ++itVoice;
        }
    }
}
/******************************************************* 해당 사운드 OFF *******************************************************/



//////////////////////////////////////////////////////// 특정 사운드의 소리 크기 조절 ////////////////////////////////////////////////////////
void SoundManager::SetVolume(const std::string& name, float volume)
{
    auto it = m_soundMap.find(name);
    if (it == m_soundMap.end())
        return;

    const SoundData* targetSound = &it->second;

    for (auto& voice : m_activeVoices)
    {
        if (voice.sound == targetSound)
        {
            // 볼륨 조절 (0.0f = 무음, 1.0f = 원래 크기, 2.0f = 2배)
            voice.sourceVoice->SetVolume(volume);
        }
    }

}

/******************************************************* 특정 사운드의 소리 크기 조절 *******************************************************/



//////////////////////////////////////////////////////// 전체 소리 크기 조절 ////////////////////////////////////////////////////////
void SoundManager::SetMasterVolume(float volume)
{
    if (m_masterVoice)
    {
        // 0.0f = 무음, 1.0f = 원래 크기, 1.0f 이상도 가능 (증폭)
        m_masterVoice->SetVolume(volume);
    }
}
/******************************************************* 전체 소리 크기 조절 *******************************************************/



//////////////////////////////////////////////////////// 마스터 볼륨 획득 ////////////////////////////////////////////////////////
float SoundManager::GetMasterVolume() const
{
    float volume(0.f);
    m_masterVoice->GetVolume(&volume);
    return volume;
}
/******************************************************* 마스터 볼륨 획득 *******************************************************/



//////////////////////////////////////////////////////// 사운드 전부 끄기 ////////////////////////////////////////////////////////
void SoundManager::StopAll()
{
    for (auto& voice : m_activeVoices)
    {
        voice.sourceVoice->Stop();
        voice.sourceVoice->FlushSourceBuffers();
        voice.sourceVoice->DestroyVoice();
    }
    m_activeVoices.clear();
}
/******************************************************* 사운드 전부 끄기 *******************************************************/



//////////////////////////////////////////////////////// WAV파일 불러오기 ////////////////////////////////////////////////////////
bool SoundManager::LoadWAVFile(const std::wstring& filePath, SoundData& outSound)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file)
        return false;

    DWORD chunkType = 0, chunkSize = 0;
    DWORD format = 0;

    file.read(reinterpret_cast<char*>(&chunkType), 4); // "RIFF"
    file.seekg(4, std::ios::cur);                     // Chunk size
    file.read(reinterpret_cast<char*>(&format), 4);   // "WAVE"

    if (chunkType != 'FFIR' || format != 'EVAW')
        return false;

    while (file.read(reinterpret_cast<char*>(&chunkType), 4))
    {
        file.read(reinterpret_cast<char*>(&chunkSize), 4);

        if (chunkType == ' tmf') // "fmt "
        {
            file.read(reinterpret_cast<char*>(&outSound.waveFormat), sizeof(WAVEFORMATEX));
            file.seekg(chunkSize - sizeof(WAVEFORMATEX), std::ios::cur);
        }
        else if (chunkType == 'atad') // "data"
        {
            outSound.audioData = new BYTE[chunkSize];
            outSound.dataSize = chunkSize;
            file.read(reinterpret_cast<char*>(outSound.audioData), chunkSize);
            break;
        }
        else
        {
            file.seekg(chunkSize, std::ios::cur); // 다음 chunk로
        }
    }

    return outSound.audioData != nullptr;
}
/******************************************************* WAV파일 불러오기 *******************************************************/




void SoundManager::Free()
{

}
