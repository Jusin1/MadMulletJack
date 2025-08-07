#pragma once
#include "CBase.h"

#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

BEGIN(Engine)

class ENGINE_DLL CFileManager : public CBase
{
	DECLARE_SINGLETON(CFileManager)
private:
	explicit CFileManager();
	virtual ~CFileManager();
	
	virtual void Free() override;
public:
	static inline std::string WStringToUTF8(const std::wstring &wstr);
	static inline std::wstring UTF8ToWString(const std::string &str);

	void SaveObjectList(const wstring &filePath, _uint iSceneID, const _tchar *szLayerTag);
	void LoadObjectList(const std::wstring &filePath, _uint iSceneID, const _tchar *szLayerTag);
private:
	
};

//Transform
void to_json(json &_j, const TRANSFORMDATA &_tData);
void from_json(const json &_j, TRANSFORMDATA &_tData);

//Texture
void to_json(json &_j, const TEXTUREDATA &_tData);
void from_json(const json &_j, TEXTUREDATA &_tData);

//GridPanel
void to_json(json &_j, const PANELDATA &_tData);
void from_json(const json &_j, PANELDATA &_tData);

//MapObject
void to_json(json &_j, const MAPOBJECTDATA &_tData);
void from_json(const json &_j, MAPOBJECTDATA &_tData);

END
