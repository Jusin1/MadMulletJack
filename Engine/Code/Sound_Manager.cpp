#include "Sound_Manager.h"

IMPLEMENT_SINGLETON(CSound_Manager)

CSound_Manager::CSound_Manager()
    : CBase()
    , m_pSystem(nullptr)
{
    ZeroMemory(m_pChannelArr, sizeof(m_pChannelArr));
}

CSound_Manager::~CSound_Manager()
{
    Free();
}

HRESULT CSound_Manager::Initialize()
{
    FMOD::System_Create(&m_pSystem);
    if (!m_pSystem) return E_FAIL;

    m_pSystem->init(MAXCHANNEL, FMOD_INIT_NORMAL, nullptr);

    LoadSoundFile();
    return S_OK;
}

void CSound_Manager::PlaySoundW(TCHAR* pSoundKey, const _uint& eID, const float& fVolume, bool loop)
{
    auto iter = std::find_if(m_mapSound.begin(), m_mapSound.end(),
        [&](auto& pair)->bool { return !lstrcmp(pSoundKey, pair.first); });

    if (iter == m_mapSound.end()) return;

    if (FMOD_OK == m_pSystem->playSound(iter->second, 0, false, &m_pChannelArr[eID]))
    {
        if (m_pChannelArr[eID])
        {
            m_pChannelArr[eID]->setVolume(fVolume);
            m_pChannelArr[eID]->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
        }
    }
    m_pSystem->update();
}

void CSound_Manager::PlayBGM(TCHAR* pSoundKey, const float& fVolume, bool loop)
{
    auto iter = std::find_if(m_mapSound.begin(), m_mapSound.end(),
        [&](auto& pair)->bool { return !lstrcmp(pSoundKey, pair.first); });

    if (iter == m_mapSound.end()) return;

    if (FMOD_OK == m_pSystem->playSound(iter->second, 0, false, &m_pChannelArr[0]))
    {
        if (m_pChannelArr[0])
        {
            m_pChannelArr[0]->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
            m_pChannelArr[0]->setVolume(fVolume);
        }
    }
    m_pSystem->update();
}

void CSound_Manager::StopSound(const _uint& eID)
{
    if (m_pChannelArr[eID])
    {
        m_pChannelArr[eID]->stop();
        m_pChannelArr[eID] = nullptr;  // 안전하게 초기화
    }
    m_pSystem->update();
}

void CSound_Manager::StopAll()
{
    for (int i = 0; i < MAXCHANNEL; ++i)
    {
        if (m_pChannelArr[i])
        {
            m_pChannelArr[i]->stop();
            m_pChannelArr[i] = nullptr;
        }
    }
    m_pSystem->update();
}

void CSound_Manager::SetChannelVolume(const _uint& eID, const float& fVolume)
{
    if (m_pChannelArr[eID])
        m_pChannelArr[eID]->setVolume(fVolume);

    m_pSystem->update();
}

int CSound_Manager::VolumeUp(const _uint& eID, const _float& _vol)
{
    m_volume = min(1.0f, m_volume + _vol);
    if (m_pChannelArr[eID])
        m_pChannelArr[eID]->setVolume(m_volume);
    return 0;
}

int CSound_Manager::VolumeDown(const _uint& eID, const _float& _vol)
{
    m_volume = max(0.0f, m_volume - _vol);
    if (m_pChannelArr[eID])
        m_pChannelArr[eID]->setVolume(m_volume);
    return 0;
}

int CSound_Manager::Pause(const _uint& eID)
{
    if (!m_pChannelArr[eID]) return 0;

    bool paused = false;
    if (m_pChannelArr[eID]->getPaused(&paused) == FMOD_OK)
    {
        paused = !paused;
        m_pChannelArr[eID]->setPaused(paused);
    }
    return 0;
}

void CSound_Manager::LoadSoundFile()
{
    _finddata_t fd;
    intptr_t handle = _findfirst("../../Client/Bin/Resource/Sounds/*", &fd);
    if (handle == -1) return;

    int iResult = 0;
    char szCurPath[128] = "../../Client/Bin/Resource/Sounds/";
    char szFullPath[128] = "";

    while (iResult != -1)
    {
        strcpy_s(szFullPath, szCurPath);
        strcat_s(szFullPath, fd.name);

        FMOD::Sound* pSound = nullptr;
        if (FMOD_OK == m_pSystem->createSound(szFullPath, FMOD_LOOP_OFF, 0, &pSound))
        {
            size_t iLength = strlen(fd.name) + 1;
            TCHAR* pSoundKey = new TCHAR[iLength];
            ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

            MultiByteToWideChar(CP_ACP, 0, fd.name, (int)iLength, pSoundKey, (int)iLength);
            m_mapSound.emplace(pSoundKey, pSound);
        }
        iResult = _findnext(handle, &fd);
    }

    m_pSystem->update();
    _findclose(handle);
}

void CSound_Manager::Free()
{
    StopAll();

    for (auto& Mypair : m_mapSound)
    {
        delete[] Mypair.first;
        if (Mypair.second) Mypair.second->release();
    }
    m_mapSound.clear();

    if (m_pSystem)
    {
        m_pSystem->close();
        m_pSystem->release();
        m_pSystem = nullptr;
    }
}
