#pragma once
#include "CBase.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CMapFactory : public CBase
{
	DECLARE_SINGLETON(CMapFactory)
	using CreateFunction = std::function<CGameObject* (void *)>;
private:
	explicit CMapFactory();
	virtual ~CMapFactory();

	virtual void Free() override;
public:
	void Register(ObjectCategory _eCategory, _uint _iType, CreateFunction _func);
	CGameObject *Create(ObjectCategory _eCategory, _uint _iType, void* _pData);

	_uint GetTargetSceneIndex() const { return m_iTargetSceneIndex; }
	void SetTargetSceneIndex(_uint _i) { m_iTargetSceneIndex = _i; }
private:
	std::map<_uint, CreateFunction> m_Register[g_ObjectCategoryCount];
	_uint m_iTargetSceneIndex{ 0 };
};

END