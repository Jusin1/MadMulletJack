#pragma once
#include "CBase.h"

#pragma push_macro("new")
#undef new
#include "json.hpp"
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
};

END

