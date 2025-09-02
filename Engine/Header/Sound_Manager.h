// Sound_Manager.h
#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "fmod.hpp"
#include <unordered_map>
#include <filesystem>
#include <string>
#pragma comment(lib,"fmod_vc.lib")

BEGIN(Engine)

class ENGINE_DLL CSound_Manager : public CBase
{
    DECLARE_SINGLETON(CSound_Manager)
public:
    CSound_Manager();
    virtual ~CSound_Manager();

public:
    HRESULT Initialize();

public:
    // 인자는 const TCHAR* 가 안전합니다.
    void PlaySoundW(const TCHAR* pSoundKey, const _uint& eID, const float& fVolume, bool loop = false);
    void PlayBGM(const TCHAR* pSoundKey, const float& fVolume, bool loop = true);

    void StopSound(const _uint& eID);
    void StopAll();

    void SetChannelVolume(const _uint& eID, const float& fVolume);
    int  VolumeUp(const _uint& eID, const _float& _vol);
    int  VolumeDown(const _uint& eID, const _float& _vol);
    int  Pause(const _uint& eID);

private:
    void LoadSoundFile(); // 재귀 로딩
    FMOD::Sound* FindSound(const std::wstring& key) const;
    static std::string WToUTF8(const std::wstring& s);

private:
    float m_volume = 0.5f;
    float m_BGMvolume = 0.5f;

    enum { MAXCHANNEL = 32 };
    FMOD::Channel* m_pChannelArr[MAXCHANNEL]{};

    // 포인터 키 대신 안전한 wstring 키 사용
    std::unordered_map<std::wstring, FMOD::Sound*> m_sounds;

    FMOD::System* m_pSystem = nullptr;
    bool m_bPause = false;

public:
    virtual void Free() override;
};

END