#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "fmod.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#include <io.h>
#pragma comment(lib,"fmod_vc.lib")

#define SOUND_MIN     0.0f
#define SOUND_DEFAULT 0.5f
#define SOUND_WEIGHT  0.1f

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
    void PlaySoundW(TCHAR* pSoundKey, const _uint& eID, const float& fVolume, bool loop = false);
    void PlayBGM(TCHAR* pSoundKey, const float& fVolume, bool loop = true); // 기본은 반복

    void StopSound(const _uint& eID);
    void StopAll();

    void SetChannelVolume(const _uint& eID, const float& fVolume);

    int  VolumeUp(const _uint& eID, const _float& _vol);
    int  VolumeDown(const _uint& eID, const _float& _vol);

    int  Pause(const _uint& eID);

private:
    void LoadSoundFile();

private:
    float m_volume    = SOUND_DEFAULT;
    float m_BGMvolume = SOUND_DEFAULT;

private:
    std::map<TCHAR*, FMOD::Sound*> m_mapSound;

    enum { MAXCHANNEL = 32 };
    FMOD::Channel* m_pChannelArr[MAXCHANNEL];  // 채널 배열

    FMOD::System* m_pSystem = nullptr;
    bool m_bPause = false;

public:
    virtual void Free() override;
};

END