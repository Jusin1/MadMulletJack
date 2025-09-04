// Sound_Manager.cpp
#include "Sound_Manager.h"
#include <codecvt>
#include <locale>
using namespace Engine;

IMPLEMENT_SINGLETON(CSound_Manager)

CSound_Manager::CSound_Manager() : CBase() {}
CSound_Manager::~CSound_Manager() { Free(); }

HRESULT CSound_Manager::Initialize()
{
    FMOD::System_Create(&m_pSystem);
    if (!m_pSystem) return E_FAIL;

    m_pSystem->init(MAXCHANNEL, FMOD_INIT_NORMAL, nullptr);
    LoadSoundFile();
    return S_OK;
}

// UTF-16 -> UTF-8 (FMOD은 UTF-8 경로가 가장 안전)
std::string CSound_Manager::WToUTF8(const std::wstring& s) {
    if (s.empty()) return {};
    int len = ::WideCharToMultiByte(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0, nullptr, nullptr);
    std::string out(len, 0);
    ::WideCharToMultiByte(CP_UTF8, 0, s.c_str(), (int)s.size(), out.data(), len, nullptr, nullptr);
    return out;
}

FMOD::Sound* CSound_Manager::FindSound(const std::wstring& key) const
{
    auto it = m_sounds.find(key);
    if (it != m_sounds.end()) return it->second;
    return nullptr;
}

void CSound_Manager::PlaySoundW(const TCHAR* pSoundKey, const _uint& eID, const float& fVolume, bool loop)
{
    if (!pSoundKey) return;
    std::wstring k(pSoundKey);

    // 우선순위: 그대로 → 확장자 제거 → 파일명만
    FMOD::Sound* snd = FindSound(k);
    if (!snd) {
        // 확장자 제거 키
        std::wstring stem = k;
        size_t dot = stem.find_last_of(L'.');
        if (dot != std::wstring::npos) stem.erase(dot);
        snd = FindSound(stem);

        if (!snd) {
            // 마지막으로 파일명만 시도
            size_t slash = k.find_last_of(L"/\\");
            if (slash != std::wstring::npos) {
                std::wstring justName = k.substr(slash + 1);
                snd = FindSound(justName);
                if (!snd) {
                    // 파일명에서 확장자 제거
                    dot = justName.find_last_of(L'.');
                    if (dot != std::wstring::npos) justName.erase(dot);
                    snd = FindSound(justName);
                }
            }
        }
    }
    if (!snd) return;

    if (FMOD_OK == m_pSystem->playSound(snd, 0, loop, &m_pChannelArr[eID])) {
        if (m_pChannelArr[eID]) {
            m_pChannelArr[eID]->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
            m_pChannelArr[eID]->setVolume(fVolume);
        }
    }
    m_pSystem->update();
}

void CSound_Manager::PlayBGM(const TCHAR* pSoundKey, const float& fVolume, bool loop)
{
    if (!pSoundKey) return;

    // 1) 재생할 사운드 찾기 (기존 FindSound 로직 재사용)
    std::wstring k(pSoundKey);
    FMOD::Sound* snd = FindSound(k);
    if (!snd) {
        std::wstring stem = k;
        size_t dot = stem.find_last_of(L'.');
        if (dot != std::wstring::npos) stem.erase(dot);
        snd = FindSound(stem);

        if (!snd) {
            size_t slash = k.find_last_of(L"/\\");
            if (slash != std::wstring::npos) {
                std::wstring justName = k.substr(slash + 1);
                snd = FindSound(justName);
                if (!snd) {
                    dot = justName.find_last_of(L'.');
                    if (dot != std::wstring::npos) justName.erase(dot);
                    snd = FindSound(justName);
                }
            }
        }
    }
    if (!snd) return;

    // 2) 기존 BGM 채널 상태 확인 (채널 0 고정)
    if (m_pChannelArr[0]) {
        bool playing = false;
        m_pChannelArr[0]->isPlaying(&playing);

        FMOD::Sound* cur = nullptr;
        m_pChannelArr[0]->getCurrentSound(&cur);

        // 같은 곡이 이미 재생 중이면 재생 안 겹치고 파라미터만 갱신
        if (playing && cur == snd) {
            m_pChannelArr[0]->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
            m_pChannelArr[0]->setVolume(fVolume);
            m_pSystem->update();
            return;
        }

        // 다른 사운드거나 정지 상태면 일단 기존 채널 정리
        m_pChannelArr[0]->stop();
        m_pChannelArr[0] = nullptr;
    }

    // 3) 새로 재생 (일단 일시정지 상태로 생성 후 설정하고 풀기)
    if (FMOD_OK == m_pSystem->playSound(snd, 0, true /* start paused */, &m_pChannelArr[0])) {
        if (m_pChannelArr[0]) {
            m_pChannelArr[0]->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
            m_pChannelArr[0]->setVolume(fVolume);
            m_pChannelArr[0]->setPosition(0, FMOD_TIMEUNIT_MS);
            m_pChannelArr[0]->setPaused(false);
        }
    }
    m_pSystem->update();
}

void CSound_Manager::StopSound(const _uint& eID)
{
    if (m_pChannelArr[eID]) {
        m_pChannelArr[eID]->stop();
        m_pChannelArr[eID] = nullptr;
    }
    m_pSystem->update();
}

void CSound_Manager::StopAll()
{
    for (int i = 0; i < MAXCHANNEL; ++i) {
        if (m_pChannelArr[i]) {
            m_pChannelArr[i]->stop();
            m_pChannelArr[i] = nullptr;
        }
    }
    m_pSystem->update();
}

void CSound_Manager::SetChannelVolume(const _uint& eID, const float& fVolume)
{
    if (m_pChannelArr[eID]) m_pChannelArr[eID]->setVolume(fVolume);
    m_pSystem->update();
}

int CSound_Manager::VolumeUp(const _uint& eID, const _float& _vol)
{
    m_volume = min(1.0f, m_volume + _vol);
    if (m_pChannelArr[eID]) m_pChannelArr[eID]->setVolume(m_volume);
    return 0;
}
int CSound_Manager::VolumeDown(const _uint& eID, const _float& _vol)
{
    m_volume = max(0.0f, m_volume - _vol);
    if (m_pChannelArr[eID]) m_pChannelArr[eID]->setVolume(m_volume);
    return 0;
}

int CSound_Manager::Pause(const _uint& eID)
{
    if (!m_pChannelArr[eID]) return 0;
    bool paused = false;
    if (m_pChannelArr[eID]->getPaused(&paused) == FMOD_OK) {
        m_pChannelArr[eID]->setPaused(!paused);
    }
    return 0;
}

void CSound_Manager::LoadSoundFile()
{
    namespace fs = std::filesystem;
    fs::path base = LR"(../../Client/Bin/Resource/Sounds)"; // 프로젝트 기준 상대경로

    std::error_code ec;
    if (!fs::exists(base, ec)) return;

    for (auto it = fs::recursive_directory_iterator(base, ec);
        it != fs::recursive_directory_iterator(); ++it)
    {
        if (ec) break;
        if (!it->is_regular_file(ec)) continue;

        const fs::path& p = it->path();
        std::wstring ext = p.extension().wstring();
        for (auto& ch : ext) ch = (wchar_t)towlower(ch);
        if (ext != L".wav" && ext != L".mp3" && ext != L".ogg") continue;

        std::string utf8Path = WToUTF8(p.wstring());
        FMOD::Sound* snd = nullptr;
        FMOD_RESULT r = m_pSystem->createSound(utf8Path.c_str(), FMOD_DEFAULT, 0, &snd);
        if (r != FMOD_OK || !snd) continue;

        // 3가지 키로 등록: 상대경로 / 파일명+확장자 / 파일명(확장자 제거)
        std::wstring rel = fs::relative(p, base, ec).wstring();
        if (!ec) {
            std::replace(rel.begin(), rel.end(), L'\\', L'/');
            m_sounds.emplace(rel, snd);
        }
        std::wstring fname = p.filename().wstring();
        m_sounds.emplace(fname, snd);

        std::wstring stem = p.stem().wstring();
        m_sounds.emplace(stem, snd);
    }
    m_pSystem->update();
}

void CSound_Manager::Free()
{
    StopAll();

    // 같은 사운드 포인터가 여러 키에 매핑되어 있으므로 중복 방지하여 release
    std::unordered_set<FMOD::Sound*> released;
    for (auto& kv : m_sounds) {
        if (kv.second && released.insert(kv.second).second) {
            kv.second->release();
        }
    }
    m_sounds.clear();

    if (m_pSystem) {
        m_pSystem->close();
        m_pSystem->release();
        m_pSystem = nullptr;
    }
}