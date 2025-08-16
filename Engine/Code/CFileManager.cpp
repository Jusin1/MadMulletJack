#include "Engine_Define.h"
#include <fstream>
#include <locale>
#include <codecvt>
#include <string>
#include "CObjectManager.h"
#include "CFileManager.h"

NLOHMANN_JSON_SERIALIZE_ENUM(WallType, {
    {WallType::WALL_HOR, "WALL_HOR"},
    {WallType::WALL_VER, "WALL_VER"},
    {WallType::INCLINE, "INCLINE"},
    {WallType::FLOOR, "FLOOR"},
    {WallType::CEILING, "CEILING"},
    {WallType::NONE, "NONE"}
    })

NLOHMANN_JSON_SERIALIZE_ENUM(ObjectCategory, {
    {ObjectCategory::WALL, "WALL"},
    {ObjectCategory::TILE, "TILE"},
    {ObjectCategory::ENV_OBJ, "ENV_OBJ"},
    {ObjectCategory::MONSTER, "MONSTER"},
    {ObjectCategory::LIGHT, "LIGHT"},
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

std::string CFileManager::WStringToUTF8(const std::wstring &wstr)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.to_bytes(wstr);
}

std::wstring CFileManager::UTF8ToWString(const std::string &str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.from_bytes(str);
}

void CFileManager::SaveObjectList(const wstring &filePath, _uint iSceneID, const _tchar *szLayerTag)
{
    std::vector<MAPOBJECTDATA> objectDatas = CObjectManager::GetInstance()->ExportObjectData(iSceneID, szLayerTag);

    json jArray = json::array();
    for (const auto &data : objectDatas)
    {
        jArray.push_back(data);
    }

    std::ofstream ofs(filePath, std::ios::out | std::ios::binary);
    if (!ofs.is_open())
    {
        MSG_BOX("CFileManager::SaveObjectList, Failed Save");
        return;
    }

    ofs << std::setw(4) << jArray << std::endl;
    ofs.close();
}

void CFileManager::LoadObjectList(const std::wstring &filePath, _uint iSceneID, const _tchar *szLayerTag)
{
    std::ifstream ifs(filePath, std::ios::in | std::ios::binary);
    if (!ifs.is_open())
    {
        MessageBox(nullptr, L"파일 열기 실패!", L"Error", MB_OK);
        return;
    }

    json jArray;
    ifs >> jArray;

    // TODO : Parsing Data Save, Scene별, 폴더별(ObjectCategory)로 저장할것
    // 단순 Data로 파싱해두고 Scene Load할때 각 Scene에 맞는 Data를 통해 Instancing 할것
    for (const auto &jObj : jArray)
    {
        MAPOBJECTDATA objData = jObj.get<MAPOBJECTDATA>();
        CObjectManager::GetInstance()->Add_GameObject(L"Proto_GameObject_DefaultPanel", iSceneID, szLayerTag,&objData);
    }
    ifs.close();
}

BEGIN(Engine)

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
        {"FilePath", CFileManager::WStringToUTF8(_tData.OriginComponentName)}
    };
}

void from_json(const json &_j, TEXTUREDATA &_tData)
{
    std::string srcString{ "" };
    _j.at("FilePath").get_to(srcString);
    _tData.OriginComponentName = CFileManager::UTF8ToWString(srcString);
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
