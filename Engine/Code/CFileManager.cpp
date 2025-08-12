#include "Engine_Define.h"
#include <fstream>
#include <locale>
#include <codecvt>
#include <string>
#include "CObjectManager.h"
#include "CFileManager.h"

NLOHMANN_JSON_SERIALIZE_ENUM(PanelType, {
    {PanelType::WALL_HOR, "WALL_HOR"},
    {PanelType::WALL_VER, "WALL_VER"},
    {PanelType::INCLINE, "INCLINE"},
    {PanelType::FLOOR, "FLOOR"},
    {PanelType::CEILING, "CEILING"},
    {PanelType::NONE, "NONE"}
    })

NLOHMANN_JSON_SERIALIZE_ENUM(OBJID, {
    {OBJID::OBJ_PLAYER, "PLAYER"},
    {OBJID::OBJ_MONSTER, "MONSTER"},
    {OBJID::OBJ_MAP, "MAP"},
    {OBJID::OBJ_FLOOR, "FLOOR"},
    {OBJID::OBJ_INCLINE, "INCLINE"},
    {OBJID::OBJ_CEILING, "CEILING"},
    {OBJID::OBJ_END, "NONE"}
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
        {"PanelType", _tData.eType},
        {"Count_X", _tData.dwCountX},
        {"Count_Y", _tData.dwCountY},
        {"Count_Z", _tData.dwCountZ},
        {"Interval", _tData.dwInterval}
    };
}

void from_json(const json &_j, PANELDATA &_tData)
{
    _j.at("PanelType").get_to(_tData.eType);
    _j.at("Count_X").get_to(_tData.dwCountX);
    _j.at("Count_Y").get_to(_tData.dwCountY);
    _j.at("Count_Z").get_to(_tData.dwCountZ);
    _j.at("Interval").get_to(_tData.dwInterval);
}

void to_json(json &_j, const MAPOBJECTDATA &_tData)
{
    _j = json
    {
        {"ObjectType", _tData.ObjType},
        {"TransformData", _tData.transform},
        {"TextureData", _tData.texture},
        {"PanelData", _tData.panelBuffer}
    };
}

void from_json(const json &_j, MAPOBJECTDATA &_tData)
{
    _j.at("ObjectType").get_to(_tData.ObjType);
    _j.at("TransformData").get_to(_tData.transform);
    _j.at("TextureData").get_to(_tData.texture);
    _j.at("PanelData").get_to(_tData.panelBuffer);
}

END
