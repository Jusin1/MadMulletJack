#pragma once
#include "Engine_Define.h"
#include "CBase.h"

namespace Engine
{
	class CGameObject;
}

class CEditorPickingManager : public CBase
{
	DECLARE_SINGLETON(CEditorPickingManager)
private:
	explicit CEditorPickingManager();
	virtual ~CEditorPickingManager();

	virtual void Free() override;
public:
	HRESULT Ready_Picking();
	void Clear_Picking();

public:
	void Add_PickingGroup(CGameObject *pGameObject);
	void Remove_PickingGroup(CGameObject *pGameObject);
public:
	_bool Picking();
	_vec3 Get_PickingPos() { return m_vPickingPos; }

private:
	std::unordered_set<CGameObject *> m_PickingList;
	_vec3 m_vPickingPos;

private:
	CGameObject *m_pSelectObject = nullptr;
};

