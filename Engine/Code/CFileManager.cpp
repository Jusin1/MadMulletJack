#include "Engine_Define.h"
#include <fstream>
#include <filesystem>
#include <locale>
#include <vector>
#include <codecvt>
#include <string>
#include "CDataManager.h"
#include "CObjectManager.h"
#include "CFileManager.h"

const filesystem::path GameDataPath = L"../../GameData/";

NLOHMANN_JSON_SERIALIZE_ENUM(WallType, {
    {WallType::WALL_HOR, "WALL_HOR"},
    {WallType::WALL_VER, "WALL_VER"},
    {WallType::INCLINE, "INCLINE"},
    {WallType::FLOOR, "FLOOR"},
    {WallType::CEILING, "CEILING"},
    {WallType::WALL_SLIDE, "WALLSLIDE"},
    {WallType::NONE, "NONE"}
    })

NLOHMANN_JSON_SERIALIZE_ENUM(PrefabType, {
    {PrefabType::SIGN_PILLAR, "SIGNPILLAR"},
    {PrefabType::ROAD, "ROAD"},
    {PrefabType::SING_PILLAR_2, "SIGNPILLAR_2"},
    {PrefabType::TILESET_1, "TILESET_1"},
    {PrefabType::Missile, "Missile"},
    {PrefabType::NONE, "NONE"}
    })

NLOHMANN_JSON_SERIALIZE_ENUM(ObjectCategory, {
    {ObjectCategory::WALL, "WALL"},
    {ObjectCategory::TILE, "TILE"},
    {ObjectCategory::ENV_OBJ, "ENV_OBJ"},
    {ObjectCategory::MONSTER, "MONSTER"},
    {ObjectCategory::LIGHT, "LIGHT"},
    {ObjectCategory::PREFAB, "PREFAB"},
    {ObjectCategory::NONE, "NONE"}
    })

IMPLEMENT_SINGLETON(CFileManager)

CFileManager::CFileManager()
{
}

CFileManager::~CFileManager()
{
}

void CFileManager::Free()
{
}

inline std::string WStringToUTF8(const std::wstring &wstr)
{
    if (wstr.empty())
        return string{};
    
    // 필요 버퍼 크기, 널제외
    int size = ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);

    if (size <= 0)
        return string{};

    std::string out(size, '\0');
    int written = ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), out.data(), size, nullptr, nullptr);

    if (written <= 0)
        return string{};

    return out;
}

inline std::wstring UTF8ToWString(const std::string &str)
{
    if (str.empty())
        return wstring{};

    // 유효하지 않은 UTF-8이면 실패시키기 옵션
    int wsize = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.c_str(), (int)str.size(), nullptr, 0);
    
    if (wsize <= 0)
        return wstring{};

    std::wstring out(wsize, L'\0');
    int ww = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.c_str(), (int)str.size(), out.data(), wsize);
    if (ww <= 0)
        return wstring{};

    return out;
}

HRESULT CFileManager::SaveDataFile(_uint iSceneID, const _tchar *szLayerTag)
{
    std::vector<MAPOBJECTDATA> objectDatas = CObjectManager::GetInstance()->ExportObjectData(iSceneID, szLayerTag);

    if (objectDatas.size() <= 0)
    {
        MSG_BOX("CFileManager::SaveDataFile, objlist is empty");
        return E_FAIL;
    }

    json jArray = json::array();
    for (const auto &data : objectDatas)
    {
        jArray.push_back(data);
    }

    // " / " 연산자 오버로딩을 통해서 파일 경로 만들기
    filesystem::path dir = GameDataPath / SceneIdToWstring(iSceneID) / szLayerTag / L"data.json";
    if (!dir.parent_path().empty())
    {
        filesystem::create_directories(dir.parent_path());
    }
    else
    {
        MSG_BOX("CFileManager::SaveDataFile, path is invalid");
        return E_FAIL;
    }

    std::ofstream ofs(dir, std::ios::out | std::ios::binary);
    if (!ofs.is_open())
    {
        MSG_BOX("CFileManager::SaveDataFile, Failed Save");
        return E_FAIL;
    }

    // setw(i) 출력될 값의 최소폭을 i 만큼 지정
    // Json 이므로 4칸 들여쓰기로 출력
    ofs << std::setw(4) << jArray << std::endl;
    ofs.close();

    return S_OK;
}

HRESULT CFileManager::LoadDataFile(_uint iSceneID, const _tchar *szLayerTag)
{
    filesystem::path dir = GameDataPath / SceneIdToWstring(iSceneID) / szLayerTag / L"data.json";
    if (dir.parent_path().empty())
    { 
        return E_FAIL;
    }

    std::ifstream ifs(dir, std::ios::in | std::ios::binary);
    if (!ifs.is_open())
    {
        return E_FAIL;
    }

    json jArray;
    ifs >> jArray;

    for (const auto &jObj : jArray)
    {
        MAPOBJECTDATA objData = jObj.get<MAPOBJECTDATA>();
        CDataManager::GetInstance()->AddData(szLayerTag, objData);
    }

    ifs.close();
    return S_OK;
}

HRESULT CFileManager::SavePrefabDataFile(PrefabType _e)
{
    std::vector<PREFABDATA> objectDatas = CObjectManager::GetInstance()->ExportPrefabData();

    if (objectDatas.size() <= 0)
    {
        return E_FAIL;
    }

    json jArray = json::array();
    for (const auto &data : objectDatas)
    {
        jArray.push_back(data);
    }

    wstring FileName{PrefabTypeToWstring(static_cast<PrefabType>(_e))};
    filesystem::path dir = GameDataPath / L"Prefab_Data" / FileName / L"data.json";

    if (!dir.parent_path().empty())
    {
        filesystem::create_directories(dir.parent_path());
    }
    else
    {
        return E_FAIL;
    }

    std::ofstream ofs(dir, std::ios::out | std::ios::binary);
    if (!ofs.is_open())
    {
        return E_FAIL;
    }

    ofs << std::setw(4) << jArray << std::endl;
    ofs.close();

    return S_OK;
}

HRESULT CFileManager::LoadPrefabDataFile(PrefabType _e)
{
    wstring FileName{ PrefabTypeToWstring(static_cast<PrefabType>(_e)) };
    filesystem::path dir = GameDataPath / L"Prefab_Data" / FileName / L"data.json";

    if (dir.parent_path().empty())
    {
        return E_FAIL;
    }

    std::ifstream ifs(dir, std::ios::in | std::ios::binary);
    if (!ifs.is_open())
    {
        return E_FAIL;
    }

    json jArray;
    ifs >> jArray;

    int iIndex{ 0 };
    for (const auto &jObj : jArray)
    {
        if (iIndex > 0)
        {
            return S_OK;
        }

        PREFABDATA PrefabData = jObj.get<PREFABDATA>();
        CDataManager::GetInstance()->AddPrefabData(_e, PrefabData);
        ++iIndex;
    }

    ifs.close();

    return S_OK;
}

HRESULT CFileManager::SaveInstancedPrefabDataFile(_uint iSceneID)
{
    std::vector<PREFABDATA> prefabDatas = CObjectManager::GetInstance()->Export_InstancedPrefabData(iSceneID);

    if (prefabDatas.size() <= 0)
    {
        return E_FAIL;
    }

    json jArray = json::array();
    for (const auto &data : prefabDatas)
    {
        jArray.push_back(data);
    }

    filesystem::path dir = GameDataPath / SceneIdToWstring(iSceneID) / L"Prefab_Layer/data.json";
    if (!dir.parent_path().empty())
    {
        filesystem::create_directories(dir.parent_path());
    }
    else
    {
        return E_FAIL;
    }

    std::ofstream ofs(dir, std::ios::out | std::ios::binary);
    if (!ofs.is_open())
    {
        return E_FAIL;
    }

    ofs << std::setw(4) << jArray << std::endl;
    ofs.close();

    return S_OK;
}

HRESULT CFileManager::LoadInstancedPrefabDataFile(_uint iSceneID)
{
    filesystem::path dir = GameDataPath / SceneIdToWstring(iSceneID) / L"Prefab_Layer/data.json";
    if (dir.parent_path().empty())
    {
        return E_FAIL;
    }

    std::ifstream ifs(dir, std::ios::in | std::ios::binary);
    if (!ifs.is_open())
    {
        return E_FAIL;
    }

    json jArray;
    ifs >> jArray;

    for (const auto &jObj : jArray)
    {
        PREFABDATA objData = jObj.get<PREFABDATA>();
        CDataManager::GetInstance()->AddInstancedPrefabData(objData);
    }

    ifs.close();
    return S_OK;
}

wstring CFileManager::SceneIdToWstring(_uint iSceneID)
{
    switch (iSceneID)
    {
    case SCENE_DEV:
        return L"Dev";
    case SCENE_TUTORIAL:
        return L"Tutorial";
    case SCENE_STAGE_1:
        return L"Stage_1";
    case SCENE_STAGE_2:
        return L"Stage_2";
    case SCENE_SNIPE:
        return L"Snipe";
    case SCENE_BOSS:
        return L"Rooftop";
    case SCENE_CAR:
        return L"Road";
    case SCENE_PREFAB:
        return L"Prefab";
    }

    return wstring{};
}

wstring CFileManager::PrefabTypeToWstring(PrefabType _e)
{
    switch (_e)
    {
    case PrefabType::SIGN_PILLAR:
        return L"Sign_Pillar";
    case PrefabType::ROAD:
        return L"Road";
    case PrefabType::SING_PILLAR_2:
        return L"Sign_Pillar_2";
    case PrefabType::TILESET_1:
        return L"Tileset_1";
    case PrefabType::Missile:
        return L"Missile";
    }

    return wstring{};
}

BEGIN(Engine)

void to_json(json &_j, const PREFABDATA &_tData)
{
    _j = json
    {
        {"PrefabType", _tData.eType},
        {"Childrens", _tData.vecChildrensData},
        {"ParentTransformData", _tData.ParentTransform}
    };
}

void from_json(const json &_j, PREFABDATA &_tData)
{
    _j.at("PrefabType").get_to(_tData.eType);
    _tData.vecChildrensData = _j.at("Childrens").get<std::vector<MAPOBJECTDATA>>();
    _j.at("ParentTransformData").get_to(_tData.ParentTransform);
}

void to_json(json &_j, const TRANSFORMDATA &_tData)
{
    _j = json
    {
        {"Right", _tData.Right},
        {"Up", _tData.Up},
        {"Look", _tData.Look},
        {"Pos", _tData.Pos}
    };
}

void from_json(const json &_j, TRANSFORMDATA &_tData)
{
    _j.at("Right").get_to(_tData.Right);
    _j.at("Up").get_to(_tData.Up);
    _j.at("Look").get_to(_tData.Look);
    _j.at("Pos").get_to(_tData.Pos);
}

void to_json(json &_j, const TEXTUREDATA &_tData)
{
    _j = json
    {
        {"FilePath", WStringToUTF8(_tData.OriginComponentName)}
    };
}

void from_json(const json &_j, TEXTUREDATA &_tData)
{
    std::string srcString{ "" };
    _j.at("FilePath").get_to(srcString);
    _tData.OriginComponentName = UTF8ToWString(srcString);
}

void to_json(json &_j, const PANELDATA &_tData)
{
    _j = json
    {
        {"Type", _tData.eType},
        {"Count_X", _tData.dwCountX},
        {"Count_Y", _tData.dwCountY},
        {"Count_Z", _tData.dwCountZ},
        {"Interval", _tData.dwInterval}
    };
}

void from_json(const json &_j, PANELDATA &_tData)
{
    _j.at("Type").get_to(_tData.eType);
    _j.at("Count_X").get_to(_tData.dwCountX);
    _j.at("Count_Y").get_to(_tData.dwCountY);
    _j.at("Count_Z").get_to(_tData.dwCountZ);
    _j.at("Interval").get_to(_tData.dwInterval);
}

void to_json(json &_j, const MAPOBJECTDATA &_tData)
{
    _j = json
    {
        {"ObjectCategory", _tData.eCategory},
        {"ObjectType", _tData.iType},
        {"Color", _tData.dwColor},
        {"TransformData", _tData.transform},
        {"TextureData", _tData.texture},
        {"PanelData", _tData.panelBuffer}
    };
}

void from_json(const json &_j, MAPOBJECTDATA &_tData)
{
    _j.at("ObjectCategory").get_to(_tData.eCategory);
    _j.at("ObjectType").get_to(_tData.iType);
    _j.at("Color").get_to(_tData.dwColor);
    _j.at("TransformData").get_to(_tData.transform);
    _j.at("TextureData").get_to(_tData.texture);
    _j.at("PanelData").get_to(_tData.panelBuffer);
}

END
