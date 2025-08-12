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
	_vec3 Get_DummyPickingPos() { return m_vDummyPickingPos; }
	CGameObject *GetPickedObject_ForDummy() { return m_pPickedOBjectForDummy; }

	_bool Picking_ForDummy();
private:
	std::unordered_set<CGameObject *> m_PickingList;
	_vec3 m_vPickingPos;
	_vec3 m_vDummyPickingPos;

private:
	CGameObject *m_pSelectObject = nullptr;
	CGameObject *m_pPickedOBjectForDummy = nullptr;
};

